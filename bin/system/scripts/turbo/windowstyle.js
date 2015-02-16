RequireSystemScript('turbo/format.js');
RequireSystemScript('turbo/file_reader.js');

if(typeof Turbo == "undefined")
    var Turbo = {};

Turbo.WindowstyleScheme = Turbo.LoadSystemScheme("windowstyle.json");

Turbo.LoadWindowStyleFile = function(path){
    try{
        var reader = new Turbo.FileReader(new RawFile("~/windowstyles/"+path));
    }
    catch(e){
        throw e + " path: "+path;
    }

    return new Turbo.WindowStyle(reader);

}

Turbo.GetSystemWindowStyle = function(){
    try{
        var reader = new Turbo.FileReader(new RawFile("#~/system.rws"));
    }
    catch(e){
        throw "Could not open system windowstyle: " + e;
    }
    
    return new Turbo.WindowStyle(reader);
}

Turbo.ColorFromInteger = function(integer){
    return new Color((integer>>24) & 0xFF, (integer>>16) & 0xFF, (integer>>8) & 0xFF, integer & 0xFF);
}

Turbo.WindowStyle = function(stream){
    
    this.__proto__ = Turbo.ReadBinaryObject(stream, Turbo.WindowstyleScheme.header);
    
    if(this.signature != Turbo.WindowstyleScheme.signature)
        throw "Bad signature. Should be " + Turbo.WindowstyleScheme.signature + " instead of " + this.signature;

    // Load corner colors
    this.colors = [
        Turbo.ColorFromInteger(this.upper_left_color),
        Turbo.ColorFromInteger(this.upper_right_color),
        Turbo.ColorFromInteger(this.lower_right_color),
        Turbo.ColorFromInteger(this.lower_left_color),
    ];
    
    this.surfaces = {};
    
    if(this.version==1){
        throw "FIXME: Version 1 Windowstyles aren't handled yet";
    }
    else{
        for(var i = 0; i<Turbo.WindowstyleScheme.component_names.length; i++){
            var dimensions = Turbo.ReadBinaryObject(stream, Turbo.WindowstyleScheme.component);
            
            var surface_bytearray = stream.read(dimensions.w*dimensions.h*4);
            
            if((typeof surface_bytearray.buffer!== "undefined") && (typeof SurfaceFromArrayBuffer == "function")){
                this.surfaces[Turbo.WindowstyleScheme.component_names[i]] = SurfaceFromArrayBuffer(dimensions.w, dimensions.h, surface_bytearray.buffer);
            }
            else{
                this.surfaces[Turbo.WindowstyleScheme.component_names[i]] = new Surface(character.w, character.h, new Color(0, 0, 0));
            
                var byte_offset = 0;
                
                for(var y = 0; y<dimensions.h; y++){
                    for(var x = 0; x<dimensions.w; x++){
                        var color = new Color(surface_bytearray[byte_offset++], 
                                              surface_bytearray[byte_offset++], 
                                              surface_bytearray[byte_offset++], 
                                              surface_bytearray[byte_offset++]);
                        this.surfaces[Turbo.WindowstyleScheme.component_names[i]].setPixel(x, y, color);
                    }
                }
            }
            
            
            
        }
    }

    this.createWindowSurface = function(w, h){
        var surface = new Surface(w, h, new Color(0, 0, 0, 0));
        
        var left = this.surfaces.left.width;
        var top = this.surfaces.top.height;
        var right = this.surfaces.right.width;
        var bottom = this.surfaces.bottom.height;
    
        surface.setClippingRectangle(left, top, w-(left+right), h-(top+bottom));
    
        if(Turbo.WindowstyleScheme.background_types[this.background_type]=="tiled" || 
           Turbo.WindowstyleScheme.background_types[this.background_type]=="tiled_gradient"){
            for(var x = this.surfaces.left.width; x<w-right; x+=this.surfaces.background.width){
                for(var y = this.surfaces.top.height; y<h-bottom; y+=this.surfaces.background.height){
                    surface.blitSurface(this.surfaces.background, x, y);
                }                    
            }
        }
        
        if(Turbo.WindowstyleScheme.background_types[this.background_type]=="gradient" || 
           Turbo.WindowstyleScheme.background_types[this.background_type]=="stretched_gradient"){
            surface.gradientRectangle(left, top, w-(right+left), h-(top+bottom),
                this.colors[0], this.colors[1], this.colors[2], this.colors[3]);
        }
        
        surface.setClippingRectangle(left, 0, w-(right+left), surface.height);
        
        // Top
        for(var x = left; x<w-right; x+=this.surfaces.top.width)
            surface.blitSurface(this.surfaces.top, x, 0);
            
        // Bottom
        for(var x = left; x<w-right; x+=this.surfaces.bottom.width)
            surface.blitSurface(this.surfaces.bottom, x, h-bottom);
            
        surface.setClippingRectangle(0, top, surface.width, h-(top+bottom));
        // Left
        for(var y = top; y<h-bottom; y+=this.surfaces.left.height)
            surface.blitSurface(this.surfaces.left, 0, y);
            
        // Right
        for(var y = top; y<h-bottom; y+=this.surfaces.right.height)
            surface.blitSurface(this.surfaces.right, w-right, y);
        
        surface.setClippingRectangle(0, 0, surface.width, surface.height);
        
        // Blit the corners
        surface.blitSurface(this.surfaces.upperleft, left-this.surfaces.upperleft.width, top-this.surfaces.upperleft.height);
        surface.blitSurface(this.surfaces.upperright, w-this.surfaces.upperright.width, top-this.surfaces.upperright.height);
        surface.blitSurface(this.surfaces.lowerright, w-this.surfaces.lowerright.width, h-this.surfaces.lowerright.height); 
        surface.blitSurface(this.surfaces.lowerleft, left-this.surfaces.lowerleft.width, h-this.surfaces.lowerleft.height);
        
        return surface;
        
    }

}

// Sphere 1.5 compatibility layer

var GetSystemWindowStyle = Turbo.GetSystemWindowStyle;
