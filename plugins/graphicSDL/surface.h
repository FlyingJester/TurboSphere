#ifndef GRAPHIC_SURFACE_HEAD
#define GRAPHIC_SURFACE_HEAD

void TS_SurfaceFinalizer(v8::Persistent<v8::Value> object, void* parameter);
v8::Handle<v8::Value> TS_surfaceZoomBlit(const v8::Arguments& args);
v8::Handle<v8::Value> TS_surfaceStretchBlit(const v8::Arguments& args);
v8::Handle<v8::Value> TS_surfaceRectangle(const v8::Arguments& args);
void TS_addToSurfaceProto(const char * name, v8::Handle<v8::Value> templ);

#endif
