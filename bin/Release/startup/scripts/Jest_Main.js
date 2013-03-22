
var fonty = GetSystemFont();
var start = GetTime();
var arrow = GetSystemArrow();
var ws = new WindowStyle("property.rws");
//var ws2 = GetSystemWindowStyle();

var imagey = new Image("test.png");
var imagey1= new Image("test1.png");

var Black  =  new Color(0,   0,   0,   255);
var Red    =  new Color(255, 0,   0,   255);
var Green  =  new Color(0,   255, 0,   128);
var Yellow =  new Color(255, 255, 0,   255);
var White  =  new Color(255, 255, 255, 255);

function blitRect(params, color){
	Rectangle(params.x, params.y, params.w, params.h, color);
}

//400x320

function game(){
  var fontu = GetSystemFont();
  var w = GetScreenWidth();
  var h = GetScreenHeight();
  var c = new Color(255, 255, 255, 255);
  var rad = 20;
  while(!IsKeyPressed(KEY_Q)){
    
    if(IsKeyPressed(KEY_D)){
      Rectangle(0, 0, w, h, Green);
    }
    
    SetClippingRectangle(128, 128, 64, 64);
    var param1 = GetClippingRectangle();
    SetClippingRectangle(0, 0, w, h);
    
    if(IsKeyPressed(KEY_SPACE)){
      SetClippingRectangle(64, 144, 256, 32);
    }
    Rectangle(0, 0, w, h, Red);
    var param2 = GetClippingRectangle();
    param2.x+=4;
    param2.y+=4;
    param2.w-=8;
    param2.h-=8;
    blitRect(param2, Green);
    
    blitRect(param1, Yellow);
    FlipScreen();
    Delay(15);
  }
}

