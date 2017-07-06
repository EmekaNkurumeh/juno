--
-- Copyright (c) 2015 rxi
--
-- This library is free software; you can redistribute it and/or modify it
-- under the terms of the MIT license. See LICENSE for details.
--


-- Override sol.graphics.init function
local init = sol.graphics.init

sol.graphics.init = function(...)
  -- Do init
  local screen = init(...)
  sol.graphics.screen = screen
  -- Bind the screen buffer's methods to the graphics module
  for k, v in pairs(sol.Buffer) do
    if not sol.graphics[k] then
      sol.graphics[k] = function(...)
        return v(screen, ...)
      end
    end
  end
  -- Unbind Buffer constructors (which make no sense bound)
  sol.graphics.fromBlank  = nil
  sol.graphics.fromFile   = nil
  sol.graphics.fromString = nil
  -- Override sol.graphics.clear() to use _clearColor if available
  local clear = sol.graphics.clear
  function sol.graphics.clear(r, g, b, a)
    local c = sol.graphics._clearColor
    r = r or (c and c[1])
    g = g or (c and c[2])
    b = b or (c and c[3])
    clear(r, g, b, 1)
  end
  -- Return main screen buffer
  return screen
end


function sol.graphics.setClearColor(...)
  sol.graphics._clearColor = { ... }
end

function sol.graphics.getClearColor(...)
  return unpack(sol.graphics._clearColor)
end


function sol.graphics._onEvent(e)
  if e.type == "resize" then
    sol.graphics.setSize(e.width, e.height)
  end
end
