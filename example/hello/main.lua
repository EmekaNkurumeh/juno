local screen, shader

function sol.onLoad()
  sol.debug.setVisible(true)
  shader = sol.Shader.fromFile("main.frag")
  local w, h = sol.graphics.getSize()
  screen = sol.Buffer.fromFile("hello.png")
  -- sol.graphics.screen = screen
end

function sol.onUpdate(dt)

end

function sol.onDraw()
  -- sol.graphics.copyPixels(screen, 0, 0, nil, nil, 4)
  shader:uniform("mex", screen)
  -- sol.graphics.copyPixels(screen)
end

function sol.onKeyDown(k)
	if k == "escape" then
		sol.system.quit()
	end
end
