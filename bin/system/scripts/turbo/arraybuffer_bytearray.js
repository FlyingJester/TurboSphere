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
if(typeof ByteArrayFromTypedArray == "undefined")
var ByteArrayFromTypedArray = function(byteview){

    if(byteview instanceof ArrayBuffer)
        byteview = new Uint8Array(byteview);

    if(!(byteview instanceof Uint8Array))
      throw "Argument 0 is not a Harmony Typed Array.";

//    Object.defineProperty(byteview, "length", {value:byteview.byteLength});
    Object.defineProperty(byteview, "concat",
        {value:function(a){
            if(!(a instanceof Uint8Array))
              throw "Argument 0 is not a ByteArray or a Harmony Typed Array.";
            var thisArray = Array.apply([], this);
            var otherArray = Array.apply([], a);
            var cArray = thisArray.concat(otherArray);
            var r = CreateByteArray(cArray.length);
            for(var i = 0;i<cArray.length; i++)
              r[i] = cArray[i];
            return r;
    }});
    Object.defineProperty(byteview, "slice",
        {value:function(a, e){
            var thisArray = Array.apply([], this);
            var sArray = thisArray.slice(a, e);
            var r = CreateByteArray(sArray.length);
            for(var i = 0;i<sArray.length; i++)
              r[i] = sArray[i];
            return r;
    }});
    
    return byteview;
}

if(typeof ByteArrayFromArrayBuffer == "undefined")
var ByteArrayFromArrayBuffer = function(buffer){
    if(buffer instanceof ArrayBuffer)
        return ByteArrayFromTypedArray(new Uint8Array(buffer));
    else throw "Argument 0 is not a Harmony Array Buffer"
}

// Replacement for ByteArray 'constructor'
if(typeof CreateByteArray == "undefined")
var CreateByteArray = function(length){
    var buffer = new ArrayBuffer(length);
    return ByteArrayFromArrayBuffer(buffer);
}

// Replacement for ByteArray 'constructor' from string
if(typeof CreateByteArrayFromString == "undefined")
var CreateByteArrayFromString = function(a){
    var r = CreateByteArray(a.length);
    for(var i = 0;i<a.length; i++)
      r[i] = a.charCodeAt(i);
    return r;
}

// Create a string from a TypedArray
if(typeof CreateStringFromByteArray == "undefined")
var CreateStringFromByteArray = function(a){

    if(a instanceof ArrayBuffer)
        var buffer = new Uint8Array(a);
    else
        var buffer = a;

    if((!(buffer instanceof Uint8Array)) && (!Array.isArray(buffer)))
        throw "Argument 0 is not a ByteArray, JS Array, or a Harmony Typed Array. Object is " + typeof a;

    var r = "";
    for(var i = 0;i<buffer.length; i++)
      r += String.fromCharCode(buffer[i]);

    return r;
}
