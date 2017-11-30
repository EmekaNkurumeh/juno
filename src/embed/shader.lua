--
-- Copyright (c) 2017 emekoi
--
-- This library is free software; you can redistribute it and/or modify it
-- under the terms of the MIT license. See LICENSE for details.
--

local init = false

function sol.Shader.init()
  if init == true then error("shader alreadty inited") end
  local shader = sol.Shader.fromString [[
  #version 120
  uniform sampler2D tex;
  void main() {
    gl_FragColor = texture2D(tex, gl_TexCoord[0].xy);
  }
  ]]
  sol.Shader.current = shader
  sol.Shader.default = shader
  sol.graphics.setShader(shader)
  init = true
end


function sol.graphics.setDefaultShader(shader)
  sol.Shader.default = shader
end


-- Overide sol.graphics.setShader function
local setShader = sol.graphics.setShader

sol.graphics.setShader = function(shader)
  if sol.Shader.default and not shader then
    setShader(sol.Shader.default)
  elseif sol.Shader.current and shader then
    sol.Shader.current = shader
    setShader(sol.Shader.current)
  end
end


function sol.graphics.getShader()
  return sol.Shader.current
end


function sol.graphics.withShader(func, shader)
  local s = sol.Shader.current
  -- sol.graphics.setShader(shader)
  func()
  -- sol.graphics.setShader(s)
end
