#pragma once

#include "SapphirePlugin.hpp"

extern "C" {

CROSSPLUGIN_EXPORT void TS_AddMethodToSurface(const char *aName, Turbo::JSCallback aFunc);
// on fail, 0 on success
CROSSPLUGIN_EXPORT int TS_SaveImage(const char *aPath, uint32_t *aPixels, unsigned aWidth, unsigned aHeight, bool absolute);
CROSSPLUGIN_EXPORT void TS_AddMethodOnVerticalBlank(Turbo::JSCallback aFunc);

}
