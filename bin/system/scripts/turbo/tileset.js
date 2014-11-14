RequireSystemScript('turbo/bytearray.js');
RequireSystemScript('colors.js');

if(typeof Turbo == "undefined")
    var Turbo = {};

Turbo.LoadTilesetFile = function(path){
    var tileset_file = new RawFile("../map/"+path);
    var tileset_buffer = tileset_file.read(tileset_file.size);

    var tileset_byteview = new Uint8Array(tileset_buffer);
    var tileset_bytearray = ByteArrayFromTypedArray(tileset_byteview);

    return new Turbo.Map(tileset_bytearray);
}


Turbo.Tile = function(array, index, surface){
    if(array.length < index+32)
      throw "Unexpected end of file.";

    this.surface = surface;
    this.image = new Image(surface);

    var at = index++;
    this.animated = array[at++];
    this.animated = Turbo.dByteCat(array[at++], array[at++]);
    this.delay    = Turbo.dByteCat(array[at++], array[at++]);
    /* reserved[1] */at++;

    var obstruction_type = array[at++];
    if(obstruction_type==2){
        segments = new Array(array[at++]);
    }
    else
      segments = [];

    switch(obstruction_type){
    case 2:
        this.loadObstructions =  function(bytearray, index){
            if(bytearray.length < index+(8*segments.length))
              throw "Unexpected end of file.";

            var at = index;
            for(var i in segments){
                segments[i] = [{x:Turbo.dByteCat(array[at++], array[at++]), y:Turbo.dByteCat(array[at++], array[at++])},
                               {x:Turbo.dByteCat(array[at++], array[at++]), y:Turbo.dByteCat(array[at++], array[at++])}];
            }

            return 8*segments.length;

        }
    case 1:
        this.loadObstructions =  function(bytearray, index){
            if(bytearray.length < index+(this.surface.width*this.surface.height))
              throw "Unexpected end of file.";
            var at = index;
            for(var y = 0; y < this.surface.height; y++){
                for(var x = 0; x < this.surface.width; x++){
                    if(bytearray[at++]==1)
                      this.segments.push([{x:x, y:y}, {x:x, y:y}]);
                }
            }
            return this.surface.width*this.surface.height;
        }
    default:
        this.loadObstructions =  function(bytearray, index){
            return 0;
        }
    }

}


Turbo.Tileset = function(bytearray, offset){

    if(typeof offset == "undefined")
      offset = 0;

    this.signature = ".rts";

    if(bytearray.length<offset+0xFF) // Size of the header
      throw "Argument 0 is to short to be a tileset.";

    var sig_array = bytearray.slice(offset+0x00, offset+0x04);
    var sig_string = CreateStringFromByteArray(sig_array);

    if((sig_string!=this.signature) && (offset==0))
      throw "Signature is invalid. Expected " + this.signature + " at " +at+ ", found " + sig_string;

    var at = offset+0x04;

    this.version = Turbo.dByteCat(bytearray[at++], bytearray[at++]);
    this.tiles   = new Array(Turbo.dByteCat(bytearray[at++], bytearray[at++]));
    this.tile_surfaces = new Array(this.tiles.length);
    this.width   = Turbo.dByteCat(bytearray[at++], bytearray[at++]);
    this.height  = Turbo.dByteCat(bytearray[at++], bytearray[at++]);
    this.BPP     = Turbo.dByteCat(bytearray[at++], bytearray[at++]);

    // The WxEditor did this in 1.4, and it SUCKS.
    if(this.BPP==0)
      this.BPP=32;

    this.compression = bytearray[at++];
    this.obstructions = bytearray[at++];

    while(at<0x100){
        if(bytearray[++at]!=0){
            throw "Expected clear data at header offset " + at;
        }
    }

    at= 0x100;

    switch(this.BPP){ // Determine how to parse color information.
        case 8:
            this.colorEquation = function(array, i){
                var cbyte = array[i];
                return new Color(cbyte & 0x7, cbyte & 0x18, cbyte &  0xE0, 0xFF);
            }
        break;
        case 16:
            this.colorEquation = function(array, i){
                var cshort = Turbo.dByteCat(bytearray[i], bytearray[i+1]);
                return new Color(cshort & 0x001F, cshort & 0x03E0, cshort & 0x7C00, cshort & 0x7000);
            }
        break;
        case 24:
            this.colorEquation = function(array, i){
                return new Color(array[i], array[i+1], array[i+2], 0xFF);
            }
        default:

        if((this.BPP>32) && (this.BPP%32==0)){
            this.colorEquation = function(array, i){
                var red   = 0;
                var green = 0;
                var blue  = 0;
                var alpha = 0;
                for(var e = 0; e < this.BPP/8; e+=4){
                    red   |= array[e];
                    blue  |= array[e+1];
                    green |= array[e+2];
                    alpha |= array[e+3];

                    red<<=8;
                    green<<=8;
                    blue<<=8;
                    alpha<<=8;

                }
                return new Color(red, blue, green, alpha);
            }
        }
        else if((this.BPP>=32)&&((this.BPP-32)%32==0)){ // Handles BPP==32.
            this.colorEquation = function(array, i){
                var e = 0;
                var red   = array[e++];
                var green = array[e++];
                var blue  = array[e++];
                var alpha = array[e++];

                for(; e < this.BPP/8; e+=3){
                    red   |= array[e];
                    blue  |= array[e+1];
                    green |= array[e+2];

                    red<<=8;
                    green<<=8;
                    blue<<=8;

                }
            }
        }
        else if((this.BPP > 24) &&(this.BPP%24==0)){
            this.colorEquation = function(array, i){
                var e = 0;
                var red   = array[e++];
                var green = array[e++];
                var blue  = array[e++];
                var alpha = 0xFF;
                for(; e < this.BPP/8; e+=3){
                    red   |= array[e];
                    blue  |= array[e+1];
                    green |= array[e+2];

                    red<<=8;
                    green<<=8;
                    blue<<=8;

                }
            }
        }
        else
          throw "Unhandled BPP of "+this.BPP;
    } // /Determine how to parse color information.

    if(bytearray.length < this.tiles.length*(this.BPP/8)*this.width*this.height)
      throw "Unexpected end of file.";

    for(var i = 0; i<this.tile_surfaces.length; i++){
        this.tile_surfaces[i] = new Surface(width, height, Black);
        for(var y = 0; y < this.height; y++){
            for(var x = 0; x < this.width; x++){
                this.tile_surfaces.setPixel(x, y, this.colorEquation(bytearray, at));
                at+=this.BPP/8;
            }
        }
    }

    // Load tile headers
    for(var i = 0; i<this.tiles.length; i++){
        this.tiles[i] = new Turbo.Tile(bytearray, at, this.tile_surfaces[i]);
        at+=32;
    }

    // Load tile obstructions
    for(var i = 0; i<this.tiles.length; i++){
        at+=this.tiles[i].loadObstructions(bytearray, at);
    }

}
