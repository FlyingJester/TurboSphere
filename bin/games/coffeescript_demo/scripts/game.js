RequireScript("coffee-script.js");
RequireSystemScript("turbo/bytearray.js");

var coffee_file = new RawFile("../scripts/game.coffee");
var coffee_src  = coffee_file.read(coffee_file.size);
var coffee_byteview = new Uint8Array(coffee_src);
var coffee_bytearray = ByteArrayFromTypedArray(coffee_byteview);

function ShouldQuit(){return IsKeyPressed(KEY_Q);}

var coffee_game = {speed:2};

coffee_game.Control = function(item){

    var dirs = 0;
    if(IsKeyPressed(KEY_LEFT))
        dirs++;
    if(IsKeyPressed(KEY_RIGHT))
        dirs++;
    if(IsKeyPressed(KEY_UP))
        dirs++;
    if(IsKeyPressed(KEY_DOWN))
        dirs++;

    var speed = this.speed;

    switch(dirs){
    case 1:
        speed = this.speed;
        break;
    case 2:
        speed = Math.sqrt(this.speed);
        break;
    case 3:
        speed = this.speed;
        break;
    case 4:
        return;
    }

    if(IsKeyPressed(KEY_LEFT))
        item.x-=speed;
    if(IsKeyPressed(KEY_RIGHT))
        item.x+=speed;
    if(IsKeyPressed(KEY_UP))
        item.y-=speed;
    if(IsKeyPressed(KEY_DOWN))
        item.y+=speed;
}

coffee_game.Bound = function(item){
    item.x = Math.max(0, item.x);
    item.y = Math.max(0, item.y);
    item.x = Math.min(GetScreenWidth()-32, item.x);
    item.y = Math.min(GetScreenHeight()-32, item.y);
}

function game(){
    (new Function(CoffeeScript.compile(CreateStringFromByteArray(coffee_bytearray))))();
    coffee_game.main();
}
