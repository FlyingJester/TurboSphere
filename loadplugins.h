#ifndef LOADPLUGINS_HEAD
#define LOADPLUGINS_HEAD
typedef const char * pluginName;

int loadAllPlugins();

void grabFuncsFromLibrary(fhandle handle);

#endif
