-- Small list of dependencies by name:
-- TARGET={LINUX, WIN}
--  raylib by raysan5 on github under zlib License
--  nfd by by mlabbe on github under zlib license
--  libspng by (a bunch of people) on libpng.org under the BSDL3.0
--  clay by nicbarker on github under zlib license
require("src/shared/shared")
load_os()
RAYLIB_VERSION = "5.5"
CLAY_VERSION = "0.13"
TEMP_FOLDERS = { "raylib", "nfd", "nativefiledialog", "libspng", "iniparser" }
parse_args()

if TARGET == "IOS" or TARGET == "ANDROID" then
  error("Targets IOS and ANDROID are not yet supported.")
end

ensure_folder("lib")
ensure_folder("include")
ensure_folder("licenses")

print("Check that compiler is available...")
if not os.execute("gcc --version") then
  error("Missing gcc. Please go install gcc.")
end

function inst_clay()
  print("Updating/Installing clay.h ...")
  if TARGET == "LINUX" then
    shell("wget https://github.com/nicbarker/clay/releases/download/v"..CLAY_VERSION.."/clay.h")
  else
    todo()
  end
  mv("clay.h", "src/clay.h")
end

function inst_nfd()
  print("Download, compile and move nfd...")
  if TARGET == "LINUX" then
    shell("git clone https://github.com/mlabbe/nativefiledialog.git")
    shell("cd nativefiledialog/build/gmake_linux && make config=release_x64")
    mv("nativefiledialog/build/lib/Release/x64/libnfd.a", "lib/")
    rm("nativefiledialog")
  else
    todo()
  end
end
function inst_libspng()
  print("Download, compile and move libspng...")
  if TARGET == "LINUX" then
    shell("git clone https://github.com/randy408/libspng.git")
    shell("gcc -fPIC libspng/spng/spng.c -shared -o lib/libspng.so")
    rm("libspng")
  else
    todo()
  end
end
function inst_raylib()
  print("Download and install raylib (for app only)...")
  if TARGET == "LINUX" then
    shell("wget https://github.com/raysan5/raylib/releases/download/" .. RAYLIB_VERSION .. "/raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
    shell("tar -xvzf raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
    mv("raylib-" .. RAYLIB_VERSION .. "_linux_amd64/lib/libraylib.so*", "lib/")
    mv("raylib-" .. RAYLIB_VERSION .. "_linux_amd64/include/*", "include/")
    rm("raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
    rm("-r raylib-" .. RAYLIB_VERSION .. "_linux_amd64")
  else
    todo()
  end
end
function inst_iniparser()
  print("Download, compile and move iniParser 4...")
  if TARGET == "LINUX" then
    if exists("iniparser") then
      rm("iniparser")
    end
    shell("git clone https://gitlab.com/iniparser/iniparser.git")
    shell("mkdir iniparser/build")
    shell("cd iniparser/build && cmake -DBUILD_STATIC_LIBS=OFF .. && make all")
    mv("iniparser/build/*.so*", "lib/")
    mv("iniparser/src/*.h", "include/")
    rm("iniparser")
  else
    todo()
  end
end
function inst_piper()
  print("Download, compile and move (lib)Piper...")
  if TARGET == "LINUX" then
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
  else
    todo()
  end
end

run_install_scripts()
