--
-- Copyright (c) 2017 emekankurumeh
--
-- This library is free software; you can redistribute it and/or modify it
-- under the terms of the MIT license. See LICENSE for details.
--

local function call(fn, ...)
  if fn then return fn(...) end
end


function juno.system.quit()
  call(juno.onQuit)
  os.exit()
end
