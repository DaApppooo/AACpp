#!/bin/env python3
help="""
Usage:
obz2cobz.py <src>.obz <dst>.cobz

Descr:
Compiles Open Board Zip files (exported from Board Builder),
into a Compiled Open Board Zip for this custom AAC program I wrote (AACpp).

This is intended to be nothing more than a prototype. A full C++ (probably)
version will be written in the future. It's just that zip+json is such a hassle
to parse in C.
"""

import json
from sys import argv, stderr, exit
import zipfile as zipf
from struct import pack
from typing import Tuple
import requests as rq
from PIL import Image
import os
import io
from tqdm import tqdm
from time import sleep
import traceback

SUPPORTED_IMAGE_FORMATS = ('png','jpg','jpeg','tga','bmp','psd','gif','hdr','pic','pnm')

def expect(cond: bool, err_msg: str):
    if not cond:
        print('ERR:', err_msg)
        quit(1)

def perror(msg: str):
    stderr.write(str(msg))

def want(cond: bool, warn_msg: str):
    if not cond:
        print('WARN:', warn_msg)
_want1_cache = 0
def want1(cond: bool, warn_msg: str, wid: int):
    """
        Warns user once. Preferable, use small numbers for wid.
    """
    global _want1_cache
    if _want1_cache & (1 << wid):
        return
    if not cond:
        print('WARN:', warn_msg)
        _want1_cache |= (1 << wid)

def info(msg: str):
    print("INFO:", msg)

def todo():
    raise NotImplementedError()

class Cell:
    name: str
    tex_id: int
    parent: int
    child: int
    obz_child_id: str | None
    obz_tex_id: str | None
    obz_id: str
    obz_xy: Tuple[int, int]
    def serialize(self) -> bytes:
        if self.name is None:
            self.name = ""
        return pack(
            f'=3siq{len(self.name)}sii',
            b'CLL',
            self.tex_id,
            len(self.name),
            self.name.encode('ascii'),
            self.parent,
            self.child
        )

class Board:
    w: int
    h: int
    cells: list[Cell]
    name: str
    obz_id: str
    def serialize(self) -> bytes:
        res = pack('=ii', self.w, self.h)
        for i in self.cells:
            res += i.serialize()
        return res

class CompiledOBZ:
    def __init__(self):
        self.textures: dict[str, bytes] | Tuple[Tuple[str, bytes], ...] = {}
        self.boards: list[Board] | Tuple[Board, ...] = []
    def find_board_with_name(self, name: str) -> int | None:
        for idx, board in enumerate(self.boards):
            if board.name == name:
                return idx
        return None

def load_img_raw(z: zipf.ZipFile, src: str | None) -> bytes | None:
    if not src: # src is None or len(src) == 0
        return None
    raw: bytes
    if src.startswith('http'):
        try:
            resp = rq.get(src)
            sleep(0.2) # avoid getting flagged as some kind of ddos stuff by any website
            raw = resp.content
        except rq.RequestException as e:
            perror(f"Failed to load image with error {type(e).__name__}:{e} from web address: {repr(src)}")
            return None
    elif src.startswith('data:image/'):
        extension = src[11:11+3]
        cur = 11+3
        if extension == 'jpe':
            extension += src[11+3]
            cur += 1
        expect(extension.lower() in SUPPORTED_IMAGE_FORMATS, f">Unknown file extension for inline image: {repr(extension)}. See README.md for supported formats. Aborting.")
        cur2 = src.find(',', cur)
        encoding = src[cur:cur2]
        expect(encoding.lower() == 'base64', f">Unsupported encoding {encoding} for inline image (only base64 is supported).")
        raw = base64.b64decode(src[cur2:])
    elif src[src.rfind('.'):] in SUPPORTED_IMAGE_FORMATS:
        with z.open(src, 'rb') as _f_image:
            raw =_f_image.read()
    else:
        expect(False, f">Unknown image source information format ({repr(src[:30])}{'...' if len(src) >= 30 else ''}).")
    # Last checks just to make sure everything is good:
    pil_img = Image.open(io.BytesIO(raw))
    try:
        pil_img.verify()
    except Exception:
        expect(False, '>Given image is broken.')
    ext = pil_img.format.lower()
    if ext == 'webp':
        buf = io.BytesIO()
        pil_img.save(buf, 'png')
        raw = buf.read()
    else:
        expect(ext in SUPPORTED_IMAGE_FORMATS, f'>Unsupported image format {repr(pil_img.format.lower())}. See README.md for the list of supported image formats.')
    pil_img.close()
    return raw

def find_position(id: str, dbl: list[list[str]]):
    for y, l in enumerate(dbl):
        for x, check_id in enumerate(l):
            if check_id == id:
                return (x,y)
    expect(False, f"Could not find cell with {id=} in grid.")

def index_when(it, func):
    for idx, elem in enumerate(it):
        if func(elem):
            return idx
    return -1

def parse_board(
    z: zipf.ZipFile,
    cobz: CompiledOBZ,
    file_handler,
    obz_id: str,
    manifest
) -> Board:
    board = Board()
    obf = json.load(file_handler)
    want(obz_id == obf['id'], f"ID mismatch (manifest specifies {obz_id} while descriptor specifies {obf['id']})")
    board.obz_id = str(obz_id) # different sources use different types, safer to use strings
    board.name = obf['name']
    celld: dict[str, Cell] = {}

    info(f"Loading board named {obf['name']}.")
    board.w = obf['grid']['columns']
    board.h = obf['grid']['rows']
    board.cells = []
    for b in tqdm(obf['buttons']):
        want1(b['border_color'] is None, "*Border color for cell/button isn't supported.", 0)
        want1(b['background_color'] is None, "*Background color for cell/button isn't supported.", 1)
        c = Cell()
        c.obz_id = str(b['id'])
        c.obz_tex_id = None
        c.obz_child_id = None
        c.parent = -1
        if b['image_id'] not in cobz.textures:
            img_src = None
            for i in obf['images']:
                if i['id'] == b['id']:
                    if 'url' in i:
                        img_src = i['url']
                    elif 'data' in i:
                        img_src = i['data']
                    else:
                        want(False, '>No known image format found.')
                    # TODO: add other keys that give an image source/data
                    break
            else:
                c.tex_id = -1
                c.obz_tex_id = None
            if raw := load_img_raw(z, img_src):
                cobz.textures[b['image_id']] = raw
                c.obz_tex_id = b['image_id']
            else:
                c.tex_id = -1
                c.obz_tex_id = None
        if 'load_board' in b:
            for id, path in manifest['paths']['boards'].items():
                if path == b['load_board']['path']:
                    break
            else:
                expect(False, f"Could not find known board id with path {repr(b['load_board']['path'])}.")
            c.obz_child_id = id
            # TODO: Add other ways to link boards
        c.name = b.get('label') # Not always specified
        c.obz_xy = find_position(c.obz_id, obf['grid']['order'])
        board.cells.append(c)
    # Sorting cells based on position
    board.cells.sort(key=lambda c: c.obz_xy[::-1]) # '[::-1]' because y position is the most important when sorting, followed by x positions.
    return board   

def parse_file(filename: str) -> CompiledOBZ:
    cobz = CompiledOBZ()
    z = zipf.ZipFile(filename, 'r')
    expect("manifest.json" in (i.filename for i in z.filelist), f"Missing manifest.json in {filename} !")
    with z.open('manifest.json', 'r') as _f_manifest:
        manifest = json.load(_f_manifest)
    expect(manifest['format'] == "open-board-0.1", f"Unknown board set format: {repr(manifest['format'])}. Expected 'open-board-0.1' instead.")
    want(len(manifest['paths']['images']) == 0, "Image specification in manifest.json is not supported.")
    want(len(manifest['paths']['sounds']) == 0, "Sound specification in manifest.json is not supported.")
    for board_id, board_path in manifest['paths']['boards'].items():
        with z.open(board_path, 'r') as _f_board:
            cobz.boards.append(
                parse_board(z, cobz, _f_board, board_id, manifest)
            )
    z.close()
    # WE THEN NEED TO LINK BOARDS BY INDEX, NOT BY ID ANYMORE
    # 1. Select root board and put it first (swap)
    for root_id, path in manifest['paths']['boards'].items():
        if path == manifest['root']:
            break
    root_idx = index_when(cobz.boards, lambda x: x.obz_id == root_id)
    cobz.boards[0], cobz.boards[root_idx] = cobz.boards[root_idx], cobz.boards[0]
    # 2. Fix and resolve indicies (in all cells)
    cobz.boards = tuple(cobz.boards)
    cobz.textures = tuple(cobz.textures.items())
    for idx, board in enumerate(cobz.boards):
        for cell in board.cells:
            cell.tex_id = index_when(
                cobz.textures,
                lambda x: x[0] == cell.obz_tex_id
            )
            if cell.obz_child_id is None:
                # TODO: REMOVE: THIS IS JUST BECAUSE BOARD BUILDER DOESN'T
                # SUPPORT LINKING WITHIN THE EXPORTED FILES
                if child_idx := cobz.find_board_with_name(cell.name):
                    cell.child = child_idx
                else:
                    cell.child = -1
            else:
                cell.child = index_when(
                    cobz.boards,
                    lambda x: x.obz_id == cell.obz_child_id
                )
            if cell.child != -1:
                for c in cobz.boards[cell.child].cells:
                    c.parent = idx
    return cobz

if __name__ == '__main__':
    if len(argv) != 3:
        print(help)
        exit(1)
    try:
        cobz = parse_file(argv[1])
        with open(argv[2], 'wb') as f:
            # NOTE: using 'q' because resman.cpp reads a i64
            f.write(pack('q', len(cobz.textures)))
            for _, tex in cobz.textures:
                f.write(b'IMG')
                f.write(tex)
            f.write(pack('q', len(cobz.boards)))
            for board in cobz.boards:
                f.write(b'BRD')
                f.write(board.serialize())
        print("Done !")
    except Exception:
        perror(traceback.format_exc())
        exit(1)
    exit(0)



