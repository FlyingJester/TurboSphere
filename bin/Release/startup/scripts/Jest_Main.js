RequireSystemScript("colors.js");

var prog = LoadShaderProgram("system.shade");

UseShaderProgram(prog);
function game(){
    while(!IsKeyPressed(KEY_Q)){
    var t = GetTime();
        if(IsKeyPressed(KEY_W))
            UseCompositeShaderProgram(prog);
        Rectangle(16, 16, 64, 64, Red);
        FlipScreen();
        Rectangle(0, 0, GetTime()-t, 16, Green);
        //Delay(10);
    }
}
