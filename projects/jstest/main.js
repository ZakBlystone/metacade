var xpos = 0;
var ypos = 0;

var dudes = []

function dude()
{
	this.life = 1
	this.x = Math.random() * 400;
	this.y = 0;
	this.r = Math.random() * 360;
	this.yvel = 0;
	this.xvel = Math.random() * 100 - 50;
}

function think(time, dt)
{
	for ( var i=1; i<16; ++i ) dudes.push( new dude() );

	var i = dudes.length;
	while(i--)
	{
		var d = dudes[i]
		d.life -= dt * 0.5;
		d.y += d.yvel * dt;
		d.x += d.xvel * dt;
		d.yvel += dt * 300;

		if ( d.y > 300 )
		{
			d.y = 300
			d.yvel *= -0.9
		}

		if ( d.life < 0 ) dudes.splice(i, 1);
	}
}

function draw()
{
	for ( var i in dudes )
	{
		var dude = dudes[i]
		var scale = 4 * dude.life * (1-dude.life);
		_r.color(1,1,1,dude.life);
		_r.sprite( dude.x, dude.y, 30 * scale, 30 * scale, dude.r, assets["ship.tex"]);
	}
}

function getKeyByValue(object, value) {
  return Object.keys(object).find(key => object[key] === value);
}

function onMouseMoved(x,y)
{
	xpos = x;
	ypos = y;
}

function onMousePressed(x,y,button,focus)
{
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