RequireSystemScript('turbo/format.js');
RequireSystemScript('turbo/file_reader.js');

if(typeof Turbo == "undefined")
    var Turbo = {};

Turbo.FontScheme = Turbo.LoadSystemScheme("font.json");

Turbo.LoadFontFile = function(path){
    try{
        var reader = new Turbo.FileReader(new RawFile("~/fonts/"+path));
    }
    catch(e){
        throw e + " path: "+path;
    }

    return new Turbo.Font(reader);

}

Turbo.GetSystemFont = function(){
    try{
        var reader = new Turbo.FileReader(new RawFile("#~/system.rfn"));
    }
    catch(e){
        throw "Could not open system font: " + e;
    }
    
    return new Turbo.Font(reader);
}

Turbo.FontStringCache = function(string, surface, width){
    this.surface = surface.clone();
    this.string  = string;
    this.width   = width;
    
    this.clone = function(){
        return new Turbo.FontStringCache(string, this.surface.clone(), width);
    }
    
}

Turbo.Font = function(stream, string_cache_size){
    
    if(typeof string_cache_size == "undefined")
        string_cache_size = 16;
        
    this.__proto__ = Turbo.ReadBinaryObject(stream, Turbo.FontScheme.header);
    
    if(this.signature != Turbo.FontScheme.signature)
        throw "Bad signature. Should be " + Turbo.FontScheme.signature + " instead of " + this.signature;
    
    this.characters = new Array(this.num_characters);
    
    // Read the characters
    for(var i = 0; i<this.num_characters; i++){
        var character = Turbo.ReadBinaryObject(stream, Turbo.FontScheme.character);
        var surface_bytearray = stream.read(character.w*character.h*4);
        
        // If ByteArray objects have a `buffer' and we can create Surfaces directly from buffers, do it.
        if((typeof surface_bytearray.buffer!== "undefined") && (typeof SurfaceFromArrayBuffer == "function")){
            character.surface = SurfaceFromArrayBuffer(character.w, character.h, surface_bytearray.buffer);
        }
        else{ // Otherwise iterate the long way.
            character.surface = new Surface(character.w, character.h, new Color(0, 0, 0));
            
            var byte_offset = 0;
            
            for(var y = 0; y<character.h; y++){
                for(var x = 0; x<character.w; x++){
                    var color = new Color(surface_bytearray[byte_offset++], 
                                          surface_bytearray[byte_offset++], 
                                          surface_bytearray[byte_offset++], 
                                          surface_bytearray[byte_offset++]);
                    character.surface.setPixel(x, y, color);
                }
            }
        }
        
        this.characters[i] = character;
        
    }
    
    this.string_cache = [];
    
    this.getTextSurfaces = function(string){
        
        var surfaces = new Array(string.length);
        
        for(var i = 0; i<string.length; i++){
            
            var code_point = string.charCodeAt(i);
            if(code_point>=this.num_characters)
                code_point = 0;
            
            surfaces[i] = this.characters[code_point].surface;
            
        }
        
        return surfaces;
        
    }
    
    this.getTextSurfaceFromCache = function(string, width){
        if(typeof width == "undefined")
            width = ~0;
            
        width>>=0;    
        
        for(var i in this.string_cache){
            if(this.string_cache[i].string===string){
                if((this.string_cache[i].width===width) ||
                    ((this.string_cache[i].width>=width) && (this.string_cache[i].surface.width<=width))){
                    return this.string_cache[i].surface.clone();
                }
            }
        }
        
        return null;
        
    }
    
    this.createTextSurface = function(string, width){
        if(typeof width == "undefined")
            width = ~0;
            
        width>>=0;    
        
        var cached_surface = this.getTextSurfaceFromCache(string, width);
        if(cached_surface)
            return cached_surface;
        
        // We need to know the actual width. We could just set the width to be `width', but we
        //   still would not know the necessary height. It's easiest to just find this out first.
        var actual_width = 0;
        var max_height = 0;
        var surfaces = this.getTextSurfaces(string);
        for(var i = 0; i<surfaces.length; i++){
            
            actual_width += surfaces[i].width;
            
            max_height = Math.max(max_height, surfaces[i].height)
            
        }
        actual_width>>=0;
        
        if(width==~0)
            width = actual_width;
        
        var actual_height = max_height * Math.ceil(actual_width/width);
        
        var string_surface = new Surface(width, actual_height, new Color(0, 0, 0, 0));
        
        var at_x = 0;
        var at_y = 0;
        for(var i = 0; i<surfaces.length; i++){
            if(at_x+surfaces[i].width>width){
                at_x = 0;
                at_y+=max_height;
            }
            string_surface.blitSurface(surfaces[i], at_x, at_y);
            at_x+=surfaces[i].width;
        }
        
        if(this.string_cache.length>=string_cache_size)
            this.string_cache.shift();
        
        this.string_cache.push(new Turbo.FontStringCache(string, string_surface, width));
        
        return string_surface;
        
    }
    
}

// Sphere 1.5 Compatibility Layer
var GetSystemFont = Turbo.GetSystemFont;

Surface.prototype.drawText = function(font, x, y, string){
    
    var cached_surface = getTextSurfaceFromCache(string);
    if(cached_surface){
        this.blitSurface(cached_surface, x, y);
        return;
    }
    else{
        var surfaces = font.getTextSurfaces(string);
        var at_x = x;
        for(var i = 0; i<surfaces.length; i++){
            
            this.blitSurface(surfaces[i], at_x, y);
            at_x+=surfaces[i].width;
            
            if(at_x>this.width)
                break;
        }
    }
}

// TODO: Clipping
Surface.prototype.drawTextBox = function(font, x, y, w, h, offset_h, string){
    
    var surface = font.createTextSurface(string, w);

    this.blitSurface(x, y+offset_h, surface);
    
}

