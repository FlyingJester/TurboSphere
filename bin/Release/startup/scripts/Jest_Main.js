
var c = new Color(0, 0, 0);
 

var fn = GetSystemFont();
var ws = new WindowStyle("property.rws");
var int = 0;
var str = "hello";
var orange = new Color(255, 128, 0, 64);
var files = GetFileList("../../system");
while(!IsKeyPressed(KEY_R)){

while(AreKeysLeft()) {
	var k = GetKey();
  switch(k) {
    case KEY_BACKSPACE: 
      str = str.substr(0, str.length-1);
      break;
    case KEY_SPACE: 
      str = str.substr(0, str.length-1);
      break;
    case KEY_ENTER:
      str = "hello";
      break;  
  }
  fn.drawText(10, 10, GetKey);
int++;
}
fn.drawText(0, 10, c instanceof Color);
fn.drawText(100, 20, c.toString());
fn.drawText(100, 10, "Press R. "+(typeof(c)));
fn.drawText(10, 200, c.constructor);
Delay(10);
fn.drawText(10, 10, str);
ws.drawWindow(10, 40, fn.getStringWidth("Files in the system directory:"), 10+(files.length*12));
fn.drawText(10, 40, "Files in the system directory:");

if(IsKeyPressed(KEY_ENTER)){
	Rectangle(10, 40, 200, 10+(files.length*12), orange);
}

for(var i = 0; i<files.length; i++){
	fn.drawText(10, 50+(i*10), files[i]);
}

FlipScreen();
}

RequireScript("MJ-12.js");

var start = GetTime();
var arrow = GetSystemArrow();
var ws = new WindowStyle("property.rws");
//var ws2 = GetSystemWindowStyle();

var imagey1= new Image("test1.png");
var imagey2= new Surface("test1.png");


//400x320

function game(){
  while(!IsKeyPressed(KEY_ESCAPE)){
	var lion = fn.wordWrapString("Plug in a joystick and try this again. It's not very much fun to use\n non existant joysticks! And here are six more words.", GetScreenWidth()-20);
	
	fn.drawText(10, 32, lion.length);
	
	for(var i = 0; i< lion.length; i++){
		fn.drawText(10, 50+(i*20), lion[i]);
	}
	delete lion;  
	
	fn.drawText(10, 10, "I hope this doesn't leak!");
	FlipScreen();  
  }
	
  imagey1.rectangle(2, 2, 4, 4, Black);
  var fontu = GetSystemFont();
  var w = GetScreenWidth();
  var h = GetScreenHeight();
  var c = new Color(255, 255, 255, 255);
  var rad = 20;
  testJS();
}


function testJS(){
  var cont = true;
  while(cont){
	  if(IsKeyPressed(KEY_ESCAPE)){cont = false}
	  fn.drawText(10, 12, "Joystick Test Menu. Press Escape to exit.");
          fn.drawText(10, 22, "Num Joysticks: "+GetNumJoysticks());
if(GetNumJoysticks()==0) {
	var lion = fn.wordWrapString("Plug in a joystick and try this again. It's not very much fun to use non existant joysticks!", GetScreenWidth()-20);
	
	fn.drawText(10, 32, lion.length);
	
	for(var i = 0; i< lion.length; i++){
		fn.drawText(10, 50+(i*20), lion[i]);
	}
	delete lion;
	
//	fn.drawText(10, 32, "Plug in a joystick or gamepad and try this again!");
}
    var r = 0;
    for(var i = 0; i<GetNumJoysticks(); i++){
	fn.drawText(10, 34+((r+i)*12), GetJoystickName(i));
	r++
	fn.drawText(10, 34+((r+i)*12), "    Number of Axes: "+GetNumJoystickAxes(i));
	r++
	for(var e = 0; e<GetNumJoystickAxes(i); e++){
		fn.drawText(10, 34+((r+i)*12), "        Axis "+e+" is at "+Math.round(GetJoystickAxis(i, e)/3276.7));
		r++;
	}
	r = 1;
        for(var e = 0; e<GetNumJoystickButtons(i); e++){
		fn.drawText(200, 34+((r+i)*12), "Button "+e+" is "+(IsJoystickButtonPressed(i, e)?"":"not ")+"pressed.");
		r++;
	}
	
	    
    }
    FlipScreen();
    Delay(15);
    }
  while(AreKeysLeft()){
	GetKey();  
  }
}
