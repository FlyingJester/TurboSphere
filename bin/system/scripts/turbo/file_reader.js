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
    // Implement this.getByte();
    // Implement this.getWord();
    // Implement this.getDWord();
    // Implement this.seek(to, whence);
    // Implement this.tell();

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
    else
        throw "No known way to determine length of input.";

    // Implement this.getByte();
    this.getByte = function(){
        return this.guts[this.at++];
    }

    // Implement this.getWord();
    this.getWord = function(){
        return Turbo.dByteCat(this.guts[this.at++], this.guts[this.at++]);
    }

    // Implement this.getDWord();
    this.getWord = function(){
        return Turbo.qByteCat(this.guts[this.at++], this.guts[this.at++], this.guts[this.at++], this.guts[this.at++]);
    }

    // Implement this.seek(to, whence);
    this.seek = function(to, whence){
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

    // Implement this.tell();
    this.tell = function(){return this.at;};

    // Aliases
    this.getChar = this.getByte;
    this.getShort = this.getWord;
    this.getInt   = this.getDWord;
    this.getLongLong = this.getQWord;

}

// Constructs a stream from a RawFile.
// Takes ownership of the RawFile it is passed.
Turbo.FileReader = function(raw, offset){

    if(typeof offset == "undefined")
        offset = 0;

    this.guts = input;
    this.guts.seek(offset);
    this.at = offset;
    // Implement this.size();
    this.size = function(){return this.guts.size;};

    // Chunks MUST be larger than 258 bytes.
    this.chunk_sizes = 1024; // This may grow at some point in flight.
    this.chunk = {size:0, offset:offset, data:null};

    this.moveChunkTo = function(to){
        if(!((this.chunk.offset <= this.at) &&
           (this.chunk.offset+this.chunk.size < to))){

            this.chunk.size = this.chunk_sizes;
            this.chunk.offset = (to>>8)<<8;
            this.file.seek(this.chunk.offset);
            this.chunk.data = this.file.read(this.chunk.size);
            this.at = this.chunk.offset+this.chunk.size;
        }
    }

    // Implement this.getByte();
    this.getByte = function(){
        this.moveChunkTo(this.at);
        return this.chunk.data[this.at-this.chunk.offset];
    }
    // Implement this.getWord();
    this.getWord = function(){
        this.moveChunkTo(this.at);
        return Turbo.dByteCat(this.chunk.data[this.at-this.chunk.offset+0],
                              this.chunk.data[this.at-this.chunk.offset+1]);
    }
    // Implement this.getDWord();
    this.getDWord = function(){
        this.moveChunkTo(this.at);
        return Turbo.qByteCat(this.chunk.data[this.at-this.chunk.offset+0],
                              this.chunk.data[this.at-this.chunk.offset+1],
                              this.chunk.data[this.at-this.chunk.offset+2],
                              this.chunk.data[this.at-this.chunk.offset+3]);
    }

    // Implement this.seek(to, whence);
    this.seek = function(to, whence){
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
        this.guts.seek(this.at);
        return this.tell();
    }


    // Implement this.tell();
    this.tell = function(){return this.at;};

    this.getChar = this.getByte;
    this.getShort = this.getWord;
    this.getInt   = this.getDWord;
    this.getLongLong = this.getQWord;

}

// Constructs a stream from a RawFile, an Array, or a ByteArray.
Turbo.Stream = function(input, offset){

    if(typeof offset == "undefined")
        offset = 0;

    if((this.guts instanceof Uint8Array) || (Array.isArray(this.guts))){
        this.__proto__ = new Turbo.ArrayReader(input, offset);
    }
    else{
        this.__proto__ = new Turbo.FileReader( input, offset);
    }



}
