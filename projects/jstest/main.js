
var width = 0;
var height = 0;
function think(time, dt)
{
	width = 200 + Math.sin(time * 2) * 100;
	height = 200 + Math.cos(time * 2) * 100;
	//print(x);
	//x = x + 1
}

function draw()
{
	_r.color(1,1,1,.5);
	_r.sprite(0,0,width,height,0,assets["ship.tex"]);
}

function getKeyByValue(object, value) {
  return Object.keys(object).find(key => object[key] === value);
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

for ( var asset in assets )
{
	var value = assets[asset]
	print( asset )
	print( value.name + " " + value.type )
}