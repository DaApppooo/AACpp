
# AACpp

An AAC (Augmentative and Alternative Communication) Board Player written in C++
that's intended to work with [Board Builder](https://app.globalsymbols.com/en/)
and other apps which export ".obz" files. This app should be 100% free to all
users, check out the license for more details.

**This project isn't done**, but I'm actively working on it. When this project
will be mostly done, check out
[Releases](https://github.com/DaApppooo/AACpp/releases) to download and install
it on your machine. Check out [dependencies](#deps) if you want to
try it out early (currently impossible). When the first release will come out,
I'd like to have a linux, windows and android version available. **Once done**
this project will continue to evolve with suggestion coming from users.

Please note that I'm not a full-time AAC user (and I actually use aacs very
rarely), so what I'm creating might not be adapted to your needs (and might
even be unusable). If there's anything of that sort, or some feature you'd like
me to implement, please post a [feature request](https://github.com/DaApppooo/AACpp/issues/new?assignees=DaApppooo&labels=&projects=&template=feature-request.md&title=)
explaining what you want or need.

## Usage

> [!WARNING]
> Everytime you open a new board, the first loading can be quite long. This is
> because Libre AAC uses a custom file format optimized for the way it works.
> This optimization process can take some time, but it's only the first time
> you open the board. Then a COBZ file should have been created next to it
> which will be reused in order to avoid recompilation.

### With Board Builder

First step is to create your own board set on
[Board Builder](https://app.globalsymbols.com/en/). This program supports most
of Board Builder features except custom label position.

> [!CAUTION]
> Make sure the cells/buttons that are linked to another board **have the same
> exact name as the board they're linked to**. This is because Board Builder
> does not support proper board linkage in the `.obz` format. So this should be
> temporary. I sent them an email about this, hoping this will be fixed soon
> enough.

When your board is done, download your board set as `OBZ`:

![Clicking on the "Download Board set as OBZ"](doc/tuto0.png)

Then launch this program. A file selection window should appear. Select the
the file you just downloaded. After at most a few seconds of loading and
compiling everything needed, you should get the board set you just created
ready to be used.

In case of any problems, see [Q&A](#qna).

### Without Board Builder

Find a way to export your board under the obz file format. Then select this
file when launching the app, or go in the settings to change your current board
for the one your just exported. If the source from which you exported your
board does not implement fully the obz file format, there might be issues when
compiling your board.

### Sharing boards

If you've already used a board with Libre AAC and would like to share it with
someone, instead of sharing the OBZ file, you can directly share the COBZ file
which should be next to it (the OBZ file must have been opened at least once).
This should reduce the loading time considerably.

<a name="deps" />

## Dependencies

> Interesting link for additional voices for piper:
> [https://brycebeattie.com/files/tts/](https://brycebeattie.com/files/tts/)

To install dependencies, `lua`, `gcc`, `g++`, `make` and `cmake`, and an access
to internet are necessary.

For linux, depending on your distribution, there are other necessary
development dependencies. See [distros](#distros)

You can easily install dependencies and have every folder setup just as needed
by running `lua install_deps.lua`. Here is the list of dependencies:
- [raylib (ZLIB)](https://github.com/raysan5/raylib)
- [clay (ZLIB)](https://github.com/nicbarker/clay)
- (Windows & Linux only) [nfd (ZLIB)](https://github.com/mlabbe/nativefiledialog)
- [libspng (BSDL3.0)](https://github.com/randy408/libspng)
- [iniparser](https://gitlab.com/iniparser/iniparser)
- [piper (MIT)](https://github.com/DaApppooo/libpiper)
  - [piper-phonemize (MIT)](https://github.com/rhasspy/piper-phonemize)
  - [onnxruntime (MIT)](https://github.com/microsoft/onnxruntime)
  - [espeak-ng (GPL 3.0+)](https://github.com/espeak-ng/espeak-ng)
    - (Compatibility layer under the BSD-2-Clause license)
    - (Android compatibility under the Apache 2.0 license)
  - [spdlog (MIT dependent on fmt)](https://github.com/gabime/spdlog)
  - [fmt (custom derived from MIT)](https://github.com/fmtlib/fmt)
- [obz2cobz (Public Domain)](https://github.com/LibreAAC/obz2cobz)
  - [CJSON (MIT)](https://github.com/DaveGamble/cJSON)
  - [stb (image and image_write) (Public Domain or MIT)](https://github.com/nothings/stb)
  - [zip (MIT)](https://github.com/kuba--/zip)
  - [plutosvg (MIT)](https://github.com/sammycage/plutosvg)

In case you need to install just one of these dependencies (ignore
sub-dependencies), you can pass the argument `only=dependency_name` with
`dependency_name` being the short name of the dependency as given above. (Ex:
`lua install_deps.lua only=nfd` will only install the `nfd` dependency).

Right now, the app will not be provided with any voice for the TTS utility.
You can either go on [here](https://github.com/rhasspy/piper/blob/master/VOICES.md)
and download both the model and the config of one of these, or download a .onnx
and its .onnx.json associated file from somewhere else, then put them right
into a folder called `tts`, next to the `bin` folder (if it doesn't exist,
just create it), and finally select your voice in the settings.

> [!WARNING]
> Voices which use Tashkeel (Arabic) are not yet supported.

## Building

### Linux, Windows

To build from source, you'll need python 3.12 (or later).

```bash
python3 bm clean
python3 bm cxml
python3 bm production
```

The `bin` folder should contain the binary/executable. It should be ran with
the current working directory being the parent directory of `bin`. Both
`assets` and `lib` should be accessible from that current working directory.

### Android

(TODO)

### IOS

(TODO)

## What features of OBZ are supported ?

Ideally, every feature of the OBZ file format + every custom feature Board
Builder might offer. Feature requests will mostly help me know on which feature
I should work in priority. Check out the
[todo](https://github.com/users/DaApppooo/projects/1) to see how the project is
going, and what you can contribute to if you want to.

<a name="qna" />

## Q&A

> There will be a few guides on installing and using the app on different
> systems. Idk.

> After fixing all of the above, it still doesn't work on my system.
- Please take the time to write an
  [issue](https://github.com/DaApppooo/AACpp/issues/new?assignees=DaApppooo&labels=&projects=&template=bug--or-problem-with-default-behavior-in-general--report.md&title=)


<a name="distros" />

## Distros

### Debian based (Linux Mint, Ubuntu, etc...)

To install needed dependencies and run the build script, just run:
```bash
# Dependencies installation:
sudo apt install build-essential git lua5.4
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
sudo apt install libgtk-3-dev libgtk2.0-dev
# Build script:
sudo apt install python3
python3 --version
# ENSURE THAT THE OUTPUT OF THE PREVIOUS COMMAND SHOWS python 3.12 (or 3.13, 3.14 etc...)
```

### Arch

```bash
# Dependencies installation:
sudo pacman -Sy base-devel git
sudo pacman -S alsa-lib mesa libx11 libxrandr libxi libxcursor libxinerama
sudo pacman -S gtk2 gtk3
# Build script:
sudo pacman -S python
python3 --version
# ENSURE THAT THE OUTPUT OF THE PREVIOUS COMMAND SHOWS python 3.12 (or 3.13, 3.14 etc...)
```
