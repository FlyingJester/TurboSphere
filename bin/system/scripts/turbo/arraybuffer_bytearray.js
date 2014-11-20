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
function ByteArrayFromTypedArray(buffer){
    var byteview;
    if(buffer instanceof ArrayBuffer)
      byteview = new Uint8Array(buffer);
    else if(buffer instanceof Uint8Array)
      byteview = buffer;
    else
      throw "Argument 0 is not a Harmony Typed Array.";

    // Tack on the members needed for ByteArray.
    Object.defineProperty(byteview, "length", {value:byteview.byteLength});
    Object.defineProperty(byteview, "concat", {
        value:function(a){
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
    });

    Object.defineProperty(byteview, "slice", {
        value:function(a, e){
            return ByteArrayFromTypedArray(new Uint8Array(this.buffer.slice(a, e)));
        }
    });

    return byteview;
}

// Replacement for ByteArray 'constructor'
function CreateByteArray(length){
    var buffer = new ArrayBuffer(length);
    var byteview = new Uint8Array(buffer);
    return ByteArrayFromTypedArray(byteview);
}

// Replacement for ByteArray 'constructor' from string
function CreateByteArrayFromString(a){
    var r = CreateByteArray(a.length);
    for(var i = 0;i<a.length; i++)
      r[i] = a.charCodeAt(i);
    return r;
}

// Create a string from a TypedArray
function CreateStringFromByteArray(a){
    if((!(a instanceof Uint8Array)) && (!(typeof a == "array")))
        throw "Argument 0 is not a ByteArray, JS Array, or a Harmony Typed Array.";

    var r = "";
    var va = Array.apply([], a);
    for(var i = 0;i<a.length; i++)
      r += String.fromCharCode(va[i]);

    return r;
}
