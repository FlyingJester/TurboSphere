#ifndef LOADPLUGINS_HEAD
#define LOADPLUGINS_HEAD
typedef const char * pluginName;

void LoadAllPlugins(v8::Isolate *isolate);
void CloseAllPlugins();

void grabFuncsFromLibrary(fhandle handle);
#endif
