#ifndef TS_ENGINE_TYPED_ARRAY_HEAD
#define TS_ENGINE_TYPED_ARRAY_HEAD
#include "v8.h"

class TS_ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
    /////
    // Oh, all of a sudden this should all be zeroed out?
    // So Node.js just gets whatever they ask for!?
	virtual void *Allocate(size_t length);
    virtual void *AllocateUninitialized(size_t length);
	virtual void Free(void *data);
	virtual void Free(void *data, size_t len);
};

#endif
