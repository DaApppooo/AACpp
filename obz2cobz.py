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
from PIL import Image, UnidentifiedImageError
import os
import io
from tqdm import tqdm
from time import sleep
import traceback
from configparser import ConfigParser
from subprocess import Popen, PIPE

def svg2png(data: bytes) -> bytes:
    f = Popen(['cairosvg', '-W', '300', '-H', '300', '-f', 'png', '-'], stdin=PIPE, stdout=PIPE)
    f.stdin.write(data)
    f.stdin.flush()
    f.stdin.close()
    raw = f.stdout.read()
    f.stdout.close()
    return bytes(raw)

DEFAULTS = ConfigParser()
DEFAULTS.read('config/defaults.ini')
SUPPORTED_IMAGE_FORMATS = ('png','jpg','jpeg','tga','bmp','psd','gif','hdr','pic','pnm','svg')
LEGACY_COLORS = {
"AliceBlue":"#F0F8FF",
"AntiqueWhite":"#FAEBD7",
"Aqua":"#00FFFF",
"Aquamarine":"#7FFFD4",
"Azure":"#F0FFFF",
"Beige":"#F5F5DC",
"Bisque":"#FFE4C4",
"Black":"#000000",
"BlanchedAlmond":"#FFEBCD",
"Blue":"#0000FF",
"BlueViolet":"#8A2BE2",
"Brown":"#A52A2A",
"BurlyWood":"#DEB887",
"CadetBlue":"#5F9EA0",
"Chartreuse":"#7FFF00",
"Chocolate":"#D2691E",
"Coral":"#FF7F50",
"CornflowerBlue":"#6495ED",
"Cornsilk":"#FFF8DC",
"Crimson":"#DC143C",
"Cyan":"#00FFFF",
"DarkBlue":"#00008B",
"DarkCyan":"#008B8B",
"DarkGoldenRod":"#B8860B",
"DarkGray":"#A9A9A9",
"DarkGrey":"#A9A9A9",
"DarkGreen":"#006400",
"DarkKhaki":"#BDB76B",
"DarkMagenta":"#8B008B",
"DarkOliveGreen":"#556B2F",
"DarkOrange":"#FF8C00",
"DarkOrchid":"#9932CC",
"DarkRed":"#8B0000",
"DarkSalmon":"#E9967A",
"DarkSeaGreen":"#8FBC8F",
"DarkSlateBlue":"#483D8B",
"DarkSlateGray":"#2F4F4F",
"DarkSlateGrey":"#2F4F4F",
"DarkTurquoise":"#00CED1",
"DarkViolet":"#9400D3",
"DeepPink":"#FF1493",
"DeepSkyBlue":"#00BFFF",
"DimGray":"#696969",
"DimGrey":"#696969",
"DodgerBlue":"#1E90FF",
"FireBrick":"#B22222",
"FloralWhite":"#FFFAF0",
"ForestGreen":"#228B22",
"Fuchsia":"#FF00FF",
"Gainsboro":"#DCDCDC",
"GhostWhite":"#F8F8FF",
"Gold":"#FFD700",
"GoldenRod":"#DAA520",
"Gray":"#808080",
"Grey":"#808080",
"Green":"#008000",
"GreenYellow":"#ADFF2F",
"HoneyDew":"#F0FFF0",
"HotPink":"#FF69B4",
"IndianRed":"#CD5C5C",
"Indigo":"#4B0082",
"Ivory":"#FFFFF0",
"Khaki":"#F0E68C",
"Lavender":"#E6E6FA",
"LavenderBlush":"#FFF0F5",
"LawnGreen":"#7CFC00",
"LemonChiffon":"#FFFACD",
"LightBlue":"#ADD8E6",
"LightCoral":"#F08080",
"LightCyan":"#E0FFFF",
"LightGoldenRodYellow":"#FAFAD2",
"LightGray":"#D3D3D3",
"LightGrey":"#D3D3D3",
"LightGreen":"#90EE90",
"LightPink":"#FFB6C1",
"LightSalmon":"#FFA07A",
"LightSeaGreen":"#20B2AA",
"LightSkyBlue":"#87CEFA",
"LightSlateGray":"#778899",
"LightSlateGrey":"#778899",
"LightSteelBlue":"#B0C4DE",
"LightYellow":"#FFFFE0",
"Lime":"#00FF00",
"LimeGreen":"#32CD32",
"Linen":"#FAF0E6",
"Magenta":"#FF00FF",
"Maroon":"#800000",
"MediumAquaMarine":"#66CDAA",
"MediumBlue":"#0000CD",
"MediumOrchid":"#BA55D3",
"MediumPurple":"#9370DB",
"MediumSeaGreen":"#3CB371",
"MediumSlateBlue":"#7B68EE",
"MediumSpringGreen":"#00FA9A",
"MediumTurquoise":"#48D1CC",
"MediumVioletRed":"#C71585",
"MidnightBlue":"#191970",
"MintCream":"#F5FFFA",
"MistyRose":"#FFE4E1",
"Moccasin":"#FFE4B5",
"NavajoWhite":"#FFDEAD",
"Navy":"#000080",
"OldLace":"#FDF5E6",
"Olive":"#808000",
"OliveDrab":"#6B8E23",
"Orange":"#FFA500",
"OrangeRed":"#FF4500",
"Orchid":"#DA70D6",
"PaleGoldenRod":"#EEE8AA",
"PaleGreen":"#98FB98",
"PaleTurquoise":"#AFEEEE",
"PaleVioletRed":"#DB7093",
"PapayaWhip":"#FFEFD5",
"PeachPuff":"#FFDAB9",
"Peru":"#CD853F",
"Pink":"#FFC0CB",
"Plum":"#DDA0DD",
"PowderBlue":"#B0E0E6",
"Purple":"#800080",
"RebeccaPurple":"#663399",
"Red":"#FF0000",
"RosyBrown":"#BC8F8F",
"RoyalBlue":"#4169E1",
"SaddleBrown":"#8B4513",
"Salmon":"#FA8072",
"SandyBrown":"#F4A460",
"SeaGreen":"#2E8B57",
"SeaShell":"#FFF5EE",
"Sienna":"#A0522D",
"Silver":"#C0C0C0",
"SkyBlue":"#87CEEB",
"SlateBlue":"#6A5ACD",
"SlateGray":"#708090",
"SlateGrey":"#708090",
"Snow":"#FFFAFA",
"SpringGreen":"#00FF7F",
"SteelBlue":"#4682B4",
"Tan":"#D2B48C",
"Teal":"#008080",
"Thistle":"#D8BFD8",
"Tomato":"#FF6347",
"Turquoise":"#40E0D0",
"Violet":"#EE82EE",
"Wheat":"#F5DEB3",
"White":"#FFFFFF",
"WhiteSmoke":"#F5F5F5",
"Yellow":"#FFFF00",
"YellowGreen":"#9ACD32"
}
LEGACY_COLORS = { k:pack('>i', int(v[1:], 16)) for k,v in LEGACY_COLORS.items() }
ERROR_COLOR = bytes([0, 0, 0, 0])

def expect(cond: bool, err_msg: str):
    if not cond:
        print('ERR:', err_msg)
        quit(1)

def perror(msg: str):
    stderr.write(str(msg) + '\n')

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

def var2fixed_utf8(s: bytes) -> bytes:
    UTF8_BIT = 1 << 7
    r = bytes([])
    n = 0
    for i in s:
        n = (n << 8) | int(i)
        if not (int(i) & UTF8_BIT):
            r += pack('=i', n)
            n = 0
    return r

class Cell:
    name: str
    tex_id: int
    parent: int
    child: int
    background: bytes
    border: bytes
    obz_child_id: str | None
    obz_tex_id: str | None
    obz_id: str
    obz_xy: Tuple[int, int]
    def serialize(self) -> bytes:
        if self.name is None:
            self.name = ""
        self.background = self.background or ERROR_COLOR
        self.border = self.border or ERROR_COLOR
        byts = self.name.encode('utf-8')
        return pack(
            f'=3siq{len(byts)}sii4B4B',
            b'CLL',
            self.tex_id,
            len(byts),
            byts,
            self.parent,
            self.child,
            *self.background,
            *self.border
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

def depercent(expr: str, max: float):
    if expr.endswith('%'):
        return float(expr[:-1])/100.0 * max
    else:
        return float(expr)
def parse_color(expr: str | None, default) -> bytes:
    if expr is None:
        return default
    if expr.startswith('rgb(') and expr.endswith(')'):
        r,g,b = (int(depercent(i.strip(), 255)) for i in expr[4:-1].split(','))
        a = 255
    elif expr.startswith('rgba(') and expr.endswith(')'):
        r,g,b,a = (int(depercent(i.strip(), 255)) for i in expr[5:-1].split(','))
    elif expr.startswith('#'):
        if len(expr) == 7:
            r,g,b = (int(c0+c1, 16) for c0,c1 in zip(expr[1::2],expr[2::2]))
            a = 255
        elif len(expr) == 9:
            r,g,b,a = (int(c0+c1, 16) for c0,c1 in zip(expr[1::2],expr[2::2]))
        else:
            want(False, f"Expected 6 or 8 digit hex color format, but got {repr(expr)}.")
            return default
    elif expr in LEGACY_COLORS:
        return LEGACY_COLORS[expr]
    else:
        want(False, f"Unknown color format: {repr(expr)}")
        return default
    return bytes([r,g,b,a])
            

def load_img_raw(z: zipf.ZipFile, src: str | None, cell_name: str) -> bytes | None:
    if not src: # src is None or len(src) == 0
        return None
    raw: bytes
    if src.startswith('http'):
        try:
            extension = src[src.rfind('.')+1:]
            resp = rq.get(src)
            sleep(0.2) # avoid getting flagged as some kind of ddos stuff by any website
            raw = resp.content
            if extension.lower() == 'svg':
                raw = svg2png(raw)
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
        if extension.lower() == 'svg':
            raw = svg2png(raw)
    elif (extension := src[src.rfind('.'):]) in SUPPORTED_IMAGE_FORMATS:
        with z.open(src, 'rb') as _f_image:
            raw =_f_image.read()
        if extension.lower() == 'svg':
            raw = svg2png(raw)
    else:
        expect(False, f">Unknown image source information format ({repr(src[:30])}{'...' if len(src) >= 30 else ''}).")
    # Last checks just to make sure everything is good:
    try:
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
    except UnidentifiedImageError:
        perror(f"Failed to identify image for cell {repr(cell_name)}, {src=}.")
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
    for b in obf['buttons']:
        # want1(b['border_color'] is None, "*Border color for cell/button isn't supported.", 0)
        # want1(b['background_color'] is None, "*Background color for cell/button isn't supported.", 1)
        c = Cell()
        c.obz_id = str(b['id'])
        c.obz_tex_id = None
        c.obz_child_id = None
        c.parent = -1
        c.background = ERROR_COLOR
        c.border = ERROR_COLOR
        c.name = b.get('label') # Not always specified
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
            if raw := load_img_raw(z, img_src, c.name):
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
        if 'background_color' in b:
            c.background = parse_color(b['background_color'], parse_color(DEFAULTS['cell']['background'], ERROR_COLOR))
        if 'border_color' in b:
            c.border = parse_color(b['border_color'], parse_color(DEFAULTS['cell']['border'], ERROR_COLOR))
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



