RequireSystemScript('turbo/map.js');
RequireSystemScript('turbo/tileset.js');
RequireSystemScript('turbo/bytearray.js');

/*///////////////////////////////////////////////////////////////////////////*/
//                       Sphere 1.5 Compatibility Layer                      //
/*///////////////////////////////////////////////////////////////////////////*/

if(typeof Turbo == "undefined")
    var Turbo = {};

Turbo.Classic = Turbo.Classic || {};
Turbo.CurrentMap = null;


var GetNumLayers   = function(){return Turbo.CurrentMap.layers.length;}
var GetLayerName   = function(i){return Turbo.CurrentMap.layers[i].name;}

var GetLayerWidth  = function(i){return Turbo.CurrentMap.layers[i].width;}
var GetLayerHeight = function(i){return Turbo.CurrentMap.layers[i].height;}

var IsLayerVisible = function(i){return Turbo.CurrentMap.layers[i].visible;}
var SetLayerVisible= function(i, val){Turbo.CurrentMap.layers[i].visible = val;}
var IsLayerReflective = function(i){return Turbo.CurrentMap.layers[i].reflective;}
var SetLayerReflective= function(i, val){Turbo.CurrentMap.layers[i].reflective = val;}

var GetLayerMask  = function(i){return Turbo.CurrentMap.layers[i].mask;}
var SetLayerMask  = function(i, val){Turbo.CurrentMap.layers[i].mask = val;}

var SetLayerScaleFactorX = function(i, _x){}
var SetLayerScaleFactorY = function(i, _y){}

var GetLayerAngle = function(i){return Turbo.CurrentMap.layers[i].group.angle;}
var SetLayerAngle = function(i, a){Turbo.CurrentMap.layers[i].group.angle = a;}

var GetNumTiles   = function(){return Turbo.CurrentMap.tileset.tiles.length;}

var SetTile       = function(x, y, layer, tile){Turbo.CurrentMap.layers[layer].field[x+(y*Turbo.CurrentMap.width)] = tile; Turbo.CurrentMap.calculateLayer(layer);}
var GetTile       = function(x, y, layer){return Turbo.CurrentMap.layers[layer].field[x+(y*Turbo.CurrentMap.width)];}

var GetTileName   = function(i){return Turbo.CurrentMap.tileset.tiles[i].name;}
    // Added for orthogonality
var SetTileName   = function(i){return Turbo.CurrentMap.tileset.tiles[i].name;}

var GetTileWidth  = function(){return Turbo.CurrentMap.tileset.width;}
var GetTileHeight = function(){return Turbo.CurrentMap.tileset.height;}


var GetTileImage  = function(i){return Turbo.CurrentMap.tileset.tiles[i].image;}
var SetTileImage  = function(i, image){Turbo.CurrentMap.tileset.tiles[i].image = image; Turbo.CurrentMap.tileset.tiles[i].surface = new Surface(image); Turbo.CurrentMap.calculateMap();}

var GetTileSurface  = function(i){return Turbo.CurrentMap.tileset.tiles[i].surface;}
var SetTileSurface  = function(i, surface){Turbo.CurrentMap.SetTileImage(i, new Image(surface));}

var GetTileDelay  = function(i){return Turbo.CurrentMap.tileset.tiles[i].delay;}
var SetTileDelay  = function(i, delay){Turbo.CurrentMap.tileset.tiles[i].delay = delay;}

var GetNextAnimatedTile = function(i){return (Turbo.CurrentMap.tileset.tiles[i].animated)?Turbo.CurrentMap.tileset.tiles[i].animated_next:i}
var SetNextAnimatedTile = function(i, next){
    Turbo.CurrentMap.tileset.tiles[i].animated = (i!=next);
    Turbo.CurrentMap.tileset.tiles[i].animated_next = next;
}

var ReplaceTilesOnLayer = function(num, oldtile, newtile){
    if(oldtile==newtile)
        return;

    var replaced = false;

    for(var i = 0; i< Turbo.CurrentMap.layers[num].field.length; i++){
        if(Turbo.CurrentMap.layers[num].field[i]==oldtile){
            Turbo.CurrentMap.layers[num].field[i] = newtile;
            replaced = true;
        }
    }

    if(replaced){
        Turbo.CurrentMap.calculateLayer(num);
    }
}

// Non-spec Helper function
var GetTriggerAt = function(_x, _y, num, from){
    for(var i in Turbo.CurrentMap.entities){
        if((from[i].layer==num) &&
            (from[i].isTrigger) &&
            (from[i].x==_x/Turbo.CurrentMap.tileset.width) &&
            (from[i].y==_y/Turbo.CurrentMap.tileset.height)){
            return from[i];
        }
    }
    return null;
}

// Non-spec Helper function
var GetZonesAt = function(_x, _y, num, from, stop_at_one){

    if(typeof stop_at_one == "undefined")
        stop_at_one = false;

    var zones = [];
    for(var i in Turbo.CurrentMap.zones){
        if(!((from.zones[i].layer==num) &&
            (from.zones[i].isTrigger)))
            continue;
        if((_x>from.zones[i].location[0].x) && (_x<from.zones[i].location[1].x) &&
           (_y>from.zones[i].location[0].y) && (_y<from.zones[i].location[1].y)){
            zones.push(from.zones[i]);
            if(stop_at_one)
                break;
        }
    }
    return zones;
}

var IsTriggerAt = function(_x, _y, num){return (GetTriggerAt(_x, _y, num, Turbo.CurrentMap));}

var ExecuteTrigger = function(map_x, map_y, layer){
    var trigger = GetTriggerAt(_x, _y, num, Turbo.CurrentMap);
    if(trigger)
        trigger.trigger_script();
}

var AreZonesAt  = function(_x, _y, num){return GetZonesAt(_x, _y, num, Turbo.CurrentMap, true).length!=0;}
var ExecuteZones= function(_x, _y, num){GetZonesAt(_x, _y, num, Turbo.CurrentMap).forEach(function(z){z.on_touch();});}

var GetNumZones = function(){return Turbo.CurrentMap.zones.length;}
var GetCurrentZone = function(){return Turbo.CurrentMap.current_zone;}

var GetZoneX    = function(z){return z.location[0].x;}
var GetZoneY    = function(z){return z.location[0].y;}
var GetZoneWidth= function(z){return z.location[1].x-z.location[0].x;}
var GetZoneHeight  = function(z){return z.location[1].y-z.location[0].y;}
var GetZoneLayer   = function(z){return z.layer;}
var SetZoneLayer   = function(z, n){return z.layer = n;}

var ExecuteZoneScript = function(z){z.on_touch();}
var RenderMap         = function(){Turbo.CurrentMap.drawMap};

var AttachInput    = function(person){Turbo.CurrentMap.input_person = person;}

var DetachInput    = function(person){Turbo.CurrentMap.unsetInput();}

var IsInputAttached = function(){return Turbo.CurrentMap.input_person;}

var GetInputPerson = function(){
    if(IsInputAttached()){
        return Turbo.CurrentMap.input_person.name;
    }
    return "None";
}

var AttachCamera    = function(person){Turbo.CurrentMap.camera_person = person;}

var DetachCamera    = function(person){Turbo.CurrentMap.unsetCamera();}

var GetCameraPerson = function(){
    if(IsInputAttached()){
        return Turbo.CurrentMap.camera_person.name;
    }
    return "None";
}

var SetUpdateScript(script){
    if(typeof script == "function"){
        Turbo.CurrentMap.update_script = script;
    }
    else{
        Turbo.CurrentMap.update_script = function(){eval(script);}
    }
}

var SetRenderScript(script){
    if(typeof script == "function"){
        Turbo.CurrentMap.render_script = script;
    }
    else{
        Turbo.CurrentMap.render_script = function(){eval(script);}
    }
}
