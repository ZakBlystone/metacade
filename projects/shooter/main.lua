
ENT_PLAYER = 0
ENT_BULLET = 1
ENT_ENEMY = 2
ENT_FX = 3

function initPlayer()

	playerX = 0
	playerY = 0

	playerTopSpeed = 30
	playerSpeedX = 0
	playerSpeedY = 0
	playerAccelRate = 3000
	playerFriction = 6
	playerNextShot = 0

	score = 0

	player = spawnPlayer()

end

function init()

	game.time = 0
	game.speed = 1
	game.targetSpeed = 1

	playing = false
	limitX = 175


	print("BULLET TYPE: " .. ENT_BULLET)

end

function drawScrollBG(x,y,w,h,s,tex)

	_r.quad(0,0,x,y,
		w,0,s+x,0+y,
		w,h,s+x,s+y,
		0,h,0+x,s+y,
		tex)

end

function drawBG(w,h)

	_r.color(0,0,0,1)
	_r.rect(0,0,w,h)

	_r.color(1,1,1,1,1)
	drawScrollBG(0,time*-.01,w,h,.5,_t.clouds)

	_r.color(.2,.2,.2,1,1)
	drawScrollBG(0,time*-.2,w,h,.6,_t.stars)

	_r.color(.4,.4,.4,1,1)
	drawScrollBG(0,time*-.04,w,h,.7,_t.stars)

	if mouseX ~= nil then
		_r.color(1,1,1,1)
		_r.rect(mouseX,mouseY,3,3)
	end

end

function drawHUD(w,h)
	_r.layer(6)

	local hp = math.max(player.health, 0) / 100

	_r.color(1,0,0,1)
	_r.rect(10,10,(w-20) * hp,10)
end

function toCoords(x,y)

	return x + _width/2, y + _height-32

end

function playerCoords()

	return toCoords(playerX, playerY)

end

function draw()

	_width, _height = _r.size()
	local w,h = _r.size()

	local sx = 0
	local sy = 0

	_r.layer(0)
	drawBG(w,h)

	if not playing then 
		return 
	end

	--local px,py = toCoords(player.x, player.y)

	_r.layer(1)
	_r.color(1,1,1,1,0)

	local x,y = playerCoords()
	drawEdicts()

	drawHUD(w,h)
end

function think(time, dt)
	--game.time = time

	if not playing then 
		game.time = game.time + dt
		return 
	end

	if slowmo then
		game.targetSpeed = 0.2
	else
		game.targetSpeed = 1
	end

	game.speed = game.speed + (game.targetSpeed - game.speed) * (1 - math.exp(dt * -4))

	dt = dt * game.speed

	sound.setMasterPitch(game.speed)

	tickEdicts(time, dt)
	game.time = game.time + dt
end

function spawnWave1()
	local delay = 0
	for i = -150, 150, 25 do
		if i ~= 0 then
			local e = spawnEnemy(-i,-320)
			e.tx = i
			e.wait = math.abs(i) / 150 --delay
			delay = delay + .3

			e.formation = function(e)
				e.tx = e.tx * -1

				if math.abs(i) ~= 100 then
					e.ty = e.ty + 100
				end

				if math.abs(i) == 50 then
					e.ty = e.ty + 50
				end
			end

			e.formation2 = function(e)

				if e.tx < 0 then
					e.tx = 150 - i
					e.ty = i*2 - 300
				else
					e.tx = -150 - i
					e.ty = -i*2 - 300
				end

				if e.tx == 0 then
					e.ty = e.ty - 200
					if i == -150 then
						e.tx = e.tx - 50
					else
						e.tx = e.tx + 50
					end
				end

			end

			e.waitformation2 = (1 - math.abs(i) / 150) * 2 + 8

			if math.abs(i) == 100 then
				e.ty = e.ty + 50
			end
		end
	end
end

function startGame()

	if playing then return end

	edicts = {}

	game.time = 0
	game.speed = 1
	game.targetSpeed = 1

	sound.setMasterPitch(1)

	ch_music = sound.play(_s.music)
	sound.setLooping(ch_music, true)
	sound.setVolume(ch_music, .2)

	playing = true

	initPlayer()

	spawnWave1()

end

function stopGame()

	if not playing then return false end

	sound.stop(ch_music)

	playing = false
	return true

end

function onMousePressed(x,y,b,focus)

end

function onMouseReleased(x,y,b,focus)

end

function onMouseMoved(x,y,dx,dy,focus)
	mouseX = x
	mouseY = y
end

function onFocusGained(e)

end

function onFocusLost(e)
	stopGame()
end

function onKeyPressed(k)

	print(_keynames[k] .. " is down")

	if k == _keys.Z then
		if not startGame() then
			k_shoot = true
		end
	end

	if k == _keys.LEFT then k_left = true end
	if k == _keys.RIGHT then k_right = true end
	if k == _keys.UP then k_up = true end
	if k == _keys.DOWN then k_down = true end
	if k == _keys.X then slowmo = true end

end

function onKeyReleased(k)
	print(_keynames[k] .. " is up")

	if k == _keys.LEFT then k_left = false end
	if k == _keys.RIGHT then k_right = false end
	if k == _keys.UP then k_up = false end
	if k == _keys.DOWN then k_down = false end
	if k == _keys.X then slowmo = false end

	if k == _keys.Z then
		k_shoot = false
	end

end

function edict(type)
	local e = { type = type }
	table.insert(edicts, e)
	return e
end

function drawEdicts()
	for _,e in pairs( edicts ) do
		if e.draw and not e.wait then e:draw() end
	end
end

function tickEdicts(time, dt)
	for i=#edicts, 1, -1 do
		if edicts[i].wait then 
			edicts[i].wait = edicts[i].wait - dt
			if edicts[i].wait <= 0 then edicts[i].wait = nil end
		else
			if edicts[i].think then edicts[i]:think(dt) end
			if edicts[i].dead then table.remove(edicts, i) end
		end
	end
end

function edictsByType(t)
	local function iter(a, i)
		i = i + 1
		while a[i] and a[i].type ~= t do
			i = i + 1
		end

		if a[i] then 
			return i, a[i] 
		end
	end

	return iter, edicts, 0
end

function spawnBullet(owner,x,y,vx,vy,life)
	local e = edict(ENT_BULLET)
	e.x = x
	e.y = y
	e.vx = vx
	e.vy = vy
	e.dmg = 4
	e.parent = owner
	e.life = life or 3
	e.think = function(e,dt)
		e.x = e.x + e.vx * dt
		e.y = e.y + e.vy * dt
		e.life = e.life - dt
		if e.life <= 0 or e.y <= -300 then e.dead = true end

		if not e.dead then
			local tgt = ENT_PLAYER
			if e.parent and e.parent.type == ENT_PLAYER then
				tgt = ENT_ENEMY
			end

			for _, ent in edictsByType(tgt) do
				local dx = e.x - ent.x
				local dy = e.y - ent.y

				local bbx = math.abs(dx)
				local bby = math.abs(dy)

				if bbx < ent.hitw and bby < ent.hith then
					if ent.takedamage then
						ent.takedamage(ent, e.dmg)
					end
					e.dead = true
				end
			end
		end

	end
	e.draw = function(e)
		local x,y = toCoords( e.x, e.y )
		_r.color(1,1,1,1,2)
		_r.sprite(x, y, 24, 24, math.atan2(-e.vy, e.vx) - math.pi * .5, e.tex)
	end

	if e.parent and e.parent.type == ENT_ENEMY then
		local ch_bullet = sound.play(_s.shoot2)
		sound.setVolume(ch_bullet, .3)
	else
		local ch_bullet = sound.play(_s.shoot)
		sound.setVolume(ch_bullet, .3)

		local result = host.bulletSpawned(x,y)
		print( tostring(result) )
	end

	return e
end

function spawnEnemy(x,y)
	local e = edict(ENT_ENEMY)
	e.x = x or 0
	e.y = y or 0
	e.tx = -100
	e.ty = -200
	e.vx = 0
	e.vy = 0
	e.accel = 10
	e.friction = 3
	e.health = 6
	e.rotation = 0
	e.rotlerp = 0
	e.time = 0
	e.waitformation = 5
	e.waitformation2 = 8
	e.hitw = 8
	e.hith = 8
	e.health = 10
	e.damaged = 0
	e.aimx = 0
	e.aimy = 0
	e.aimdir = math.pi/2
	e.aimvel = 0
	e.aimspeed = math.random(5,20)/20
	e.aimfriction = e.aimspeed + 1
	e.nextshot = math.random(1,2)
	e.bulletspeed = 200

	for _,pl in edictsByType(ENT_PLAYER) do
		e.target = pl
		break
	end

	e.takedamage = function(e,dmg)
		e.damaged = .5
		e.health = e.health - dmg
		if e.health <= 0 then
			e.dead = true
			score = score + 10

			spawnExplosion(e.x, e.y)
			local ch_hit = sound.play(_s.splode)
			sound.setVolume(ch_hit, .3)
			sound.setPitch(ch_hit, 1.6)
		else
			local ch_hit = sound.play(_s.hit)
			sound.setVolume(ch_hit, .3)
			sound.setPitch(ch_hit, 1.6)
		end
	end
	e.think = function(e,dt)
		local dx = e.tx - e.x
		local dy = e.ty - e.y
		local dist = math.sqrt(dx*dx + dy*dy)

		local frac = math.min(dist, 40) / 40
		local rot = math.atan2(-dy, dx) + math.pi/2
		local nrot = rot * frac * frac

		e.rotlerp = e.rotlerp + (nrot - e.rotlerp) * (1 - math.exp(dt * -4))
		e.rotation = e.rotlerp

		e.vx = e.vx + e.accel * dx * dt
		e.vx = e.vx * math.exp(dt * -(e.friction))

		e.vy = e.vy + e.accel * dy * .2 * dt
		e.vy = e.vy * math.exp(dt * -(e.friction))

		e.x = e.x + dt * e.vx
		e.y = e.y + dt * e.vy

		e.time = e.time + dt

		if e.time > e.waitformation and e.formation then
			e.formation(e)
			e.formation = nil
		end

		if e.time > e.waitformation2 and e.formation2 then
			e.formation2(e)
			e.formation2 = nil
		end

		if e.x < -300 or e.x > 300 then
			e.dead = true
		end

		if e.damaged > 0 then
			e.damaged = e.damaged - dt
		end

		local angle = math.atan2(e.target.y - e.y, e.target.x - e.x)
		local delta = angle - e.aimdir

		if delta > math.pi then
			delta = delta - math.pi * 2
		elseif delta < -math.pi then
			delta = delta + math.pi * 2
		end

		e.aimvel = e.aimvel + delta * math.exp(dt * -e.aimspeed)
		e.aimvel = e.aimvel * math.exp(dt * -e.aimfriction)
		e.aimdir = e.aimdir + e.aimvel * dt

		local aim_cos = math.cos(e.aimdir)
		local aim_sin = math.sin(e.aimdir)
		e.aimx = e.x + aim_cos * 500
		e.aimy = e.y + aim_sin * 500


		e.nextshot = e.nextshot - dt
		if math.abs(delta) < .1 and e.target and not e.target.dead then
			if e.nextshot <= 0 then
				e.nextshot = math.random(5,20)/10

				local b = spawnBullet(e,e.x,e.y,aim_cos * e.bulletspeed,aim_sin * e.bulletspeed)
				b.tex = _t.shoot2
				b.dmg = 6
			end
		end
	end
	e.draw = function(e)
		local x,y = toCoords(e.x, e.y)

		_r.layer(2)
		_r.color(1,1,1,1,2)
		_r.sprite(x,y,32,32,e.rotation,math.fmod(time, .1) > .05 and _t.enemy0 or _t.enemy1)

		if e.damaged > 0 and math.fmod(time, .1) > .05 then
			_r.layer(3)
			_r.color(1,1,1,1,1)
			_r.sprite(x,y,32,32,e.rotation,_t.enemymask)
		end

		_r.layer(3)
		local x2, y2 = toCoords(e.aimx, e.aimy)
		local dx, dy = (y2-y), (x2-x)
		local r = math.atan2(dy, dx)
		local d = math.sqrt(dx*dx + dy*dy)
		_r.color(.1,0,0,1,1)
		_r.sprite((x+x2)/2, (y+y2)/2, 2, d, r)
	end
	return e
end

function spawnPlayer(x,y)
	local e = edict(ENT_PLAYER)
	e.x = x or 0
	e.y = y or 0
	e.vx = 0
	e.vy = 0
	e.ax = 0
	e.ay = 0
	e.hitw = 10
	e.hith = 10
	e.nextshoot = 0
	e.altshoot = false
	e.health = 100
	e.damaged = 0
	e.takedamage = function(e,dmg)
		e.damaged = .5
		e.health = e.health - dmg
		if e.health <= 0 then
			e.dead = true

			spawnExplosion(e.x, e.y)
			local ch_hit = sound.play(_s.dead)
			sound.setVolume(ch_hit, .3)
			sound.setPitch(ch_hit, 1.6)
		else
			local ch_hit = sound.play(_s.hit)
			sound.setVolume(ch_hit, .3)
			sound.setPitch(ch_hit, 1.6)
		end
	end
	e.think = function(e,dt)

		local move_horiz = 0
		if k_left then move_horiz = -1
		elseif k_right then move_horiz = 1 end

		local move_vert = 0
		if k_up then move_vert = -1
		elseif k_down then move_vert = 1 end

		e.vx = e.vx + playerAccelRate * move_horiz * dt
		e.vx = e.vx * math.exp(dt * -playerFriction)

		e.vy = e.vy + playerAccelRate * move_vert * dt
		e.vy = e.vy * math.exp(dt * -playerFriction)

		e.x = e.x + dt * e.vx
		e.y = e.y + dt * e.vy

		if e.x > limitX then
			e.x = limitX
			e.vx = e.vx * -.5
		end

		if e.x < -limitX then
			e.x = -limitX
			e.vx = e.vx * -.5
		end

		if e.y > 0 then
			e.y = 0
			e.vy = e.vy * -.5
		end

		if k_shoot and e.nextshoot < time then
			local x,y = e.x, e.y
			local width = 6

			--[[if e.altshoot then
				x = x - width
				e.altshoot = false
			else
				x = x + width
				e.altshoot = true
			end]]

			local b = spawnBullet(e,x,y,0,-800)
			b.tex = _t.shoot

			e.nextshoot = time + .1
		end

		if e.damaged > 0 then
			e.damaged = e.damaged - dt
		end

	end
	e.draw = function(e)
		local r = -e.vx / 1200
		local x,y = toCoords(e.x, e.y)
		_r.color(1,1,1,1,2)
		_r.sprite(x,y,32,32,r,math.fmod(time, .1) > .05 and _t.ship0 or _t.ship1)

		if e.damaged > 0 and math.fmod(time, .05) > .025 then
			_r.layer(3)
			_r.color(1,1,1,1,1)
			_r.sprite(x,y,32,32,r,_t.shipmask)
		end
	end

	local ch_woosh = sound.play(_s.woosh)
	sound.setVolume(ch_woosh, .3)
	sound.setPitch(ch_woosh, 1)

	return e
end

function spawnFX(x,y,life)
	local e = edict(ENT_FX)
	e.x = x
	e.y = y
	e.life = life or 1
	e.lifetime = life
	e.frac = 0
	e.think = function(e,dt)
		e.life = e.life - dt
		if e.life <= 0 then
			e.dead = true
		end
		e.frac = math.min(1 - (e.life / e.lifetime), 1)
	end
	return e
end

function spawnExplosion(x,y)
	local e = spawnFX(x,y,1)
	local sp = 1/4
	e.rotation = math.random(0,360) * (math.pi / 180)
	e.spin = math.random(-10,10)/20
	e.up = 1 + math.random(1,10)/10
	e.scale = 1
	e.draw = function(e)
		local b = 1 - e.frac
		local x,y = toCoords(e.x, e.y)
		local frame = e.frac * 16
		local u = math.floor(frame % 4) * sp
		local v = math.floor(frame / 4) * sp
		_r.layer(4)
		_r.color(b,b,b,1,1)
		_r.sprite(x,y,32 * e.scale,32 * e.scale,e.rotation,_t.explode,u,v,u+sp,v+sp)

		--[[_r.color(b,.7*b,.3*b,1,1)
		local sqrtfrac = math.sqrt(e.frac)
		for i=36, 360, 36 do
			local lr = i * (math.pi / 180)
			local r = 10 + sqrtfrac * 100
			local c = math.cos(lr) * r
			local s = math.sin(lr) * r
			_r.sprite(x+c,y-s,2,4 + b * 20,lr - math.pi * .5)
		end]]
	end

	local base = e.think

	e.think = function(e,dt)
		base(e,dt)
		e.rotation = e.rotation + e.spin * dt
		e.scale = e.scale + e.up * dt
	end
	return e
end