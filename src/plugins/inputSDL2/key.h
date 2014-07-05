#ifndef KEY_HEAD
#define KEY_HEAD
#include <pluginsdk/plugin.h>

Turbo::JSFunction IsKeyPressed(Turbo::JSArguments args);
Turbo::JSFunction IsAnyKeyPressed(Turbo::JSArguments args);
Turbo::JSFunction GetKey(Turbo::JSArguments args);
Turbo::JSFunction AreKeysLeft(Turbo::JSArguments args);

#endif
