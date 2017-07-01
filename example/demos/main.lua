local Color = require "color"
local util = require "util"

function sol.onLoad()
	math.randomseed(sol.time.getNow())
  G.framebuffer = sol.Buffer.fromBlank(G.width, G.height)
  G.postbuffer = G.framebuffer:clone()
  G.font = sol.Font.fromEmbedded(16)
  G.bgcolor = Color["peach"]
end

function sol.onUpdate(dt)
  require("stalker").update()
  util.draw_stroke(G.font, "Galaxy", 2, 2, "orange")
  util.print(G.font, "Galaxy", 4 + G.font:getWidth("Galaxy"), 2, "orange", "brown")


  collectgarbage()
  collectgarbage()
end

function sol.onDraw()
  G.postbuffer = G.framebuffer:clone()
  G.framebuffer:clear(unpack(G.bgcolor))
  G.framebuffer:reset()
  sol.graphics.copyPixels(G.postbuffer, 0, 0, nil, G.scale, G.scale)
end

function sol.onKeyDown(k)
	if k == "escape" then
		sol.system.quit()
	end
end
