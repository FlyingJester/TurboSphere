#include "typed_array.hpp"
#include <cstdlib>
#include <v8-platform.h>
#include <TSPR/concurrent_queue.h>

namespace Turbo {

void * ArrayBufferAllocator::Allocate(size_t length){
    return calloc(length, 1);
}
void ArrayBufferAllocator::Free(void *data){
	free(data);
}

void *ArrayBufferAllocator::AllocateUninitialized(size_t length){
    return malloc(length);
}

void ArrayBufferAllocator::Free(void *data, size_t unused){
    free(data);
}

}
