#include "bytearray.h"
#include "rawfile.h"

v8Function TS_CreateByteArray(V8ARGS){
    if(args.Length()<1){
        THROWERROR(" Error: Called with no argument.");
    }
    CHECK_ARG_INT(0);
    BEGIN_OBJECT_WRAP_CODE;
    TS_ByteArray *array = (TS_ByteArray *)malloc(sizeof(TS_ByteArray));
	size_t num = args[0]->Uint32Value();
    array->length = num;
    array->data = (char *)calloc(num, 1);
    END_OBJECT_WRAP_CODE(ByteArray, array);
}

v8Function ByteArrayConcat(V8ARGS){
    if(args.Length()<1)
        THROWERROR(" Error: Called with no arguments.");
    v8::Local<v8::Object> bytearray = v8::Local<v8::Object>::Cast(args[0]);
    TS_ByteArray *arraytoadd = (TS_ByteArray*)bytearray->GetAlignedPointerFromInternalField(0);
    TS_ByteArray *arrayin = GET_SELF(TS_ByteArray*);
    *arrayin = AppendByteArrays(*arrayin, *arraytoadd);
    return v8::Undefined();
}

v8Function HashByteArray(V8ARGS){
    if(args.Length()<1)
        THROWERROR(" Error: Called with no arguments.");

    v8::Local<v8::Object> bytearray = v8::Local<v8::Object>::Cast(args[0]);
    TS_ByteArray *array = (TS_ByteArray*)bytearray->GetAlignedPointerFromInternalField(0);

    const char * m = TS_HashByteArray(*array);
    auto ret = v8::String::New(m);
    free((void *)m);
    return ret;
}

v8Function ByteArraySlice(V8ARGS){
    if(args.Length()<1)
        THROWERROR(" Error: Called with no arguments.");
    TS_ByteArray *array = GET_SELF(TS_ByteArray*);
    //bool end = false;
    long long endnum = array->length;
    CHECK_ARG_INT(0);
    if(args.Length()==2){
        CHECK_ARG_INT(1);
        endnum = args[1]->IntegerValue();
        //end = true;
    }
    BEGIN_OBJECT_WRAP_CODE;
    TS_ByteArray *arrayout = (TS_ByteArray *)malloc(sizeof(TS_ByteArray));
	long long num = args[0]->IntegerValue();
	if(num<0){
	    if(-num>=array->length)
            THROWERROR_RANGE(" Error: Argument 0 is negative.");
	}
	else if(num>=array->length)
            THROWERROR_RANGE(" Error: Argument 0 is out of bounds.");

	if(endnum<0){
	    if(-endnum>=array->length)
            THROWERROR_RANGE(" Error: Argument 1 is out of bounds.");
        endnum = array->length+endnum;
	}
	else if(endnum>=array->length)
            THROWERROR_RANGE(" Error: Argument 1 is out of bounds.");

    size_t length = endnum-num;
    arrayout->length = length;
    arrayout->data = (char *)calloc(length, 1);
    memcpy(arrayout->data, (array->data)+num, length);
    END_OBJECT_WRAP_CODE(ByteArray, arrayout);
}

v8Function CreateByteArrayFromString(V8ARGS){
    if(args.Length()<0)
        THROWERROR(" Error: Called with no arguments.");
    CHECK_ARG_STR(0);

    BEGIN_OBJECT_WRAP_CODE;

    v8::String::AsciiValue str(args[0]);

    TS_ByteArray *array = (TS_ByteArray *)malloc(sizeof(TS_ByteArray));
    TS_ByteArray temparray = StringToByteArray(*str);

    array->data = temparray.data;
    array->length = temparray.length;

    END_OBJECT_WRAP_CODE(ByteArray, array);

}

v8Function CreateStringFromByteArray(V8ARGS){
    if(args.Length()<0)
        THROWERROR(" Error: Called with no arguments.");
    v8::Local<v8::Object> bytearray = v8::Local<v8::Object>::Cast(args[0]);
    TS_ByteArray *array = (TS_ByteArray*)bytearray->GetAlignedPointerFromInternalField(0);

    char *str = (char *)calloc(array->length+1, 1);
    memcpy(str, array->data, array->length);

    return v8::String::New(str);
}

/*
CreateByteArrayFromString(string): Create a ByteArray from string.
CreateStringFromByteArray(byte_array): Convert a ByteArray into a string.
*/
