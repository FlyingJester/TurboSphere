RequireSystemScript("turbo/bytearray.js");
RequireSystemScript("turbo/format.js");
RequireSystemScript("turbo/map.js");
RequireSystemScript("turbo/tileset.js");

if(typeof Turbo == "undefined")
    var Turbo = {};
Turbo.Classic = Turbo.Classic || {};

Turbo.Entity = function(x, y, layer, name, destroy){
    this.x = x; this.y = y;
    this.layer = layer;
    this.name = name;
    this.destroy_on_map_change = destroy;
}

Turbo.Person = function(x, y, layer, name, destroy, spriteset){

    if(typeof destroy == "undefined")
        destroy = true;

    this.speed = 1.0;
    this.spriteset = spriteset;

    this.__proto__ = new Turbo.Entity (x, y, layer, name, destroy);

    this.onCreate  = function(){};
    this.onDestroy = function(){};
    this.onTalk    = function(){};
    this.onTouch   = function(){};
    this.onGenerate= function(){};
}

Turbo.Trigger = function(x, y, layer, name){
    this.__proto__ = new Turbo.Entity(x, y, layer, name, true);
    this.onTrigger = function(){};
}

// Remember to increment the cursor by length.
// Length includes the size value.
Turbo.Classic.readString = Turbo.Classic.readString || function(bytearray, at){

    if(bytearray.length<at)
      throw "Unexpected end of file.";

    var len = Turbo.dByteCat(bytearray[at++], bytearray[at++]);

    if(bytearray.length<at+len)
      throw "Unexpected end of file.";

    var str = bytearray.slice(at, at+len);

    var string = CreateStringFromByteArray(str);

    return {length:len+2, string:string};

}

Turbo.LoadEntity = function(array, i, entity){
    var at = i;

    var type = Turbo.dByteCat(array[at++], array[at++]);

    var data = {x:Turbo.dByteCat(array[at++], array[at++]),
                   y:Turbo.dByteCat(array[at++], array[at++]),
                   layer:Turbo.dByteCat(array[at++], array[at++])};

    /*reserved[8]*/ at+=8;

    var script_names = ['onCreate', 'onDestroy', 'onTalk', 'onTouch', 'onGenerate'];

    var string = Turbo.Classic.readString(array, at);
    at+=string.length;
    var name = string.string;

    var string = Turbo.Classic.readString(array, at);
    at+=string.length;
    var spriteset = string.string;

    if(type==1){
        entity = new Turbo.Person(data.x, data.y, data.layer, name, true, spriteset);

        for(var j in script_names){
            var string = Turbo.Classic.readString(array, at);
            at+=string.length;
            entity[script_names[j]] = function(){eval(string.string);};
        }

        /*reserved[16]*/ at+=16;
    }
    else{
        entity = new Turbo.Person(data.x, data.y, data.layer, name, true, spriteset);

        var string = Turbo.Classic.readString(array, at);
        at+=string.length;
        entity.onTrigger = function(){eval(string.string);};
        entity.onTrigger = function(){eval(string.string);};

    }



    return i-at;
}
