-- Small list of dependencies by name:
-- TARGET={LINUX, WIN}
--  PROG=aac
--   raylib by raysan5 on github under zlib License
--   nfd by by mlabbe on github under zlib license
--   libspng by (a bunch of people) on libpng.org under the BSDL3.0
--   clay by nicbarker on github under zlib license
-- TARGET={LINUX,WIN,ANDROID}
--  PROG=obz2cobz
--   zip by kuba-- on github under MIT license
--   cJSON by DaveGamble on github under MIT license
--   plutosvg by sammycage under MIT license
--   stb_image by nothings under public domain
require("shared")

load_os() -- declares TARGET
RAYLIB_VERSION = "5.5"
CJSON_VERSION = "1.7.18"
ZIP_VERSION = "0.3.3"
PLUTOSVG_VERSION = "0.0.6"
PLUTOVG_VERSION = "1.0.0" -- dependency of plutosvg
ONLY = nil
for _, a in pairs(arg) do
  if startswith(a, "target=") then
    TARGET = string.upper(string.sub(a, string.find(a, "=")+1))
    assert(
       TARGET == "LINUX"
    or TARGET == "WIN"
    or TARGET == "IOS"
    or TARGET == "ANDROID",
      TARGET .. " is and unknown target. Expected LINUX, WIN, IOS or ANDROID."
    )
  elseif startswith(a, "only=") then
    ONLY = string.sub(a, string.len("only=")+1)
  end
end
TEMP_FOLDERS = { "raylib", "plutosvg", "nfd", "nativefiledialog", "libspng", "iniparser" }
function inst(title)
  return ONLY == nil or title == ONLY:lower()
end

if TARGET == "IOS" or TARGET == "ANDROID" then
  error("Targets IOS and ANDROID are not yet supported.")
end

if not exists("lib") then
  shell("mkdir lib")
end
if not exists("include") then
  shell("mkdir include")
end
if not exists("licenses") then
  shell("mkdir licenses")
end

print("Check that compiler is available...")
if not os.execute("gcc --version") then
  error("Missing gcc. Please go install gcc.")
end

if inst("clay") then
  print("Updating/Installing clay.h ...")
  if TARGET == "LINUX" then
    shell("wget https://github.com/nicbarker/clay/releases/download/v0.13/clay.h")
  elseif TARGET == "WIN" then
    os.exit(1)
    -- shell("wget https://raw.githubusercontent.com/nicbarker/clay/refs/heads/main/clay.h")
  end
  mv("clay.h", "src/clay.h")
end

print("Grab latest release from raylib for target=" .. TARGET)
if TARGET == "LINUX" then
  if inst("raylib") then
    print("Download and install raylib (for app only)...")
    shell("wget https://github.com/raysan5/raylib/releases/download/" .. RAYLIB_VERSION .. "/raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
    shell("tar -xvzf raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
    mv("raylib-" .. RAYLIB_VERSION .. "_linux_amd64/lib/libraylib.so*", "lib/")
    mv("raylib-" .. RAYLIB_VERSION .. "_linux_amd64/include/*", "include/")
    rm("raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
    rm("-r raylib-" .. RAYLIB_VERSION .. "_linux_amd64")
  end

  if inst("nfd") then
    print("Download, compile and move nfd...")
    shell("git clone https://github.com/mlabbe/nativefiledialog.git")
    shell("cd nativefiledialog/build/gmake_linux && make config=release_x64")
    mv("nativefiledialog/build/lib/Release/x64/libnfd.a", "lib/")
    rm("nativefiledialog")
  end

  if inst("libspng") then
    print("Download, compile and move libspng...")
    shell("git clone https://github.com/randy408/libspng.git")
    shell("gcc -fPIC libspng/spng/spng.c -shared -o lib/libspng.so")
    rm("libspng")
  end

  if inst("zip") then
    print("Download, compile and move zip...")
    shell("wget https://github.com/kuba--/zip/archive/refs/tags/v"..ZIP_VERSION..".tar.gz")
    shell("tar -xzvf v"..ZIP_VERSION..".tar.gz")
    shell("mkdir -p zip-"..ZIP_VERSION.."/build")
    shell("cd zip-"..ZIP_VERSION.."/build && cmake -DBUILD_SHARED_LIBS=true -DBUILD_STATIC_PIC=OFF .. && cmake --build .")
    mv("zip-"..ZIP_VERSION.."/build/*.so", "lib/")
    mv("zip-"..ZIP_VERSION.."/LICENSE.txt", "licenses/zip.txt")
    rm("v"..ZIP_VERSION..".tar.gz*")
    rm("zip-"..ZIP_VERSION)
  end

  if inst("cjson") then
    print("Download and move cJSON...")
    shell("wget https://github.com/DaveGamble/cJSON/archive/refs/tags/v"..CJSON_VERSION..".tar.gz")
    shell("tar -xzvf v"..CJSON_VERSION..".tar.gz")
    mv("cJSON-"..CJSON_VERSION.."/cJSON.*", "obz2cobz/")
    mv("cJSON-"..CJSON_VERSION.."/LICENSE", "licenses/cJSON.txt")
    rm("v"..CJSON_VERSION..".tar.gz*")
    rm("cJSON-"..CJSON_VERSION)
  end

  if inst("plutosvg") then
    print("Download, compile and move plutosvg...")
    if exists("plutosvg") then
      rm("plutosvg")
    end
    shell("git clone --recursive https://github.com/sammycage/plutosvg.git")
    shell("cd plutosvg && cmake -B build . && cmake --build build")
    mv("plutosvg/source/*", "include/")
    mv("plutosvg/plutovg/include/plutovg.h", "include/")
    -- mv("plutosvg/build/plutovg/*.a", "include") -- reactivate this line if plutosvg.a is not enough
    mv("plutosvg/build/*.so", "lib/")
    mv("plutosvg/LICENSE", "licenses/plutosvg.txt")
    rm("plutosvg")
  end

  if inst("stb_image") then
    print("Download and move stb_image...")
    shell("wget https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h")
    mv("stb_image.h", "obz2cobz/stb_image.h")
  end

  if inst("iniparser") then
    print("Download, compile and move iniParser 4...")
    if exists("iniparser") then
      rm("iniparser")
    end
    shell("git clone https://gitlab.com/iniparser/iniparser.git")
    shell("mkdir iniparser/build")
    shell("cd iniparser/build && cmake -DBUILD_STATIC_LIBS=OFF .. && make all")
    mv("iniparser/build/*.so*", "lib/")
    mv("iniparser/src/*.h", "include/")
    rm("iniparser")
  end

  if inst("piper") then
    print("Download, compile and move (lib)Piper...")
    if exists("libpiper") then
      rm("libpiper")
    end
    shell("git clone https://github.com/DaApppooo/libpiper.git")
    shell("wget https://github.com/rhasspy/piper-phonemize/releases/download/2023.11.14-4/piper-phonemize_linux_$(uname -m).tar.gz")
    shell("mkdir -p libpiper/lib/Linux-$(uname -m)/")
    shell("tar -xzvf piper-phonemize_linux_$(uname -m).tar.gz")
    mv("piper_phonemize", "libpiper/lib/Linux-$(uname -m)/")
    shell("cd libpiper && make all")
    if not exists("include/piper") then
      shell("mkdir include/piper")
    end
    mv("libpiper/build/libpiper.so", "lib/")
    mv("libpiper/src/cpp/*.h*", "include/piper")
    mv("libpiper/src/cpp/utf8/", "include/piper")
    mv("libpiper/install/libpiper.so", "lib/")
    mv("libpiper/install/libpiper_phonemize.so*", "lib/")
    mv("libpiper/install/libonnxruntime.so*", "lib/")
    mv("libpiper/install/libespeak-ng.so*", "lib/")
    mv("libpiper/build/pi/include/onnxruntime_c_api.h", "include/piper/")
    mv("libpiper/build/pi/include/onnxruntime_cxx_api.h", "include/piper/")
    mv("libpiper/build/pi/include/onnxruntime_cxx_inline.h", "include/piper/")
    -- mv("libpiper/install/espeak-ng-data/", "assets/") -- enable espeak phonemes
    if not exists("include/piper/espeak-ng/") then
      shell("mkdir include/piper/espeak-ng/")
    end
    mv("libpiper/build/pi/include/espeak-ng/speak_lib.h", "include/piper/espeak-ng")
    if not exists("include/piper/piper-phonemize/") then
      shell("mkdir include/piper/piper-phonemize/")
    end
    mv("libpiper/build/pi/include/piper-phonemize/phoneme_ids.hpp", "include/piper/piper-phonemize/")
    mv("libpiper/build/pi/include/piper-phonemize/phonemize.hpp", "include/piper/piper-phonemize/")
    mv("libpiper/build/pi/include/piper-phonemize/shared.hpp", "include/piper/piper-phonemize/")
    mv("libpiper/build/pi/include/piper-phonemize/tashkeel.hpp", "include/piper/piper-phonemize/")
    rm("libpiper")
    rm("piper-phonemize*")
  end

  print("Done.")
elseif TARGET == "WIN" then
  print("Download and install raylib (for app only)...")
  shell("Invoke-WebRequest https://github.com/raysan5/raylib/releases/download/" .. RAYLIB_VERSION .. "/raylib-" .. RAYLIB_VERSION .. "_win64_mingw-w64.zip -OutFile raylib.zip")
  shell("Expand-Archive -LiteralPath raylib.zip -Destination raylib")
  mv("raylib/raylib-" .. RAYLIB_VERSION .. "_win64_mingw-w64/lib/libraylib.a", "lib")
  mv("raylib/raylib-" .. RAYLIB_VERSION .. "_win64_mingw-w64/include/*", "include")
  rm("raylib.zip")
  rm("raylib")
  print("Compile and move nfd...")
  shell("cd nativefiledialog/build/gmake_windows && make config=release_x64")
  mv("nativefiledialog/build/lib/Release/x64/libnfd.a", "lib")
  print("Compile and move libspng...")
  shell("gcc -c libspng/spng/spng.c -o spng.o")
  shell("ar rcs lib/libspng.a spng.o")
  rm("spng.o")
end

