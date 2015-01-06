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

    this.speed = 1.0;
    this.spriteset = spriteset;

    this.direction_i = 0;
    this.validate_direction = function(){
        this.direction = this.spriteset.directions[this.direction_i].name;
    }
    
    this.validate_direction();
    
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

function SetPersonX(name, x){
    Turbo.GetPersonThrow(name).x = x;
}

function SetPersonY(name, y){
    Turbo.GetPersonThrow(name).y = y;
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

}

/*
      SetPersonFrame(name, frame)
        - sets which frame from which direction to display
      
      GetPersonX(name)
      GetPersonY(name)
            
      GetPersonLayer(name)
        - Gets the position of the person on the map.
          The position is based on the middle of the spriteset's base
          rectangle.
      GetPersonXFloat(name)
      GetPersonYFloat(name)
        - Gets the position of the person on the map in floating point 
          accuracy.
      
      GetPersonDirection(name)
      GetPersonFrame(name)
        - gets the frame and direction that are currently being displayed

      SetPersonSpeed(name, speed)
      SetPersonSpeedXY(name, speed_x, speed_y)
        - sets the speed at which a person moves at

      GetPersonSpeedX(name)
      GetPersonSpeedY(name)
        - gets the speed at which a person moves at

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

      GetPersonSpriteset(name)
        - returns the person's spriteset.

      SetPersonSpriteset(name, spriteset)
        - set's the person's spriteset to spriteset
        e.g. SetPersonSpriteset("Jimmy", LoadSpriteset("jimmy.running.rss"));

      GetPersonBase(name)
        - returns the person's base obstruction object.

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
        

      SetPersonVisible(name, visible)
        - sets the person's visible status, true = visible, false = not visible
        e.g. SetPersonVisible(GetCurrentPerson(), !IsPersonVisible(GetCurrentPerson()));

      GetPersonData(name)
        - gets a data object assiocated with the person 'name'
        There are certain default properties/values filled in by the engine, they are:
        num_frames - the number of frames for the person's current direction
        num_directions - the number of directions for the person
        width - the width of the spriteset's current frame
        height - the height of the spriteset's current frame
        leader - the person that this person is following, or "" if no-one...
        Any other properties are free for you to fill with values

        e.g. var data = GetPersonData("Jimmy");
        var num_frames = data["num_frames"];

      SetPersonData(name, data)
        - sets the 'data' object assiocated with the person 'name'
        e.g.
        var data = GetPersonData("Jimmy");
        data["talked_to_jimmy"] = true;
        SetPersonData("Jimmy", data);

      SetPersonValue(name, key, value)
        - SetPersonValue("Jimmy", "talked_to_jimmy", true); // same as code above

      GetPersonValue(name, key)
        - GetPersonValue("Jimmy", "num_frames"); // same as previous code above

      FollowPerson(name, leader, pixels)
        - makes the sprite 'name' follow 'pixels' pixels behind sprite 'leader'.
        If this function is called like:
        FollowPerson(name, "", 0),
        the person will detach from anyone it is following.
      
      SetPersonScript(name, which, script)
        - sets 'script' as the thing 'name' does in a certain event
          the five events are
          SCRIPT_ON_CREATE
          SCRIPT_ON_DESTROY
          SCRIPT_ON_ACTIVATE_TOUCH
          SCRIPT_ON_ACTIVATE_TALK
          SCRIPT_COMMAND_GENERATOR
          (SCRIPT_COMMAND_GENERATOR will be called when the command queue for
           the person runs out (for random movement thingies, etc))
      
      CallPersonScript(name, which)
        - calls a person's script from code
         'which' constants are the same as for SetPersonScript()
      
      GetCurrentPerson()
        - best when called from inside a PersonScript handler
          it will return the name of the person for whom the current script 
          is running

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

