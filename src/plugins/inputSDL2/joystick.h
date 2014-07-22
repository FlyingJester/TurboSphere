#ifndef INPUT_JOYSTICK_H
#define INPUT_JOYSTICK_H
#include <pluginsdk/plugin.h>

#define JOY_PNP 0;

Turbo::JSFunction GetNumJoysticks(Turbo::JSArguments);
Turbo::JSFunction GetJoystickName(Turbo::JSArguments);
Turbo::JSFunction GetNumJoystickButtons(Turbo::JSArguments);
Turbo::JSFunction GetNumJoystickAxes(Turbo::JSArguments);
Turbo::JSFunction IsJoystickButtonPressed(Turbo::JSArguments);
Turbo::JSFunction GetJoystickAxis(Turbo::JSArguments);

void InitJoystick(void);
void CloseJoystick(void);

#endif
