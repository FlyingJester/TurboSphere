function Angle()
{
	this.r = 0;
	this.d = 0;
	this.sina = 0;
	this.cosa = 1;

	var PI = Math.PI;

	this.setRadians = function(s) {
		this.r = s;
		this.d = this.clamp(s/PI*180);
		this.sina = Math.sin(s);
		this.cosa = Math.cos(s);
	};

	this.setDegrees = function(s) {
		this.d = this.clamp(s);
		this.r = this.d/180*PI;
		this.sina = Math.sin(this.r);
		this.cosa = Math.cos(this.r);
	};
}

Angle.prototype.clamp = function(deg) {
	if (deg >= 360) return (deg - 360);
	else if (deg < 0) return (deg + 360);
	else return deg;
}
/*
function GetTime() {
    return Date.now();
}
*/
//var Red = new Color(255, 0, 0, 255);

function game()
{
	
	var SW = GetScreenWidth();
	var SH = GetScreenHeight();

	var ox = SW/2;
	var oy = SH/2;
	var s = new ship();

	var red = new Color(255, 0, 0);
	var green = new Color(0, 255, 0);
	var white = new Color(255, 255, 255);	

	var im1 = new Image("test1.png");
	var t = GetTime();
	for(var fp = 0;  fp<=100000; fp++) {
		Line(SW/4, SH/2, SW-SW/4, SH/2, green);
		Line(SW/2, SH/4, SW/2, SH-SH/4, green);

		Line(ox, oy, ox+s.vx*SW/4, oy+s.vy*SH/4, white);
		//Point(ox+s.headx*SW/4, oy+s.heady*SH/4, red);

		s.draw();
		s.update();
		im1.transformBlit(0, 0, 50, 0, 100, 100, 0, 50);
		
		FlipScreen();
	}
	t = GetTime()-t;
	var fonty = GetSystemFont();
	while(!IsKeyPressed(KEY_ESCAPE)){
		fonty.drawText(10, 10, "We drew 10,000 frames in "+t+" milliseconds.");
		FlipScreen();
	}
}

function ship()
{
	this.image = new Image("test.bmp");
	this.vx = 0; this.vy = 0;
	this.x = 0; this.y = 0;
	this.headx = 0; this.heady = 0;
	this.angle = new Angle();

	this.getSpeed = function() {
		return Math.sqrt(this.vx * this.vx + this.vy * this.vy);
	};

	this.stime = 0;
}

ship.prototype.draw = function() {
	this.image.rotateBlit(this.x, this.y, this.angle.r);
}

ship.prototype.update = function() {
	this.x += this.vx;
	this.y += this.vy;

	if (this.stime + 25 < GetTime()) {
		if (IsKeyPressed(KEY_UP)) {
			this.vx += (this.headx-this.vx)/60;
			this.vy += (this.heady-this.vy)/60;
		}
		if (IsKeyPressed(KEY_DOWN)) {
			this.angle.degrees += 180;
			this.headx = this.angle.cosa;
			this.heady = this.angle.sina;
		}
		if (IsKeyPressed(KEY_LEFT)) {
			this.angle.setDegrees(this.angle.d - 5);
			this.headx = this.angle.cosa;
			this.heady = this.angle.sina;
		}
		if (IsKeyPressed(KEY_RIGHT)) {
			this.angle.setDegrees(this.angle.d + 5);
			this.headx = this.angle.cosa;
			this.heady = this.angle.sina;
		}
		this.stime = GetTime();
	}
}

