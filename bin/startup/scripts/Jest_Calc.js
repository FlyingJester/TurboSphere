var ang = 0;
var y = 0;
var fn = GetSystemFont();
var orange = new Color(255, 128, 0, undefined);
	var ck = 0;
while(!IsKeyPressed(KEY_R)){
	while (AreKeysLeft()) {
		var k = GetKey();
		switch (k) {
			case KEY_UP: y --; break;
			case KEY_DOWN: y ++; break;
		
			case KEY_BACKSPACE:y = 0; break;
		}
		ck = k;
		Rectangle((GetScreenWidth()/2)-8, (GetScreenHeight()/2)-8, 16, 16, orange);
	}

fn.drawText(10, 20, "The key you pressed was "+GetKeyString(ck, IsKeyPressed(KEY_SHIFT)));

fn.drawText(10, 10, "Y was "+y);
ang+=0.1;
Rectangle((GetScreenWidth()/2)+(64*(Math.sin(ang))), (GetScreenHeight()/2)+(64*(Math.cos(ang))), 16, 16, orange);
Delay(15);
FlipScreen();
}