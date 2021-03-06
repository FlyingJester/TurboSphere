RequireSystemScript('turbo/bytearray.js');
RequireSystemScript('turbo/format.js');
RequireSystemScript('turbo/map.js');
RequireSystemScript('turbo/tileset.js');
RequireSystemScript('turbo/spriteset.js');
RequireSystemScript('turbo/segment.js');

if(typeof Turbo == "undefined")
    var Turbo = {};
Turbo.Classic = Turbo.Classic || {};

Turbo.EntityScheme = Turbo.LoadSystemScheme("entity.json");

Turbo.SimpleWalkHandler = function(person, x, y, layer, tileset){
    
    if(!person.ignore_tile_obstructions){
        var box = {x:x+person.base.x1, y:y+person.base.y1, w:person.base.x2-person.base.x1, h:person.base.y2-person.base.y1};

        var s_x = Math.max(0, Math.floor(box.x/tileset.width));
        var e_x = Math.min(Math.ceil((box.x+box.w)/tileset.width), layer.width);
        var s_y = Math.max(0, Math.floor(box.y/tileset.height));
        var e_y = Math.min(Math.ceil((box.y+box.h)/tileset.height), layer.height);
            
        for(var tile_y = s_y; tile_y<e_y; tile_y++){
            for(var tile_x = s_x; tile_x<e_x; tile_x++){
                var tile_num = layer.field[tile_x + (tile_y*layer.width)];
                if(tileset.tiles[tile_num].unobstructed)
                continue;

                var tile_segments = tileset.tiles[tile_num].segments;
                var tile_pix_x = tile_x*tileset.width;
                var tile_pix_y = tile_y*tileset.height;
                for(var i = 0; i<tile_segments.length; i++){
                    var segment = {
                        x1:tile_segments[i].x1+tile_pix_x,
                        y1:tile_segments[i].y1+tile_pix_y,
                        x2:tile_segments[i].x2+tile_pix_x,
                        y2:tile_segments[i].y2+tile_pix_y
                    };
                    if(Turbo.SegmentIntersectsBox(segment, box))
                        return null;
                }
            }
        } // for tiles...
    } // ignore_tile_obstructions

    return {x:x, y:y};

}

Turbo.walk_handler = Turbo.SimpleWalkHandler;

Turbo.Entity = function(x, y, layer, name, destroy){
    this.x = x; this.y = y;
    this.draw_offset = {x:0, y:0};
    this.layer = layer;
    this.name = name;
    this.is_input_person = false;
    this.destroy_on_map_change = destroy;
    this.queued_commands = [];
    
    this.onCreate  = function(){};
    this.onDestroy = function(){};
    this.onTalk    = function(){};
    this.onTouch   = function(){};
    this.onGenerate= function(){};
    this.onTrigger = function(){};
    
    this.draw = function(){}
    
}

Turbo.GetPerson = function(name){

    if(Turbo.current_map){
        return Turbo.current_map.getPerson(name);
    }
    else{     
        for(var i in Turbo.spooled_entities){
            if(Turbo.spooled_entities[i].name==name)
                return Turbo.spooled_entities[i];
        }
        
        return null;
    }
}

// Small helper to validate names.
Turbo.GetPersonThrow = function(name){
    var person = Turbo.GetPerson(name);
    
    if(!person){
        throw "No such person " + name;
    }
    
    return person;
    
}

Turbo.Person = function(x, y, layer, name, destroy, spriteset){

    if(typeof destroy == "undefined")
        destroy = true;

    this.speed = {x:1.0, y:1.0};
    this.visible = true;
    
    this.setSpriteset = function(s){
        this.spriteset = s;
        this.base = {x1:s.base_x1, y1:s.base_y1, x2:s.base_x2, y2:s.base_y2};
        
        if(this.base.x2<this.base.x1){
            var tmp_x = this.base.x2;
            this.base.x2 = this.base.x1;
            this.base.x1 = tmp_x;
        }
        if(this.base.y2<this.base.y1){
            var tmp_y = this.base.y2;
            this.base.y2 = this.base.y1;
            this.base.y1 = tmp_y;
        }
        
    }
    
    this.ignore_list = [];
    this.ignore_tile_obstructions = false;
    this.ignore_person_obstructions = false;
    
    this.setSpriteset(spriteset);

    this.direction_i = 0;
    
    this.setImageFromCurrentFrame = function(){
        this.shape.image = this.spriteset.directions[this.direction_i].frames[this.frame].image;
    }

    this.validateDirection = function(){
        this.direction = this.spriteset.directions[this.direction_i].name;
        this.frame = 0;
        this.frame_counter = 0;
        
        this.setImageFromCurrentFrame();
        
    }
    
    this.getDirection = function(){
        return this.spriteset.directions[this.direction_i];
    }
    
    this.getFrame = function(){
        return this.getDirection().frames[this.frame];
    }
    
    this.setDirection = function(dir_name){
    
        if(dir_name==this.direction)
            return;
        
        for(var i = 0; i<this.spriteset.directions.length; i++){
            
            if(this.spriteset.directions[i].name==dir_name){
                this.direction_i = i;
                this.validateDirection();
                return;
            }
        }
        
        throw "Frame " + dir_name + " does not exist in spriteset for " + this.name;
        
    }
    
    // Purely a bookkeeping function, does not actually perform movement
    this.step = function(){
        
        this.frame_counter++;
        if(this.frame_counter>=this.getFrame().delay){
            
            this.frame_counter = 0;
            
            this.frame++;
            this.frame%=this.getDirection().frames.length;
            
            this.setImageFromCurrentFrame();
        }
    }
    
    // Intended for taking a one-pixel step.
    this.canWalkTo = function(x, y, layer, tileset){ Turbo.walk_handler(this, x, y, layer, tileset)?true:false; }
    this.canWalkAt = function(x, y){return this.canWalkTo(x, y, Turbo.current_map.layers[this.layer], Turbo.current_map.tileset)};
    this.tryWalkTo = function(x, y, map){

        var m_layer = map.layers[this.layer];
        
        var to = Turbo.walk_handler(this, x, y, map.layers[this.layer], map.tileset);
        if(to){
            this.x = to.x;
            this.y = to.y;
            return true;
        }
        
        return false;
    }
    
    {
        var new_proto = new Turbo.Entity(x, y, layer, name, destroy);
        for(var i in new_proto){
            this[i] = new_proto[i];
        }
    }
    
    
    this.shape = new Shape([{x:0, y:0}, {x:this.spriteset.width, y:0}, {x:this.spriteset.width, y:this.spriteset.height}, {x:0, y:this.spriteset.height}], this.spriteset.images[0]);
    this.group = new Group([this.shape], Turbo.default_shader);

    this.validateDirection();
    
    this.draw = function(camera_p){
        
        this.group.x = this.x-camera_p.x;
        this.group.y = this.y-camera_p.y;
        
        this.group.draw();
        
    }

}

Turbo.Trigger = function(x, y, layer){
    this.__proto__ = new Turbo.Entity(x, y, layer, "", true);
}

Turbo.LoadEntity = function(stream){

    var base = Turbo.ReadBinaryObject(stream, Turbo.EntityScheme.header);
    
    if(base.type==Turbo.EntityScheme.type.person){
        var entity = Turbo.ReadBinaryObject(stream, Turbo.EntityScheme.person);
        var person = Turbo.Person(base.x, base.y, base.layer, entity.name, true, entity.spriteset_file);
        
        person.onCreate     = function(){eval(entity.on_create);}
        person.onDestroy    = function(){eval(entity.on_destroy);}
        person.onTalk       = function(){eval(entity.on_talk);}
        person.onTouch      = function(){eval(entity.on_touch);}
        person.onGenerate   = function(){eval(entity.on_generate);}
        
        return person;
        
    }
    else if(base.type==Turbo.EntityScheme.type.trigger){
        var entity = Turbo.ReadBinaryObject(stream, Turbo.EntityScheme.trigger);
        var trigger = new Turbo.Entity(base.x, base.y, base.layer, "", true);
        
        trigger.onTrigger = function(){eval(entity.trigger_function);}
        
        return trigger;
        
    }
    else throw "Unknown entity type "+ base.type;
}

/*///////////////////////////////////////////////////////////////////////////*/
//                       Sphere 1.5 Compatibility Layer                      //
/*///////////////////////////////////////////////////////////////////////////*/

const SCRIPT_ON_CREATE          = "SCRIPT_ON_CREATE";
const SCRIPT_ON_DESTROY         = "SCRIPT_ON_DESTROY";
const SCRIPT_ON_ACTIVATE_TOUCH  = "SCRIPT_ON_ACTIVATE_TOUCH";
const SCRIPT_ON_ACTIVATE_TALK   = "SCRIPT_ON_ACTIVATE_TALK";
const SCRIPT_COMMAND_GENERATOR  = "SCRIPT_COMMAND_GENERATOR";

const COMMAND_WAIT              = "COMMAND_WAIT";
const COMMAND_ANIMATE           = "COMMAND_ANIMATE";
const COMMAND_FACE_NORTH        = "COMMAND_FACE_NORTH";
const COMMAND_FACE_NORTHEAST    = "COMMAND_FACE_NORTHEAST";
const COMMAND_FACE_EAST         = "COMMAND_FACE_EAST";
const COMMAND_FACE_SOUTHEAST    = "COMMAND_FACE_SOUTHEAST";
const COMMAND_FACE_SOUTH        = "COMMAND_FACE_SOUTH";
const COMMAND_FACE_SOUTHWEST    = "COMMAND_FACE_SOUTHWEST";
const COMMAND_FACE_WEST         = "COMMAND_FACE_WEST";
const COMMAND_FACE_NORTHWEST    = "COMMAND_FACE_NORTHWEST";
const COMMAND_MOVE_NORTH        = "COMMAND_MOVE_NORTH";
const COMMAND_MOVE_EAST         = "COMMAND_MOVE_EAST";
const COMMAND_MOVE_SOUTH        = "COMMAND_MOVE_SOUTH";
const COMMAND_MOVE_WEST         = "COMMAND_MOVE_WEST";

Turbo.default_commands = {
    COMMAND_WAIT:function(){
        return false;
    },
    COMMAND_ANIMATE:function(that){
        that.step();
        return true;
    },
    COMMAND_FACE_NORTH:function(that){
        SetPersonDirection(that.name, "north");
        return true;
    },
    COMMAND_FACE_NORTHEAST:function(that){
        SetPersonDirection(that.name, "northeast");
        return true;
    },
    COMMAND_FACE_EAST:function(that){
        SetPersonDirection(that.name, "east");
        return true;
    },
    COMMAND_FACE_SOUTHEAST:function(that){
        SetPersonDirection(that.name, "southeast");
        return true;
    },
    COMMAND_FACE_SOUTH:function(that){
        SetPersonDirection(that.name, "south");
        return true;
    },
    COMMAND_FACE_SOUTHWEST:function(that){
        SetPersonDirection(that.name, "southwest");
        return true;
    },
    COMMAND_FACE_WEST:function(that){
        SetPersonDirection(that.name, "west");
        return true;
    },
    COMMAND_FACE_NORTHWEST:function(that){
        SetPersonDirection(that.name, "northwest");
        return true;
    },
    COMMAND_MOVE_NORTH:function(that){
        that.tryWalkTo(that.x, that.y-that.speed.y, Turbo.current_map);
        return true;
    },
    COMMAND_MOVE_EAST:function(that){
        that.tryWalkTo(that.x+that.speed.x, that.y, Turbo.current_map);
        return true;
    },
    COMMAND_MOVE_SOUTH:function(that){
        that.tryWalkTo(that.x, that.y+that.speed.y, Turbo.current_map);
        return true;
    },
    COMMAND_MOVE_WEST:function(that){
        that.tryWalkTo(that.x-that.speed.x, that.y, Turbo.current_map);
        return true;
    }
}

function GetPersonOffsetX(name){
    return Turbo.GetPersonThrow(name).offset.x;
}

function SetPersonOffsetX(name, x){
    Turbo.GetPersonThrow(name).offset.x = x;
}

function GetPersonOffsetY(name){
    return Turbo.GetPersonThrow(name).offset.y;
}

function SetPersonOffsetY(name, y){
    Turbo.GetPersonThrow(name).offset.y = y;
}

function GetPersonX(name){
    return Turbo.GetPersonThrow(name).x;
}

function SetPersonX(name, x){
    Turbo.GetPersonThrow(name).x = x;
}

function GetPersonY(name){
    return Turbo.GetPersonThrow(name).y;
}

function SetPersonY(name, y){
    Turbo.GetPersonThrow(name).y = y;
}

function GetPersonXFloat(name){
    return GetPersonX(name);
}

function GetPersonYFloat(name){
    return GetPersonX(name);
}

function GetPersonLayer(name){
    return Turbo.GetPersonThrow(name).layer;
}

function SetPersonLayer(name, layer){
    Turbo.GetPersonThrow(name).layer = layer;
}

function SetPersonXYFloat(name, x, y){
    SetPersonX(name, x);
    SetPersonY(name, y);
}

function SetPersonDirection(name, direction){
    
    Turbo.GetPersonThrow(name).setDirection(direction);
}

function SetPersonFrame(name, frame){
    var person = Turbo.GetPersonThrow(name);
    
    if(frame>=person.getDirection().frames.length){
        throw "Frame number " + frame + " out of range for spriteset for " + name + ". Max is " + person.getDirection().frames.length;
    }
    
    person.frame = frame;
    
    
}

function GetPersonDirection(name){
    return Turbo.GetPersonThrow(name).getDirection();
}

function GetPersonFrame(name){
    return Turbo.GetPersonThrow(name).frame;
}

function SetPersonSpeed(name, speed){
      SetPersonSpeedX(name, speed);
      SetPersonSpeedY(name, speed);
}

function SetPersonSpeedXY(name, speed_x, speed_y){
      SetPersonSpeedX(name, speed_x);
      SetPersonSpeedY(name, speed_y);
}

function GetPersonSpeed(name){
    return GetPersonSpeedX(name);
}

function GetPersonSpeedX(name){
    return Turbo.GetPersonThrow(name).speed.x;
}

function GetPersonSpeedY(name){
    return Turbo.GetPersonThrow(name).speed.y;
}

/*

      SetPersonFrameRevert(name, delay)
        - sets the delay between when the person last moved and returning to
          first frame. The delay is in frames. 0 disables this behaviour.

      GetPersonFrameRevert(name)
        - gets the delay between when the person last moved and returning to
          first frame.  The delay is in frames. 0 disables this behaviour.

      SetPersonScaleFactor(name, scale_w, scale_h)
        - rescales the sprite to a certain scale specified by scale_w and scale_h.
          Scaling is determined by floating point numbers like, 1.5 would scale
          the person to 1.5 times its normal size based on his original sprite
          size.

      SetPersonScaleAbsolute(name, width, height)
        - rescales the sprite to width pixels and height pixels.
*/

function GetPersonSpriteset(name){
    return Turbo.GetPersonThrow(name).spriteset;
}

function SetPersonSpriteset(name, spriteset){
     Turbo.GetPersonThrow(name).setSpriteset(spriteset);
}

function GetPersonBase(name){
    Turbo.GetPersonThrow(name).base;
}

/*

      SetPersonMask(name, color)
        - sets a color multiplier to use when drawing sprites.  if the color is
          RGBA:(255, 0, 0, 255), only the red elements of the sprite are drawn.
          If the color is RGBA:(255, 255, 255, 128), the sprite is drawn at
          half transparency.

      GetPersonMask(name)
        - returns the person's current mask

*/

function SetPersonAngle(name, angle){
    Turbo.GetPersonThrow(name).group.angle = angle;
}

function GetPersonAngle(name){
    return Turbo.GetPersonThrow(name).group.angle;
}

function GetPersonVisible(name){
    return Turbo.GetPersonThrow(name).visible;
}

function SetPersonVisible(name, visible){
    Turbo.GetPersonThrow(name).visible = visible;
}

function IsPersonVisible(name){
    return GetPersonVisible(name);
}

// This is SOLELY for legacy purposes. It serves no useful function now.
function GetPersonData(name){
    
    var person = Turbo.GetPersonThrow(name);
    var data_kludge;
    
    if(typeof person.data_kludge != "undefined"){
        data_kludge = person.data_kludge;
    }
    else{
        data_kludge = {};
    }
    
    data_kludge.num_frames = person.getDirection().frames.length;
    data_kludge.num_directions = person.spriteset.directions.length;
    data_kludge.width  = person.spriteset.width;
    data_kludge.height = person.spriteset.height;
    data_kludge.leader = "";
    
    return data_kludge;
    
}

// This is SOLELY for legacy purposes. It serves no useful function now.
function SetPersonData(name, data_kludge){
    Turbo.GetPersonThrow(name).data_kludge = data_kludge;
}

function GetPersonValue(name, key){
    var person = Turbo.GetPersonThrow(name);
    if((typeof person.data_kludge != "undefined") && (typeof person.data_kludge[key] != "undefined"))
        return person.data_kludge[key];
    
    return "";
        
}

function SetPersonValue(name, key, value){
    var person = Turbo.GetPersonThrow(name);
    if(typeof person.data_kludge != "undefined")
        person.data_kludge = {};
    
    person.data_kludge[key] = value;
}

/*

      FollowPerson(name, leader, pixels)
        - makes the sprite 'name' follow 'pixels' pixels behind sprite 'leader'.
        If this function is called like:
        FollowPerson(name, "", 0),
        the person will detach from anyone it is following.

*/

function GetPersonScriptFunctional(name, which){
    var person = Turbo.GetPersonThrow(name);
    
    switch(which){

        case SCRIPT_ON_CREATE:
        return function(){person.onCreate()};
        break;
        
        case SCRIPT_ON_DESTROY:
        return function(){person.onDestroy()};
        break;
        
        case SCRIPT_ON_ACTIVATE_TOUCH:
        return function(){person.onTouch()};
        break;
        
        case SCRIPT_ON_ACTIVATE_TALK:
        return function(){person.onTalk()};
        break;
        
        case SCRIPT_COMMAND_GENERATOR:
        return function(){person.onGenerate()};
        break;

        default:
        throw "No such script type as " + script;
    }    
}

function SetPersonScript(name, which, script){
    var person = Turbo.GetPersonThrow(name);
        
    var func;
    if(typeof script=="function"){
        func = function(){script()};
    }
    else{
        func = function(){eval(script);}
    }
        
    switch(which){

        case SCRIPT_ON_CREATE:
        person.onCreate = func;
        break;
        
        case SCRIPT_ON_DESTROY:
        person.onDestroy = func;
        break;
        
        case SCRIPT_ON_ACTIVATE_TOUCH:
        person.onTouch = func;
        break;
        
        case SCRIPT_ON_ACTIVATE_TALK:
        person.onTalk = func;
        break;
        
        case SCRIPT_COMMAND_GENERATOR:
        person.onGenerate = func;
        break;

        default:
        throw "No such script type as " + script;
    }    
}

function CallPersonScript(name, which){
    GetPersonScriptFunctional(name, which)();
}

function QueuePersonCommand(name, command, immediate){
    
    var person = Turbo.GetPersonThrow(name);
    var cmd = Turbo.default_commands[command];
    
    if(immediate){
        cmd(person);
    }
    else{
        person.queued_commands.push(cmd);
    }
}

function QueuePersonScript(name, script, immediate){
    
    var person = Turbo.GetPersonThrow(name);
    var cmd = GetPersonScriptFunctional(name, script);
    
    if(immediate){
        cmd(person);
    }
    else{
        person.queued_commands.push(cmd);
    }
}

function ClearPersonCommands(name){
    Turbo.GetPersonThrow(name).queued_commands = [];
}

function IsCommandQueueEmpty(name){
    return Turbo.GetPersonThrow(name).queued_commands.length==0;
}

function IsPersonObstructed(name, _x, _y){
    return Turbo.GetPersonThrow(name).canWalkAt(x, y);
}

function GetObstructingTile(name, _x, _y){
    var p = Turbo.GetPersonThrow(name);
    
    var was_ignoring = p.ignore_person_obstructions;
    p.ignore_person_obstructions = false
    
    var w = p.canWalkAt(x, y);
    p.ignore_person_obstructions = was_ignoring;
    
    if(w) return null;
    
    return GetTileAt(x, y, p.layer);
}

function GetObstructingPerson(name, _x, _y){
    var p = Turbo.GetPersonThrow(name);
    
    var was_ignoring = p.ignore_tile_obstructions;
    p.ignore_tile_obstructions = false
    
    var w = p.canWalkAt(x, y);
    p.ignore_tile_obstructions = was_ignoring;
    
    if(w) return null;
    
    return GetPersonAt(x, y, p.layer);
}

function IgnorePersonObstructions(name, ignore){
    Turbo.GetPersonThrow(name).ignore_person_obstructions = (ignore)?true:false;
}

function IsIgnoringPersonObstructions(name){
    return Turbo.GetPersonThrow(name).ignore_person_obstructions;
}

function IgnoreTileObstructions(name, ignore){
    Turbo.GetPersonThrow(name).ignore_tile_obstructions = (ignore)?true:false;
}

function IsIgnoringTileObstructions(name){
    return Turbo.GetPersonThrow(name).ignore_tile_obstructions;
}

function GetPersonAt(x, y, layer){
    var people = GetPersonList();
    for(var i = 0; i<GetPersonList(); i++){
        if(people[i].layer!=layer)
            continue;
        var box = {x:x+people[i].base.x1, y:y+people[i].base.y1, w:people[i].base.x2-people[i].base.x1, h:people[i].base.y2-people[i].base.y1};
        if(Turbo.PointInBox({x:x, y:y}, box))
            return people[i].name;
    }
}

function GetPersonIgnoreList(name){
    var names = [];
    Turbo.GetPersonThrow(name).ignore_list.forEach(function(i){names.push(i.name);});
    return names;
}

function SetPersonIgnoreList(name, ignore_list){
    list = [];
    ignore_list.forEach(list.push(Turbo.GetPersonThrow(i)));
    Turbo.GetPersonThrow(name).ignore_list = list;
}

/*

      SetTalkActivationKey(key)
      GetTalkActivationKey()
        - set key used to activate talk scripts

      SetTalkDistance(pixels)
      GetTalkDistance()
        - set distance to check for talk script activation
*/

