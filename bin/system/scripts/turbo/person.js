RequireSystemScript('turbo/bytearray.js');
RequireSystemScript('turbo/format.js');
RequireSystemScript('turbo/map.js');
RequireSystemScript('turbo/tileset.js');
RequireSystemScript('turbo/spriteset.js');

if(typeof Turbo == "undefined")
    var Turbo = {};
Turbo.Classic = Turbo.Classic || {};

Turbo.Entity = function(x, y, layer, name, destroy){
    this.x = x; this.y = y;
    this.draw_offset = {x:0, y:0};
    this.layer = layer;
    this.name = name;
    this.destroy_on_map_change = destroy;
}

Turbo.GetPerson = function(name){
    if(Turbo.CurrentMap==null){
        for(var i in Turbo.SpooledEntities){
            if(Turbo.SpooledEntities[i].name==name)
                return Turbo.SpooledEntities[i];
        }
        
        return null;
    }
    else{
        return Turbo.CurrentMap.GetPerson(name);
    }
}

// Small helper to validate names.
Turbo.GetPersonThrow = function(name){
    var person = Turbo.GetPerson(name);
    if(person==null){
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
    }
    
    
    this.setSpriteset(spriteset);

    this.direction_i = 0;
    this.validateDirection = function(){
        this.direction = this.spriteset.directions[this.direction_i].name;
        this.frame = 0;
    }
    this.validateDirection();
    
    this.getDirection = function(){
        return this.spriteset.directions[person.spriteset.direction_i];
    }
    
    this.__proto__ = new Turbo.Entity(x, y, layer, name, destroy);

    this.onCreate  = function(){};
    this.onDestroy = function(){};
    this.onTalk    = function(){};
    this.onTouch   = function(){};
    this.onGenerate= function(){};
}

Turbo.Trigger = function(x, y, layer, name){
    this.__proto__ = new Turbo.Entity(x, y, layer, name, true);
    this.onTrigger = function(){};
}

// Remember to increment the cursor by length.
// Length includes the size value.
Turbo.Classic.readString = Turbo.Classic.readString || function(bytearray, at){

    if(bytearray.length<at)
      throw "Unexpected end of file.";

    var len = Turbo.dByteCat(bytearray[at++], bytearray[at++]);

    if(bytearray.length<at+len)
      throw "Unexpected end of file.";

    var str = bytearray.slice(at, at+len);

    var string = CreateStringFromByteArray(str);

    return {length:len+2, string:string};

}

Turbo.LoadEntity = function(array, i, entity){
    var at = i;

    var type = Turbo.dByteCat(array[at++], array[at++]);

    var data = {x:Turbo.dByteCat(array[at++], array[at++]),
                   y:Turbo.dByteCat(array[at++], array[at++]),
                   layer:Turbo.dByteCat(array[at++], array[at++])};

    /*reserved[8]*/ at+=8;

    var script_names = ['onCreate', 'onDestroy', 'onTalk', 'onTouch', 'onGenerate'];

    var string = Turbo.Classic.readString(array, at);
    at+=string.length;
    var name = string.string;

    var string = Turbo.Classic.readString(array, at);
    at+=string.length;
    var spriteset = string.string;

    if(type==1){
        entity = new Turbo.Person(data.x, data.y, data.layer, name, true, spriteset);

        for(var j in script_names){
            var string = Turbo.Classic.readString(array, at);
            at+=string.length;
            entity[script_names[j]] = function(){eval(string.string);};
        }

        /*reserved[16]*/ at+=16;
    }
    else{
        entity = new Turbo.Person(data.x, data.y, data.layer, name, true, spriteset);

        var string = Turbo.Classic.readString(array, at);
        at+=string.length;
        entity.onTrigger = function(){eval(string.string);};
        entity.onTrigger = function(){eval(string.string);};

    }



    return i-at;
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
    var person = Turbo.GetPersonThrow(name);
    
    for(var i in person.spriteset.directions){
        if(person.spriteset.directions[i].name == direction){
            person.direction_i = i;
            person.validateDirection();
            return;
        }
    }
    
    throw "No direction " + direction + " in spriteset for person " + name;
    
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
    var person = Turbo.GetPersonThrow(name);
        switch(which){

        case SCRIPT_ON_CREATE:
        person.onCreate();
        break;
        case SCRIPT_ON_DESTROY:
        person.onDestroy();
        break;
        case SCRIPT_ON_ACTIVATE_TOUCH:
        person.onTouch();
        break;
        case SCRIPT_ON_ACTIVATE_TALK:
        person.onTalk();
        break;
        case SCRIPT_COMMAND_GENERATOR:
        person.onGenerate();
        break;
        
        default:
        throw "No such script type as " + script;
    }    
}

/*
      
      QueuePersonCommand(name, command, immediate)
        - adds a command to the person's command queue
          the commands are:
            COMMAND_WAIT
            COMMAND_ANIMATE
            COMMAND_FACE_NORTH
            COMMAND_FACE_NORTHEAST
            COMMAND_FACE_EAST
            COMMAND_FACE_SOUTHEAST
            COMMAND_FACE_SOUTH
            COMMAND_FACE_SOUTHWEST
            COMMAND_FACE_WEST
            COMMAND_FACE_NORTHWEST
            COMMAND_MOVE_NORTH
            COMMAND_MOVE_EAST
            COMMAND_MOVE_SOUTH
            COMMAND_MOVE_WEST
          (note: these *might* change in a future release
          'immediate', if true, will execute the command go right away
          if false, it will wait until the next frame)
      
      QueuePersonScript(name, script, immediate)
        - adds a script command to the person's queue
      
      ClearPersonCommands(name)
        - clears the command queue of sprite with the 'name'

      IsCommandQueueEmpty(name)
        - returns true if the person 'name' has an empty command queue

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

