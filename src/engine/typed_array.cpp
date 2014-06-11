#include "typed_array.h"
#include <cstdlib>

void * TS_ArrayBufferAllocator::Allocate(size_t length){
	return malloc(length);
}
void TS_ArrayBufferAllocator::Free(void *data){
	free(data);
}

void *TS_ArrayBufferAllocator::AllocateUninitialized(size_t length){
    return calloc(length, 1);
}

void TS_ArrayBufferAllocator::Free(void *data, size_t unused){
    free(data);
}
