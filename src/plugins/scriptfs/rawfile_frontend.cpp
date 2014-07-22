#include "rawfile.h"
DECLARE_OBJECT_TEMPLATES(RawFile);


void TS_RawFileFinalizer(V8FINALIZERARGS){

    TS_RawFile* rf = (TS_RawFile*)parameter;
    delete rf;
    object->Dispose();
}

void RawfileInit(void){
    INIT_OBJECT_TEMPLATES(RawFile);
    SET_CLASS_NAME(RawFile, "RawFile");
    ADD_TO_PROTO(RawFile, "read",            RawfileRead);
    ADD_TO_PROTO(RawFile, "write",           RawfileWrite);
    ADD_TO_PROTO(RawFile, "close",           CloseRawfile);
    ADD_TO_PROTO(RawFile, "getPosition",     RawfileGetPosition);
    ADD_TO_PROTO(RawFile, "setPosition",     RawfileSetPosition);
    ADD_TO_PROTO(RawFile, "getSize",         RawfileSetPosition);

}

v8Function RawfileGetPosition(V8ARGS){
    return v8::Uint32::New(GET_SELF(TS_RawFile*)->getPosition());
}

v8Function RawfileSetPosition(V8ARGS){
    if(args.Length()<1)
        THROWERROR(" Error: Called with no arguments.");
    CHECK_ARG_INT(0);
	size_t num = args[0]->Uint32Value();
    TS_RawFile *file = GET_SELF(TS_RawFile*);

    if(file->getPosition()+num>file->getSize()){
        THROWERROR_RANGE(" Error: Seeking past end of file.");
    }
    file->setPosition(num);
    return v8::Undefined();
}

v8Function RawfileGetLength(V8ARGS){
    return v8::Uint32::New(GET_SELF(TS_RawFile*)->getSize());
}

v8Function RawfileRead(V8ARGS){
    if(args.Length()<1)
        THROWERROR(" Error: Called with no arguments.");
    CHECK_ARG_INT(0);
	size_t num = args[0]->Uint32Value();
    TS_RawFile *file = GET_SELF(TS_RawFile*);

    if(file->getPosition()+num>file->getSize()){
        THROWERROR_RANGE(" Error: Reading past end of file.");
    }
    BEGIN_OBJECT_WRAP_CODE;
    TS_ByteArray *array = (TS_ByteArray *)calloc(sizeof(TS_ByteArray), 1);
    array->length = num;

    //This is not ideal.
    array->data = file->read(num).data;

    END_OBJECT_WRAP_CODE(ByteArray, array);

}

v8Function RawfileWrite(V8ARGS){
    if(args.Length()<1)
        THROWERROR(" Error: Called with no arguments.");
    v8::Local<v8::Object> bytearray = v8::Local<v8::Object>::Cast(args[0]);
    TS_ByteArray *array = (TS_ByteArray*)bytearray->GetAlignedPointerFromInternalField(0);
    size_t n = GET_SELF(TS_RawFile*)->write(*array);
    return v8::Uint32::New(n);
}

v8Function CloseRawfile(V8ARGS){
    return v8::Undefined();
}

v8Function HashFromFile(V8ARGS){
    if(args.Length()<1){
        THROWERROR(" Error: Called with no arguments.");
    }

    CHECK_ARG_STR(0);

    BEGIN_OBJECT_WRAP_CODE;

    TS_RawFile *rf = new TS_RawFile();

    v8::String::AsciiValue str(args[0]);
    int error = rf->Open(0, false);
    if(error!=SCRIPTFS_ERROR_NONE){
        delete rf;
        char errorDesc[64];
        sprintf(errorDesc, " Error: %s", ScriptFSDescribeError(error));
        THROWERROR(errorDesc);
    }
    octawordP m = rf->md5();
    const char *n = StringFromMD5(m);
    auto ret = v8::String::New(n);
    free((void *)n);
    free(m);
    return ret;
}

v8Function OpenRawfile(V8ARGS){
    if(args.Length()<2){
        THROWERROR(" Error: Called with no arguments.");
    }

    CHECK_ARG_STR(0);
    CHECK_ARG_BOOL(1);

    BEGIN_OBJECT_WRAP_CODE;

    TS_RawFile *rf = new TS_RawFile();

    v8::String::AsciiValue str(args[0]);
    int error = rf->Open(0, args[1]->BooleanValue());
    if(error!=SCRIPTFS_ERROR_NONE){
        delete rf;
        char errorDesc[64];
        sprintf(errorDesc, " Error: %s", ScriptFSDescribeError(error));
        THROWERROR(errorDesc);
    }
    END_OBJECT_WRAP_CODE(RawFile, rf);

}
v8Function RawfileGetPosition(V8ARGS);
v8Function RawfileSetPosition(V8ARGS);
v8Function RawfileWrite(V8ARGS);
v8Function CloseRawfile(V8ARGS);
