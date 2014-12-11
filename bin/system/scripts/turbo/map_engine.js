RequireSystemScript('turbo/format.js');
RequireSystemScript('turbo/map.js');
RequireSystemScript('turbo/tileset.js');
RequireSystemScript('turbo/spriteset.js');
RequireSystemScript('turbo/bytearray.js');

if(typeof Turbo == "undefined")
    var Turbo = {};

Turbo.Classic = Turbo.Classic || {};

/*///////////////////////////////////////////////////////////////////////////*/
//                       Sphere 1.5 Compatibility Layer                      //
/*///////////////////////////////////////////////////////////////////////////*/

const SCRIPT_ON_ENTER_MAP       = 0;
const SCRIPT_ON_LEAVE_MAP       = 10;
const SCRIPT_ON_LEAVE_MAP_NORTH = 20;
const SCRIPT_ON_LEAVE_MAP_EAST  = 30;
const SCRIPT_ON_LEAVE_MAP_SOUTH = 40;
const SCRIPT_ON_LEAVE_MAP_WEST  = 50;

// These variables are the ONLY global state of the map engine.
//   Everything else is represented inside a map and transferred when the map
//   is changed. The Spooled arrays acumulate values until the MapEngine is
//   called.
Turbo.CurrentMap = null;
Turbo.NextMap = null;
Turbo.IsChangingMaps = false;
// Holds an array of {which:SCRIPT_ON_BLANK, script:function(){/*...*/}}
Turbo.SpooledDefaultScripts = [];
Turbo.SpooledEntities = [];

// Someday this won't be necessary.
function MapMaybeString(map){

    if (typeof map === "string"){
        map = Turbo.LoadMapFile(map);
    }

    if(!(Turbo.CurrentMap instanceof Turbo.Map))
        throw "Turbo.CurrentMap is not a Turbo.Map (" + Turbo.CurrentMap + ")";

    return map;
}

/* Changing Maps and Exiting the Map Engine:
    Setting IsChangingMaps will make the MapEngine set CurrentMap to NextMap at
    the end of the next frame (and unset IsChangingMaps). If when a new frame
    starts !CurrentMap is true then the MapEngine will exit.
*/
function ChangeMap(map){
    Turbo.NextMap = MapMaybeString(map);
    Turbo.IsChangingMaps = true;
}

function GetCurrentMap(){
    if(!Turbo.CurrentMap)
        return "";

    return Turbo.CurrentMap.name;
}

function ExitMapEngine(){
    Turbo.CurrentMap = null;
    Turbo.NextMap = null;
    Turbo.IsChangingMaps = true;
}

function IsMapEngineRunning(){
    return !(Turbo.CurrentMap || Turbo.NextMap || Turbo.IsChangingMaps);
}

function UpdateMapEngine(){}

function SetMapEngineFrameRate(fps){
    if(typeof fps != "number")
        throw "FPS is not a Number type";

    if(fps==NaN)
        throw "FPS is NaN";

    if(Turbo.CurrentMap)
        Turbo.CurrentMap.fps = fps;
}

function GetMapEngineFrameRate(){
    if(Turbo.CurrentMap)
        return Turbo.CurrentMap.fps;
    return 60;
}


function MapEngine(map, fps){

    if(typeof fps == "undefined")
        fps = 60;

    Turbo.CurrentMap.fps = fps;

    Turbo.CurrentMap = MapMaybeString(map);

    Turbo.SpooledDefaultScripts.forEach(function(i){Turbo.CurrentMap.default_scripts[i.which] = i.script});

    while(Turbo.CurrentMap){

        var time = GetSeconds();

        // Perform map logic

        //...

        // Perform update_script
        Turbo.CurrentMap.update_script();

        // Draw Map
        Turbo.CurrentMap.drawMap();

        // render_script
        Turbo.CurrentMap.render_script();

        // We only change maps at the end of a frame.
        if(Turbo.IsChangingMaps){
            Turbo.CurrentMap = Turbo.NextMap;
            Turbo.NextMap = null;
            Turbo.IsChangingMaps = false;
        }

        // Throttle FPS
        frame_surplus = (GetSeconds()-time) + ((1/Turbo.CurrentMap.fps)*1000);
        Delay(Math.max(frame_surplus, 0));
    }

    Turbo.CurrentMap = null;
    Turbo.NextMap = null;
    Turbo.IsChangingMaps = false;

    Turbo.CurrentMap.default_scripts.forEach(function(i, e){Turbo.SpooledDefaultScripts.push({which:e, script:i})})

}

function CallMapScript(which){
    Turbo.CurrentMap.functions[which]();
}

function SetDefaultMapScript(which, script){

    if(typeof script != "function")
        script = function(){eval(script);}

    if(Turbo.CurrentMap)
      Turbo.CurrentMap.default_scripts[which] = script;
    else{
        for(var i in Turbo.SpooledDefaultScripts){
            if(Turbo.SpooledDefaultScripts[i].which == which){
                Turbo.SpooledDefaultScripts[i].script = script;
                return;
            }
        }
        Turbo.SpooledDefaultScripts.push({which:which, script:script});
    }

}

function CallDefaultMapScript(which){
    if(Turbo.CurrentMap)
      Turbo.CurrentMap.default_scripts[which]();
    else{
        for(var i in Turbo.SpooledDefaultScripts){
            if(Turbo.SpooledDefaultScripts[i].which == which){
                Turbo.SpooledDefaultScripts[i].script();
                return;
            }
        }
    }
}

function GetNumLayers(){return Turbo.CurrentMap.layers.length;}
function GetLayerName(i){return Turbo.CurrentMap.layers[i].name;}

function GetLayerWidth(i){return Turbo.CurrentMap.layers[i].width;}
function GetLayerHeight(i){return Turbo.CurrentMap.layers[i].height;}

function IsLayerVisible(i){return Turbo.CurrentMap.layers[i].visible;}
function SetLayerVisible(i, val){Turbo.CurrentMap.layers[i].visible = val;}
function IsLayerReflective(i){return Turbo.CurrentMap.layers[i].reflective;}
function SetLayerReflective(i, val){Turbo.CurrentMap.layers[i].reflective = val;}

function GetLayerMask(i){return Turbo.CurrentMap.layers[i].mask;}
function SetLayerMask(i, val){Turbo.CurrentMap.layers[i].mask = val;}

function SetLayerScaleFactorX(i, _x){}
function SetLayerScaleFactorY(i, _y){}

function GetLayerAngle(i){return Turbo.CurrentMap.layers[i].group.angle;}
function SetLayerAngle(i, a){Turbo.CurrentMap.layers[i].group.angle = a;}

function GetNumTiles(){return Turbo.CurrentMap.tileset.tiles.length;}

function SetTile(x, y, layer, tile){Turbo.CurrentMap.layers[layer].field[x+(y*Turbo.CurrentMap.width)] = tile; Turbo.CurrentMap.calculateLayer(layer);}
function GetTile(x, y, layer){return Turbo.CurrentMap.layers[layer].field[x+(y*Turbo.CurrentMap.width)];}

function GetTileName(i){return Turbo.CurrentMap.tileset.tiles[i].name;}
    // Added for orthogonality
function SetTileName(i){return Turbo.CurrentMap.tileset.tiles[i].name;}

function GetTileWidth(){return Turbo.CurrentMap.tileset.width;}
function GetTileHeight(){return Turbo.CurrentMap.tileset.height;}


function GetTileImage(i){return Turbo.CurrentMap.tileset.tiles[i].image;}
function SetTileImage(i, image){Turbo.CurrentMap.tileset.tiles[i].image = image; Turbo.CurrentMap.tileset.tiles[i].surface = new Surface(image); Turbo.CurrentMap.calculateMap();}

function GetTileSurface(i){return Turbo.CurrentMap.tileset.tiles[i].surface;}
function SetTileSurface(i, surface){Turbo.CurrentMap.SetTileImage(i, new Image(surface));}

function GetTileDelay(i){return Turbo.CurrentMap.tileset.tiles[i].delay;}
function SetTileDelay(i, delay){Turbo.CurrentMap.tileset.tiles[i].delay = delay;}

function GetNextAnimatedTile(i){return (Turbo.CurrentMap.tileset.tiles[i].animated)?Turbo.CurrentMap.tileset.tiles[i].animated_next:i}
function SetNextAnimatedTile(i, next){
    Turbo.CurrentMap.tileset.tiles[i].animated = (i!=next);
    Turbo.CurrentMap.tileset.tiles[i].animated_next = next;
}

function ReplaceTilesOnLayer(num, oldtile, newtile){
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
function GetTriggerAt(_x, _y, num, from){
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
function GetZonesAt(_x, _y, num, from, stop_at_one){

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

function IsTriggerAt(_x, _y, num){return (GetTriggerAt(_x, _y, num, Turbo.CurrentMap));}

function ExecuteTrigger(map_x, map_y, layer){
    var trigger = GetTriggerAt(_x, _y, num, Turbo.CurrentMap);
    if(trigger)
        trigger.trigger_script();
}

function AreZonesAt(_x, _y, num){return GetZonesAt(_x, _y, num, Turbo.CurrentMap, true).length!=0;}
function ExecuteZones(_x, _y, num){GetZonesAt(_x, _y, num, Turbo.CurrentMap).forEach(function(z){z.on_touch();});}

function GetNumZones(){return Turbo.CurrentMap.zones.length;}
function GetCurrentZone(){return Turbo.CurrentMap.current_zone;}

function GetZoneX(z){return z.location[0].x;}
function GetZoneY(z){return z.location[0].y;}
function GetZoneWidth(z){return z.location[1].x-z.location[0].x;}
function GetZoneHeight(z){return z.location[1].y-z.location[0].y;}
function GetZoneLayer(z){return z.layer;}
function SetZoneLayer(z, n){return z.layer = n;}

function ExecuteZoneScript(z){z.on_touch();}
function RenderMap(){Turbo.CurrentMap.drawMap};

function AttachInput(person){Turbo.CurrentMap.input_person = person;}

function DetachInput(person){Turbo.CurrentMap.unsetInput();}

function IsInputAttached(){return Turbo.CurrentMap.input_person;}

function GetInputPerson(){
    if(IsInputAttached()){
        return Turbo.CurrentMap.input_person.name;
    }
    return "";
}

function AttachCamera(person){Turbo.CurrentMap.camera_person = person;}

function DetachCamera(person){Turbo.CurrentMap.unsetCamera();}

function GetCameraPerson(){
    if(IsInputAttached()){
        return Turbo.CurrentMap.camera_person.name;
    }
    return "";
}

function SetCameraX(x){
    Turbo.CurrentMap.camera.x = x+(GetScreenWidth()/2);
}

function SetCameraY(y){
    Turbo.CurrentMap.camera.y = y+(GetScreenHeight()/2);
}

function GetCameraX(){
    return Turbo.CurrentMap.camera.x;
}

function GetCameraY(){
    return Turbo.CurrentMap.camera.y;
}

function MapToScreenX(layer, x){}
function MapToScreenY(layer, y){}
function ScreenToMapX(layer, x){}
function ScreenToMapY(layer, y){}


function SetUpdateScript(script){
    if(typeof script == "function"){
        Turbo.CurrentMap.update_script = script;
    }
    else{
        Turbo.CurrentMap.update_script = function(){eval(script);}
    }
}

function SetRenderScript(script){
    if(typeof script == "function"){
        Turbo.CurrentMap.render_script = script;
    }
    else{
        Turbo.CurrentMap.render_script = function(){eval(script);}
    }
}
