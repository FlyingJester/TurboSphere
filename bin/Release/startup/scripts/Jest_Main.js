	var far = new TTFFont("DejaVuSans.ttf");
	var fonty2 = GetSystemFont();
	var Red		= new Color(255, 0, 0, 255);
	var Green	= new Color(0, 255, 0, 128);
	var Blue	= new Color(0, 0, 255, 255);
		//Rectangle(64, 32, GetScreenWidth()-64, GetScreenHeight()-16, Red);
		//Rectangle(10, 10, 10, 10, Green);
		Triangle(64, 32, 16, GetScreenHeight()-16, GetScreenWidth()-64, 256, Blue);
		Triangle(0, 0, 0, 16, 16, 0, Blue);
		far.drawText(128, 65, "Do a barrel roll!");
		FlipScreen();
		//GetKey();
		RequireScript("MJ-12.js");
		FlipScreen();
//		m.Stop();
		
function game(){
		var r = new Sound("Attack.ogg", true);
		var m = new Sound("spiders.mid", true);
		//var r = m;
		
		//var m = new Sound("Attack.wav");
		
		r.play();
		//m.Play();
		//GetKey();
	var fn = GetSystemFont();
	var ws1 = new WindowStyle("property.rws");
	var ws = GetSystemWindowStyle();
	var Red		= new Color(255, 0, 0, 255);
	var Green	= new Color(0, 255, 0, 128);
	var Blue	= new Color(0, 0, 255, 255);
	var im1 = new Image("test1.png");
	var im2 = new Image("sphere.png");
	var sf1 = new Surface("test1.png");
	var sf2 = new Surface("sphere.png");
		//m.Stop();
	var str = "Sphere rfn fonts...IN OPENGL!";
	
	var sf3 = im2.createSurface();
		var mjMap = new MjEngineMap("test", "test");
	sf3.blitSurface(mjMap.controltile, 0, 0);
	sf3.line(0, 0, 16, 16, Red);
	sf3 = sf3.createImage();
	var endt = GetTime()+2000
	/*
	for(var i = 0; i<endt; i=GetTime()){
		FlipScreen();
		Rectangle(GetMouseX(), GetMouseY(), 16, 16, Red);
		fonty2.drawText(10, 10, endt-GetTime());
	}
	*/
	var sfMJ;
	var Rad1 = 18;
	var Rad2 = 16;
	var firstDraw = true;
	var ss = new SpriteSet("test.rss");
	var f = 0;
	var z = GetTime();
	while(!IsKeyPressed(KEY_Q)){
		f++;
		if(f==10000)
			break;
		var s = ss.images;
		for(var i in s){
			s[i].blit(i*16,  GetScreenHeight()-32);
		}
		
		s = null;
		
		far.drawText(32+GetScreenWidth()/2, 10, 'Press Q to just give up.');
		
		if(!IsKeyPressed(KEY_SPACE)){
			GradientCircle(GetScreenWidth()-64, 64, Rad1, Green, Blue, IsKeyPressed(KEY_A)||IsKeyPressed(KEY_Z));
			OutlinedCircle(GetScreenWidth()-100, 64, Rad2, Red, IsKeyPressed(KEY_A)||IsKeyPressed(KEY_X));
			FilledCircle(GetScreenWidth()-100, 56, 8, Blue, IsKeyPressed(KEY_A)||IsKeyPressed(KEY_C));
		
		}
		
		if(IsKeyPressed(KEY_PERIOD)){
			Rad1+=0.1;
		}
		if(IsKeyPressed(KEY_COMMA)){
			Rad1-=0.1;
		}
		
		if(IsKeyPressed(KEY_R)){
			Rad1 = 18;
			Rad2 = 16;
		}
		
		ws1.drawWindow(128, 22, fn.getStringWidth(str), 12);
		fn.drawText(128, 22, str);
		Rectangle(48, 16, 32, 32, Blue);
		Rectangle(16, 16, 32, 32, Red);
		Rectangle(32, 32, 32, 32, Green);
		Rectangle(48, 48, 32, 32, Red);
		
		im1.blit(64, 16);
		
		if(IsKeyPressed(KEY_M)){
			m.play(true);
			r.stop();
		}
		if(IsKeyPressed(KEY_N)){
			r.play();
			m.stop();
		}
		
		far.drawText(16, GetScreenHeight()-32, "Do a barrel roll! With TTF Fonts! The mjMap's width is "+mjMap.image.width+" and its height is "+mjMap.image.height);
		far.drawText(200, GetScreenHeight()-16, "Attack.ogg's length is "+r.getLength()+", Spider's length is "+m.getLength());
		
		Triangle(GetScreenWidth(), GetScreenHeight(), GetScreenWidth()-16, GetScreenHeight(), GetScreenWidth(), GetScreenHeight()-16, Red);
		
		FlipScreen();
		if(IsKeyPressed(KEY_J)){
		if(firstDraw){
			sfMJ = mjMap.image.createImage();
			firstDraw = false;
		}
		sfMJ.blit(-16, -16);
		}
		GradientTriangle(64, 32, 16, GetScreenHeight()-16, GetScreenWidth()-64, 256, Blue, Green, Red);
		
		ws.drawWindow(128, 22, 128, 256);
		fn.drawTextBox(128, 44, 128, 256, 0, "This is a text box. These things gave me a bit of trouble, but now they work. I tried to make them lightning fast, which made it hard to debug any problems. But some solid reasoning and carefully spent blood, sweat, and tears, and now it works just pretty good. Sometiems cuts the lines a little early, but overall OK.");
		
		im1.transformBlit(92, 128, 160, 144, 160, 196, 92, 256);
		
		
		im2.blit(256, 64)
		im2.blitMask(256, 92, Red);
		im2.blitMask(256, 128, Green);
		im2.blitMask(256, 144, Blue);
		im2.zoomBlit(196, 196, 2, Red);
		im2.rotateBlitMask(256, 228, GetTime()/256, Green);
		
		sf1.blit(-4, -4);
		im1.blit(0, 0);
		sf1.blit(4, 4);
		im1.blit((GetScreenWidth()/2)-8, (GetScreenHeight()/2)-8);
		sf3.blit(92, 32);
		GradientLine(GetScreenWidth()-64, GetScreenHeight()-64, GetMouseX(), GetMouseY(), Green, Red);
		Line(GetScreenWidth()-24, GetScreenHeight()-96, GetMouseX(), GetMouseY(), Green, Red);
		Rectangle(0, 0, GetTime()-t, 16, Red);
		Rectangle(0, 0, GetTime()-t, 16, Green);
		
		//Delay(16);
		
		var t = GetTime();
	}
	z = GetTime()-z;
	delete m;
	delete r;
	
	while(!IsKeyPressed(KEY_ESCAPE)){
		fonty2.drawText(10, 10, 'Wait until GC has been performed. Ideally that won\'t cause a segfault.');
		fonty2.drawText(10, 20, 'Assuming V8 ever decides to GC. Probably not, there aren\'t too many lost objects here.');
		
		fonty2.drawText(10, 30, 'Press Escape to just give up.');
		fonty2.drawText(10, 50, "'the quick brown fox jumped over the lazy dog.'");
		fonty2.drawText(10, 60, '"THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG!"');
		fonty2.drawText(10, 70, "Aptiva? We drew 10,000 frames in "+z+" milliseconds. Good? You be the judge.");
		
		Delay(10);
		FlipScreen();
	}
}