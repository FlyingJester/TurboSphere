RequireSystemScript('turbo/format.js');

if(typeof Turbo == "undefined")
    var Turbo = {};

Turbo.Classic = Turbo.Classic || {};

Turbo.SpritesetScheme= Turbo.LoadSystemScheme("spriteset.json");

Turbo.LoadSpritesetFile = function(path){
    return new Turbo.Spriteset(new Turbo.FileReader(new RawFile("../spritesets/"+path)));
}

Turbo.Spriteset = function(stream){

    // Load spriteset header as base.
    this.__proto__ = Turbo.ReadBinaryObject(stream, Turbo.SpritesetScheme.header);

    if(this.signature != Turbo.SpritesetScheme.signature)
        throw "Bad signature. Should be " + Turbo.SpritesetScheme.signature + " instead of " + this.signature;

    if((this.version!=1)&&(this.version!=2)&&(this.version!=3))
        throw "Unsupported or invalid version " + this.version + ". Version must be 1, 2, or 3";

    // Setup the images and directions based on version.
    // Note that version 3 has a dynamic number of images.
    switch(this.version){
    case 1: // Everything hardcoded.
        this.images = new Array(64);
        this.num_directions = 8;
        break;
    case 2: // Dynamic number of directions (after first 8), simple listed frames/images.
        this.images = [];
        break;
    case 3: // Indexed frame images and fully dynamic directions.
        this.images = new Array(this.num_images);
        break;
    }

    this.directions = new Array(this.num_directions);
    
    // Versions 1 and 2 set things up to look like a version 3 spriteset.
    switch(this.version){
    case 1:
        for(var d = 0; d < 8; d++){
            this.directions[d] = {name:Turbo.SpritesetScheme.directions[d], frames:new Array(8)};
            for(var f = 0; f < 8; f++){
                var frame_num = (d<<3)+f;
                var im = ImageFromArrayBuffer(stream.read(this.width*this.height*4).buffer);
                this.images[frame_num] = im;
                this.directions[d].frames[f] = {image:im, delay:1, frame_index:frame_num, index:frame_num};
            }
        }
        break;
    case 2:
        var frame_num = 0;
        for(var d = 0; d < 8; d++){
            var name = (d<8)?(Turbo.SpritesetScheme.directions[d]):(d+"");
            var direction = Turbo.ReadBinaryObject(stream, Turbo.SpritesetScheme.v2_direction);
            this.directions[d] = {num_frames:direction.num_frames, frames:new Array(direction.num_frames), name:name};

            for(var f = 0; f<direction.num_frames; f++){
                this.directions[d].frames[f] = Turbo.ReadBinaryObject(stream, Turbo.SpritesetScheme.v2_frame);
                this.directions[d].frames[f].index = this.directions[d].frames[f].frame_index = frame_num++;
                var im = ImageFromArrayBuffer(stream.read(this.width*this.height*4).buffer);
                this.images.push(im);
                this.directions[d].frames[f].image = im;
            }
        }
        break;
    case 3:
        // Contrary to the previous versions, a version 3 spriteset consists of a series of images and then a set of direction and frame metadata.

        // Read in all the images.
        for(var i = 0; i<this.images.length; i++){
            this.images[i] = ImageFromArrayBuffer(stream.read(this.width*this.height*4).buffer);
            throw this.images[i];
        }

        // Load up the directions
        for(var d = 0; d<this.num_directions; d++){
            this.directions[d] = Turbo.ReadBinaryObject(stream, Turbo.SpritesetScheme.direction);
            this.directions[d].frames = new Array(this.directions[d].num_frames);

            for(var f = 0; f<this.directions[d].num_frames; f++){
                this.directions[d].frames[f] = Turbo.ReadBinaryObject(stream, Turbo.SpritesetScheme.frame);
                this.directions[d].frames[f].index = this.directions[d].frames[f].frame_index;
                this.directions[d].frames[f].image = this.images[this.directions[d].frames[f].frame_index];
            }
        }
        break;
    }

    //Sphere 1.5 compat:
    this.base = {x1:this.base_x1, y1:this.base_y1, x2:this.base_x2, y2:this.base_y2};

}

/*///////////////////////////////////////////////////////////////////////////*/
//                       Sphere 1.5 Compatibility Layer                      //
/*///////////////////////////////////////////////////////////////////////////*/

function LoadSpriteset(path){return Turbo.LoadSpritesetFile(path);}

