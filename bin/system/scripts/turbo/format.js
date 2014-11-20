RequireSystemScript('turbo/for_each.js');
RequireSystemScript('turbo/bytearray.js');

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

    var scheme_file = new RawFile(FILESYSTEM_SYSTEM+"/formats/"+name);
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

    if(!(array.length-offset > Turbo.GetSchemeLength(scheme)))
        throw "Unexpected end of input.";

    var obj = {};

    scheme.forEach(function(element){
        element_size =element.size;
        switch(element.type){
        case "number":
            switch(element.size){
            case 1:
                obj[element.name] = array[offset];
                break;
            case 2:
                obj[element.name] = Turbo.dByteCat(array[offset], array[offset+1]);
                break;
            case 4:
                obj[element.name] = Turbo.qByteCat(array[offset], array[offset+1], array[offset+2], array[offset+3]);
                break;
            default:
                throw "Invalid number size of " + element.size + " for element '" + element.name + "'";
            }
        break;
        case "flag":
            if(element.size!=1)
                throw "Flag of size " + element.size + ", only flags of size 1 supported.";
            obj[element.name] = (array[offset] && true);
        break;
        case "string":
            var string = Turbo.Classic.readString(array, offset);
            element_size = string.length; // Adjust the size to match the string.
            obj[element.name] = string.string;
        break;
        }
        offset += element_size;
    });

    return obj;

}
