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
Turbo.ObstructionDebugColor = new Color(255, 0, 255);

Turbo.Tile = function(stream, tex_coords, size, surface, debug_obstructions){

    if(typeof debug_obstructions == "undefined")
        debug_obstructions = false;

    {
        var header = Turbo.ReadBinaryObject(stream, Turbo.TileScheme.header);
        for(var i in header){
            this[i] = header[i];
        }
    }

    this.surface = surface;

    this.tex_coords = tex_coords;

    this.unobstructed = this.num_segments==0;

    // this.segments is initialized depending on the Obstruction type, since we already know its
    // length with obs type 2, but we need to fully iterate the bytemap for type 1.
    switch(this.obstruction_type){
    case 2:
        this.segments = new Array(this.num_segments);
        this.loadObstructions = function(stream){
            for(var i = 0; i< this.segments.length; i++){
                this.segments[i] = Turbo.ReadBinaryObject(stream, Turbo.SegmentScheme.data);
                if(debug_obstructions)
                    surface.line(this.segments[i].x1, this.segments[i].y1, this.segments[i].x2, this.segments[i].y2, Turbo.ObstructionDebugColor);
            }
        }
    break;
    case 1: // Bytemapped obstructions
        throw "Don't do that.";
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
    //this.__proto__ = Turbo.ReadBinaryObject(stream, Turbo.TilesetScheme.header);
    {
        var header = Turbo.ReadBinaryObject(stream, Turbo.TilesetScheme.header);
        for(var i in header){
            this[i] = header[i];
        }
    }

    if(this.signature!=Turbo.TilesetScheme.signature)
      throw "Signature must be "+Turbo.TilesetScheme.signature+" instead of " + this.signature;

    if(this.num_tiles==0)
      throw "Zero tiles in tileset";

    if(this.BPP==0)
      throw "BPP of 0";

    this.tiles        = new Array(this.num_tiles);
    var tile_surfaces = new Array(this.num_tiles);

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


    // Slight performance drain while the nested conditinal gets spun up, but it's less verbose.
    for(var i = 0; i<tile_surfaces.length; i++){
        if((this.BPP==32) && (typeof SurfaceFromArrayBuffer=="function")){
            var data = stream.read((this.BPP/8)*this.width*this.height);

            // We love you, SpiderMonkey
            tile_surfaces[i] = SurfaceFromArrayBuffer(this.width, this.height, data.buffer);
        }
        else{ // Fall back to slow (but effective) methods since we can't hotwire the surface.
            var sf = new Surface(this.width, this.height, Black);

            for(var y = 0; y < this.height; y++){
                for(var x = 0; x < this.width; x++){
                    var color =  this.colorEquation(stream);
                    tile_surfaces[i].setPixel(x, y, color);
                }
            }
        }
    }

    // Create the surface for the atlas.
    this.surface_atlas = new Surface(this.width * this.num_tiles, this.height, new Color(0, 0, 0, 0));

    // Load tile headers
    for(var i = 0; i<this.tiles.length; i++){

        var x1 = i*this.width;
        var x2 = (i+1)*this.width;

        this.tiles[i] = new Turbo.Tile(stream,
            [{u:x1/this.surface_atlas.width, v:0.0}, {u:x2/this.surface_atlas.width, v:0.0},
             {u:x2/this.surface_atlas.width, v:1.0}, {u:x1/this.surface_atlas.width, v:1.0}], this, tile_surfaces[i], false);
    
        this.tiles[i].loadObstructions(stream);
        
        this.surface_atlas.blitSurface(tile_surfaces[i], x1, 0);
    
    }

    this.calculateTileAtlas = function(){
        
        this.tiles.forEach(function(i){
            this.blitSurface(i.surface, i.tex_coords.x1*this.width, 0);
        }, this.surface_atlas)
        
        this.image_atlas = new Image(this.surface_atlas);
        
    }
    
    this.calculateTileAtlas();

}
