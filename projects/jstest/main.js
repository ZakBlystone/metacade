
var x = 0
var y = 0
function think(time, dt)
{
	//print(x);
	//x = x + 1
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