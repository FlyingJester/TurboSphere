#ifndef SCRIPTFS_BYTEARRAY_HEAD
#define SCRIPTFS_BYTEARRAY_HEAD
#include <cstdlib>
#include "scriptfs.h"
#include "../common/plugin.h"

EXTERN_OBJECT_TEMPLATES(ByteArray);
void TS_ByteArrayFinalizer(V8FINALIZERARGS);

void ByteArrayInit(void);

struct MINMEMALIGN TS_ByteArray {
    size_t length;
    char *
#ifndef _MSC_VER
		MINMEMALIGN 
#endif
		data;
};

const char *ByteArrayToString(TS_ByteArray);
TS_ByteArray StringToByteArray(char *);
TS_ByteArray StringToByteArray(const char *);
TS_ByteArray AppendByteArrays(TS_ByteArray in, TS_ByteArray toadd);

TS_ByteArray CreateByteArray(size_t size);
void FreeByteArray(TS_ByteArray array);
const char *TS_HashByteArray(TS_ByteArray array);

v8Function TS_CreateByteArray(V8ARGS);
v8Function ByteArrayConcat(V8ARGS);
v8Function ByteArraySlice(V8ARGS);
v8Function CreateByteArrayFromString(V8ARGS);
v8Function CreateStringFromByteArray(V8ARGS);

#endif
