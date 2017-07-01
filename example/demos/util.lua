local Color = require "color"

local util = {}

local stroke = {
  { -1, -1 }, { -1,  0 }, { -1,  1 },
  { -0, -1 },             { -0,  1 },
  {  1, -1 }, {  1,  0 }, {  1,  1 },
}

local nstroke = {
  { -1,  1, 1 },
  {  1,  1, 1 },
  {  0,  2, 1 },
  { -1,  0, 0 },
  {  1,  0, 0 },
  {  0, -1, 0 },
  {  0,  1, 0 },
}

function util.print(font, txt, x, y, c1, c2)
  G.framebuffer:reset()
  local s = 2
  local txt = font:render(txt)
  for i = 1, #nstroke do
    local x = (x - s) + (1 + nstroke[i][1]) * s
    local y = (y - s) + (1 + nstroke[i][2]) * s
    if nstroke[i][3] == 1 then
      G.framebuffer:setColor(unpack(Color[c2]))
    else
      G.framebuffer:setColor(unpack(Color[c1]))
    end
    G.framebuffer:draw(txt, x, y)
  end
  G.framebuffer:setColor(unpack(Color["black"]))
  G.framebuffer:draw(txt, x, y)

  G.framebuffer:reset()
end

function util.draw_stroke(font, str, x, y, c)
  G.framebuffer:reset()
  G.framebuffer:setColor(unpack(Color[c]))
  for k, v in ipairs(stroke) do
    G.framebuffer:drawText(font, str, x + v[1], y + v[2])
  end
  G.framebuffer:setColor(unpack(Color["black"]))
  G.framebuffer:drawText(font, str, x, y, w)
  G.framebuffer:reset()
end

return util
