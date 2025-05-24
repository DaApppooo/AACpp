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
WAYLAND = "yes"
TEMP_FOLDERS = { "nfd", "nativefiledialog", "libspng", "iniparser" }
parse_args()

if TARGET == "IOS" or TARGET == "ANDROID" then
  error("Targets IOS and ANDROID are not yet supported.")
end

ensure_folder("lib")
ensure_folder("include")
ensure_folder("licenses")

CMAKE_FLAGS = ""
if TARGET == "WIN" then
  CMAKE_SHORT_FLAGS = ("-DCMAKE_MAKE_PROGRAM='"..
                popen("where make")..
                "'"
                )
  CMAKE_FLAGS = "-G 'MinGW Makefiles' -DCMAKE_C_COMPILER='"..popen("where gcc")..
                "' -DCMAKE_CXX_COMPILER='"..
                popen("where g++")..
                "' "..CMAKE_SHORT_FLAGS
end

print("Check that compiler is available...")
if not os.execute("gcc --version") then
  error("Missing gcc. Please go install gcc.")
end

function inst_clay()
  print("Updating/Installing clay.h ...")
  wget("https://github.com/nicbarker/clay/releases/download/v"..CLAY_VERSION.."/clay.h", "clay.h")
  mv("clay.h", "src/clay.h")
end

function inst_nfd()
  print("Download, compile and move nfd...")
  if TARGET == "LINUX" then
    shell("git clone https://github.com/mlabbe/nativefiledialog.git")
    shell("cd nativefiledialog/build/gmake_linux && make config=release_x64")
    mv("nativefiledialog/build/lib/Release/x64/libnfd.a", "lib/")
    rm("nativefiledialog")
  elseif TARGET == "WIN" then
    print("Skipping for windows because nfd just cannot compile for some reason.")
    -- shell("git clone https://github.com/mlabbe/nativefiledialog.git")
    -- shell("; cd nativefiledialog/build/gmake_windows ; make config=release_x64")
    -- mv("nativefiledialog/build/lib/Release/x64/libnfd.a", "lib/")
    -- rm("nativefiledialog")
  else
    todo()
  end
end
function _inst_zlib()
  print("Downloadn, compile and move zlib...")
  if TARGET == "LINUX" then
    if os.execute("printf '#include <zlib.h>\nint main() {return 0;}' | gcc -cx - -o _test") then
      rm("_test")
      print("ZLIB dev files were found. Skipping compilation.")
      return
    end
    print("ZLIB dev files weren't found. ")
    todo()
  elseif TARGET == "WIN" then
    if exists("zlib") then
      rm("zlib")
    end
    shell("git clone https://github.com/madler/zlib.git")
    shell("gcc -c zlib/*.c -O3")
    shell("gcc -fPIC *.o -shared -o libz.dll")
    rm("*.o")
    mv("libz.dll", "lib")
    mv("zlib/*.h", "include")
  end
end
function inst_libspng()
  _inst_zlib()
  print("Download, compile and move libspng...")
  if TARGET == "LINUX" then
    shell("git clone https://github.com/randy408/libspng.git")
    shell("gcc -fPIC libspng/spng/spng.c -shared -o lib/libspng.so")
    rm("libspng")
  elseif TARGET == "WIN" then
    shell("git clone https://github.com/randy408/libspng.git")
    shell("gcc -fPIC libspng/spng/spng.c -L lib -lz -I include -shared -o lib/libspng.dll")
    rm("libspng")
  else
    todo()
  end
end
function inst_raylib()
  print("Download and install raylib (for app only)...")
  if exists("raylib/") then
    rm("raylib/")
  end
  shell("git clone https://github.com/LibreAAC/raylib.git")
  if TARGET == "LINUX" then
    shell("cd raylib/src && make PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=SHARED")
    mv("raylib/src/libraylib.so*", "lib/")
  elseif TARGET == "WIN" then
    shell("; cd raylib/src ; make PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=SHARED")
    mv("raylib/src/raylib.dll", "lib/")
  else
    todo()
  end
  mv("raylib/src/raylib.h", "include/")
  mv("raylib/src/raymath.h", "include/")
  -- rm("raylib/")
end
function inst_iniparser()
  print("Download, compile and move iniParser 4...")
  if TARGET == "LINUX" then
    if exists("iniparser") then
      rm("iniparser")
    end
    shell("git clone https://gitlab.com/iniparser/iniparser.git")
    mkdir("iniparser/build")
    shell("cd iniparser/build && cmake -DBUILD_STATIC_LIBS=OFF .. && make all")
    mv("iniparser/build/*.so*", "lib/")
    mv("iniparser/src/*.h", "include/")
    rm("iniparser")
  elseif TARGET == "WIN" then
    if exists("iniparser") then
      rm("iniparser")
    end
    shell("git clone https://gitlab.com/iniparser/iniparser.git")
    mkdir("iniparser/build")
    shell("; cd iniparser/build ; cmake -DBUILD_STATIC_LIBS=OFF .. "..CMAKE_FLAGS.." ; make all")
    mv("iniparser/build/*.dll*", "lib/")
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
    wget("https://github.com/rhasspy/piper-phonemize/releases/download/2023.11.14-4/piper-phonemize_linux_$(uname -m).tar.gz",
          "piper-phonemize.tar.gz")
    mkdir("libpiper/lib/Linux-$(uname -m)/")
    extract("piper-phonemize.tar.gz")
    mv("piper-phonemize/licenses/uni-algo/LICENSE.md", "licenses/piper.uni-algo.md")
    mv("piper-phonemize", "libpiper/lib/Linux-$(uname -m)/")
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
    mv("libpiper/install/espeak-ng-data", "assets/")
    mv("libpiper/build/pi/include/onnxruntime_c_api.h", "include/piper/")
    mv("libpiper/build/pi/include/onnxruntime_cxx_api.h", "include/piper/")
    mv("libpiper/build/pi/include/onnxruntime_cxx_inline.h", "include/piper/")
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
  elseif TARGET == "WIN" then
    if exists("libpiper") then
      rm("libpiper")
    end
    shell("git clone https://github.com/DaApppooo/libpiper.git")
    wget("https://github.com/rhasspy/piper-phonemize/releases/download/2023.11.14-4/piper-phonemize_windows_amd64.zip", "ppwa.zip")
    shell("mkdir -p libpiper/lib/Windows-amd64/")
    extract("ppwa.zip")
    -- No licenses ???
    -- mv("piper-phonemize/licenses/uni-algo/LICENSE.md", "licenses/piper.uni-algo.md")
    mv("piper-phonemize", "libpiper/lib/Windows-amd64/")
    shell("; cd libpiper ; cmake -Bbuild -DCMAKE_INSTALL_PREFIX=install "..CMAKE_FLAGS)
    shell("; cd libpiper ; cmake --build build --config Release "..CMAKE_SHORT_FLAGS)
    shell("; cd libpiper ; cmake --install build "..CMAKE_SHORT_FLAGS)
    if not exists("include/piper") then
      shell("mkdir include/piper")
    end
    mv("libpiper/build/libpiper.dll", "lib/")
    mv("libpiper/src/cpp/*.h*", "include/piper")
    mv("libpiper/src/cpp/utf8/", "include/piper")
    mv("libpiper/install/libpiper.dll", "lib/")
    mv("libpiper/install/libpiper_phonemize.dll*", "lib/")
    mv("libpiper/install/libonnxruntime.dll*", "lib/")
    mv("libpiper/install/libespeak-ng.dll*", "lib/")
    mv("libpiper/install/espeak-ng-data", "assets/")
    mv("libpiper/build/pi/include/onnxruntime_c_api.h", "include/piper/")
    mv("libpiper/build/pi/include/onnxruntime_cxx_api.h", "include/piper/")
    mv("libpiper/build/pi/include/onnxruntime_cxx_inline.h", "include/piper/")
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
    rm("ppwa.zip")
  else
    todo()
  end
end
function inst_obz2cobz()
  print("Downloading, compiling and moving obz2cobz...")
  if exists("obz2cobz") then
    rm("obz2cobz")
  end
  ensure_folder("'lib/obz2cobz'")
  shell("git clone --recursive https://github.com/LibreAAC/obz2cobz.git")
  if TARGET == "WIN" then
    shell("; cd obz2cobz ; lua install_deps.lua target=WIN")
    shell("; cd obz2cobz ; lua build.lua LD_LIBRARY_PATH=lib/obz2cobz/ target=WIN")
  else
    shell("cd obz2cobz && lua install_deps.lua target="..TARGET)
    shell("cd obz2cobz && lua build.lua LD_LIBRARY_PATH=lib/obz2cobz/ target="..TARGET)
  end
  mv("obz2cobz/bin/*", "assets/")
  mv("obz2cobz/lib/*", "lib/obz2cobz/")
  rm("obz2cobz")
end

run_install_scripts()
if TARGET == "WIN" then
  mv("lib/*.dll", "bin")
end

