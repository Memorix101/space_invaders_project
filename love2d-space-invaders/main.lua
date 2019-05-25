-- https://github.com/Memorix101/space_invaders_project/
-- Made by Memorix101

score = 0
speed = 300
player_pos = {} -- x = 0, y = 0
player_pos.x = 640/2 - 28/2
player_pos.y = 440
bullets = {} --tex = nil, x = 0, y = 0
enemy_bullet = {} -- tex = nil, x = 0, y = 0
enemies = {} -- tex = nil, x = 0, y = 0, startPos = 0, goLeft = false, shootTimer = 0, shootTimerLimit = 0, alive = true
explosions = {} -- tex = nil, x = 0, y = 0, currentAnim = 0.0, quad = nil, animationFinished = false

player_alive = true
key_firePressed = false
game_start = false
start_timer = 0.0
itemCount = 0
rowCount = 0
currentAnim = 0
gameover = false

-- Clamps a number to within a certain range, with optional rounding
function math.clamp(val, lower, upper)
  assert(val and lower and upper, "not very useful error message here")
  if lower > upper then lower, upper = upper, lower end -- swap if boundaries supplied the wrong way
  return math.max(lower, math.min(upper, val))
end

function love.conf(t)
  t.version = "11.2"
end

function init()
  love.window.setMode(640, 480, {resizable=false, vsync=false, minwidth=640, minheight=480})
  love.window.setTitle("Space Invaders for LÖVE")
  local imagedata = love.image.newImageData('rd/icon.png')
  love.window.setIcon(imagedata)
end

function load_assets()
  vermin_ttf = love.graphics.newFont("rd/vermin_vibes_1989.ttf", 28)
  space3_tex = love.graphics.newImage("rd/space3.png")
  player_tex = love.graphics.newImage("rd/player.png")
  bullet_tex = love.graphics.newImage("rd/bullet.png")
  enemy_bullet_tex = love.graphics.newImage("rd/enemy-bullet.png")
  gameover_tex = love.graphics.newImage("rd/gameover_ui.png")
  win_tex = love.graphics.newImage("rd/win_ui.png")
  fmg_splash_tex = love.graphics.newImage("rd/fmg_splash.png")
  enemy_tex = love.graphics.newImage("rd/invader32x32x4.png")
  explo_tex = love.graphics.newImage("rd/explode.png")
  enemy_quad = love.graphics.newQuad(0, 0, 32, 32, enemy_tex:getWidth(), enemy_tex:getHeight())
  music = love.audio.newSource("rd/bodenstaendig.ogg", "stream")
  snd_blaster = love.audio.newSource("rd/blaster.ogg", "stream")
  snd_pusher = love.audio.newSource("rd/pusher.ogg", "stream")
  snd_explo = love.audio.newSource("rd/explode1.ogg", "stream")
end

function init_enemies()
  local i = 0
  repeat
    if math.fmod(i, 10) == 0 then
      itemCount = 0;
      rowCount = rowCount + 1
    end
    itemCount = itemCount + 1
    local offset = 40
    table.insert(enemies, {
    tex = enemy_tex, 
    x = itemCount * 40, 
    y = offset + 40 * rowCount, 
    startPos = itemCount * 40, 
    goLeft = false, shootTimer = 0, 
    shootTimerLimit = math.random(3, 30),
    alive = true })
    i = i + 1
  until(i == 40)
end

function love.load()
  init() -- Setup everything
  load_assets() -- Load assets
  init_enemies()

  music:setLooping(true)
  music:play()
end

function enemy_update(dt)
  for i in ipairs(enemies)
  do
    if enemies[i].x >= enemies[i].startPos + 200 then
      enemies[i].goLeft = true 
    end

    if enemies[i].x <= enemies[i].startPos - 40 then
      enemies[i].goLeft = false
    end

    if enemies[i].goLeft == false then
      enemies[i].x = enemies[i].x + (150 * dt)
    else
      enemies[i].x = enemies[i].x - (150 * dt)
    end

    enemies[i].shootTimer = enemies[i].shootTimer + dt

    if enemies[i].shootTimer >= enemies[i].shootTimerLimit then
      enemies[i].shootTimer = 0
      snd_pusher:stop()
      snd_pusher:play()
      table.insert(enemy_bullet, {tex = enemy_bullet_tex, x = enemies[i].x - 4, y = enemies[i].y + 4})
    end

  end

  currentAnim = currentAnim + (15 * dt)

  if currentAnim >= 4 then
    currentAnim = 0
  end

  enemy_quad:setViewport(32 * math.floor(currentAnim), 0, 32, 32)

end

function enemy_bullet_update(dt)
  for i in ipairs(enemy_bullet)
  do
    enemy_bullet[i].y = enemy_bullet[i].y + (speed * dt)
    if enemy_bullet[i].y > 480 then
      table.remove(enemy_bullet, i)
    end
  end
end

function explo_update(dt)
  --for i = #explosions, 1, -1 
  for i in ipairs(explosions)
  do
    explosions[i].currentAnim = explosions[i].currentAnim + (15 * dt)

    if explosions[i].currentAnim >= 16 then
      explosions[i].animationFinished = true
    end

    explosions[i].quad:setViewport(128 * math.floor(explosions[i].currentAnim), 0, 128, 128)

    if explosions[i].animationFinished == true then
      table.remove(explosions, i)
    end

  end
end

function bullet_update(dt)
  for i in ipairs(bullets)
  do
    bullets[i].y = bullets[i].y - (speed * dt)
    if bullets[i].y < 12 then
      table.remove(bullets, i)
    end
  end
end

function collision_stuff()
  -- enemy_bullet x player
  for b = #enemy_bullet, 1, -1 
  do 
    if enemy_bullet[b].x > player_pos.x and enemy_bullet[b].x < player_pos.x + player_tex:getWidth() and 
    enemy_bullet[b].y > player_pos.y and enemy_bullet[b].y < player_pos.y + player_tex:getHeight() and 
    player_alive == true 
    then        
			player_alive = false
      table.insert(explosions, {
        tex = explo_tex, 
        x = player_pos.x - 128 / 2, 
        y = player_pos.y - 128 / 2, 
        currentAnim = 0.0, 
        quad = love.graphics.newQuad(0, 0, 128, 128, explo_tex:getWidth(), explo_tex:getHeight()),
         animationFinished = false
        })
			table.remove(enemy_bullet, b)
			snd_explo:stop()
			snd_explo:play()
			break
    end
  end
  
  -- player_bullet x enemies
  for i = #bullets, 1, -1 
  do 
    for e = #enemies, 1, -1 
      do
        if bullets[i].x > enemies[e].x and bullets[i].x < enemies[e].x + 32 and
          bullets[i].y > enemies[e].y and bullets[i].y < enemies[e].y + 32 and
          enemies[e].alive == true 
          then
          table.insert(explosions, {
            tex = explo_tex, 
            x = bullets[i].x - 128 / 2, 
            y = bullets[i].y - 128 / 2, 
            currentAnim = 0.0, 
            quad = love.graphics.newQuad(0, 0, 128, 128, explo_tex:getWidth(), explo_tex:getHeight()), 
            animationFinished = false
          })          
            table.remove(bullets, i)
          table.remove(enemies, e)
          snd_explo:stop()
          snd_explo:play()
				  score = score + 100;
				  break
      end
		end
	end
end

function restart()
	gameover = false
	score = 0
	for i in pairs (enemies) do
    enemies[i] = nil
  end
	currentAnim = 0.0;
  for i in pairs (bullets) do
    bullets[i] = nil
  end
  for i in pairs (enemy_bullet) do
    enemy_bullet[i] = nil
  end
  for i in pairs (explosions) do
    explosions[i] = nil
  end
  rowCount = 0
	itemCount = 0
	init_enemies()
	player_pos.x = 640 / 2 - player_tex:getWidth() / 2
	player_alive = true
end

function love.update(dt)

  start_timer = start_timer + dt

  if start_timer >= 3 then
    game_start = true
  end

  if love.keyboard.isDown("space") then
    if key_firePressed == false then
      key_firePressed = true
      snd_blaster:stop()
      snd_blaster:play()
      table.insert(bullets, {tex = bullet_tex, x = player_pos.x + 10, y = player_pos.y - 5})
    end
  else
    key_firePressed = false
  end

  if love.keyboard.isScancodeDown("right") or love.keyboard.isScancodeDown("d") then
    player_pos.x = player_pos.x + (speed * dt)     
  end

  if love.keyboard.isScancodeDown("escape") then
    love.event.quit()
  end

  if love.keyboard.isScancodeDown("return") and gameover == true then
    restart()
  end

  if love.keyboard.isScancodeDown("left") or love.keyboard.isScancodeDown("a") then
    player_pos.x = player_pos.x - (speed * dt)  
  end

  enemy_update(dt)
  enemy_bullet_update(dt)
  bullet_update(dt)
  explo_update(dt)
  collision_stuff()

  player_pos.x = math.clamp(player_pos.x, 0, 640 - 32)

end

function love.draw()
  
  if game_start == true then
    love.graphics.draw(space3_tex, 0, 0)

    for i in ipairs(bullets)
    do
      love.graphics.draw(bullets[i].tex, bullets[i].x, bullets[i].y)
    end

    for i in ipairs(enemy_bullet)
    do
      love.graphics.draw(enemy_bullet[i].tex, enemy_bullet[i].x, enemy_bullet[i].y)
    end

    for i in ipairs(explosions)
    do
    -- print("explo: " .. tostring(explosions[i].x).." - ".. tostring(explosions[i].y)) 
    -- print("explo: " ..tostring(explosions[i]) .. " " .. tostring(math.floor(explosions[i].currentAnim))) 
    -- love.graphics.newQuad(128*math.floor(explosions[i].currentAnim), 0, 128, 128, explo_tex:getWidth(), explo_tex:getHeight())
    love.graphics.draw(explosions[i].tex, explosions[i].quad, explosions[i].x, explosions[i].y)
    end

    for i in ipairs(enemies)
    do
      love.graphics.draw(enemies[i].tex, enemy_quad, enemies[i].x, enemies[i].y)
    end

    if player_alive == true then
      love.graphics.draw(player_tex, player_pos.x, player_pos.y)
    else
      gameover = true
      love.graphics.draw(gameover_tex, 0, 0)
    end

    if #enemies <= 0 then
      gameover = true
      love.graphics.draw(win_tex, 0, 0)
    end

    -- Setting the font so that it is used when drawing the string.
	  love.graphics.setFont(vermin_ttf)
 
    local ttf_w = vermin_ttf:getWidth("SCORE: " .. string.format("%04d", score))
    love.graphics.print("SCORE: " .. string.format("%04d", score), 480 - ttf_w*0.1, 25)
    -- love.graphics.print("Enemies " .. tostring(#enemies), 0, 0)
  else
    love.graphics.draw(fmg_splash_tex, 0, 0)
  end
  
end