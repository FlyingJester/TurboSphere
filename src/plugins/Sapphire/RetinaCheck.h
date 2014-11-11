#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* aWindow is an Objective C NSWindow.
 Returns the nearest integer scaling factor-1 if it is Retina,
 0 otherwise.
*/
int TS_IsRetina(void *aWindow);

/* Returns the scaling factor for aWindow. */

float TS_GetScalingFactor(void *aWindow);

#ifdef __cplusplus
}
#endif
