# This only meant to build dependencies
# To compile the project, please use bm
# IF RAN ON WINDOWS, THIS IS INTENDED TO RUN ON POWERSHELL
#
# Usage:
# You can optionally specify ARCH=<x86_64 | x86>
# A hell lot of if else to match your system platform
#  and available tools you have (ex: with or without zenity on linux)

ARCH ?= x86_64
NFD_REPO = https://github.com/mlabbe/nativefiledialog.git

ifeq ($(ARCH), x86_64)
NFD_CONFIG = release_x64
else
ifeq ($(ARCH), x86)
NFD_CONFIG = release_x86
else
# $(Unknown release architecture. ARCH must be either x86_64 or x86.)
endif
endif

ifeq ($(OS),Windows_NT)
NFD_BUILD_DIR = build/gmake_windows
else
ifeq ($(UNAME_S),Linux)
NFD_BUILD_DIR = build/gmake_linux
endif
ifeq ($(UNAME_S),Darwin)
NFD_BUILD_DIR = build/gmake_macosx
endif
endif

deps:
	git clone $(NFD_REPO) .nfd
ifeq ($(ARCH), x86_64)
ifeq ($(UNAME_S),Linux)
# linux is a special bitch, she has two (2) utilities with which we can compile
	@(cd .nfd/$(NFD_BUILD_DIR)_zenity && make config=release_x64) || (cd .nfd/$(NFD_BUILD_DIR)_zenity && make config=release_x64) && mv .nfd/build/lib/Release/*/libnfd.* ../../../liba
else
	@cd .nfd/$(NFD_BUILD_DIR) && make config=release_x64 && mv .nfd/build/lib/Release/*/libnfd.* ../lib
endif
endif
ifeq ($(ARCH), x86)
ifeq ($(UNAME_S),Linux)
# linux is a special bitch, she has two (2) utilities with which we can compile
	@(cd .nfd/$(NFD_BUILD_DIR)_zenity && make config=release_x64) || (cd .nfd/$(NFD_BUILD_DIR)_zenity && make config=release_x64) && mv .nfd/build/lib/Release/*/libnfd.* ../../../liba
else
	@cd .nfd/$(NFD_BUILD_DIR) && make config=release_x64 && mv .nfd/build/lib/Release/*/libnfd.* ../lib
endif
endif
	cp .nfd/src/include/nfd.h src/nfd.h
ifeq ($(OS),Windows_NT)
	Remove-Item -Path .nfd -Recurse
	wget https://github.com/nicbarker/clay/releases/latest/download/clay.h -OutFile src/clay.h
else
	rm -r .nfd
	wget https://github.com/nicbarker/clay/releases/latest/download/clay.h
	mv clay.h src/clay.h
endif

clean:
	rm lib/*
