RequireSystemScript("colors.js");
RequireSystemScript("turbo/map_engine.js");
RequireSystemScript("turbo/bytearray.js");
RequireSystemScript("turbo/image.js");

var s = new Surface(64, 64, Black);
var BlankIm = new Image(new Surface(1, 1, White));
var BlankIm2 = new Image(BlankIm.createSurface());
var DefaultShader = GetDefaultShaderProgram();

var f = CreateByteArray(10);

var g = CreateByteArrayFromString("I thought what I'd do is I'd pretened I was one of those deaf mutes.");
var h = CreateByteArrayFromString(" But should I?");

// 32 = 8
// 128 = 32

function GetNumTesselations(size){
    return size>>2;
}


function sDraw(){
    if(typeof this.onDraw === 'function')
      this.onDraw();
    this.Group.setX(this.x);
    this.Group.setY(this.y);
    this.Group.angle = this.angle-(3*Math.PI/2);
        this.angle-=(Math.floor(this.angle/(Math.PI*2.0))*Math.PI*2.0);
    this.Group.Draw();
}


function Asteroid(x, y, size, speed, rotationspeed, dir){
    this.x = x;
    this.y = y;

    this.size = size;
    this.speed = speed;
    this.angle = 0;
    this.mangle = dir;

    this.tesNum = GetNumTesselations(size);

    this.Points = [];
    this.Shapes = [];
    // Calculate some random points to make the surface.
    for(let i = 0; i< this.tesNum; i++){
        var angle = (i/this.tesNum)*Math.PI*2;
        angle+=(Math.random()*Math.PI/8)-(Math.PI/16);
        var len = size;
        len+=Math.round(Math.sqrt(Math.random()*size))-1;

        // One in 64 chance to make a really low size.
        if((Math.random()*32)<1)
          len/=4;

        this.Points.push(new Vertex(Math.cos(angle)*len, Math.sin(angle)*len));
    }

    for(let i = 0; i< this.tesNum; i++)
      this.Shapes.push(new Shape([this.Points[i], this.Points[(i+1)%this.tesNum]], BlankIm2));

    this.Group = new Group(this.Shapes, DefaultShader);

    this.Group.x = x;
    this.Group.y = y;

    this.lastDrawn = 0;

    this.onDraw = function(){
        if(this.lastDrawon==0)
          this.lastDrawn = GetSeconds();

        var factor = (GetSeconds()-this.lastDrawn);

        this.angle+=rotationspeed*factor;
        this.x+=Math.cos(this.mangle)*this.speed;
        this.y+=Math.sin(this.mangle)*this.speed;

        this.x %= GetScreenWidth();
        this.y %= GetScreenHeight();
        if(this.x<0)
          this.x = GetScreenWidth();
        if(this.y<0)
          this.y = GetScreenHeight();

        this.angle-=(Math.floor(this.angle/(Math.PI*2.0))*Math.PI*2.0);

        this.lastDrawn = GetSeconds();
    }

    this.Break = function(angle){
        if(this.size < 4)
          return [];

        return [
            new Asteroid(this.x+100, this.y, this.size, this.speed+(Math.random()-0.4), rotationspeed+(Math.random()-0.25), this.mangle+angle+Math.PI),
            new Asteroid(this.x-100, this.y, this.size, this.speed+(Math.random()-0.4), rotationspeed+(Math.random()-0.25), this.mangle+angle-Math.PI),
        ]
    }

    this.tryCollide = function(ast){
        for(let i in ast){
            var dx = this.x-ast[i].x;
            var dy = this.y-ast[i].y;

            if(ast[i]===this)
              continue;

            var d = Math.sqrt((dx*dx)+(dy*dy));

            if(d<(this.size+ast[i].size))
              return true;

        }
        return false;
    }

}

function SpaceShip(x, y){

    this.Points = [
      new Vertex(0, 32),
      new Vertex(8, 0),
      new Vertex(16, 32),
      new Vertex(8, 28)
    ];
    this.Shapes = [
      new Shape([this.Points[0], this.Points[1]], BlankIm),
      new Shape([this.Points[1], this.Points[2]], BlankIm),
      new Shape([this.Points[2], this.Points[3]], BlankIm),
      new Shape([this.Points[3], this.Points[0]], BlankIm),
    ];

    this.Group = new Group(this.Shapes, DefaultShader);

    if(false){

        var msg = "";
        var j = 0;

        for(var i in this.Group.shapes){
            for(var e in this.Group.shapes[i].vertices){
                msg+= " " + j + ": (x " + this.Group.shapes[i].vertices[e].x + " y " + this.Group.shapes[i].vertices[e].y + ")";
                j++;
            }
        }

        Abort(msg);

    }

    this.x = x;
    this.y = y;
    this.Group.rotX = 8;
    this.Group.rotY = 16;

    this.velocity = 0;
    this.xvelocity = 0;
    this.yvelocity = 0;
    this.angle = 0.0;
    this.lasttime = 0;
    this.lasttimeA = 0;

    this.ResetTime = function(){this.lasttime = 0;this.lasttimeA=0;}

    this.AccelCheck = function(){return false;};
    this.DecelCheck = function(){return false;};
    this.RotateLeftCheck  = function(){return false;};
    this.RotateRightCheck = function(){return false;};

    this.Move2 = function(duration){
        var lvelocity = (this.velocity * 100 * duration);
        if(lvelocity==0)
          return;

        var lx = Math.cos(this.angle) * lvelocity;
        var ly = Math.sin(this.angle) * lvelocity;

        this.xvelocity = lx / 1000;
        this.yvelocity = ly / 1000;

        this.x += this.xvelocity;
        this.y += this.yvelocity;

        this.x %= GetScreenWidth();
        this.y %= GetScreenHeight();

    }

    this.Move = function(duration){
        var lvelocity = (100 * duration);
        if(lvelocity==0)
          return;

        this.x += this.xvelocity;
        this.y += this.yvelocity;

        this.x %= GetScreenWidth();
        this.y %= GetScreenHeight();
        if(this.x<0)
          this.x = GetScreenWidth();
        if(this.y<0)
          this.y = GetScreenHeight();


    }

    this.MoveAuto = function(){
        var t = GetSeconds();

        if(this.lasttime==0)
          this.lasttime = t;

        if(this.AccelCheck()){
            this.xvelocity+=((t-this.lasttime)/500)*Math.cos(this.angle)*1000;
            this.yvelocity+=((t-this.lasttime)/500)*Math.sin(this.angle)*1000;

            this.xvelocity = Math.min(this.xvelocity, 10.0*(t-this.lasttime)*1000);
            this.yvelocity = Math.min(this.yvelocity, 10.0*(t-this.lasttime)*1000);
        }

        this.Move(GetSeconds() - this.lasttime);

        this.lasttime = GetSeconds();

    }

    this.AdjustAngle = function(){
        var t = GetSeconds();

        if(this.lasttimeA==0)
          this.lasttimeA = t;

        if(this.RotateLeftCheck())
          this.angle -= (Math.PI*(t-this.lasttimeA)/512.0)*1000.0;
        if(this.RotateRightCheck())
          this.angle += (Math.PI*(t-this.lasttimeA)/512.0)*1000.0;

        this.lasttimeA = t;

    }

//    this.Draw = sDraw;
}

SpaceShip.prototype.Draw = sDraw;
Asteroid.prototype.Draw = sDraw;

function game(){

    var Astral = Turbo.LoadMapFile("test.rmp");

    Astral.calculateMap();

    var Testim = new BlackMamba.Image(new Surface("noob.bmp"));

    var Live = true;
    var LilZ = new SpaceShip(GetScreenWidth()/2, GetScreenHeight()/2);

    var Asteroids = [
        new Asteroid(GetScreenWidth()*3/4, GetScreenHeight()/4, 32, 1, 0.5, Math.random()*Math.PI*2.0),
        new Asteroid(GetScreenWidth()/4, GetScreenHeight()*3/4, 32, 2, 0.6, Math.random()*Math.PI*2.0),
    ]

    LilZ.AccelCheck = function(){return IsKeyPressed(KEY_UP);};
    LilZ.DecelCheck = function(){return true;};
    LilZ.RotateLeftCheck  = function(){return IsKeyPressed(KEY_LEFT); };
    LilZ.RotateRightCheck = function(){return IsKeyPressed(KEY_RIGHT);};

    var b = false;

    while(Live){

        var f = GetTime();


        if(AreKeysLeft()){
            var k = GetKey();
            if(k==KEY_Q)
              Live = false;
        }


        Astral.drawMap();
        LilZ.Draw();

        for(let i in Asteroids){
            Asteroids[i].Draw();
            if(false && !b){
                if(Asteroids[i].tryCollide(Asteroids)){
                    var r = Asteroids[i].Break();
                    for(let e in r)
                      Asteroids.push(r[e]);

                    Asteroids.splice(i, 1);

                    Abort(Asteroids.length);

                    b = true;
                    break;
                }
            }
        }

        LilZ.MoveAuto();
        LilZ.AdjustAngle();

        FlipScreen();
        var t = f - ( GetTime() -16 );
        if(t>0)
          Delay(t);

    }
}
