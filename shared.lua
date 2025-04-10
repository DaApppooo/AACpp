function shell(res)
  if TARGET == "WIN" then
    res = "powershell.exe " + res
  end
  print(res)
  if not os.execute(res) then
    print("Error occured. Exiting installation...")
    for i,f in pairs(TEMP_FOLDERS) do
      if exists(f) then
        rm(f)
      end
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
  if src == dst and isdir(dst) then
    rm(dst)
  end
  print(src, "->", dst)
  if TARGET == "WIN" then
    os.execute("powershell.exe mv -Force " .. src .. " " .. dst)
  elseif TARGET == "LINUX" then
    os.execute("mv -f " .. src .. " " .. dst)
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

function load_os()
  TARGET = "LINUX"
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
      return TARGET
    end
  end

	-- ask LuaJIT first
	if jit then
	  print(jit.os)
		return jit.os
	end

	-- Unix, Linux variants
	local fh,err = assert(io.popen("uname -o 2>/dev/null","r"))
	if fh then
		osname = fh:read()
		TARGET = "LINUX"
		return osname
	end

	TARGET = "WIN"
	return osname or "Windows"
end

