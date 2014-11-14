// Supplies a ByteArray object for engines that don't have one natively.

RequireSystemScript("turbo/arraybuffer_bytearray.js");

// Internal functions for working with ByteArrays.
// NOTE These functions may chang at any time, it is NOT safe to use them.

if(typeof Turbo == "undefined")
  Turbo = {};

Turbo.dByteCat = function(first, second){
    return (first) | (second<<8);
}


Turbo.qByteCat = function(first, second, third, fourth){
    return (Turbo.dByteCat(third, fourth) << 16) | Turbo.dByteCat(first, second)
}


