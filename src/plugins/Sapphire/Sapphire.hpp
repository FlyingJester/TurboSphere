#pragma once

#define PLUGINNAME "Sapphire"
#define BRACKNAME "[" PLUGINNAME "]"

#include <pluginsdk/plugin.h>

#define Frmask 0x000000ff
#define Fgmask 0x0000ff00
#define Fbmask 0x00ff0000
#define Famask 0xff000000

#define CHANNEL_MASKS Frmask, Fgmask, Fbmask, Famask
