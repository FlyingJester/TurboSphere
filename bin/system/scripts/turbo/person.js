RequireSystemScript('turbo/bytearray.js');
RequireSystemScript('turbo/format.js');
RequireSystemScript('turbo/map.js');
RequireSystemScript('turbo/tileset.js');
RequireSystemScript('turbo/spriteset.js');

if(typeof Turbo == "undefined")
    var Turbo = {};
Turbo.Classic = Turbo.Classic || {};

Turbo.EntityScheme = Turbo.LoadSystemScheme("entity.json");

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

    if(Turbo.CurrentMap){
        return Turbo.CurrentMap.getPerson(name);
    }
    else{     
        for(var i in Turbo.SpooledEntities){
            if(Turbo.SpooledEntities[i].name==name)
                return Turbo.SpooledEntities[i];
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
    this.canWalkTo = function(x, y, layer, tileset){
        
        var box = {x:x+this.base.x1, y:y+this.base.y1, w:this.base.x2-this.base.x1, h:this.base.y2-this.base.y1};
        
        var s_x = Math.max(0, Math.floor(box.x/tileset.width));
        var e_x = Math.min(Math.ceil((box.x+box.w)/tileset.width), layer.width);
        var s_y = Math.max(0, Math.floor(box.y/tileset.height));
        var e_y = Math.min(Math.ceil((box.y+box.h)/tileset.height), layer.height);
        
    //  for(var tile_y = s_y; tile_y<e_y; tile_y++){
    //      for(var tile_x = s_x; tile_x<e_x; tile_x++){
        
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
                        return false;
                }
            }
        }

        return true;
    }
    
    this.tryWalkTo = function(x, y, map){

        var m_layer = map.layers[this.layer];
        
        while(this.x<x){
            if(!this.canWalkTo(this.x+1, this.y, m_layer, map.tileset))
                break;
            this.x++;
        }
        while(this.x>x){
            if(!this.canWalkTo(this.x-1, this.y, m_layer, map.tileset))
                break;
            this.x--;
        }
        while(this.y<y){
            if(!this.canWalkTo(this.x, this.y+1, m_layer, map.tileset))
                break;
            this.y++;
        }
        while(this.y>y){
            if(!this.canWalkTo(this.x, this.y-1, m_layer, map.tileset))
                break;
            this.y--;
        }
        
        return (this.x==x) && (this.y==y);
        
    }
    
    this.__proto__ = new Turbo.Entity(x, y, layer, name, destroy);
    
    this.shape = new Shape([{x:0, y:0}, {x:this.spriteset.width, y:0}, {x:this.spriteset.width, y:this.spriteset.height}, {x:0, y:this.spriteset.height}], this.spriteset.images[0]);
    this.group = new Group([this.shape], Turbo.default_shader);

    this.validateDirection();
    
    this.draw = function(camera_p){
        
        this.group.x = this.x+camera_p.x;
        this.group.y = this.y+camera_p.y;
        
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

Turbo.DefaultCommands = {
    COMMAND_WAIT:function(){},
    COMMAND_ANIMATE:function(that){that.step();},
    COMMAND_FACE_NORTH:function(that){
        SetPersonDirection(that.name, "north");
    },
    COMMAND_FACE_NORTHEAST:function(that){
        SetPersonDirection(that.name, "northeast");
    },
    COMMAND_FACE_EAST:function(that){
        SetPersonDirection(that.name, "east");
    },
    COMMAND_FACE_SOUTHEAST:function(that){
        SetPersonDirection(that.name, "southeast");
    },
    COMMAND_FACE_SOUTH:function(that){
        SetPersonDirection(that.name, "south");
    },
    COMMAND_FACE_SOUTHWEST:function(that){
        SetPersonDirection(that.name, "southwest");
    },
    COMMAND_FACE_WEST:function(that){
        SetPersonDirection(that.name, "west");
    },
    COMMAND_FACE_NORTHWEST:function(that){
        SetPersonDirection(that.name, "northwest");
    },
    COMMAND_MOVE_NORTH:function(that){
        that.tryWalkTo(that.x, that.y-that.speed.y, Turbo.CurrentMap);
    },
    COMMAND_MOVE_EAST:function(that){
        that.tryWalkTo(that.x+that.speed.x, that.y, Turbo.CurrentMap);
    },
    COMMAND_MOVE_SOUTH:function(that){
        that.tryWalkTo(that.x, that.y+that.speed.y, Turbo.CurrentMap);
    },
    COMMAND_MOVE_WEST:function(that){
        that.tryWalkTo(that.x-that.speed.x, that.y, Turbo.CurrentMap);
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
*/


/*

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

      GetPersonAngle(name)
        - returns the person's angle that is used

      SetPersonAngle(name, angle)
        - sets the angle which the person should be drawn at
        Note:
          Zero is no rotation, and angles are in radians.
          It does not rotate the spritesets obstruction base.

      SetPersonMask(name, color)
        - sets a color multiplier to use when drawing sprites.  if the color is
          RGBA:(255, 0, 0, 255), only the red elements of the sprite are drawn.
          If the color is RGBA:(255, 255, 255, 128), the sprite is drawn at
          half transparency.

      GetPersonMask(name)
        - returns the person's current mask

      IsPersonVisible(name)
        - returns the person's visible status 
*/

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
    var cmd = Turbo.DefaultCommands[command];
    
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

/*
      
      IsPersonObstructed(name, x, y)
      // returns true if person 'name' would be obstructed at (x, y)

      GetObstructingTile(name, x, y)
      // returns -1 if name isn't obstructed by a tile at x, y,
       - returns the tile index of the tile if name is obstructed at x, y

      GetObstructingPerson(name, x, y)
      // returns "" if name isn't obstructed by person at x, y,
       - returns the name of the person if name is obstructed at x, y

      IgnorePersonObstructions(person, ignore)
       - Sets whether 'person' should ignore other spriteset bases

      IsIgnoringPersonObstructions(person)
       - Returns true if 'person' is ignoring person obstructions, else false
       
      IgnoreTileObstructions(person, ignore)
       - Sets whether 'person' should ignore tile obstructions

      IsIgnoringTileObstructions(person)
       - Returns true if 'person' is ignoring tile obstructions, else false

      GetPersonIgnoreList(person)
       - Returns a list of people that 'name' is ignoring

      SetPersonIgnoreList(person, ignore_list)
       - Tells 'person' to ignore everyone in ignore_list
       e.g. SetPersonIgnoreList("White-Bomberman", ["bomb", "powerup"]);
       Tells White-Bomberman to not be obstructed by bombs or powerups

      SetTalkActivationKey(key)
      GetTalkActivationKey()
        - set key used to activate talk scripts

      SetTalkDistance(pixels)
      GetTalkDistance()
        - set distance to check for talk script activation
*/

