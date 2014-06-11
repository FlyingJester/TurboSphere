#ifndef LOADPLUGINS_HEAD
#define LOADPLUGINS_HEAD
typedef const char * pluginName;

int loadAllPlugins();
int CloseAllPlugins();

void grabFuncsFromLibrary(fhandle handle);
#endif
