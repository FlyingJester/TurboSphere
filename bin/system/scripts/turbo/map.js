RequireSystemScript('turbo/tileset.js');
RequireSystemScript('turbo/bytearray.js');

if(typeof Turbo == "undefined")
    var Turbo = {};
Turbo.Classic = Turbo.Classic || {};

Turbo.LoadMapFile = function(path){
    var map_file = new RawFile("../maps/"+path);
    var map_buffer = map_file.read(map_file.size);

    var map_byteview = new Uint8Array(map_buffer);
    var map_bytearray = ByteArrayFromTypedArray(map_byteview);

    return new Turbo.Map(map_bytearray);
}

// Remember to increment the cursor by length.
// Length includes the size value.
Turbo.Classic.readString = function(bytearray, at){

    if(bytearray.length<at)
      throw "Unexpected end of file."

    var len = Turbo.dByteCat(bytearray[at++], bytearray[at++]);

    if(bytearray.length<at+len)
      throw "Unexpected end of file.";

    var str = bytearray.slice(at, at+len);

    var string = CreateStringFromByteArray(str);

    return {length:len+2, string:string};

}


Turbo.Map = function(bytearray, offset){

    if(typeof offset == "undefined")
      offset = 0;

    this.signature = ".rmp";

    if(bytearray.length<0x100) // Header plus the first string byte.
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
    this.strings = new Array(Turbo.dByteCat(bytearray[at++], bytearray[at++]));
    this.function = new Array(this.strings.length-3);
    this.zones   = new Array(bytearray[at++]);

    while(at<0x100){
        if(bytearray[++at]!=0){
            throw "Expected clear data at header offset " + at;
        }
    }

    at = 0x100;

    for(var i in this.strings){
        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        this.strings[i] = string.string;
    }

    for(var i in this.functions){
        this.functions[i] = function(){eval(this.strings[i+3]);};
    }



    for(var i = 0; i< this.layers.length; i++){

        if(bytearray.length<at+29) // Up to the first byte of the name string.
          throw "Unexpected end of file.";

        var size = {w:bytearray[at++], h:bytearray[at++]};
        var flags= bytearray[at++];

        var segmentbuffer = new Float32Array(bytearray.buffer.slice(at, at+16));

        var parallax = {
            x:segmentbuffer[0],
            y:segmentbuffer[1]
        };
        var scrolling= {
            x:segmentbuffer[2],
            y:segmentbuffer[3]
        };

        at+=16;

        var segments = new Array(Turbo.qByteCat(bytearray[at++], bytearray[at++], bytearray[at++], bytearray[at++]));

        /*reflective, reserved[3]*/ at+=4;


        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        var name = string.string;

        var data = new Uint16Array(bytearray, at, size.w*size.h*2);

        at+=size.w*size.h*2;

        for(var e in segments){

            if(bytearray.length<at+8)
              throw "Unexpected end of file.";

            var segmentbuffer = new Uint32Array(bytearray.buffer.slice(at, at+16));

            segments[e] = [{x:segmentbuffer[0], y:segmentbuffer[1]},
                           {x:segmentbuffer[2], y:segmentbuffer[3]}];

            at+=16;
        }

        this.layers[i] = {size:size, flags:flags, parallax:parallax, scrolling:scrolling, name:name, segments:segments, field:data};

    }

    for(var i = 0; i< this.entities.length; i++){
        this.entities[i] = {x:Turbo.dByteCat(bytearray[at++], bytearray[at++]),
                       y:Turbo.dByteCat(bytearray[at++], bytearray[at++]),
                       layer:Turbo.dByteCat(bytearray[at++], bytearray[at++])};

        var type = Turbo.dByteCat(bytearray[at++], bytearray[at++]);

        /*reserved[8]*/ at+=8;


        var script_names = ['on_create', 'on_destroy', 'on_talk', 'on_touch', 'on_generate'];

        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        this.entities[i].name = string.string;

        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        this.entities[i].spriteset = string.string;

        switch(type){
        case 1:

            for(var j in script_names){
                var string = Turbo.Classic.readString(bytearray, at);
                at+=string.length;
                this.entities[i][script_names[j]] = function(){eval(string.string);};
            }

        break;
        case 2:

            for(var j in script_names){
                var string = Turbo.Classic.readString(bytearray, at);
                at+=string.length;
                this.entities[i][script_names[j]] = function(){};
            }

        }

        /*reserved[16]*/ at+=16;

        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        this.entities[i].trigger_script = function(){eval(string.string);};

    }

    for(var i = 0; i< this.zones.length; i++){

        if(bytearray.length < at+12+4)
          throw "Unexpected end of file.";

        zonebuffer = new Uint16Array(bytearray.buffer.slice(at, at+12));

        this.zones[i] = {location:[{x:zonebuffer[0],
                               y:zonebuffer[1]},
                              {x:zonebuffer[2],
                               y:zonebuffer[3]}],
                               layer:zonebuffer[4],
                               steps:zonebuffer[5]};

        /*Reserved[4], and the twelve we pulled from the zonebuffer.*/at+=4+12;

        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        this.zones[i].on_touch = function(){eval(string.string);};
        this.zones[i].cursor = 0;

        this.zones[i].walk = function(){
            this.cursor++;
            if(this.cursor==this.steps){
                this.cursor=0;
                this.on_touch();
            }
        }

        this.zones[i].tryWalk = function(_x, _y){
            if((_x>this.location[0].x) &&
               (_x<this.location[1].x) &&
               (_y>this.location[0].y) &&
               (_y<this.location[1].y)){
                this.walk();
            }
        }
    }

    if(this.strings[0]){
        this.tileset = Turbo.LoadTilesetFile(strings[0]);
    }
    else{
        this.tileset = new Turbo.Tileset(bytearray, at);
    }

}
