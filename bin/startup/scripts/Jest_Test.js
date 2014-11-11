var sys_font = GetSystemFont();

function Alert(text) {
    Rectangle(0, 0, GetScreenWidth(), GetScreenHeight(), new Color(0, 0, 0, 0));
    sys_font.drawText(0, 0, text);
    FlipScreen();
    GetKey();
}

function TestRawSpeed() {
    var t = GetTime();
    for (var i = 0; i < 1000000000; ++i) { }
    t = GetTime() - t;
    Alert(t);
}

function game(){
TestRawSpeed();
}
