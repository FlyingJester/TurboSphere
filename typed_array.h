#ifndef TS_ENGINE_TYPED_ARRAY_HEAD
#define TS_ENGINE_TYPED_ARRAY_HEAD
#include "v8.h"

class TS_ArrayBufferAllocator : public v8::ArrayBuffer::Allocator{
public:
	virtual void *Allocate(size_t length);
	virtual void Free(void *data);
};

#endif