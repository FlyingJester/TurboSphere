#include "bytearray.h"
#include "rawfile.h"
#include <cstring>
#include "../common/plugin.h"
DECLARE_OBJECT_TEMPLATES(ByteArray);

void TS_ByteArrayFinalizer(V8FINALIZERARGS){

    TS_ByteArray* array = (TS_ByteArray*)parameter;
    free(array->data);
    free(array);
    object->Dispose();
}

void ByteArrayInit(void){
    INIT_OBJECT_TEMPLATES(ByteArray);
    SET_CLASS_NAME(ByteArray, "ByteArray");
    ADD_TO_PROTO(ByteArray, "concat", ByteArrayConcat);
    ADD_TO_PROTO(ByteArray, "slice", ByteArraySlice);
}

const char *TS_HashByteArray(TS_ByteArray array){
    md5_t md5_p;
    md5_init(&md5_p);

    unsigned long long digest[2]; //This is where this typedef gets ugly.

    md5_buffer(array.data, array.length, digest);


    return StringFromMD5(digest);

}

const char *ByteArrayToString(TS_ByteArray array){
    char *str = (char *)malloc(array.length+1);
    str[array.length] = 0;
    memcpy(str, array.data, array.length);
    //NULL chars are bad. I don't actually know how to best deal with this.
    //HACK: But a little one. This is a band aid. Ideally just splice out the NULLs.
    for(size_t i = 0; i<array.length; i++)
        if(str[i]==0)
            str[i] = 32;

    return str;
}

TS_ByteArray StringToByteArray(char * str){
    TS_ByteArray array = CreateByteArray(strlen(str));
    memcpy(array.data, str, array.length);
    return array;
}

TS_ByteArray StringToByteArray(const char *str){
    TS_ByteArray array = CreateByteArray(strlen(str));
    memcpy(array.data, str, array.length);
    return array;
}

TS_ByteArray AppendByteArrays(TS_ByteArray in, TS_ByteArray toadd){
    in.data = (char*)realloc(in.data, in.length+toadd.length);
    memcpy(in.data+in.length, toadd.data, toadd.length);
    in.length+=toadd.length;
    return in;
}

TS_ByteArray CreateByteArray(size_t size){
    TS_ByteArray array;
    array.length = size;
    array.data = (char *)malloc(size);
    return array;
}
void FreeByteArray(TS_ByteArray array){
    free(array.data);
}
