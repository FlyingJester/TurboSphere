/////
// An implementation of the Sphere 1.x ByteArray object using EcmaScript 6 TypedArrays.
//
// Tested to work in SpiderMonkey and V8.
// Written by FlyingJester.
//
// Most of these functions work with any Uint8Array or a ByteArray. They do not work
// with ByteArrays as defined by Sphere 1.x
//

// Wrapper function to allow any TypedArray to 'become' a ByteArray.
function ByteArrayFromTypedArray(byteview){
    if(!(byteview instanceof Uint8Array))
      throw "Argument 0 is not a Harmony Typed Array.";

    byteview.length = byteview.byteLength;
    byteview.concat = function(a){
        if(!(a instanceof Uint8Array))
            throw "Argument 0 is not a ByteArray or a Harmony Typed Array.";
        var thisArray = Array.apply([], this);
        var otherArray = Array.apply([], a);
        var cArray = thisArray.concat(otherArray);
        var r = CreateByteArray(cArray.length);
        for(var i = 0;i<cArray.length; i++)
            r[i] = cArray[i];
        return r;
    }
    
    byteview.slice = function(a, e){
        return ByteArrayFromTypedArray(new Uint8Array(this.buffer.slice(a, e)));
    }
    
    return byteview;
}

function ByteArrayFromArrayBuffer(buffer){
    if(buffer instanceof ArrayBuffer)
        return ByteArrayFromTypedArray(new Uint8Array(buffer));
    else throw "Argument 0 is not a Harmony Array Buffer"
}

// Replacement for ByteArray 'constructor'
function CreateByteArray(length){
    var buffer = new ArrayBuffer(length);
    return ByteArrayFromArrayBuffer(buffer);
}

// Replacement for ByteArray 'constructor' from string
function CreateByteArrayFromString(a){
    var r = CreateByteArray(a.length);
    for(var i = 0;i<a.length; i++){
      r[i] = a.charCodeAt(i);
    }
    return r;
}

// Create a string from a TypedArray
function CreateStringFromByteArray(a){
    if((!(a instanceof Uint8Array)) && (!Array.isArray(a)))
        throw "Argument 0 is not a ByteArray, JS Array, or a Harmony Typed Array. Object is " + typeof a;

    var r = "";
    for(var i = 0;i<a.length; i++)
      r += String.fromCharCode(a[i]);

    return r;
}
