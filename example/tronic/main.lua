

function sol.onLoad(dt)
  sol.debug.setVisible(true)
  G.field = sol.Buffer.fromBlank(G.width, G.height)
  G.field:drawBox(0, 0, G.width, G.height, 1, 1, 1)
  G.tickTimer = 0
  -- Initialise player
  G.player = {
    x = 20,
    y = G.height / 2,
    direction = "down",
    color = { 1, 1, 1 },
  }
  -- Initialise AIs
  G.ai = {}
  for i = 1, 3 do
    table.insert(G.ai, {
      x = ((G.width - 40) / 3) * i + 20,
      y = G.height / 2,
      dead = false,
      direction = "down",
      turnTimer = 0,
      turnRate = 10 + math.random(20),
      color = ({ { 1, 0, 0 }, { 0, 1, 1 }, { 1, 1, 0 } })[i],
    })
  end

  sol.graphics.setShader(sol.Shader.fromString([[
  #version 120

  uniform sampler2D tex;
  uniform float elapsed;

  vec2 radialDistortion(vec2 coord, float dist) {
    vec2 cc = coord - 0.5;
    float elapsed = 1;
    dist = dot(cc, cc) * dist + cos(elapsed * .3) * .01;
    return (coord + cc * (1.0 + dist) * dist);
  }


  vec4 effect(sampler2D tex, vec2 tc) {
    vec2 tcr = radialDistortion(tc, .24)  + vec2(.001, 0);
    vec2 tcg = radialDistortion(tc, .20);
    vec2 tcb = radialDistortion(tc, .18) - vec2(.001, 0);
    vec4 res = vec4(texture2D(tex, tcr).r, texture2D(tex, tcg).g, texture2D(tex, tcb).b, 1)
      - cos(tcg.y * 128. * 3.142 * 2) * .03
      - sin(tcg.x * 128. * 3.142 * 2) * .03;
    return res * texture2D(tex, tcg).a;
  }


  void main() {
    gl_FragColor = effect(tex, gl_TexCoord[0].xy);
  }
]]))
end


function sol.onKeyDown(k,e)
  -- Handle player movement keys
  if k == "left" or k == "up" or k == "down" or k == "right" then
    G.player.direction = k
  end
  -- Handle game restart key
  if k == "r" then
    sol.onLoad()
  end
end


local function isPixelBlack(x, y)
  local r, g, b = G.field:getPixel(x, y)
  return r == 0 and g == 0 and b == 0
end


local function nextPosition(bike, steps)
  steps = steps or 1
  return ({
    left  = function() return bike.x - steps, bike.y end,
    right = function() return bike.x + steps, bike.y end,
    up    = function() return bike.x, bike.y - steps end,
    down  = function() return bike.x, bike.y + steps end,
  })[bike.direction]()
end


local function randomDirection()
  return ({ "left", "right", "up", "down" })[math.random(4)]
end


local function updateAi(ai, dt)
  -- Do random turn timer and random turn
  ai.turnTimer = ai.turnTimer - 1
  if ai.turnTimer <= 0 then
    ai.turnTimer = math.random(ai.turnRate)
    ai.direction = randomDirection()
  end
  -- Do obstacle avoidance
  for lookahead = 4, 1, -1 do
    for i = 1, 8 do
      if not isPixelBlack(nextPosition(ai, lookahead)) then
        ai.direction = randomDirection()
      end
    end
  end
end


local function updateBike(bike)
  -- Don't update the bike if its dead
  if bike.dead then
    return
  end
  -- Move bike
  bike.x, bike.y = nextPosition(bike)
  -- Kill the bike if it collided with something
  if not isPixelBlack(bike.x, bike.y) then
    bike.dead = true
    return
  end
  -- Draw bike
  G.field:setPixel(bike.x, bike.y, unpack(bike.color))
end


local function onTick()
  -- Update AIs
  for i, ai in ipairs(G.ai) do
    updateAi(ai)
    updateBike(ai)
  end
  -- Update player
  updateBike(G.player)
end


function sol.onUpdate(dt)
  -- Update tick timer
  G.tickTimer = G.tickTimer - dt
  while G.tickTimer <= 0 do
    onTick()
    G.tickTimer = G.tickTimer + .03
  end
  -- Player is dead? Restart the game
  if G.player.dead then
    sol.onLoad()
  end
end


function sol.onDraw()
  local sx, sy = math.floor(sol.graphics.getWidth() / G.width), math.floor(sol.graphics.getHeight() / G.height)
  print(sx, sy, sol.graphics.getWidth(), sol.graphics.getHeight())
  sol.graphics.draw(G.field, 0, 0, nil, nil, sx, sy)
end
