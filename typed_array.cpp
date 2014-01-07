#include "typed_array.h"
#include <cstdlib>

void * TS_ArrayBufferAllocator::Allocate(size_t length){
	return malloc(length);
}
void TS_ArrayBufferAllocator::Free(void *data){
	free(data);
}
