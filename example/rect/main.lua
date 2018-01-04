local alpha = 0

function sol.onLoad()
	--sol.graphics.floodFill(0, 0, 1, 0, 1)
end


function sol.onUpdate(dt)
	alpha = (alpha + dt / 2) % 1
end


function sol.onDraw()
	sol.graphics.setAlpha(alpha)
	sol.graphics.drawRect(0, 0, 255, 255, 1, 1, 1)
end