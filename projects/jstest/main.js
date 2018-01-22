var phys = Box2D()

var dudes = []
var particles = []
var worldscale = 10
var gravity = new phys.b2Vec2(0.0, 40.0);
var world = new phys.b2World( gravity );

_s.master_pitch = 1

var listener = new phys.JSContactListener();
listener.BeginContact = function (contactPtr) {
    var contact = phys.wrapPointer( contactPtr, phys.b2Contact );
    var bodyA = contact.GetFixtureA().GetBody();
    var bodyB = contact.GetFixtureB().GetBody();

    var avel = bodyA.GetLinearVelocity();
    var bvel = bodyB.GetLinearVelocity();

    var aspeed = Math.sqrt( avel.get_x() * avel.get_x() + avel.get_y() * avel.get_y() )
    var bspeed = Math.sqrt( bvel.get_x() * bvel.get_x() + bvel.get_y() * bvel.get_y() )
    var impact = aspeed + bspeed

    impact *= worldscale
    impact /= 200.0;

    var volume = Math.min( impact, 1 )

    if ( volume < .1 ) return;
    //print(volume)

	var chan = _s.play( assets["shoot.snd"] )
	_s.pitch( chan, Math.random() + .5 )
	_s.volume( chan, volume / 4 )
}
listener.EndContact = function() {};
listener.PreSolve = function() {};
listener.PostSolve = function() {};

world.SetContactListener( listener );

function init()
{
	var music = _s.play( assets["music.snd"] )
	_s.volume( music, .25 )
	//_s.pitch( music, .8 )
	_s.loop( music, true )
	print("play music")
}

function particle(x, y, speed, ratemul)
{
	speed = speed || 1
	ratemul = ratemul || 1
	this.x = x
	this.y = y
	this.liferate = (Math.random() * .5 + .5) * ratemul
	this.vx = (Math.random() * 50 - 25) * speed
	this.vy = (Math.random() * 50 - 25) * speed
	this.life = 1
}

function dude(x, y)
{
	this.life = 1
	this.x = x || Math.random() * 400;
	this.y = y || 0;
	this.r = 0
	this.w = (Math.random() * 40 + 10) / worldscale
	this.h = (Math.random() * 40 + 10) / worldscale

	var bd = new phys.b2BodyDef()
	bd.set_type(phys.b2_dynamicBody)
	bd.set_position( new phys.b2Vec2( x, y ) )
	this.body = world.CreateBody( bd )

	var shape = new phys.b2PolygonShape();
	shape.SetAsBox(this.w/2, this.h/2);

	this.body.CreateFixture( shape, 5.0 )
}

function makeSegment(x0,y0,x1,y1)
{
	var bd = new phys.b2BodyDef()
	var ground = world.CreateBody( bd )
	var shape = new phys.b2EdgeShape();
	shape.Set(
		new phys.b2Vec2(x0 / worldscale, y0 / worldscale), 
		new phys.b2Vec2(x1 / worldscale, y1 / worldscale));
	ground.CreateFixture(shape, 0.0);
}

makeSegment(-400.0, 300.0, 400.0, 300.0)
makeSegment(0.0, 0.0, 0.0, 300.0)
makeSegment(400.0, 0.0, 400.0, 300.0)

var curtime = 0

function think(time, dt)
{
	curtime = time

	var i = dudes.length;
	while(i--)
	{
		var d = dudes[i]
		d.x = d.body.GetPosition().get_x()
		d.y = d.body.GetPosition().get_y()
		d.r = -d.body.GetAngle()
		d.life -= dt * 0.1;

		if ( d.life < 0 ) 
		{
			var fx = _s.play( assets["place.snd"] )
			_s.volume( fx, .15 )
			_s.pitch( fx, 3 )

			for ( j=1; j<10; ++j )
			{
				particles.push( new particle(d.x * worldscale, d.y * worldscale, 4) )
			}

			world.DestroyBody( d.body )
			dudes.splice(i, 1);
		}
	}

	var i = particles.length;
	while(i--)
	{
		var p = particles[i]
		p.x += p.vx * dt
		p.y += p.vy * dt
		p.vy = p.vy + dt * 80
		p.life -= dt * p.liferate;

		if ( p.life < 0 ) 
		{
			particles.splice(i, 1);
		}
	}

	world.Step(dt, 3, 3);
}

function draw()
{
	_r.layer++
	for ( var i in particles )
	{
		_r.color(1,1,1,1)
		_r.sprite( 
			particles[i].x, 
			particles[i].y, 
			3 * particles[i].life, 3 * particles[i].life, 0);
	}

	_r.layer--

	for ( var i in dudes )
	{
		var dude = dudes[i]
		var scale = 1 - (Math.max( dude.life - .98, 0 )) * 50
		var scaleout = Math.min( dude.life, .02 ) * 50

		scale *= scale
		scale *= ( scaleout * scaleout )
		_r.color(1,dude.life,dude.life*dude.life,1);

		if ( dude.life < .05 && ( dude.life * 120 ) % 1 > .3 )
		{
			_r.color(1,1,1,1)
		}

		_r.sprite( 
			dude.x * worldscale, 
			dude.y * worldscale, 
			dude.w * worldscale * scale, 
			dude.h * worldscale * scale, dude.r); //, assets["ship.tex"]
	}

	_r.layer--
	_r.color(0,.2,.6,1.0)
	_r.rect(0,0,400,300)
}

function getKeyByValue(object, value) 
{
	return Object.keys(object).find(key => object[key] === value);
}

function onMousePressed(x,y,button,focus)
{
	var newdude = new dude(x / worldscale,y / worldscale);
	dudes.push( newdude );

	var fx = _s.play( assets["place.snd"] )
	_s.volume( fx, .25 )
	_s.pitch( fx, Math.random() * .2 + 1 )

	for ( i=1; i<10; ++i )
	{
		particles.push( new particle(x, y, 4, 4) )
	}

	print("Mouse Pressed: " + x + " " + y + " : " + button + " [" + focus + "]");
}

function onMouseReleased(x,y,button,focus)
{
	print("Mouse Released: " + x + " " + y + " : " + button + " [" + focus + "]");
}

function onKeyPressed(code)
{
	print("Key Pressed: " + getKeyByValue(keys, code) )
}