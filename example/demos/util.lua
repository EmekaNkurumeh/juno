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

  local txt = font:render(txt)
  for i = 1, #nstroke do
    local x = (x - 2) + (1 + nstroke[i][1]) * 2
    local y = (y - 2) + (1 + nstroke[i][2]) * 2
    if nstroke[i][3] > 1 then

    else

    end
    G.framebuffer:draw(txt, x, y)
  end
  G.framebuffer:setColor(unpack(Color["black"]))
  G.framebuffer:draw(txt, x, y)

  G.framebuffer:reset()
end

return util
