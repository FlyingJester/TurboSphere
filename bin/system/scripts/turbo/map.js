RequireSystemScript('turbo/tileset.js');
RequireSystemScript('turbo/bytearray.js');

if(typeof Turbo == "undefined")
    var Turbo = {};
if(typeof Turbo.Classic == "undefined")
    var Turbo.Classic = {};

Turbo.OpenMapFile = function(path){
    var map_file = new RawFile("../maps/"+path);
    var map_buffer = map_file.read(map_file.size());

    var map_byteview = new Uint8Array(map_buffer);
    var map_bytearray = ByteArrayFromTypedArray(byteview);

    return new Turbo.Map(map_bytearray);
}


Turbo.dByteCat = function(first, second){
    return (first<<8) | second;
}


Turbo.qByteCat = function(first, second, third, fourth){
    return (Turbo.dByteCat(first, second) << 16) | Turbo.dByteCat(third, fourth)
}


// Remember to increment the cursor by length.
// Length includes the size value.
Turbo.Classic.readString = function(bytearray, at){

    if(bytearray.length()<at)
      throw "Unexpected end of file."

    var len = Turbo.dByteCat(bytearray[at++], bytearray[at++]);

    if(bytearray.length()<at+len)
      throw "Unexpected end of file.";

    var str = bytearray.slice(at, at+len);
    var string = CreateStringFromByteArray(str);

    return {length:len+2, string:string};

}


Turbo.Map = function(bytearray){

    this.signature = ".rmp";

    if(bytearray.length()<0x100) // Header plus the first string byte.
      throw "Argument 0 is to short to be a map. Or a stormtrooper.";

    var sig_array = bytearray.slice(0x00, 0x04);
    var sig_string = CreateStringFromByteArray(sig_array);

    if(sig_string!=this.signature)
      throw "Signature is invalid. Expected " + this.signature + ", found " + sig_string;

    var at = 0x04;

    this.version = Turbo.dByteCat(bytearray[at++], bytearray[at++]);
    this.type    = bytearray[at++];
    this.layers  = new Array(bytearray[at++]);
    /*reserved*/ at++;
    this.entities= new Array(bytearray[at++]);
    this.startx  = Turbo.dByteCat(bytearray[at++], bytearray[at++]);
    this.starty  = Turbo.dByteCat(bytearray[at++], bytearray[at++]);
    this.start_layer     = bytearray[at++];
    this.start_direction = bytearray[at++];
    this.functions = new Array(Turbo.dByteCat(bytearray[at++], bytearray[at++]));
    this.zones   = new Array(bytearray[at++]);

    at = 0xFF;

    for(var i in functions){
        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        functions[i] = function(){eval(string.string);};
    }

    for(var i in layers){

        if(bytearray.length()<at+29) // Up to the first byte of the name string.
          throw "Unexpected end of file.";

        var size = {w:bytearray[at++], h:bytearray[at++]};
        var flags= bytearray[at++];

        var parallax = {
            x: Turbo.qByteCat(bytearray[at++], bytearray[at++], bytearray[at++], bytearray[at++]),
            y: Turbo.qByteCat(bytearray[at++], bytearray[at++], bytearray[at++], bytearray[at++])
        };
        var scrolling= {
            x: Turbo.qByteCat(bytearray[at++], bytearray[at++], bytearray[at++], bytearray[at++]),
            y: Turbo.qByteCat(bytearray[at++], bytearray[at++], bytearray[at++], bytearray[at++])
        };

        var segments = new Array(Turbo.qByteCat(bytearray[at++], bytearray[at++], bytearray[at++], bytearray[at++]));

        /*reflective, reserved[3]*/ at+=4;


        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        var name = string.string;

        var data = new Uint16Array(bytearray, at, size.w*size.h*2);

        at+=size.w*size.h*2;

        for(var e in segments){

            if(bytearray.length()<at+8)
              throw "Unexpected end of file.";

            segments[e] = [{x:Turbo.QByteCat(bytearray[at++], bytearray[at++], bytearray[at++], bytearray[at++]), y:Turbo.qByteCat(bytearray[at++], bytearray[at++], bytearray[at++], bytearray[at++])},
                           {x:Turbo.qByteCat(bytearray[at++], bytearray[at++], bytearray[at++], bytearray[at++]), y:Turbo.qByteCat(bytearray[at++], bytearray[at++], bytearray[at++], bytearray[at++])}];
        }

        layers[i] = {size:size, flags:flags, parallax:parallax, scrolling:scrolling, name:name, segments:segments, field:data};

    }

    for(var i in entities){
        entities[i] = {x:Turbo.dByteCat(bytearray[at++], bytearray[at++]),
                       y:Turbo.dByteCat(bytearray[at++], bytearray[at++]),
                       layer:Turbo.dByteCat(bytearray[at++], bytearray[at++])};

        var type = Turbo.dByteCat(bytearray[at++], bytearray[at++]);

        /*reserved[8]*/ at+=8;

        switch(type){
        case 1:

            var script_names = ['on_create', 'on_destroy', 'on_talk', 'on_touch', 'on_generate'];

            var string = Turbo.Classic.readString(bytearray, at);
            at+=string.length;
            entities[i].name = string.string;

            var string = Turbo.Classic.readString(bytearray, at);
            at+=string.length;
            entities[i].spriteset = string.string;



            for(var j in script_names){
                var string = Turbo.Classic.readString(bytearray, at);
                at+=string.length;
                entities[i][script_names[j]] = function(){eval(string.string);};
            }

            /*reserved[16]*/ at+=16;

        break;
        case 2:

            for(var j in script_names){
                entities[i][script_names[j]] = function(){};
            }

            var string = Turbo.Classic.readString(bytearray, at);
            at+=string.length;
            entities[i].on_touch = function(){eval(string.string);};
        }

    }

    for(var i in zones){
        zones[i] = {location:[{x:Turbo.dByteCat(bytearray[at++], bytearray[at++]),
                               y:Turbo.dByteCat(bytearray[at++], bytearray[at++])},
                              {x:Turbo.dByteCat(bytearray[at++], bytearray[at++]),
                               y:Turbo.dByteCat(bytearray[at++], bytearray[at++])}],
                               layer:Turbo.dByteCat(bytearray[at++], bytearray[at++]),
                               steps:Turbo.dByteCat(bytearray[at++], bytearray[at++])};
        at+=4;

        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        zones[i].on_touch = function(){eval(string.string);};
        zones[i].cursor = 0;

        zones[i].walk = function(){
            this.cursor++;
            if(this.cursor==this.steps){
                this.cursor=0;
                this.on_touch();
            }
        }

        zones[i].tryWalk = function(_x, _y){
            if((_x>this.location[0].x) &&
               (_x<this.location[1].x) &&
               (_y>this.location[0].y) &&
               (_y<this.location[1].y)){
                this.walk();
            }
        }
    }

}
