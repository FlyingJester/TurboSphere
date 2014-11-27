#pragma once
#ifdef __cplusplus
#include <v8.h>

// Defined in the engine proper.
void GarbageCollect(const v8::FunctionCallbackInfo<v8::Value> &args);

namespace Turbo {
// Gets Time down to Microsecond accuracy.
void GetSeconds(const v8::FunctionCallbackInfo<v8::Value> &args);
void GetTime(const v8::FunctionCallbackInfo<v8::Value> &args);
void Delay(const v8::FunctionCallbackInfo<v8::Value> &args);

}

extern "C"
#endif // _cplusplus
double TS_GetSeconds();
