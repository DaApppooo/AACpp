
# AACpp

An AAC (Augmentative and Alternative Communication) Board Player written in C++
that's intended to work with [Board Builder](https://app.globalsymbols.com/en/)
and other apps which export ".obz" files.

**This project isn't done**, but I'm actively working on it. When this project
will be mostly done, check out
[Releases](https://github.com/DaApppooo/AACpp/releases) to download and install
it on your *machine*. Check out [dependencies](#deps) if you want to
try it out early.

Please note that I'm not a full-time AAC user (and I actually use aacs very
rarely), so what I'm creating might not be adapted to your needs (and might
even be unusable). If there's anything of that sort, or some feature you'd like
me to implement, please post a [feature request](.) {i haven't made the issue
template for now} explaining what you want or need.

It uses [Raylib](https://github.com/raysan5/raylib/) for general window
operations, input, etc..., mostly [Clay](https://github.com/nicbarker/clay) for
layout (waiting for the grid feature to use it for everything), and stb_image
to fix some problems with raylib. To ask for files,
[Native File Dialog](https://github.com/mlabbe/nativefiledialog) was used.

## Usage

First step is to create your own board set on
[Board Builder](https://app.globalsymbols.com/en/). Then, export it with the
`.obz` format:

[!Clicking on the "Download Board set as OBZ"](doc/tuto0.png)

Then launch this program. A file selection window should appear. Select the
the file you just downloaded. After at most a few seconds of loading and
compiling everything needed, you should get the board set you just created
ready to be used.

In case of any problems, see [Q&A](#qna).

<a name="deps" />

## Dependencies

In order to build from source, you'll need a few things:

- raylib dynamic library (referenced higher up in this document)

- [python 3](https://www.python.org/downloads/) (version >= 3.10)

> [!IMPORTANT]
> Please note that native file dialog library is shipped as a static
> library available in the `lib` folder when cloning this repository. But
> should be recompiled for your system

To run the program without simply unziping a release, you'll need:

- raylib dynamic library (referenced higher up in this document)

- python 3

- python [pillow](https://pypi.org/project/pillow/)
- (FOR NOW, THIS MIGHT CHANGE)

## What feature of `obz` is supported ?

It's actually easier to say what isn't supported, yet:

- Custom image and text position

- ... (todo)

<a name="qna">

## Q&A

- > The program doesn't produce any sound.
-
- It's most likely that your system doesn't have a default TTS client. In this
- case, if you're on linux or windows, you need to have a `TTS` environement
- variable. For android, please file an [issue](https://github.com/DaApppooo/AACpp/issues/new?assignees=DaApppooo&labels=&projects=&template=bug--or-problem-with-default-behavior-in-general--report.md&title=)

- > After fixing all of the above, it still doesn't work on my system.
- 
- Please take the time to write an
- [issue](https://github.com/DaApppooo/AACpp/issues/new?assignees=DaApppooo&labels=&projects=&template=bug--or-problem-with-default-behavior-in-general--report.md&title=)

