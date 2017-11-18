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


local defautShader

function sol.graphics.setDefaultShader(shader)
  defaultShader = shader
end


-- Overide sol.graphics.setShader function
local setShader, currentShader = sol.graphics.setShader

sol.graphics.setShader = function(shader)
  if shader ~= currentShader and shader then
    currentShader = shader
    setShader(currentShader)
    print("using new shader", shader:getWarnings())
  elseif shader == nil and defaultShader then
    setShader(defaultShader)
    print("using default shader", defaultShader:getWarnings())
  else
    shader = sol.Shader.fromString [[
    #version 120
    uniform sampler2D tex;
    void main() {
      //gl_FragColor = texture2D(tex, gl_TexCoord[0].xy);
      gl_FragColor = vec4(1, 1, 1, 1);
    }
    ]]
    sol.graphics.setDefaultShader(shader)
    setShader(shader)
    print("setting default shader", shader:getWarnings())
  end
end


function sol.graphics.getShader()
  return currentShader
end


function sol.graphics.withShader(func, shader)
  local s = currentShader
  sol.graphics.setShader(shader)
  func()
  sol.graphics.setShader(s)
end


function sol.graphics._onEvent(e)
  if e.type == "resize" then
    sol.graphics.setSize(e.width, e.height)
  end
end
