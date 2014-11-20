
if(typeof Turbo == "undefined")
    var Turbo = {};
Turbo.Classic = Turbo.Classic || {};

// Remember to increment the cursor by length.
// Length includes the size value.
Turbo.Classic.readString = Turbo.Classic.readString || function(bytearray, at){

    if(bytearray.length<at)
      throw "Unexpected end of file."

    var len = Turbo.dByteCat(bytearray[at++], bytearray[at++]);

    if(bytearray.length<at+len)
      throw "Unexpected end of file.";

    var str = bytearray.slice(at, at+len);

    var string = CreateStringFromByteArray(str);

    return {length:len+2, string:string};

}

Turbo.LoadEntity = function(array, i entity){
    var at = i;
    entitiy = {x:Turbo.dByteCat(array[at++], array[at++]),
                   y:Turbo.dByteCat(array[at++], array[at++]),
                   layer:Turbo.dByteCat(array[at++], array[at++]),
                   isTrigger:false};

    var type = Turbo.dByteCat(array[at++], array[at++]);

    /*reserved[8]*/ at+=8;

    var script_names = ['on_create', 'on_destroy', 'on_talk', 'on_touch', 'on_generate'];

    var string = Turbo.Classic.readString(array, at);
    at+=string.length;
    entitiy.name = string.string;

    var string = Turbo.Classic.readString(array, at);
    at+=string.length;
    entitiy.spriteset = string.string;

    switch(type){
    case 1:

        for(var j in script_names){
            var string = Turbo.Classic.readString(array, at);
            at+=string.length;
            entitiy[script_names[j]] = function(){eval(string.string);};
        }

    break;
    case 2:

        for(var j in script_names){
            var string = Turbo.Classic.readString(array, at);
            at+=string.length;
            entitiy[script_names[j]] = function(){};
        }
        entitiy.isTrigger = true;

    }

    /*reserved[16]*/ at+=16;

    var string = Turbo.Classic.readString(array, at);
    at+=string.length;
    entitiy.trigger_script = function(){eval(string.string);};

    return i-at;
}
