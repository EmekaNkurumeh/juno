local demo = {
 start    = { next =  "vortex" },
 vortex   = { next = "sphere" },
 sphere   = { next = "drippy" },

 drippy   = { next = "rad_bot" },
 rad_bot  = { next = "ripple" },
 ripple   = { next = "smoke" },

 smoke    = { next = "wave" },
 wave     = { next = "wheel" },
 wheel    = { next = "carpet" },

 carpet   = { next = "torus" },
 torus    = { next = "whorl" },
 whorl    = { next = "start" },
}

local sz = 0
for _, v in pairs(demo) do
  v.__index = v
  sz = sz + 1
end
demo.len = sz

function demo.start.init()
  self.t = 0
end

function demo.start.update()

end


return demo
