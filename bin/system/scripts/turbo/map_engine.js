RequireSystemScript('turbo/format.js');
RequireSystemScript('turbo/map.js');
RequireSystemScript('turbo/tileset.js');
RequireSystemScript('turbo/person.js');
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
Turbo.current_map = null;
Turbo.current_person = null; // Backward compatibility only.
Turbo.next_map = null;
Turbo.changing_maps = false;
// Holds an array of {which:SCRIPT_ON_BLANK, script:function(){/*...*/}}
Turbo.spooled_default_scripts = [];
Turbo.spooled_render_script = null;
Turbo.spooled_update_script = null;
Turbo.spooled_entities = [];

Turbo.delay_scripts = [];

Turbo.IsMapEngineRunning = function(){
    return (Turbo.current_map || Turbo.next_map || Turbo.changing_maps);
}
var IsMapEngineRunning = Turbo.IsMapEngineRunning;

// Someday this won't be necessary.
function MapMaybeString(map){

    if (typeof map === "string"){
        map = Turbo.LoadMapFile(map);
    }

    return map;
}

/* Changing Maps and Exiting the Map Engine:
    Setting changing_maps will make the MapEngine set current_map to next_map at
    the end of the next frame (and unset changing_maps). If when a new frame
    starts !current_map is true then the MapEngine will exit.
*/
function ChangeMap(map){
    Turbo.next_map = MapMaybeString(map);
    Turbo.IsChangingMaps = true;
}

function GetCurrentMap(){
    if(!Turbo.current_map)
        return "";

    return Turbo.current_map.name;
}

function ExitMapEngine(){
    Turbo.current_map = null;
    Turbo.next_map = null;
    Turbo.changing_maps = true;
}

function UpdateMapEngine(){}

function SetMapEngineFrameRate(fps){
    if(typeof fps != "number")
        throw "FPS is not a Number type";

    if(fps==NaN)
        throw "FPS is NaN";

    if(Turbo.current_map!=null)
        Turbo.current_map.fps= fps;
}

function GetMapEngineFrameRate(){
    if(Turbo.current_map)
        return Turbo.current_map.fps;
    return 60;
}

// Map Engine implementation.
function MapEngine(map, fps){

    if(typeof fps == "undefined")
        fps = 60;

    if(Turbo.current_map!=null)
        Turbo.current_map.fps =fps;

    Turbo.current_map = MapMaybeString(map);

    Turbo.spooled_default_scripts.forEach(function(i){Turbo.current_map.default_scripts[i.which] = i.script;});
    
    Turbo.spooled_entities.forEach(function(i){
        if(i.is_camera_person)
            Turbo.current_map.camera_person = i;
        if(i.is_input_person)
            Turbo.current_map.input_person = i;
        Turbo.current_map.addEntity(i);
    });

    if(Turbo.spooled_render_script)
        SetRenderScript(Turbo.spooled_render_script);
        
    if(Turbo.spooled_update_script)
        SetRenderScript(Turbo.spooled_update_script);

    var fps_interval = ((1/Turbo.current_map.fps)*1000); // In seconds.
    
    // Event loop.
    while(Turbo.current_map){

        var time = GetSeconds(); // In seconds.

        // Perform map logic
        AreKeysLeft();
        
        // Queue movement if we have an input person and movement keys are pressed
        if(Turbo.current_map.input_person){
            if(IsKeyPressed(KEY_UP)){
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_MOVE_NORTH);
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_FACE_NORTH);
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_ANIMATE);
            }
                    
            if(IsKeyPressed(KEY_DOWN)){
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_MOVE_SOUTH);
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_FACE_SOUTH);
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_ANIMATE);
                        
            }
            if(IsKeyPressed(KEY_LEFT)){
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_MOVE_WEST);
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_FACE_WEST);
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_ANIMATE);
                        
            }
            if(IsKeyPressed(KEY_RIGHT)){
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_MOVE_EAST);
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_FACE_EAST);
                Turbo.current_map.input_person.queued_commands.push(Turbo.default_commands.COMMAND_ANIMATE);
                        
            }
        }
                
        // Perform queue commands
        Turbo.current_map.entities.forEach(
            function(i){
                var len = i.queued_commands.length;
                for(var e = 0; e<len; e++){
                    i.queued_commands[e](i);
                }

                i.queued_commands = [];
            }
        );
        
        // Perform Delay scripts
        for(var i = 0; i<Turbo.delay_scripts.length;){
            if(--Turbo.delay_scripts[i].frame==0){
                Turbo.delay_scripts[i].func();
                Turbo.delay_scripts.splice(i, 1);
            }
            else i++;
        }
        
        // Perform update_script
        Turbo.current_map.update_script();

        // Draw Map
        Turbo.current_map.drawMap();

        // render_script
        Turbo.current_map.render_script();

        FlipScreen();

        // We only change maps at the end of a frame.
        if(Turbo.changine_maps){
            Turbo.current_map = Turbo.next_map;
            Turbo.next_map = null;
            Turbo.changing_maps = false;
        }

        var frame_surplus = (fps_interval-((GetSeconds()-time)*1000)); // In milliseconds.
        if(frame_surplus>0)
            Delay(Math.max(frame_surplus, 1)); // In milliseconds.

    }

    Turbo.current_map = null;
    Turbo.next_map = null;
    Turbo.changing_maps = false;

    Turbo.current_map.default_scripts.forEach(function(i, e){Turbo.spooled_default_scripts.push({which:e, script:i})})

}

function CallMapScript(which){
    Turbo.current_map.functions[which]();
}

function SetDefaultMapScript(which, script){

    if(typeof script != "function")
        script = new Function(script);

    if(Turbo.current_map)
      Turbo.current_map.default_scripts[which] = script;
    else{
        for(var i in Turbo.spooled_default_scripts){
            if(Turbo.spooled_default_scripts[i].which == which){
                Turbo.spooled_default_scripts[i].script = script;
                return;
            }
        }
        Turbo.spooled_default_scripts.push({which:which, script:script});
    }

}

function CallDefaultMapScript(which){
    if(Turbo.current_map)
      Turbo.current_map.default_scripts[which]();
    else{
        for(var i in Turbo.spooled_default_scripts){
            if(Turbo.spooled_default_scripts[i].which == which){
                Turbo.spooled_default_scripts[i].script();
                return;
            }
        }
    }
}

function GetNumLayers(){return Turbo.current_map.layers.length;}
function GetLayerName(i){return Turbo.current_map.layers[i].name;}

function GetLayerWidth(i){return Turbo.current_map.layers[i].width;}
function GetLayerHeight(i){return Turbo.current_map.layers[i].height;}

function IsLayerVisible(i){return Turbo.current_map.layers[i].visible;}
function SetLayerVisible(i, val){Turbo.current_map.layers[i].visible = val;}
function IsLayerReflective(i){return Turbo.current_map.layers[i].reflective;}
function SetLayerReflective(i, val){Turbo.current_map.layers[i].reflective = val;}

function GetLayerMask(i){return Turbo.current_map.layers[i].mask;}
function SetLayerMask(i, val){Turbo.current_map.layers[i].mask = val;}

function SetLayerScaleFactorX(i, _x){}
function SetLayerScaleFactorY(i, _y){}

function GetLayerAngle(i){return Turbo.current_map.layers[i].group.angle;}
function SetLayerAngle(i, a){Turbo.current_map.layers[i].group.angle = a;}

function GetNumTiles(){return Turbo.current_map.tileset.tiles.length;}

function SetTile(x, y, layer, tile){
    Turbo.current_map.layers[layer].field[x+(y*Turbo.current_map.width)] = tile; 
    Turbo.current_map.calculateLayer(layer);
}
function GetTile(x, y, layer){return Turbo.current_map.layers[layer].field[x+(y*Turbo.current_map.width)];}
function GetTileAt(x, y, layer){
    var layer = Turbo.current_map.layers[layer];
    var tx = (x/Turbo.current_map.tileset.width)<<0;
    var ty = (y/Turbo.current_map.tileset.height)<<0;
    return layer.field[tx + (ty*layer.width)];
}

function GetTileName(i){return Turbo.current_map.tileset.tiles[i].name;}
    // Added for orthogonality
function SetTileName(i){return Turbo.current_map.tileset.tiles[i].name;}

function GetTileWidth(){return Turbo.current_map.tileset.width;}
function GetTileHeight(){return Turbo.current_map.tileset.height;}


function GetTileImage(i){return Turbo.current_map.tileset.tiles[i].image;}
function SetTileImage(i, image){
    Turbo.current_map.tileset.tiles[i].surface = new Surface(image); 
    Turbo.current_map.tileset.tiles[i].surface = new Surface(image); 
    Turbo.current_map.calculateMap();
}

function GetTileSurface(i){return Turbo.current_map.tileset.tiles[i].surface;}
function SetTileSurface(i, surface){Turbo.current_map.SetTileImage(i, surface.clone());}

function GetTileDelay(i){return Turbo.current_map.tileset.tiles[i].delay;}
function SetTileDelay(i, delay){Turbo.current_map.tileset.tiles[i].delay = delay;}

function GetNextAnimatedTile(i){return (Turbo.current_map.tileset.tiles[i].animated)?Turbo.current_map.tileset.tiles[i].animated_next:i}
function SetNextAnimatedTile(i, next){
    Turbo.current_map.tileset.tiles[i].animated = (i!=next);
    Turbo.current_map.tileset.tiles[i].animated_next = next;
}

function ReplaceTilesOnLayer(num, oldtile, newtile){
    if(oldtile==newtile)
        return;

    var replaced = false;

    for(var i = 0; i< Turbo.current_map.layers[num].field.length; i++){
        if(Turbo.current_map.layers[num].field[i]==oldtile){
            Turbo.current_map.layers[num].field[i] = newtile;
            replaced = true;
        }
    }

    if(replaced){
        Turbo.current_map.calculateLayer(num);
    }
}

// Non-spec Helper function
function GetTriggerAt(_x, _y, num, from){
    for(var i in Turbo.current_map.entities){
        if((from[i].layer==num) &&
            (from[i].isTrigger) &&
            (from[i].x==_x/Turbo.current_map.tileset.width) &&
            (from[i].y==_y/Turbo.current_map.tileset.height)){
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
    for(var i in Turbo.current_map.zones){
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

function IsTriggerAt(_x, _y, num){return (GetTriggerAt(_x, _y, num, Turbo.current_map))?true:false;}

function ExecuteTrigger(map_x, map_y, layer){
    var trigger = GetTriggerAt(_x, _y, num, Turbo.current_map);
    if(trigger)
        trigger.trigger_script();
}

function AreZonesAt(_x, _y, num){return GetZonesAt(_x, _y, num, Turbo.current_map, true).length!=0;}
function ExecuteZones(_x, _y, num){GetZonesAt(_x, _y, num, Turbo.current_map).forEach(function(z){z.on_touch();});}

function GetNumZones(){return Turbo.current_map.zones.length;}
function GetCurrentZone(){return Turbo.current_map.current_zone;}

function GetZoneX(z){return z.location[0].x;}
function GetZoneY(z){return z.location[0].y;}
function GetZoneWidth(z){return z.location[1].x-z.location[0].x;}
function GetZoneHeight(z){return z.location[1].y-z.location[0].y;}
function GetZoneLayer(z){return z.layer;}
function SetZoneLayer(z, n){return z.layer = n;}

function ExecuteZoneScript(z){z.on_touch();}
function RenderMap(){Turbo.current_map.drawMap};

function AttachInput(person){
    
    if(Turbo.current_map)
        for(var i in Turbo.current_map.entities){
            if(Turbo.current_map.entities[i].name==person){
                Turbo.current_map.input_person = Turbo.current_map.entities[i];
                return;
            }
        }
    else
        Turbo.spooled_entities.forEach(function(i){i.is_input_person = (i.name==person);});
    
}

function DetachInput(){
    
    if(Turbo.current_map)
        Turbo.current_map.input_person = null;
    else
        Turbo.spooled_entities.forEach(function(i){i.is_input_person = false;});
    
}

function DetachCamera(){
    
    if(Turbo.current_map)
        Turbo.current_map.unsetCamera();
    else
        Turbo.spooled_entities.forEach(function(i){i.is_input_person = false;});
    
}

function AttachCamera(person){

    if(Turbo.current_map)
        for(var i in Turbo.current_map.entities){
            if(Turbo.current_map.entities[i].name==person){
                Turbo.current_map.camera_person = Turbo.current_map.entities[i];
                return;
            }
        }
    else
        Turbo.spooled_entities.forEach(function(i){i.is_camera_person = (i.name==person);});
    
}

function IsInputAttached(){

    if(Turbo.current_map)
        return Turbo.current_map.input_person;
    else
        for(var i in entity_list)
            if(i.is_camera_person)
                return true;
}

function GetInputPerson(){
    if(IsInputAttached()){
        return Turbo.current_map.input_person.name;
    }
    return "";
}

function GetCameraPerson(){
    if(IsInputAttached()){
        return Turbo.current_map.camera_person.name;
    }
    return "";
}

function SetCameraX(x){
    Turbo.current_map.camera.x = x+(GetScreenWidth()/2);
}

function SetCameraY(y){
    Turbo.current_map.camera.y = y+(GetScreenHeight()/2);
}

function GetCameraX(){
    return Turbo.current_map.camera.x;
}

function GetCameraY(){
    return Turbo.current_map.camera.y;
}

// TODO: Make these conversions respect scrolling and parallax
Turbo.MapToScreen = function(layer, x, y){
    return {
        x:x-Turbo.current_map.camera.x,
        y:y-Turbo.current_map.camera.y
    };
}
Turbo.ScreenToMap = function(layer, x, y){
    return {
        x:x+Turbo.current_map.camera.x,
        y:y+Turbo.current_map.camera.y
    };
}
function MapToScreenX(layer, x){return Turbo.MapToScreen(layer, x, 0).x;}
function MapToScreenY(layer, y){return Turbo.MapToScreen(layer, 0, y).y;}
function ScreenToMapX(layer, x){return Turbo.ScreenToMap(layer, x, 0).x;}
function ScreenToMapY(layer, y){return Turbo.ScreenToMap(layer, 0, y).y;}

function SetUpdateScript(script){
    if(!Turbo.current_map){
        Turbo.spooled_update_script = script;
    }
    else{
        if(typeof script == "function"){
            Turbo.current_map.update_script = script;
        }
        else{
            Turbo.current_map.update_script = new Function(script);
        }
    }
}

function SetRenderScript(script){
    if(!Turbo.current_map){
        Turbo.spooled_render_script = script;
    }
    else{
        if(typeof script == "function"){
            Turbo.current_map.render_script = script;
        }
        else{
            Turbo.current_map.render_script = new Function(script);
        }        
    }
}

// Person Control
function CreatePerson(name, spriteset, destroy_on_map_change){

    if(typeof destroy_on_map_change == "undefined")
        destroy_on_map_change = true;

    var loaded_spriteset;

    if(typeof spriteset == "string"){
        loaded_spriteset = Turbo.LoadSpritesetFile(spriteset);
    }
    else{
        loaded_spriteset = spriteset;
    }
    
    if(Turbo.IsMapEngineRunning()){
        Turbo.current_map.addPerson(new Turbo.Person(Turbo.current_map.start_x, Turbo.current_map.start_y,
                                                    Turbo.current_map.start_layer, name, destroy_on_map_change, loaded_spriteset));
    }
    else{
        Turbo.spooled_entities.push(new Turbo.Person(0, 0, 0, name, destroy_on_map_change, loaded_spriteset));
    }

}

function GetCurrentPerson(){
    return Turbo.current_person;
}

function SetDelayScript(frames, script){
    var func = script;
    if(typeof func == "string")
        func = new Function(script);

    Turbo.delay_scripts.push({frames:frames, func:func});
}
