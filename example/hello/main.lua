local screen

function sol.onLoad()
  sol.debug.setVisible(true)
  sol.Shader.fromFile("main.frag")
  local w, h = sol.graphics.getSize()
  screen = sol.Buffer.fromFile("hello.png")
end

function sol.onUpdate(dt)

end

function sol.onDraw()
  sol.graphics.copyPixels(screen, 0, 0, nil, nil, 4)
end

function sol.onKeyDown(k)
	if k == "escape" then
		sol.system.quit()
	end
end
