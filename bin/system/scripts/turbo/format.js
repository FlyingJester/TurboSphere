RequireSystemScript('turbo/for_each.js');
RequireSystemScript('turbo/bytearray.js');
RequireSystemScript('turbo/json2.js');
RequireSystemScript('turbo/binaryfloat.js');

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

Turbo.AddSchemeElementToObject = function(element, object, buffer){
    var at = 0;
    for(var i in scheme){
        var element = scheme[i];

        if(at+element.size > array.length)
            throw "Unexpected end of file. Element " + i + " (" + element.name + ") needs " + element.size + " bytes at offset " + at + ", but the file is only " + array.length + " bytes long.";

        if(element.type=="number"){
            if(element.size==1)
                object[element.name] = array[at++];
            else if(element.size==2)
                object[element.name] = Turbo.dByteCat(array[at++], array[at++]);
            else if(element.size==4)
                object[element.name] = Turbo.qByteCat(array[at++], array[at++], array[at++], array[at++]);
            else
                throw "Invalid number size of " + element.size + " for element '" + element.name + "'";
        }
        else if(element.type=="flag"){
            if(element.size!=1)
                throw "Flag of size " + element.size + ", only flags of size 1 supported.";
            object[element.name] = !(!(array[at++]));
        }
        else if(element.type=="string"){
            var string = Turbo.Classic.readString(array, at);
            at += string.length; // Adjust the size to match the string.
            object.string_length += string.string.length;
            if(string.length>0xFF)
                throw "This is a temporary warning. " + "Unexpected end of file. Element " + i + " (" + element.name + ") needs " + (element.size+string.length) + " bytes at offset " + at + ", but the file is only " + array.length + " bytes long.";
            object[element.name] = string.string;
        }
        else if(element.type=="fixed_string"){
            var element_str_buffer = array.slice(at, at+element.size);
            object[element.name] = CreateStringFromByteArray(element_str_buffer);
            at+=element.size;
            if(element.size!=object[element.name].length)
                throw "Bad math, doofus. Should be " + element.size + ", was " + object[element.name].length;
        }
        else if(element.type=="float"){
            object[element.name] = ieee754.read(array, at, false /* <= Is this right? */, (element.size*6)-1, element.size);
            at+=element.size;
        }
        else{
            at+=element.size;
        }
    }

    return at;

}

Turbo.ReadBinaryObject = function(from, offset, scheme){

    var scheme_length = Turbo.GetSchemeLength(scheme);

    var length = from.length || from.size;

    if(!(length-offset > scheme_length))
        throw "Unexpected end of input.";

    var SliceArray;

    // If we don't need to jump through ByteArray/TypedArray hoops, just slice it.
    if((typeof from.buffer == "undefined") || (typeof from.buffer.slice != "function")){
        SliceArray = function(from_, to){return from.slice(from_, to);}
    }
    else if (typeof from.read == "function"){
        SliceArray = function(from_, to){if(typeof to == "undefined") to = from.size; from.seek(offset+from_); return from.read(to-from_);}
    }
    else {
        SliceArray = function(from_, to){return from.buffer.slice(from_, to);}
    }

    var obj = {string_length:0};

    var at = offset;

    for(var i in scheme){
        var element = scheme[i];
        at+=AddSchemeElementToObject(element, obj, SliceArray(at));
    }

    if(at-offset-obj.string_length!=scheme_length)
        throw "Bad math, doofus. Should be " + scheme_length + ", was " + (at-offset-obj.string_length) + " with " + obj.string_length + " bytes of strings.";

    return obj;

}

Turbo.ReadBinaryDataWithReader = function(reader, scheme){
    var data = Turbo.ReadBinaryObject(reader, reader.tell(), Turbo.EntityScheme.header);
    at += Turbo.GetSchemeLength(Turbo.EntityScheme.header) + this.entities[i].string_length;
}
