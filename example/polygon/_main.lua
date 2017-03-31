--[[
  Let's assume you want to draw an N-sided polygon of radius r, centred at (0,0). Then the n vertices are given by:

  x[n] = r * cos(2*pi*n/N)
  y[n] = r * sin(2*pi*n/N)
  where 0 <= n < N. Note that cos and sin here are working in radians, not degrees (this is pretty common in most programming languages).

  If you want a different centre, then just add the coordinates of the centre point to each (x[n], y[n]). If you want a different orientation, you just need to add a constant angle. So the general form is:

  x[n] = r * cos(2*pi*n/N + theta) + x_centre
  y[n] = r * sin(2*pi*n/N + theta) + y_centre
]]

local w, h = juno.graphics.getSize()
local txt = juno.Font.fromEmbedded(14):render("No project loaded")
local txtPost = txt:clone()
local txtMask = txt:clone()
local particles = {}

function juno.onLoad()
  juno.graphics.setClearColor(0.15, 0.15, 0.15)
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

function juno.onUpdate(dt)
  local n = juno.time.getTime()
  for _, p in ipairs(particles) do
    p.x = math.cos(n * p.r) * 100
    p.z = math.sin(n * p.r)
  end
end

function juno.onKeyDown(k)
  if k == "escape" then
    os.exit()
  end
end

function juno.onDraw()
  -- Draw particles
  juno.graphics.setBlend("add")
  local lastx, lasty
  for _, p in ipairs(particles) do
    local x, y = (p.x * p.z) + w / 2, (p.y * p.z) + w / 2
    juno.graphics.setAlpha(p.a)
    juno.graphics.drawPixel(x, y)
    if lastx then
      juno.graphics.setAlpha(.3)
      juno.graphics.drawLine(x, y, lastx, lasty)
    end
    lastx, lasty = x, y
  end
  -- Draw text
  local n = juno.time.getTime() * 2
  local x = (1 + math.sin(n)) * txtMask:getWidth() / 2
  txtPost:copyPixels(txt)
  txtMask:clear(1, 1, 1, .5)
  txtMask:drawRect(x - 10, 0, 20, 100, 1, 1, 1, .6)
  txtMask:drawRect(x -  5, 0, 10, 100, 1, 1, 1, 1)
  juno.bufferfx.mask(txtPost, txtMask)
  local tx, ty = (h - txt:getWidth()) / 2, (h - txt:getHeight()) / 2
  juno.graphics.reset()
  juno.graphics.draw(txtPost, tx, ty + 130)
