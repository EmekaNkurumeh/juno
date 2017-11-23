--
-- Copyright (c) 2017 emekoi
--
-- This library is free software; you can redistribute it and/or modify it
-- under the terms of the MIT license. See LICENSE for details.
--

function sol.graphics.setDefaultShader(shader)
  sol.Shader.default = shader
end


-- Overide sol.graphics.setShader function
local setShader = sol.graphics.setShader

sol.graphics.setShader = function(shader)
  if shader and shader ~= sol.Shader.current then
    sol.Shader.current = shader
    setShader(sol.Shader.current)
  elseif sol.Shader.default and shader == nil then
    setShader(sol.Shader.default)
  else
    shader = sol.Shader.fromString [[
    #version 120
    uniform sampler2D tex;
    void main() {
      gl_FragColor = texture2D(tex, gl_TexCoord[0].xy);
    }
    ]]
    setShader(shader)
    sol.graphics.setDefaultShader(shader)
  end
end


function sol.graphics.getShader()
  return sol.Shader.current
end


function sol.graphics.withShader(func, shader)
  local s = sol.Shader.current
  sol.graphics.setShader(shader)
  func()
  sol.graphics.setShader(s)
end
