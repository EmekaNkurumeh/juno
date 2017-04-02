--
-- Copyright (c) 2015 rxi
--
-- This library is free software; you can redistribute it and/or modify it
-- under the terms of the MIT license. See LICENSE for details.
--

-------------------------------------------------------------------------------
-- Functions missing from standard library
-------------------------------------------------------------------------------

local function call(fn, ...)
  if fn then return fn(...) end
end

local function merge(...)
  local res = {}
  for i = 1, select("#", ...) do
    local t = select(i, ...)
    if t then
      for k, v in pairs(t) do
        res[k] = v
      end
    end
  end
  return res
end

local function push(arr, ...)
  for i = 1, select("#", ...) do
    table.insert(arr, (select(i, ...)))
  end
end


local doneOnError = false
local exit = os.exit
local traceback = debug.traceback

local function onError(msg)
  if not doneOnError then
    doneOnError = true
    sol.onError(msg, traceback())
  else
    print("\n" .. msg .. "\n" .. traceback())
    exit(1)
  end
end

-------------------------------------------------------------------------------
-- Init callbacks
-------------------------------------------------------------------------------

local eventHandlers = {
  keydown = function(e)
    call(sol.debug._onEvent, e)
    call(sol.keyboard._onEvent, e)
    call(sol.onKeyDown, e.key, e.char)
    end,
  keyup = function(e)
    call(sol.keyboard._onEvent, e)
    call(sol.onKeyUp, e.key)
    end,
  mousemove = function(e)
    call(sol.mouse._onEvent, e)
    call(sol.onMouseMove, e.x, e.y)
    end,
  mousebuttondown = function(e)
    call(sol.mouse._onEvent, e)
    call(sol.onMouseDown, e.x, e.y, e.button)
    end,
  mousebuttonup = function(e)
    call(sol.mouse._onEvent, e)
    call(sol.onMouseUp, e.x, e.y, e.button)
    end,
  resize = function(e)
    call(sol.graphics._onEvent, e)
    end,
  quit = function(e)
    sol.system.quit()
    end,
}

local function onStepMain()
  for i, e in ipairs(sol.system.poll()) do
    call(eventHandlers[e.type], e)
  end
  call(sol.time.step)
  call(sol.onUpdate, call(sol.time.getDelta))
  call(sol.graphics.clear)
  call(sol.onDraw)
  call(sol.debug._draw)
  call(sol.keyboard.reset)
  call(sol.mouse.reset)
end

function sol._onStep()
  xpcall(onStepMain, onError)
end

function sol._onAudio(...)
  local res
  local args = { ... }
  xpcall(function()
    res = call(sol.onAudio, unpack(args))
  end, onError)
  return res
end

local pcallFunc
local pcallArgs = {}
local pcallWrap = function()
  return pcallFunc(unpack(pcallArgs))
end

function sol._pcall(fn, ...)
  pcallFunc = fn
  -- Fill argument table with new arguments, discard old args
  local n = select("#", ...)
  for i = 1, n do
    pcallArgs[i] = select(i, ...)
  end
  if #pcallArgs > n then
    for i = n + 1, #pcallArgs do
      pcallArgs[i] = nil
    end
  end
  -- Do call
  return xpcall(pcallWrap, onError)
end

function sol.onError(msg, stacktrace)
  -- Create and print error string
  local tab = "    "
  local str =
    msg:gsub("\t", tab):gsub("\n+$", "") .. "\n\n" ..
    stacktrace:gsub("\t", tab)
  print("Error:\n" .. str)
  -- Override event handlers
  eventHandlers = {
    quit = function() os.exit() end,
    keydown = function(e) if e.key == "escape" then os.exit() end end,
  }
  -- Disable debug
  call(sol.debug.setVisible, false)
  call(sol.mouse.setVisible, true)

  --  Init error state
  local font, bigfont
  local done = false
  local alpha = 0

  function sol.onUpdate()
    -- The initialisation of the error state's graphics is defered to the
    -- first onUpdate() call in case the error occurs in the audio thread in
    -- which case it won't be able to change the openGL state
    sol.graphics.reset()
    sol.graphics.setClearColor(.15, .16, .2)
    font = sol.Font.fromEmbedded(14)
    bigfont = sol.Font.fromEmbedded(40)
    -- Init update function
    function sol.onUpdate(dt)
      if alpha == 1 then
        done = true
      else
        alpha = math.min(alpha + dt / .5, 1)
      end
    end
  end

  function sol.onAudio() end

  function sol.onDraw()
    sol.graphics.setAlpha(alpha)
    sol.graphics.drawText(bigfont, "Error", 40, 40)
    sol.graphics.drawText(font, str, 40, 120)
    -- As this screen won't change after its faded in we can sleep here to
    -- avoid having to redraw too often -- this will reduce CPU usage
    if done then
      sol.time.sleep(.1)
    end
  end
end


-------------------------------------------------------------------------------
-- Init filesystem
-------------------------------------------------------------------------------

-- Mount project paths
if sol._argv[2] then
  -- Try to mount all arguments as package
  for i = 2, #sol._argv do
    sol.fs.mount(sol._argv[i])
  end
else
  -- Try to mount default packages (pak0, pak1, etc.)
  local dirs = { sol.system.info("exedir") }
  if sol.system.info("os") == "osx" then
    table.insert(dirs, sol.system.info("exedir") .. "/../Resources")
  end
  for _, dir in ipairs(dirs) do
    local idx = 0
    while sol.fs.mount(dir .. "/pak" .. idx) do
      idx = idx + 1
    end
    if idx ~= 0 then break end
  end
end

-- Add filesystem-compatible package loader
table.insert(package.loaders, 1, function(modname)
  modname = modname:gsub("%.", "/")
  for x in package.path:gmatch("[^;]+") do
    local filename = x:gsub("?", modname)
    if sol.fs.exists(filename) then
      return assert(loadstring(sol.fs.read(filename), "=" .. filename))
    end
  end
end)

-- Add extra package paths
package.path = package.path .. ";?/init.lua"



-------------------------------------------------------------------------------
-- Init config
-------------------------------------------------------------------------------

local c = {}
if sol.fs.exists("config.lua") then
  c = call(require, "config")
end

local config = merge({
  title       = "Sol " .. sol.getVersion(),
  width       = 500,
  height      = 500,
  maxfps      = 60,
  samplerate  = 44100,
  buffersize  = 2048,
  fullscreen  = false,
  resizable   = false,
  borderless  = false,
}, c)

if not config.identity then
  config.identity = config.title:gsub("[^%w]", ""):lower()
end


-------------------------------------------------------------------------------
-- Init filesystem write path
-------------------------------------------------------------------------------

local appdata = sol.system.info("appdata")
local path = appdata .. "/sol/" .. config.identity

sol.fs.setWritePath(path)
sol.fs.mount(path)


-------------------------------------------------------------------------------
-- Init modules
-------------------------------------------------------------------------------

sol.graphics.init(config.width, config.height, config.title,
                   config.fullscreen, config.resizable, config.borderless)
sol.graphics.setMaxFps(config.maxfps)
sol.graphics.setClearColor(0, 0, 0)
sol.audio.init(config.samplerate, config.buffersize)


-------------------------------------------------------------------------------
-- Init project
-------------------------------------------------------------------------------

if sol.fs.exists("main.lua") then
  -- Load project file
  xpcall(function() require "main" end, onError)
else
  -- No project file -- init "no project loaded" screen
  local w, h = sol.graphics.getSize()
  local txt = sol.Font.fromEmbedded(16):render("No project loaded")
  local txtPost = txt:clone()
  local txtMask = txt:clone()
  local particles = {}

  function sol.onLoad()
    sol.graphics.setClearColor(0.15, 0.15, 0.15)
    for i = 1, 30 do
      local p = {
        x = 0,
        y = (i / 30) * 100,
        z = 0,
        r = (i / 30) * 2,
      }
      table.insert(particles, p)
    end
  end

  function sol.onUpdate(dt)
    local n = sol.time.getTime()
    for _, p in ipairs(particles) do
      p.x = math.cos(n * p.r) * 100
      p.z = math.sin(n * p.r)
    end
  end

  function sol.onKeyDown(k)
    if k == "escape" then
      os.exit()
    end
  end

  function sol.onDraw()
    -- Draw particles
    sol.graphics.setBlend("add")
    local lastx, lasty
    for _, p in ipairs(particles) do
      local x, y = (p.x * p.z) + w / 2, (p.y * p.z) + w / 2
      sol.graphics.setAlpha(p.a)
      sol.graphics.drawPixel(x, y)
      if lastx then
        sol.graphics.setAlpha(.3)
        sol.graphics.drawLine(x, y, lastx, lasty)
      end
      lastx, lasty = x, y
    end
    -- Draw text
    local n = sol.time.getTime() * 2
    local x = (1 + math.sin(n)) * txtMask:getWidth() / 2
    txtPost:copyPixels(txt)
    txtMask:clear(1, 1, 1, .5)
    txtMask:drawRect(x - 10, 0, 20, 100, 1, 1, 1, .6)
    txtMask:drawRect(x -  5, 0, 10, 100, 1, 1, 1, 1)
    sol.bufferfx.mask(txtPost, txtMask)
    local tx, ty = (h - txt:getWidth()) / 2, (h - txt:getHeight()) / 2
    sol.graphics.reset()
    sol.graphics.draw(txtPost, tx, ty + 130)
  end

end

xpcall(function() call(sol.onLoad) end, onError)
