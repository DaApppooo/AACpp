#!/bin/bash

if [ "$1" = "x86" ]; then
  ARC="x86"
else
  echo "Selected x64 by default (use x86 as parameter to for 32bit library compilation)"
  ARC="x64"
fi

# Clay
wget https://github.com/nicbarker/clay/releases/latest/download/clay.h
mv clay.h src/clay.h
# NativeFileDialog
git clone https://github.com/mlabbe/nativefiledialog.git .nfd
cp .nfd/src/include/nfd.h src/nfd.h

SRC=$(pwd)
PS3="Select compile target: "
ZENITY_OPT='linux zenity (requires gtk-?)'

select target in 'linux' "$ZENITY_OPT" 'windows' 'macosx'
do
  case $target in
    'linux' | '')
      cd .nfd/build/gmake_linux
      break;;
    "$ZENITY_OPT")
      cd .nfd/build/gmake_linux_zenity
      break;;
    'windows')
      cd .nfd/build/gmake_windows
      break;;
    'macosx')
      cd .nfd/build/gmake_macosx
      break;;
    *)
      echo "Please select a valid option...";;
  esac
done

make config=release_"$ARC"
cd "$SRC"
mv .nfd/build/lib/Release/*/libnfd.* lib

rm -rf .nfd
echo 'Dependency installation summary:'
echo '- Downloaded clay.h from https://github.com/nicbarker/clay/releases/latest/download/clay.h'
echo '- Cloned nativefiledialog from https://github.com/mlabbe/nativefiledialog.git '
echo '- Compiled nativefiledialog and added to library'
echo '- Removed unecessary files.'

