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


function shell(fmt, ...)
  local res = string.format(fmt, ...)
  if TARGET == "WIN" then
    res = "powershell.exe " + res
  end
  print(res)
  if not os.execute(res) then
    print("Error occured. Exiting dependency installation...")
    if exists("nativefiledialog") then
      rm("nativefiledialog")
    end
    if exists("raylib") then
      rm("raylib")
    end
    if exists("libspng") then
      rm("libspng")
    end
    os.exit(1)
  end
end
function popen(fmt, ...)
  local res = string.format(fmt, ...)
  print(res)
  return io.popen(res,"r")
end
function rm(path)
  assert(not startswith(path, "/"), "rm (the lua function) rejects absolute paths just to be safe")
  print("×", path)
  if TARGET == "WIN" then
    os.execute("powershell.exe rm -Recurse -Force " .. path)
  elseif TARGET == "LINUX" then
    os.execute("rm -rf " .. path)
  end
end
function mv(src, dst)
  assert(not startswith(src, "/"), "mv (the lua function) rejects absolute paths just to be safe")
  assert(not startswith(dst, "/"), "mv (the lua function) rejects absolute paths just to be safe")
  print(src, "->", dst)
  if TARGET == "WIN" then
    os.execute("powershell.exe mv -Force " .. src .. " " .. dst)
  elseif TARGET == "LINUX" then
    os.execute("mv " .. src .. " " .. dst)
  end
end
-- Check if a file or directory exists in this path
function exists(file)
   local ok, err, code = os.rename(file, file)
   if not ok then
      if code == 13 then
         -- Permission denied, but it exists
         return true
      end
   end
   return ok, err
end

--- Check if a directory exists in this path
function isdir(path)
   -- "/" works on both Unix and Windows
   return exists(path.."/")
end

function startswith(s, part)
  return string.sub(s, 0, string.len(part)) == part
end
function endswith(s, part)
  return string.sub(s, string.len(s) - string.len(part)) == part
end

TARGET = "LINUX"
RAYLIB_VERSION = "5.5"
CJSON_VERSION = "1.7.18"
ZIP_VERSION = "0.3.3"
PLUTOSVG_VERSION = "0.0.6"
PLUTOVG_VERSION = "1.0.0" -- dependency of plutosvg
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
  end
end

if TARGET == "IOS" or TARGET == "ANDROID" then
  error("Targets IOS and ANDROID are not yet supported.")
end

if not isdir("lib") then
  shell("mkdir lib")
end
if not isdir("include") then
  shell("mkdir include")
end
if not isdir("licenses") then
  shell("mkdir licenses")
end

print("Check that compiler is available...")
if not os.execute("gcc --version") then
  error("Missing gcc. Please go install gcc.")
end

print("Updating/Installing clay.h ...")
if TARGET == "LINUX" then
  shell("wget https://github.com/nicbarker/clay/releases/download/v0.13/clay.h")
elseif TARGET == "WIN" then
  exit(1)
  -- shell("wget https://raw.githubusercontent.com/nicbarker/clay/refs/heads/main/clay.h")
end
mv("clay.h", "src/clay.h")

print("Cloning repos...")
shell("git clone https://github.com/mlabbe/nativefiledialog.git")
shell("git clone https://github.com/randy408/libspng.git")

print("Grab latest release from raylib for target=" .. TARGET)
if TARGET == "LINUX" then
  print("Download and install raylib (for app only)...")
  shell("wget https://github.com/raysan5/raylib/releases/download/" .. RAYLIB_VERSION .. "/raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
  shell("tar -xvzf raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
  mv("raylib-" .. RAYLIB_VERSION .. "_linux_amd64/lib/libraylib.a", "lib")
  mv("raylib-" .. RAYLIB_VERSION .. "_linux_amd64/include/*", "include")
  rm("raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
  rm("-r raylib-" .. RAYLIB_VERSION .. "_linux_amd64")

  print("Compile and move nfd...")
  shell("cd nativefiledialog/build/gmake_linux && make config=release_x64")
  mv("nativefiledialog/build/lib/Release/x64/libnfd.a", "lib")

  print("Compile and move libspng...")
  shell("gcc -c libspng/spng/spng.c -o spng.o")
  shell("ar rcs lib/libspng.a spng.o")
  
  print("Download, compile and move zip (by kuba-- on github)...")
  shell("wget https://github.com/kuba--/zip/archive/refs/tags/v"..ZIP_VERSION..".tar.gz")
  shell("tar -xzvf v"..ZIP_VERSION..".tar.gz")
  shell("mkdir -p zip-"..ZIP_VERSION.."/build")
  shell("cd zip-"..ZIP_VERSION.."/build && cmake -DBUILD_SHARED_LIBS=false .. && cmake --build .")
  mv("zip-"..ZIP_VERSION.."/build/*.a", "lib")
  mv("zip-"..ZIP_VERSION.."/LICENSE.txt", "licenses/zip.txt")

  print("Download and move cJSON...")
  shell("wget https://github.com/DaveGamble/cJSON/archive/refs/tags/v"..CJSON_VERSION..".tar.gz")
  shell("tar -xzvf v"..CJSON_VERSION..".tar.gz")
  mv("cJSON-"..CJSON_VERSION.."/cJSON.*", "obz2cobz")
  mv("cJSON-"..CJSON_VERSION.."/LICENSE", "licenses/cJSON.txt")

  print("Download, compile and move plutosvg...")
  if exists("plutosvg") then
    rm("plutosvg")
  end
  shell("git clone --recursive https://github.com/sammycage/plutosvg.git")
  shell("cd plutosvg && cmake -B build . && cmake --build build")
  mv("plutosvg/source/*", "include")
  mv("plutosvg/plutovg/include/plutovg.h", "include")
  -- mv("plutosvg/build/plutovg/*.a", "include") -- reactivate this line if plutosvg.a is not enough
  mv("plutosvg/build/*.a", "lib")
  mv("plutosvg/LICENSE", "licenses/plutosvg.txt")

  print("Download and move stb_image...")
  shell("wget https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h")
  mv("stb_image.h", "obz2cobz/stb_image.h")

  print("Cleaning up...")
  rm("v"..ZIP_VERSION..".tar.gz*")
  rm("v"..CJSON_VERSION..".tar.gz*")
  rm("cJSON-"..CJSON_VERSION)
  rm("zip-"..ZIP_VERSION)
  rm("plutosvg")
  rm("spng.o")
  rm("nativefiledialog")
  rm("libspng")
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

