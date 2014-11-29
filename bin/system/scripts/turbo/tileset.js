RequireSystemScript('turbo/segment.js');
RequireSystemScript('turbo/bytearray.js');
RequireSystemScript('colors.js');
RequireSystemScript('turbo/format.js');

if(typeof Turbo == "undefined")
    var Turbo = {};

Turbo.LoadTilesetFile = function(path){

    return new Turbo.Tileset(new Turbo.FileReader(new RawFile("../maps/"+path)));

}

Turbo.TileScheme = Turbo.LoadSystemScheme("tile.json");
Turbo.TilesetScheme = Turbo.LoadSystemScheme("tileset.json");

Turbo.Tile = function(stream, image, size){

    this.__proto__ = Turbo.ReadBinaryObject(stream, Turbo.TileScheme.header);

    this.image = image;

    // this.segments is initialized depending on the Obstruction type, since we already know its
    // length with obs type 2, but we need to fully iterate the bytemap for type 1.
    switch(this.obstruction_type){
    case 2:
        this.segments = new Array(this.num_segments);
        this.loadObstructions = function(stream){
            for(var i = 0; i< this.segments.length; i++){
                this.segments[i] = Turbo.ReadBinaryObject(stream, Turbo.SegmentScheme.data);
            }
        }
    break;
    case 1: // Bytemapped obstructions
        this.segments = [];
        this.loadObstructions = function(stream){

            // Ensure that the stream is big enough.
            if(stream.size() - stream.tell() < (this.size.width*this.size.height))
              throw "Unexpected end of file.";

            for(var y = 0; y < this.size.height; y++){
                for(var x = 0; x < this.size.width; x++){
                // A value other than 0 indicates that this point (1px by 1px) is obstructed.
                // This is an ugly way to represent it, but this is a very deprecated feature
                // to begin with.
                    if(stream.readByte())
                      this.segments.push([{x:x, y:y}, {x:x, y:y}]);
                }
            }
        }
    break;
    case 0:
        this.segments = [];
        this.loadObstructions = function(stream){

        }
    break;
    default:
        throw "Unknown obstruction type number " + this.obstruction_type;
    }

}


Turbo.Tileset = function(stream){

    // Load tileset header as base.
    this.__proto__ = Turbo.ReadBinaryObject(stream, Turbo.TilesetScheme.header);

    if(this.signature!=Turbo.TilesetScheme.signature)
      throw "Signature must be "+Turbo.TilesetScheme.signature+" instead of " + this.signature;

    if(this.num_tiles==0)
      throw "Zero tiles in tileset";

    if(this.BPP==0)
      throw "BPP of 0";

    this.tiles         = new Array(this.num_tiles);
    this.tile_surfaces = new Array(this.num_tiles);

    switch(this.BPP){ // Determine how to parse color information.
        case 32: // Technically this is the only thing we need to support
            this.colorEquation = function(stream){
                return new Color(stream.readByte(), stream.readByte(), stream.readByte(), stream.readByte());
            }
        break;
        case 8:
            this.colorEquation = function(stream){ // R3G2B3
                var cbyte = stream.readByte();
                return new Color(cbyte & 0x7, cbyte & 0x18, cbyte &  0xE0, 0xFF);
            }
        break;
        case 16:
            this.colorEquation = function(stream){ // R5G5B5A1
                var cshort = stream.readShort();

                // Bit shift 0x01F (Which is 0b00011111, five 1's) over so that it lines up with component n.
                // Bitwise and it with the value.
                // Shift it back to put it at the least-significant 5 bits. This is the component value.
                // Multiply by eight (aka bitshift left by 3) to convert to RGBA32-style.
                var el_calc = function(n){return (cshort & ((0x001F << (n*5)))>>(n*5))<<3;}  // v Check if the sixteenth bit is set. This is the one-bit alpha channel.
                return new Color(el_calc(1), el_calc(2), el_calc(3), (cshort & 0x8000)?0xFF:0);
            }
        break;
        case 24:
            this.colorEquation = function(stream){
                return new Color(stream.readByte(), stream.readByte(), stream.readByte(), 0xFF);
            }
        break;
        default:

        if((this.BPP>32) && (this.BPP%32==0)){ // Balanced multibyte RGBA.
            this.colorEquation = function(stream){
                var components = [0, 0, 0, 0];
                for(var i in components)
                  for(var e = 0; e< (this.BPP>>5); e++){
                      components[i] = stream.readByte();
                      components[i] <<= 8;
                  }
                return new Color(components[0], components[1], components[2], components[3]);
            }
        }
        else
          throw "Unhandled BPP of "+this.BPP;
    } // /Determine how to parse color information.

    if(stream.size() - stream.tell() < this.tiles.length*(this.BPP/8)*this.width*this.height)
      throw "Unexpected end of file.";

    if((this.BPP==32) && (typeof ImageFromArrayBuffer=="function")){
        var tile_images = new Array(this.num_tiles);
        for(var i = 0; i< this.num_tiles; i++){
            var data = stream.read((this.BPP/8)*this.width*this.height);
            tile_images[i] = ImageFromArrayBuffer(this.width, this.height, data);
        }
        // Load tile headers
        for(var i = 0; i<this.tiles.length; i++){
            this.tiles[i] = new Turbo.Tile(stream, tile_images[i], this);
        }
    }
    else {
        // Slight performance drain while the nested conditinal gets spun up, but it's less verbose.
        for(var i = 0; i<this.tile_surfaces.length; i++){
            if((this.BPP==32) && (typeof SurfaceFromArrayBuffer=="function")){
                var data = stream.read((this.BPP/8)*this.width*this.height);

                // SurfaceFromArrayBuffer is evil, and you should not use it outside the Turbo runtime.
                // If you MUST (or are just a jerk and just don't want to listen), remember that it
                //   poisons the ArrayBuffer you give it.
                this.tile_surfaces[i] = SurfaceFromArrayBuffer(this.width, this.height, data);

            }
            else{ // Fall back to slow (but effective) methods since we can't hotwire the surface.
                var sf = new Surface(this.width, this.height, Black);

                for(var y = 0; y < this.height; y++){
                    for(var x = 0; x < this.width; x++){
                        var color =  this.colorEquation(stream);
                        this.tile_surfaces[i].setPixel(x, y, color);
                    }
                }
            }
        }

        // Load tile headers
        for(var i = 0; i<this.tiles.length; i++){
            this.tiles[i] = new Turbo.Tile(stream, new Image(this.tile_surfaces[i]), this);
        }
    }
    // Load tile obstructions
    for(var i = 0; i<this.tiles.length; i++){
        this.tiles[i].loadObstructions(stream);
    }

    this.RedrawTiles = function(){
        for(var i in this.tiles){
            this.tile_surfaces[i] = this.tiles[i].surface;
            this.tile[i].image = new Image(this.tiles[i].surface);
        }
    }

}
