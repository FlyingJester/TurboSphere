	var far = new TTFFont("DejaVuSans.ttf");
	var Red		= new Color(255, 0, 0, 255);
	var Green	= new Color(0, 255, 0, 128);
	var Blue	= new Color(0, 0, 255, 255);
		//Rectangle(64, 32, GetScreenWidth()-64, GetScreenHeight()-16, Red);
		//Rectangle(10, 10, 10, 10, Green);
		Triangle(64, 32, 16, GetScreenHeight()-16, GetScreenWidth()-64, 256, Blue);
		Triangle(0, 0, 0, 16, 16, 0, Blue);
		//far.drawText(128, 65, "Do a barrel roll!");
		FlipScreen();
		GetKey();
		//SetRenderScript("Rectangle(10, 10, 16, 16, Red);");
		MapEngine("test.rmp");
		GetKey();
		
function game(){
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
	
	var sf3 = im2.createSurface();
	var str = "Sphere rfn fonts...IN OPENGL!";
	while(!IsKeyPressed(KEY_Q)){
		var t = GetTime();
		ws1.drawWindow(128, 22, fn.getStringWidth(str), 12);
		fn.drawText(128, 22, str);
		Rectangle(48, 16, 32, 32, Blue);
		Rectangle(16, 16, 32, 32, Red);
		Rectangle(32, 32, 32, 32, Green);
		Rectangle(48, 48, 32, 32, Red);
		
		im1.blit(64, 16);
		
		
		far.drawText(16, GetScreenHeight()-16, "Do a barrel roll! With TTF Fonts!");
		
		Triangle(GetScreenWidth(), GetScreenHeight(), GetScreenWidth()-16, GetScreenHeight(), GetScreenWidth(), GetScreenHeight()-16, Red);
		
		FlipScreen();
		
		GradientTriangle(64, 32, 16, GetScreenHeight()-16, GetScreenWidth()-64, 256, Blue, Green, Red);
		
		ws.drawWindow(128, 22, 128, 256);
		fn.drawTextBox(128, 44, 128, 244, 0, "This is a text box. These things gave me a bit of trouble, but now they work. I tried to make them lightning fast, which made it hard to debug any problems. But some solid reasoning and carefully spent blood, sweat, and tears, and now it works just pretty good. Sometiems cuts the lines a little early, but overall OK.");
		
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
		Delay(16);
	}
}