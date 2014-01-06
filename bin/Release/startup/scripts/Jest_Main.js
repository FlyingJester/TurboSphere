RequireSystemScript("colors.js");
var im = new Image("sphere.png");
var im2 = new Image("test1.png");
var im3 = new Image("test.bmp");
//var prog = LoadShaderProgram("system.shade");
//UseShaderProgram(prog);
function game(){
    var lol = [];

    while(!IsKeyPressed(KEY_Q)){
        var t = GetTime();
        Delay(0);
        var i = 0;
        for(var x = 0; x<GetScreenWidth();){
            for(var y = 0; y<GetScreenWidth();){
                if((i)%2)
                    im.blit(x, y);
                else{
                    im2.blit(x, y);
                    im3.blit(x+16, y);
                    im3.blit(x, y+16);
                    im2.blit(x+16, y+16);
                }
                y+=im.height;
                i++;
            }
            x+=im.width;

        }
        Rectangle(0, 0, GetScreenWidth(), GetScreenHeight(), new Color(0, 0, 0, 128));
        if(IsKeyPressed(KEY_W))
            GarbageCollect();
        Rectangle(16, 16, 64, 64, Red);
        var l = new Color(10, 10, 10, 0);
        Rectangle(0, 0, GetTime()-t, 16, Green);
        im.blit(GetMouseX(), GetMouseY());

        Rectangle(GetMouseX(), GetMouseY(), 8, 8, Yellow);

        FlipScreen();
    }
}
