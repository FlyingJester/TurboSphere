#include "RetinaCheck.h"
#include <AppKit/NSWindow.h>

int TS_IsRetina(void *aWindow){
    float lFactor = TS_GetScalingFactor(aWindow);
    return (int)(lFactor-0.5f);
}

float TS_GetScalingFactor(void *aWindow){
    NSWindow *lWindow = aWindow;
    return [lWindow backingScaleFactor];
}
