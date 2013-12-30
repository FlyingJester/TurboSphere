if(GetVersion()<2){
	RequireSystemScript("time.js");
}
RequireSystemScript("colors.js");

//The answer to life, the univers, and everything:
//acceleration = (G*Mass)/(distance*distance);

const G = 6.667;

function Velocity(angle, speed){
	this.angle = angle;
	this.speed = speed;
}

function AddVector(a, b){
	var ax = a.speed*Math.cos(a.angle);
	var ay = a.speed*Math.sin(a.angle);
	
	var bx = b.speed*Math.cos(b.angle);
	var by = b.speed*Math.sin(b.angle);
	
	var nx = ax+bx;
	var ny = ay+by;
	
	var nspeed = Math.sqrt((nx*nx)+(ny*ny));
	
	var nangle = Math.atan2(ny,nx);
	
	return new Velocity(nangle, nspeed);
	
}

function Body(mass, x, y, angle, speed){
	this.mass = mass;
	this.x = x;
	this.y = y;
	this.angle = angle;
	this.speed = speed;
}

function GeoSet(bodies){
	this.bodies = bodies;
	
	this.streamers = [];
	
	this.Calculate = function(){
		var b = this.bodies;
		for(var i in b){
			for(var e in b){
				if(i==e)
					continue;
				if(b[e].mass<0.1)
					continue;
				var dx = b[i].x-b[e].x;
				var dy = b[i].y-b[e].y;
				
				if(dx==0)
					continue;
				var angle = Math.atan2(dy, dx);
				
				if(isNaN(angle))
					Abort("lol");
				
				var d = Math.sqrt((dx*dx)+(dy*dy));
				var accel = ((G*b[e].mass)/(d*d));
				
				var gv = AddVector(new Velocity(angle, accel), b[i]);
				
				b[i].speed = gv.speed;
				b[i].angle = gv.angle;
				
			}
			
			//this.streamers.push({x:b[i].x, y:b[i].y, c:CreateColor(255, 0, 0), a:0});
			
			b[i].x-=Math.cos(b[i].angle)*b[i].speed;
			b[i].y-=Math.sin(b[i].angle)*b[i].speed;
		}
	}
	
}

function DrawModelView(GS){
	GS.Calculate();
	for(var i in GS.bodies){
		var b = GS.bodies[i];
		FilledCircle((b.x>>1)+100, (b.y>>1), Math.max(b.mass>>1, 4), White);
	}
	for(var i in GS.streamers){
		var s = GS.streamers[i];
		s.c.green = Math.min(s.a>>4, 128);
		s.c.blue 	= Math.min(s.a>>4, 224);
		Math.max(1/s.a, 128);
		
		s.a++;
		Point((s.x>>1)+100, (s.y>>1), s.c);
	}
}

var f = GetSystemFont();

function game(){
	var GS = new GeoSet([new Body(56, 300, 440, 0, -0.002), new Body(0.1, 300, 820, 0, 1)]);
	while(!IsKeyPressed(KEY_Q)){
		var t = GetTime();
		
		DrawModelView(GS);
		f.drawText(GetScreenWidth()-80, 10, "Speed: "+(GS.bodies[1].speed));
		f.drawText(GetScreenWidth()-80, 20, "Speed: "+(GS.bodies[0].speed));
		FlipScreen();
		while(GetTime()<t+2){}
	}
}