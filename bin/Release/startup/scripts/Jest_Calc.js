var ang = 0;
var orange = new Color(255, 128, 0, undefined);
while(!IsKeyPressed(KEY_Q)){
	while (AreKeysLeft()) {
		var k = GetKey();
		switch (k) {
			case KEY_UP: y -= 16; break;
			case KEY_DOWN: y += 16; break;
		}
	}

ang+=0.1;

Rectangle((GetScreenWidth()/2)+(64*(Math.sin(ang))), (GetScreenHeight()/2)+(64*(Math.cos(ang))), 16, 16, orange);
Delay(15);
FlipScreen();
}