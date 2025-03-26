#!/bin/bash

mkdir --parents android/sdk android/build assets include lib/armeabi-v7a lib/arm64-v8a lib/x86 lib/x86_64 src
cd android/build
mkdir --parents obj dex res/values src/com/raylib/game assets
mkdir --parents lib/armeabi-v7a lib/arm64-v8a lib/x86 lib/x86_64
mkdir --parents res/drawable-ldpi res/drawable-mdpi res/drawable-hdpi res/drawable-xhdpi
cd ../..

# openjdk
# wget https://download.java.net/java/GA/jdk24/1f9ff9062db4449d8ca828c504ffae90/36/GPL/openjdk-24_linux-x64_bin.tar.gz -o openjdk.tar.gz || { exit 1; }
# tar -xvzf openjdk.tar.gz || { exit 1; }
java --version || { echo 'please install java before runing this script.'; exit 1; }

# android command line tools
wget https://dl.google.com/android/repository/commandlinetools-linux-11076708_latest.zip cli.zip
unzip cli.zip -d android/sdk
if [ ! -d android/sdk/cmdline-tools ]; then
  echo 'Missing cmdline-tools folder in android/sdk'
  exit 1
fi
cd android/sdk/cmdline-tools/bin
./sdkmanager --update --sdk_root=../.. || { echo 'sdk update failed'; exit 1; }
./sdkmanager --install "build-tools;29.0.3" --sdk_root=../.. || { echo 'installation of build tools failed'; exit 1; }
./sdkmanager --install "platform-tools" --sdk_root=../.. || { echo 'installation of platform-tools failed'; exit 1; }
./sdkmanager --install "platforms;android-29" --sdk_root=../.. || { echo 'installation of platforms;android-29 failed'; exit 1; }
cd ../../../..

# android ndk
wget https://dl.google.com/android/repository/android-ndk-r27c-linux.zip -o ndk.zip || { exit 1; }
unzip ndk.zip || { exit 1; }
mv android-ndk-r* android/ndk
if [ ! -d android/ndk/build ]; then
  echo 'Missing build folder in android/ndk'
  exit 1
fi
if [ ! -d android/ndk/meta ]; then
  echo 'Missing meta folder in android/ndk'
  exit 1
fi

echo 'Building raylib static libraries for android:'
shell("git clone https://github.com/raysan5/raylib --depth 1")
cd raylib/src
cp raylib.h raymath.h rlgl.h ../../include
make clean
make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=arm ANDROID_API_VERSION=34 || { echo 'arm build failed'; exit 1; }
mv libraylib.a ../../lib/armeabi-v7a
make clean
make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=arm64 ANDROID_API_VERSION=34 || { echo 'arm64 build failed'; exit 1; }
mv libraylib.a ../../lib/arm64-v8a
make clean
make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=x86 ANDROID_API_VERSION=34 || { echo 'x86 build failed'; exit 1; }
mv libraylib.a ../../lib/x86
make clean
make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=x86_64 ANDROID_API_VERSION=34 || { echo 'x86_64 build failed'; exit 1; }
mv libraylib.a ../../lib/x86_64
make clean
cd ../..  



