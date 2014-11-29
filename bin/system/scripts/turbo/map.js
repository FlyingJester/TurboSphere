RequireSystemScript('turbo/format.js');
RequireSystemScript('turbo/tileset.js');
RequireSystemScript('turbo/person.js');
RequireSystemScript('turbo/segment.js');
RequireSystemScript('turbo/zone.js');
RequireSystemScript('turbo/bytearray.js');
RequireSystemScript('turbo/file_reader.js');
RequireSystemScript('turbo/for_each.js');
RequireSystemScript('turbo/object_assign.js');

if(typeof Turbo == "undefined")
    var Turbo = {};
if(typeof Turbo == "undefined")
    Turbo.Classic = {};
if(typeof Turbo.default_shader == "undefined")
    Turbo.default_shader = GetDefaultShaderProgram();

Turbo.MapScheme   = Turbo.LoadSystemScheme("map.json");
Turbo.EntityScheme= Turbo.LoadSystemScheme("entity.json");

Turbo.LoadMapFile = function(path){

    return new Turbo.Map(new Turbo.FileReader(new RawFile("../maps/"+path)));
}

Turbo.Map = function(stream, compat){

    if(typeof compat == "undefined")
      compat = true;


    this.fps = 60;

    // This is the actual blitting coordinates for the upper-left most part of the screen.
    //  The results of all camera location calculations goes in here.
    this.camera = {x:0, y:0};

    this.unsetCamera = function(){
        this.camera_person = null;
    }

    this.unsetInput = function(){
        this.input_person = null;
    }

    this.unsetCamera();
    this.unsetInput();

    // Load map header as base.
    this.__proto__ = Turbo.ReadBinaryObject(stream, Turbo.MapScheme.header);

    // The stream should at least _say_ that it's a Sphere map.
    if(this.signature != Turbo.MapScheme.signature)
        throw "Bad signature. Should be " + Turbo.MapScheme.signature + " instead of " + this.signature;

    // Load map strings.
    this.strings = Turbo.ReadBinaryObject(stream, Turbo.MapScheme.strings);

    // Load functions from strings.
    this.functions = [];
    for(var i = 0; i<this.num_strings; i++){
        if(i<3)
            continue;

        this.functions.push(function(){eval(this.strings[Turbo.MapScheme.strings[i].name]);});
    }

    this.layers = new Array(this.num_layers);
    for(var i = 0; i< this.num_layers; i++){

        // Load the layer header.
        this.layers[i] = Turbo.ReadBinaryObject(stream, Turbo.MapScheme.layer);

        // Load layer tile data.
        // *2 because layer elements are 16-bits long.
        this.layers[i].field = stream.read(this.layers[i].width*this.layers[i].height*2);

        // Parse flags so we can kill the messenger.
        //   Magic equations from sphere/docs/internal/map.rmp.txt
        this.layers[i].visibile = (~this.layers[i].flags) & 1;
        this.layers[i].has_parallax = this.layers[i].flags & 2;
        this.layers[i].mask = new Color(0xFF, 0xFF, 0xFF);
        this.layers[i].shader = Turbo.default_shader;

        // Pop off the bit field.
        delete this.layers[i].flags;

        // Load all segments.
        this.layers[i].segments = new Array(this.layers[i].num_segments);
        for(var e = 0; e < this.layers[i].num_segments; e++){
            this.layers[i].segments[e] = Turbo.ReadBinaryObject(stream, Turbo.SegmentScheme.data);
        } // For var e ... layers[i].segments
    } // For var i ... layers

    this.entities = new Array(this.num_entities);
    for(var i = 0; i< this.entities.length; i++){

        // Load the layer header.
        this.entities[i] = Turbo.ReadBinaryObject(stream, Turbo.EntityScheme.header);
    } // For var i ... entities

    this.zones = new Array(this.num_zones);

    for(var i = 0; i<this.zones.length; i++){
        //Abort(i);

        var position = Turbo.ReadBinaryObject(stream, Turbo.SegmentScheme.data)
        this.zones[i] = Turbo.ReadBinaryObject(stream, Turbo.ZoneScheme.header);
        // Composite the segment into the zone.
        Object.assign(this.zones[i], position);

        // Set up the script for the zone.
        this.zones[i].onActivate = function(){eval(zones[i].scripts);};
        this.zones[i].cursor = 0;

        // This callback performs a ste
        this.zones[i].walk = function(){
            this.cursor++;
            if(this.cursor==this.steps){
                this.cursor=0;
                this.current_zone = this.zones[i];
                this.onActivate();
            }
        }

        // calls walk() if the given coordinates are inside the bouding box of the zone.
        this.zones[i].tryWalk = function(_x, _y){
            if((_x>this.x1) &&
               (_x<this.x2) &&
               (_y>this.y1) &&
               (_y<this.y2)){
                this.walk();
            }
        }
    } // for var i ... zones

    if(this.strings.tileset_file){
        this.tileset = Turbo.LoadTilesetFile(this.strings.tileset_file);
    }
    else{
        this.tileset = new Turbo.Tileset(stream);
    }

    this.calculateLayer = function(i){
        var shapes = [];

        for(var y = 0; y<this.layers[i].height; y++){
            for(var x = 0; x<this.layers[i].width; x++){
                var at = x+(y*this.layers[i].width);

                if(typeof this.layers[i].field[at] == "number" && this.layers[i].field[at]>this.tileset.tiles.length)
                    throw "Tile number " + this.layers[i].field[at] + " at " + at+ " Out of range";

                var x1 = x*this.tileset.width;
                var x2 = x1+this.tileset.width;
                var y1 = y*this.tileset.width;
                var y2 = y1+this.tileset.width;

                shapes.push(new Shape([
                    {x:x1, y:y1},
                    {x:x2, y:y1},
                    {x:x2, y:y2},
                    {x:x1, y:y2}],
                  this.tileset.tiles[this.layers[i].field[at]].image)
                );

            }
        }

        this.layers[i].group = new Group(shapes, this.layers[i].shader);

    }

    this.calculateMap = function(){
        for(var i in this.layers){
            this.calculateLayer(i);
        }
    }

    this.calculateCamera = function(){
        if(this.camera_person){
            this.camera.x = this.camera_person.x;
            this.camera.y = this.camera_person.y;
        }
    }

    this.drawLayer = function(i){
        this.calculateCamera();
        this.layers[i].group.x = this.camera.x;
        this.layers[i].group.y = this.camera.y;
        this.layers[i].group.draw();
    }

    this.drawMap = function(){
        for(var i in this.layers){
            // TODO: render Persons.
            this.drawLayer(i);
        }

    }

}


