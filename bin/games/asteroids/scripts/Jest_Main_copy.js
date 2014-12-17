RequireSystemScript("colors.js");

var s = new Surface(64, 64, Black);
var Fonty = GetSystemFont();

s.drawText(Fonty, 0, 0, "Sphere");
s.drawText(Fonty, 4, 16, "Fonts!");

var SphereLogo = LoadImage("sphere.png");

var PointerImage = new Image(s);
var LogoImage = new Image(new Surface("sphere.png"));
var DefaultShader = GetDefaultShaderProgram();

var z = new Surface("sphere.png");
var BlankIm = new Image(new Surface(1, 1, White));

z.save("noob.png");
z.save("noob.tga");
z.save("noob.bmp");
z.save("noob.jpg");

var Vertices1 = [new Vertex(0, 0),   new Vertex(64, 0),  new Vertex(64, 64), new Vertex(0,  64) ];
var Vertices2 = [new Vertex(64, 0),  new Vertex(128,0),  new Vertex(128,64), new Vertex(64, 64) ];
var Vertices3 = [new Vertex(128,0),  new Vertex(192,0),  new Vertex(192,64), new Vertex(128,64) ];
var Vertices4 = [new Vertex(0,128),  new Vertex(64,128), new Vertex(64,192), new Vertex(0, 192) ];

var Shape1 = new Shape(Vertices1, PointerImage);
var Shape2 = new Shape(Vertices2, BlankIm);
var Shape3 = new Shape(Vertices3, PointerImage);
var Shape4 = new Shape(Vertices4, PointerImage);

var PointerGroup = new Group([Shape1, Shape2, Shape3, Shape4], DefaultShader);

var LogoShapes =[];


function SpaceShip(x, y){

  this.Points = [
    new Vertex(0, 32),
    new Vertex(8, 0),
    new Vertex(16, 32),
    new Vertex(8, 28)
  ];
  this.shapes = [
    new Shape([this.Points[0], this.Points[1]], BlankIm),
    new Shape([this.Points[1], this.Points[2]], BlankIm),
    new Shape([this.Points[2], this.Points[3]], BlankIm),
    new Shape([this.Points[3], this.Points[0]], BlankIm),
  ];

  this.group = new Group(this.shapes, DefaultShader);

  this.x = x;
  this.y = y;

  this.Draw = function(){
    this.group.setX(this.x);
    this.group.setY(this.y);
    this.group.Draw();
    //Abort("lol")
  }

}



for(var j = 0; j<GetScreenWidth()/32; j++){
    var k = j+1;
    for(var i = 0; i<GetScreenHeight()/32; i++){
        var e = i+1;
        var Vertices = [new Vertex(j*32, i*32),   new Vertex(k*32, i*32),  new Vertex(k*32, e*32), new Vertex(j*32,  e*32)];

        if(e>GetScreenHeight()/32){
            for(var f in Vertices)
                Vertices[f].x+=16;
        }

        LogoShapes.push(new Shape(Vertices, LogoImage));
    }
}
var LogoGroup = new Group(LogoShapes, DefaultShader);

var CursorShape = new Shape(Vertices1, PointerImage);
var Cursor = new Group(CursorShape, DefaultShader);

function game(){

    var sr = Fonty.getStringWidth("Around the ragged rocks, the ragged rascal ran.");
   // Abort(sr);
    var i = 0;
    var q = GetTime();
    var Live = true;
    var LilZ = new SpaceShip(GetScreenWidth()/2, GetScreenHeight()/2);

    while(Live){

        if(AreKeysLeft()){
            var k = GetKey();
            if(k==KEY_Q)
              Live = false;
        }

        PointerGroup.Draw();
        //LogoGroup.Draw();

        Cursor.setPosition(GetMouseX(), GetMouseY());
//        Cursor.setX(GetMouseX());
       // Cursor.setY(GetMouseY());
//        Cursor.setY(GetMouseY());

        //Abort(i);

        //Abort("GetScreenWidth: " + GetScreenWidth() + ", GetScreenHeight: " + GetScreenHeight());

        Cursor.Draw();

        LilZ.Draw();

        LilZ.x = GetMouseX();
        LilZ.y = GetMouseY();

        SphereLogo.blit(GetMouseX()+32, GetMouseY()+32);
        FlipScreen();

        //if(i++>=2500)
        //  break;
    }
    return (GetTime() - q);
}
