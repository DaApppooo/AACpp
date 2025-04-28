-- A temporary "small" lua script to handle piper installation and setup easily
require("src/shared/shared")
function lcount(s, char)
  local c = 0
  local p = 1
  while p < string.len(s) do
    if s[p] == char then
      c = c + 1
    else
      return c
    end
    p = p + 1
  end
  return c
end
function to_tree(f,deep,prec)
  local R = {}
  local l = ""
  if deep == nil then deep = 0 end
  if prec == nil then
    while l ~= nil and string.find(l, "* ") == nil do
      l = f:read()
    end
  else
    l = prec
  end
  if l == nil then return {} end
  local nspaces = string.find(l, "* ") - 1
  while nspaces >= deep*4 do
    if nspaces == deep*4 then -- same level
      table.insert(R, {value=string.sub(l,nspaces+2)})
      l = f:read()
    else -- nsapces > deep*4 -- descend deeper
      local next, children = to_tree(f,deep+1,l)
      R[#R]["children"] = children
      l = next
    end
    if l == nil then return l, R end
    nspaces = string.find(l, "* ") - 1
  end
  return l, R
end

function select(T, info, err)
  for i,o in pairs(T) do
    local f = string.find(o["value"], "-")
    if f then
      print(string.sub(o["value"],0,f))
    else
      print(o["value"])
    end
  end
  local idx = -1
  local val = ""
  repeat
    io.write(info)
    val = io.read()
    for i,o in pairs(T) do
      if string.find(o["value"], val) then
        idx = i
        val = o["value"]
        break
      end
    end
    if idx < 0 then print(err, val) end
  until idx > 0
  return idx, val
end
function yesno(info, default_)
  repeat
    io.write(info)
    local test = io.read()
    if test == nil then return default_ end
    if string.len(test) == 0 then return default_ end
    if test:lower() == 'y' then return true end
    if test:lower() == 'n' then return false end
  until false
end

function extract_addrs(s)
  local first_start = string.find(s, "http")
  local first_end = string.find(s, ")", first_start)
  local second_start = string.find(s, "http", first_end)
  local second_end = string.find(s, ")", second_start)
  return string.sub(s, first_start, first_end-1),
         string.sub(s, second_start, second_end-1)
end

load_os()
SETTINGS = "assets/settings.ini"
MODELS_PATH = "tts"
PIPER_VERSION = "2023.11.14-2"
TEMP_FOLDERS = { "piper" }
if not isdir(MODELS_PATH) then
  shell("mkdir -p " .. MODELS_PATH)
end
if not isdir("licenses") then
  shell("mkdir licenses")
end

if yesno("Install new voice ? (Y/n):", true) then
  if TARGET == "LINUX" then
    shell("wget https://raw.githubusercontent.com/rhasspy/piper/refs/heads/master/VOICES.md --output-document /tmp/piper.md")
    local MD = io.open("/tmp/piper.md", "r")
    local _, T = to_tree(MD,0)
    MD:close()
    print("\033[0;33mNOTE\033[0m: Check out https://rhasspy.github.io/piper-samples/ to listen to samples !")

    print("LANGUAGE SELECTION:")
    local ilang, lang = select(
      T,
      "Select language (english name, identifier or full name): ",
      "Unknown language:"  
    )
    print("VOICE SELECTION:")
    local ivoice, voice = select(
      T[ilang]["children"],
      "Select voice (name or part of name): ",
      "Unknown voice:"
    )
    print("QUALITY SELECTION:")
    local iqual, qual = select(
      T[ilang]["children"][ivoice]["children"],
      "Select quality (name or part of name): ",
      "Unknown quality:"
    )
    print("Selected package:"..lang..' '..voice..' '..qual)
    local res = T[ilang]["children"][ivoice]["children"][iqual]["value"]
    local filename =
      MODELS_PATH .. '/' .. ilang .. "-" .. ivoice .. "-" .. iqual
    local onnx, jason = extract_addrs(res)
    print(onnx)
    print(jason)
    shell("wget " .. onnx .. " --output-document " .. filename .. ".onnx")
    shell("wget " .. jason .. " --output-document " .. filename .. ".onnx.json")
    TTS_PARAMS = filename..".onnx"
  elseif TARGET == "WIN" then
    todo()
  end
else
  TTS_PARAMS = ""
end

if yesno("Install (or reinstall) piper binary ? (y/N):", false) then
  if TARGET == "LINUX" then
    print("For linux there are 3 binaries depending on your architecture:")
    OPTIONS = { {value="x86_64"}, {value="armv7l"}, {value="aarch64"} }
    local _, arch = select(OPTIONS, "Select one (name or part of it): ", "Unknown architecture: ")
    shell("wget https://github.com/rhasspy/piper/releases/download/"..PIPER_VERSION.."/piper_linux_"..arch..".tar.gz")
    -- shell("wget https://")
    shell("tar -xzvf piper_linux_"..arch..".tar.gz")
    mv("piper", "tts/")

    shell("wget https://raw.githubusercontent.com/rhasspy/piper/refs/heads/master/LICENSE.md")
    mv("LICENSE.md", "licenses/piper.txt")
    rm("piper_linux_"..arch..".tar.gz")
  elseif TARGET == "WIN" then
    todo()
  end
end

print("Changing config to use piper and use downloaded voice.")
local f = io.open(SETTINGS, "r")
local new_config = ""
for l in f:lines() do
  if l:startswith("mode=") then
    new_config = new_config .. "mode=piper\n"
  elseif l:startswith("param=") then
    new_config = new_config .. "param="..TTS_PARAMS.."\n"
  else
    new_config = new_config .. l .. "\n"
  end
end
f:close()
f = io.open(SETTINGS, "w")
f:write(new_config)
f:close()
print("Done.")

