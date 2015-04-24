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
    var scheme = scheme_file.readString();
    return JSON.parse(scheme);
}

Turbo.GetSchemeLength = function(scheme){
    var s = 0;
    scheme.forEach(function(i){s += i.size;});
    return s;
}

Turbo.AddSchemeElementToObject = function(element, object, reader){

    if(element.size > reader.size() - reader.tell())
        throw "Unexpected end of file. Element " + element + " (" + element.name + ") needs " + element.size + " bytes at offset " + reader.tell() + ", but the file is only " + reader.tell() + " bytes long.";

    if(element.type=="number"){
        if(element.size==1)
            object[element.name] = reader.getByte();
        else if(element.size==2)
            object[element.name] = reader.getWord();
        else if(element.size==4)
            object[element.name] = reader.getDWord();
        else
            throw "Invalid number size of " + element.size + " for element '" + element.name + "'";
    }
    else if(element.type=="flag"){
        if(element.size!=1)
            throw "Flag of size " + element.size + ", only flags of size 1 supported.";
        object[element.name] = !(!(reader.getByte()));
    }
    else if(element.type=="string"){

        var len = reader.getWord();

        object[element.name] = CreateStringFromByteArray(reader.read(len));

    }
    else if(element.type=="fixed_string"){
        object[element.name] = CreateStringFromByteArray(reader.read(element.size));
    }
    else if(element.type=="float"){

        object[element.name] = ieee754.read(reader.read(element.size), 0, false /* <= Is this right? */, (element.size*6)-1, element.size);
    }
    else{
        reader.seek(element.size, Turbo.SEEK_CUR);
    }
   // if(element.name=="num_layers")
   //     throw object[element.name] + ": num_layers " + element.type +" "+element.size+ " "+  " " + reader.tell();


}

Turbo.ReadBinaryObject = function(from, scheme){

    var scheme_length = Turbo.GetSchemeLength(scheme);

    if(!(from.size()-from.tell() >= scheme_length))
        throw "Unexpected end of input.";

    var obj = {string_length:0};

    scheme.forEach(function(i){Turbo.AddSchemeElementToObject(i, obj, from);});

    return obj;

}

