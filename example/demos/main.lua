local Color = require "color"
local util = require "util"
local demo = require "demos"

function sol.onLoad()
	math.randomseed(sol.time.getNow())
  G.framebuffer = sol.Buffer.fromBlank(G.width, G.height)
  G.postbuffer = G.framebuffer:clone()
  G.font = sol.Font.fromEmbedded(16)
  G.bgcolor = Color["peach"]
  G.tick, G.cursor = 0, { demo = "start", pos = 1}
  G.state = "menu"
  G.keys = {
    up     = { "up", "w" },
    down   = { "down", "s" },
    action = { "z", "space", "enter" },
    back   = { "x" },
  }
end

function sol.onUpdate(dt)
  require("stalker").update()
  util.draw_stroke(G.cursor.pos, 0, 0, "pink")
  -- util.print(G.font, "tick: " .. G.tick, 0, 0, "indigo", "dark-purple")

  if sol.keyboard.wasPressed(unpack(G.keys.up)) then
    G.cursor.pos = math.max(G.cursor.pos - 1, 1)
  elseif sol.keyboard.wasPressed(unpack(G.keys.down)) then
    G.cursor.pos = math.min(G.cursor.pos + 1, demo.len)
  end

  G.tick = G.tick + .0045
  collectgarbage()
  collectgarbage()
end

function sol.onDraw()
	local i = 1
  for k, v in pairs(demo) do
		if type(v) == "table" then
			local str = k
			for c = 1, #k do
	  		if k:sub(c, c) == "_" then
	   			str = k:sub(1, c - 1) .. " " .. k:sub(c + 1, #k)
	  		end
	 		end
			-- local str = (G.tick % 0.2 < 0.15 and G.cursor.pos == i) and "> " .. str or str
			local x = G.cursor.pos == i and 24 + G.font:getWidth("_") or 24
			local y = 8 + (G.font:getHeight(str) * i )
			util.draw_stroke(str, x, 8 + (G.font:getHeight(str) * i ), "pink")
			if G.tick % 0.2 < 0.15 and G.cursor.pos == i then
				util.draw_stroke(">", x - G.font:getWidth("_"), 8 + (G.font:getHeight(str) * i ), "pink")
			end
			i = i + 1
		end
  end

  G.postbuffer = G.framebuffer:clone()
  G.framebuffer:clear(unpack(G.bgcolor))
  G.framebuffer:reset()
  sol.graphics.copyPixels(G.postbuffer, 0, 0, nil, G.scale, G.scale)
end

function sol.onKeyDown(k)
	if k == "escape" then
		sol.system.quit()
	elseif k == "r" then
    sol.onLoad()
  end
end
