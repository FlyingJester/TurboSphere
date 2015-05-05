RequireSystemScript('turbo/format.js'); // For Turbo.dByteCat and Turbo.qByteCat

if(typeof Turbo == "undefined")
    var Turbo = {}

Turbo.SEEK_SET = 0;
Turbo.SEEK_CUR = 1;
Turbo.SEEK_END = 2;

// A stream object is defined as such:
/*
Namespace.StreamType = function(input, offset){

    if(typeof offset == "undefined")
        offset = 0;

    // Implement this.size();
    // Implement this.read(num); // Can return a ByteArray or an Array
    // Implement this.getByte();
    // Implement this.getWord();
    // Implement this.getDWord();
    // Implement this.seek(to, whence);
    // Implement this.tell();
    // Implement this.slice(from, to); // Can return a ByteArray or an Array

    this.getChar = this.getByte;
    this.getShort = this.getWord;
    this.getInt   = this.getDWord;
    this.getLongLong = this.getQWord;

}
*/

// Constructs a stream from an Array or a ByteArray.
Turbo.ArrayReader = function(input, offset){

    if(typeof offset == "undefined")
        offset = 0;

    this.guts = input;
    this.at = offset;

    // Implement this.size();
    if(typeof this.guts.length != "undefined")
        this.size = function(){return this.guts.length;};
    else if(typeof this.guts.byteLength != "undefined")
        this.size = function(){return this.guts.byteLength;};
    else if(typeof this.guts.byteLength != "undefined")
        this.size = function(){return this.guts.byteLength;};
    else
        throw "No known way to determine length of input.";

    // Implement this.read(num); // Can return a ByteArray or an Array
    this.read = function(num){
        var to = this.at+num;
        return this.guts.slice(this.at, to);
        this.at = to;
    }

    // Implement this.getByte();
    this.getByte = function(){
        return this.guts[this.at++];
    }

    // Implement this.getWord();
    this.getWord = function(){
        return Turbo.dByteCat(this.guts[this.at++], this.guts[this.at++]);
    }

    // Implement this.getDWord();
    this.getDWord = function(){
        return Turbo.qByteCat(this.guts[this.at++], this.guts[this.at++], this.guts[this.at++], this.guts[this.at++]);
    }

    // Implement this.seek(to, whence);
    this.seek = function(to, whence){

        if(typeof whence == "undefined")
            whence = Turbo.SEEK_SET;

        switch(whence){
        case Turbo.SEEK_SET:
                if(to>this.size())
                    throw "Invalid seek. Tried to seek to " + to + ", maximum possible is " + this.size();
                this.at = offset + to;
            break;
            case Turbo.SEEK_CUR:
                if(to>this.size()-this.at)
                    throw "Invalid seek. Tried to seek to " + to + ", maximum possible is " + this.size()-this.at;
                this.at += to;
            break;
            case Turbo.SEEK_END:
                if(to>this.size())
                    throw "Invalid seek. Tried to seek to " + to + ", maximum possible is " + this.size();
                this.at = offset+this.size()-to;
            break;
            default:
                throw "Argument two must be Turbo.SEEK_SET, Turbo.SEEK_CUR, or Turbo.SEEK_END.";
        }
        return this.tell();
    }


    // Implement this.slice(from, to); //< Return value must have [] operator.
    this.slice = function(from, to){return this.guts.slice(from, to);};

    // Implement this.tell();
    this.tell = function(){return this.at;};

    // Aliases
    this.getChar = this.getByte;
    this.getShort = this.getWord;
    this.getInt   = this.getDWord;
    this.getLongLong = this.getQWord;

}

Turbo.FileReader = function(input, offset){

    if(typeof offset == "undefined")
        offset = 0;

    this.guts = input;
    this.guts.setPosition(offset);

    // Implement this.size();
    this.size = function(){return this.guts.size;}
    
    
    if(typeof ArrayBuffer == "undefined"){
        // Implement this.read(num); // Can return a ByteArray or an Array
        this.read = function(n){return this.guts.read(n);}
        // Implement this.getByte();
        this.getByte = function(){return this.guts.read(1)[0];}
        // Implement this.getWord();
        this.getWord = function(){
            var that = this.guts.read(2); 
            return Turbo.dByteCat(that[0], that[1]);
        }
        // Implement this.getDWord();
        this.getDWord = function(){
            var that = this.guts.read(4);
            return Turbo.qByteCat(that[0], that[1], that[2], that[3]);
        }
    }
    else{
        // Implement this.read(num); // Can return a ByteArray or an Array
        this.read = function(n){return ByteArrayFromTypedArray(this.guts.read(n));}
        // Implement this.getByte();
        this.getByte = function(){return (new Int8Array(this.guts.read(1)))[0];}
        // Implement this.getWord();
        this.getWord = function(){return (new Int16Array(this.guts.read(2)))[0];}
        // Implement this.getDWord();
        this.getDWord = function(){return (new Int32Array(this.guts.read(4)))[0];}
    }
    
    // Implement this.seek(to, whence);
    this.seek = function(to, whence){

        if(typeof whence == "undefined")
            whence = Turbo.SEEK_SET;

        var at = this.guts.getPosition();

        switch(whence){
        case Turbo.SEEK_SET:
                if(to>this.size())
                    throw "Invalid seek. Tried to seek to " + to + ", maximum possible is " + this.size();
                at = to;
            break;
            case Turbo.SEEK_CUR:
                if(to>this.size()-this.at)
                    throw "Invalid seek. Tried to seek to " + to + ", maximum possible is " + this.size()-at;
                at += to;
            break;
            case Turbo.SEEK_END:
                if(to>this.size())
                    throw "Invalid seek. Tried to seek to " + to + ", maximum possible is " + this.size();
                this.at = this.size()-to;
            break;
            default:
                throw "Argument two must be Turbo.SEEK_SET, Turbo.SEEK_CUR, or Turbo.SEEK_END.";
        }
        this.guts.setPosition(at);
        return this.tell();
    }
    // Implement this.tell();
    this.tell = function(){
        return this.guts.getPosition();
    }
    // Implement this.slice(from, to); // Can return a ByteArray or an Array
    this.slice = function(from, to){
        var len = to-from;
        var at = this.guts.getPosition();

        this.guts.setPosition(from);
        var data = this.read(len);

        this.guts.setPosition(at);

        return data;
    }

    this.getChar = this.getByte;
    this.getShort = this.getWord;
    this.getInt   = this.getDWord;
    this.getLongLong = this.getQWord;

}

// Constructs a stream from a RawFile.
// Takes ownership of the RawFile it is passed.
// Attempts to work around V8 having poor ArrayBuffer implementation.
Turbo.BrokenFileReader = function(input, offset){

    if(typeof offset == "undefined")
        offset = 0;

    this.guts = input;
    this.guts.setPosition(offset);
    this.at = offset;
    // Implement this.size();
    this.size = function(){return this.guts.size;};

    // Chunks MUST be larger than 258 bytes.
    this.chunk_sizes = 1024; // This may grow at some point in flight.
    this.chunk = {size:0, offset:offset, data:null};

    this.chunkReset = function(to){
        this.at = this.guts.getPosition();
        this.chunk.size = this.chunk_sizes;
        this.chunk.offset = (to>>8)<<8;
        this.guts.setPosition(this.chunk.offset);
        this.chunk.data = this.guts.read(this.chunk.size);
        this.guts.setPosition(this.at);

        if(typeof GarbageCollect == "function")
            GarbageCollect();
    }

    this.moveChunkTo = function(to){
        if(!((this.chunk.offset <= this.at) &&
           (this.chunk.offset+this.chunk.size < to))){
            this.chunkReset(to);
        }
    }

    // Implement this.read(num); // Can return a ByteArray or an Array
    this.read = function(num){

        if(this.chunk.offset>this.at)
            throw "Illegal state.";

        if(num+this.chunk.offset < this.chunk.size + this.tell()){
            return new Uint8Array(this.chunk.data.slice((this.at - this.chunk.offset), (this.at - this.chunk.offset)+num));
        }

        this.mover = this.moveChunkTo;

        while(this.chunk_sizes-255<num){
            this.chunk_sizes<<=1;
            this.mover = this.chunkReset;
        }

        if(this.chunk.offset>this.at)
            throw "Illegal state.";

        this.mover(this.at);
        var data = new Uint8Array(this.guts.read(num));
        this.tell();

        if(data.length!=num)
            throw "Error creating buffer. Should have been " + num + " bytes long but was " + data.length;

        return data;
    }

    // Implement this.getByte();
    this.getByte = function(){

        this.moveChunkTo(this.at);
        return this.chunk.data[this.at-this.chunk.offset];
    }
    // Implement this.getWord();
    this.getWord = function(){
        this.moveChunkTo(this.at);

        if(this.chunk.offset>this.at)
            throw "Illegal state.";

        return Turbo.dByteCat(this.chunk.data[this.at-this.chunk.offset+0],
                              this.chunk.data[this.at-this.chunk.offset+1]);
    }
    // Implement this.getDWord();
    this.getDWord = function(){
        this.moveChunkTo(this.at);

        if(this.chunk.offset>this.at)
            throw "Illegal state.";

        return Turbo.qByteCat(this.chunk.data[this.at-this.chunk.offset+0],
                              this.chunk.data[this.at-this.chunk.offset+1],
                              this.chunk.data[this.at-this.chunk.offset+2],
                              this.chunk.data[this.at-this.chunk.offset+3]);
    }

    // Implement this.seek(to, whence);
    this.seek = function(to, whence){

        if(typeof whence == "undefined")
            whence = Turbo.SEEK_SET;

        switch(whence){
        case Turbo.SEEK_SET:
                if(to>this.size())
                    throw "Invalid seek. Tried to seek to " + to + ", maximum possible is " + this.size();
                this.at = offset + to;
            break;
            case Turbo.SEEK_CUR:
                if(to>this.size()-this.at)
                    throw "Invalid seek. Tried to seek to " + to + ", maximum possible is " + this.size()-this.at;
                this.at += to;
            break;
            case Turbo.SEEK_END:
                if(to>this.size())
                    throw "Invalid seek. Tried to seek to " + to + ", maximum possible is " + this.size();
                this.at = offset+this.size()-to;
            break;
            default:
                throw "Argument two must be Turbo.SEEK_SET, Turbo.SEEK_CUR, or Turbo.SEEK_END.";
        }
        this.moveChunkTo(this.at);
        this.guts.setPosition(this.at);
        return this.tell();
    }

    // Implement this.slice(from, to); //< Return value must have [] operator.
    this.slice = function(from, to){
        // Minus 255 to account for alignment.
        while(this.chunk_sizes-0xFF<to-from){
            this.chunk_sizes<<1;
        }

        this.moveChunkTo(from);

        if(this.chunk.offset>this.at)
            throw "Illegal state.";

        return new Uint8Array(this.chunk.data.buffer.slice(to-this.chunk.offset));

    }


    // Implement this.tell();
    this.tell = function(){this.at = this.guts.getPosition(); return this.guts.getPosition();};

    this.getChar = this.getByte;
    this.getShort = this.getWord;
    this.getInt   = this.getDWord;
    this.getLongLong = this.getQWord;

    this.chunkReset(this.at);

}

// Constructs a stream from a RawFile, an Array, or a ByteArray.
Turbo.Stream = function(input, offset){

    if(typeof offset == "undefined")
        offset = 0;

    if(((typeof Uint8Array == "function") && (input instanceof Uint8Array)) || (Array.isArray(input))){
        var reader = new Turbo.ArrayReader(input, offset);
    }
    else{
        var reader = new Turbo.FileReader(input, offset);
    }

    if(typeof reader != "undefined"){
        for(var i in header){
            this[i] = header[i];
        }
    }

    if(!((typeof this.size == "function")
     &&  (typeof this.read == "function")
     &&  (typeof this.getByte == "function")
     &&  (typeof this.getWord == "function")
     &&  (typeof this.getDWord == "function")
     &&  (typeof this.seek == "function")
     &&  (typeof this.tell == "function")
     &&  (typeof this.slice == "function")))
        throw "Did not create a working Reader object. Types :" + typeof this.size + typeof this.read + typeof this.getByte + typeof this.getWord + typeof this.getDWord + typeof this.seek + typeof this.tell + typeof this.slice;

}
