RequireSystemScript('turbo/for_each.js');
RequireSystemScript('turbo/bytearray.js');
RequireSystemScript('turbo/json2.js');

if(typeof Turbo == "undefined")
    var Turbo = {};

if(typeof Turbo.Classic == "undefined")
    Turbo.Classic = {};

/*///////////////////////////////////////////////////////////////////////////*/
//                 Decode Binary Files Based on a JSON Scheme                //
/*///////////////////////////////////////////////////////////////////////////*/

Turbo.dByteCat = function(first, second){
    return (first) | (second<<8);
}

Turbo.qByteCat = function(first, second, third, fourth){
    return (Turbo.dByteCat(third, fourth) << 16) | Turbo.dByteCat(first, second)
}

// Remember to increment the cursor by length.
// Length includes the size value.
Turbo.Classic.readString = Turbo.Classic.readString || function(array, at){

    if(array.length<at)
      throw "Unexpected end of file."

    var len = Turbo.dByteCat(array[at++], array[at++]);

    if(array.length<at+len)
      throw "Unexpected end of file.";

    var str = array.slice(at, at+len);

    var string = CreateStringFromByteArray(str);

    return {length:len+2, string:string};

}

Turbo.LoadSystemScheme = function(name){

    var scheme_file = new RawFile("#~/formats/"+name);
    var scheme_buffer = scheme_file.read(scheme_file.size);

    var scheme_byteview = new Uint8Array(scheme_buffer);
    var scheme_bytearray = ByteArrayFromTypedArray(scheme_byteview);

    return JSON.parse(CreateStringFromByteArray(scheme_bytearray));
}

Turbo.GetSchemeLength = function(scheme){
    var s = 0;
    scheme.forEach(function(i){s += i.size;});
    return s;
}

Turbo.ReadBinaryObject = function(array, offset, scheme){

    var scheme_length = Turbo.GetSchemeLength(scheme);
    if(!(array.length-offset > scheme_length))
        throw "Unexpected end of input.";

    var obj = {};

    var at = offset;

    for(var i in scheme){
        var element = scheme[i];
        if(element.type=="number"){
            if(element.size==1)
                obj[element.name] = array[at++];
            else if(element.size==2)
                obj[element.name] = Turbo.dByteCat(array[at++], array[at++]);
            else if(element.size==4)
                obj[element.name] = Turbo.qByteCat(array[at++], array[at++], array[at++], array[at++]);
            else
                throw "Invalid number size of " + element.size + " for element '" + element.name + "'";
        }
        else if(element.type=="flag"){
            if(element.size!=1)
                throw "Flag of size " + element.size + ", only flags of size 1 supported.";
            obj[element.name] = !(!(array[at++]));
        }
        else if(element.type=="string"){
            var string = Turbo.Classic.readString(array, offset);
            at += string.length; // Adjust the size to match the string.
            obj[element.name] = string.string;
        }
        else if(element.type=="fixed_string"){
            var element_str_buffer = array.slice(at, at+element.size);
            obj[element.name] = CreateStringFromByteArray(element_str_buffer);
            at+=element.size;

            if(element.size!=obj[element.name].length)
                throw "Bad math, doofus. Should be " + element.size + ", was " + obj[element.name].length;

        }
        else{
            at+=element.size;
        }
    }

    if(at-offset!=scheme_length)
        throw "Bad math, doofus. Should be " + scheme_length + ", was " + (at-offset);

    return obj;

}
