function branch(len)
  G.field:drawLine(256, G.height, 256, G.height - len)
end

function sol.onLoad()
  G.field = sol.Buffer.fromBlank(G.width, G.height)
  G.field:drawBox(0, 0, G.width, G.height)
  branch(171)
end

function sol.onKeyDown(k,e)
  -- Handle game restart key
  if k == "r" then
    sol.onLoad()
  end
end


function sol.onUpdate(dt)

end

function sol.onDraw()
  sol.graphics.draw(G.field, 0, 0, nil, nil,G.scale)
end
