
function shell(fmt, ...)
  local res = string.format(fmt, ...)
  print(res)
  if not os.execute(res) then
    print("Error occured. Exiting dependency installation...")
    os.exit(1)
  end
end
function popen(fmt, ...)
  local res = string.format(fmt, ...)
  print(res)
  return io.popen(res,"r")
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

local TARGET = "LINUX"
local RAYLIB_VERSION = "5.5"
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

print("Check that compiler is available...")
if not os.execute("gcc --version") then
  error("Missing gcc. Please go install gcc.")
end

print("Cloning repos...")
shell("git clone https://github.com/mlabbe/nativefiledialog.git")
shell("git clone https://github.com/randy408/libspng.git")

print("Grab latest release from raylib for target=" .. TARGET)
if TARGET == "LINUX" then
  print("Download and install raylib (for app only)...")
  shell("wget https://github.com/raysan5/raylib/releases/download/" .. RAYLIB_VERSION .. "/raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
  shell("tar -xvzf raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
  shell("mv raylib-" .. RAYLIB_VERSION .. "_linux_amd64/lib/libraylib.a lib")
  shell("mv raylib-" .. RAYLIB_VERSION .. "_linux_amd64/include/* include")
  shell("rm raylib-" .. RAYLIB_VERSION .. "_linux_amd64.tar.gz")
  shell("rm -r raylib-" .. RAYLIB_VERSION .. "_linux_amd64")
  print("Compile and move nfd...")
  shell("cd nativefiledialog/build/gmake_linux && make config=release_x64")
  shell("mv nativefiledialog/build/lib/Release/x64/libnfd.a lib")
  print("Compile and move libspng...")
  shell("gcc -c libspng/spng/spng.c -o spng.o")
  shell("ar rcs lib/libspng.a spng.o")
  shell("rm spng.o")
  shell("rm -rf nativefiledialog")
  shell("rm -rf libspng")
elseif TARGET == "WIN" then
  print("Download and install raylib (for app only)...")
  shell("Invoke-WebRequest https://github.com/raysan5/raylib/releases/download/" .. RAYLIB_VERSION .. "/raylib-" .. RAYLIB_VERSION .. "_win64_mingw-w64.zip -OutFile raylib.zip")
  shell("Expand-Archive -LiteralPath raylib.zip -Destination raylib")
  shell("mv raylib/raylib-" .. RAYLIB_VERSION .. "_win64_mingw-w64/lib/libraylib.a lib")
  shell("mv raylib/raylib-" .. RAYLIB_VERSION .. "_win64_mingw-w64/include/* include")
  os.remove("raylib.zip")
  os.remove("raylib")
  print("Compile and move nfd...")
  shell("cd nativefiledialog/build/gmake_windows && make config=release_x64")
  shell("mv nativefiledialog/build/lib/Release/x64/libnfd.a lib")
  print("Compile and move libspng...")
  shell("gcc -c libspng/spng/spng.c -o spng.o")
  shell("ar rcs lib/libspng.a spng.o")
  os.remove("spng.o")
end

