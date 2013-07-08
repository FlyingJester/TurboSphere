#ifndef INPUT_JOYSTICK_H
#define INPUT_JOYSTICK_H
#include "../common/plugin.h"

#define JOY_PNP 0;

v8Function GetNumJoysticks(V8ARGS);
v8Function GetJoystickName(V8ARGS);
v8Function GetNumJoystickButtons(V8ARGS);
v8Function GetNumJoystickAxes(V8ARGS);
v8Function IsJoystickButtonPressed(V8ARGS);
v8Function GetJoystickAxis(V8ARGS);

void InitJoystick(void);
void CloseJoystick(void);

#endif
