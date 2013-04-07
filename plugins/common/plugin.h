#ifndef COMMON_PLUGIN_HEAD
#define COMMON_PLUGIN_HEAD

#include "../../configmanager/opengame.h"

#include "../../v8.h"

#if defined _WIN32
#include "../../SDL/SDL.h"
#include "../../SDL/SDL_ttf.h"
#include "../../SDL/SDL_image.h"
#include "../../SDL/SDL_audio.h"
#include "../../SDL/SDL_mixer.h"
#include "../../SDL/SDL_thread.h"

#else

#include "/usr/include/SDL/SDL.h"
#include "/usr/include/SDL/SDL_ttf.h"
#include "/usr/include/SDL/SDL_image.h"
#include "/usr/include/SDL/SDL_audio.h"
#include "/usr/include/SDL/SDL_mixer.h"
#include "/usr/include/SDL/SDL_thread.h"
#endif

typedef const char* cstring;
typedef v8::Handle<v8::Value> v8Function;
typedef void** functionArray;
typedef v8Function* v8FunctionArray;
typedef const char* functionName;
typedef const char* variableName;
typedef const char* TS_name;
typedef TS_name*	nameArray;
typedef const char* initFunction;

#define CATEGORY_REQUIRES	(TS_name)"REQ"
#define CATEGORY_IS			(TS_name)"IS"
#define CATEGORY_SDL_12		(TS_name)"SDL 1.2"
#define CATEGORY_GRAPHIC	(TS_name)"graphic"
#define CATEGORY_FONT		(TS_name)"font"

typedef v8::Handle<v8::Value> v8External;
typedef v8::Persistent<v8::FunctionTemplate> v8Constructor;
typedef v8::Persistent<v8::ObjectTemplate> v8InstanceTemplate;
typedef v8::Persistent<v8::ObjectTemplate> v8PrototypeTemplate;

#define V8ARGS const v8::Arguments& args
#define V8FUNCPOINTER(name)\
	(void *)((v8Function (*)(V8ARGS))(name))

#ifndef PLUGINNAME

#define THROWERROR(name)\
    return v8::ThrowException(v8::String::New(name));

#define THROWERROR_RANGE(name)\
	return v8::ThrowException(v8::Exception::RangeError(v8::String::New(name)));

#define THROWERROR_REFERENCE(name)\
	return v8::ThrowException(v8::Exception::ReferenceError(v8::String::New(name)));

#define THROWERROR_SYNTAX(name)\
	return v8::ThrowException(v8::Exception::SyntaxError(v8::String::New(name)));

#define THROWERROR_TYPE(name)\
	return v8::ThrowException(v8::Exception::TypeError(v8::String::New(name)));

#define THROWERROR_MSG(name)\
	return v8::ThrowException(v8::Exception::Error(v8::String::New(name)));

#define CHECK_ARG_INT(index, name) \
    if (!args[index]->IsNumber()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New(name)));\
    }

#define CHECK_ARG_STR(index, name) \
    if (!args[index]->IsString()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New(name)));\
    }

#define CHECK_ARG_OBJ(index, name) \
    if (!args[index]->IsObject()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New(name)));\
    } \

#define CHECK_ARG_BOOL(index, name) \
    if (!args[index]->IsBoolean()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New(name)));\
    } \

#define CHECK_ARG_ARRAY(index, name) \
    if (!args[index]->IsArray()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New(name)));\
    } \

#else

#include <string>

#define THROWERROR(name)\
    return v8::ThrowException(v8::String::New((std::string(PLUGINNAME " ")+std::string(__func__)+std::string(name)).c_str()));

#define THROWERROR_RANGE(name)\
	return v8::ThrowException(v8::Exception::RangeError(v8::String::New((std::string(PLUGINNAME " ")+std::string(__func__)+std::string(name)).c_str())));

#define THROWERROR_REFERENCE(name)\
	return v8::ThrowException(v8::Exception::ReferenceError(v8::String::New((std::string(PLUGINNAME " ")+std::string(__func__)+std::string(name)).c_str())));

#define THROWERROR_SYNTAX(name)\
	return v8::ThrowException(v8::Exception::SyntaxError(v8::String::New((std::string(PLUGINNAME " ")+std::string(__func__)+std::string(name)).c_str())));

#define THROWERROR_TYPE(name)\
	return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string(PLUGINNAME " ")+std::string(__func__)+std::string(name)).c_str())));

#define THROWERROR_MSG(name)\
	return v8::ThrowException(v8::Exception::Error(v8::String::New((std::string(PLUGINNAME " ")+std::string(__func__)+std::string(name)).c_str())));

#define CHECK_ARG_INT(index) \
    if (!args[index]->IsNumber()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string(PLUGINNAME " ")+std::string(__func__)+std::string("Error: Argument " #index " is not an integer.")).c_str())));\
    }

#define CHECK_ARG_STR(index) \
    if (!args[index]->IsString()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string(PLUGINNAME " ")+std::string(__func__)+std::string("Error: Argument " #index " is not a string.")).c_str())));\
    }

#define CHECK_ARG_OBJ(index) \
    if (!args[index]->IsObject()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string(PLUGINNAME " ")+std::string(__func__)+std::string("Error: Argument " #index " is not an object.")).c_str())));\
    } \

#define CHECK_ARG_BOOL(index) \
    if (!args[index]->IsBoolean()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string(PLUGINNAME " ")+std::string(__func__)+std::string("Error: Argument " #index " is not a boolean.")).c_str())));\
    } \

#define CHECK_ARG_ARRAY(index) \
    if (!args[index]->IsArray()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string(PLUGINNAME " ")+std::string(__func__)+std::string("Error: Argument " #index " is not an array.")).c_str())));\
    } \


#endif

    #define QuitGracefully \
    printf("Exiting TurboSphere...\n");\
    exitContext();\
    QuitAll();

    void QuitAll();
#define FINALIZER(NAME) void TS_##NAME##Finalizer(v8::Persistent<v8::Value> object, void* parameter)
//Note!
//This function requires you name your objects as:
//obj_nameInsttempl
//obj_nametempl
//TS_obj_nameFinalizer
//

#define DECLARE_OBJECT_TEMPLATES(JSOBJ) \
v8::Persistent<v8::FunctionTemplate> JSOBJ##templ;\
v8::Persistent<v8::ObjectTemplate> JSOBJ##Insttempl;\
v8::Persistent<v8::ObjectTemplate> JSOBJ##proto


#define INIT_OBJECT_TEMPLATES(JSOBJ) \
    JSOBJ##templ    = *v8::FunctionTemplate::New();\
	JSOBJ##Insttempl= *JSOBJ##templ->InstanceTemplate();\
    JSOBJ##proto    = *JSOBJ##templ->PrototypeTemplate()

#define CLOSE_OBJECT_TEMPLATES(JSOBJ) \
    JSOBJ##templ.Clear();\
	JSOBJ##Insttempl.Clear();\
    JSOBJ##proto.Clear();\
    JSOBJ##templ.Dispose();\
	JSOBJ##Insttempl.Dispose();\
    JSOBJ##proto.Dispose()



#define ADD_TO_PROTO(JSOBJ, PROPNAME, CALL){\
    JSOBJ##proto->Set(PROPNAME, v8::FunctionTemplate::New(CALL));\
}

#define BEGIN_OBJECT_WRAP_CODE \
    v8::HandleScope temporary_scope;\
    v8External external;

#define END_OBJECT_WRAP_CODE(JSOBJ, TO_WRAP){\
    external = v8::External::New(TO_WRAP);\
    JSOBJ##Insttempl->SetInternalFieldCount(1);\
    v8::Local<v8::Function> JSOBJ##ctor = JSOBJ##templ->GetFunction();\
	v8::Local<v8::Object> JSOBJ##obj = JSOBJ##ctor->NewInstance();\
    v8::Persistent<v8::Object> P##JSOBJ##obj = v8::Persistent<v8::Object>::New(JSOBJ##obj);\
    P##JSOBJ##obj.MakeWeak(TO_WRAP, TS_##JSOBJ##Finalizer);\
	P##JSOBJ##obj->SetInternalField(0, external);\
    return temporary_scope.Close(P##JSOBJ##obj);\
}

/*
#define INTERNAL_CHECK_int(ARGNAME, WORDS)\
if(!ARGNAME->isInteger()){THROWERROR(WORDS)}

#define INTERNAL_CHECK_double(ARGNAME, WORDS)\
if(!ARGNAME->isNumber()){THROWERROR(WORDS)}

#define INTERNAL_CHECK_float(ARGNAME, WORDS)\
if(!ARGNAME->isNumber()){THROWERROR(WORDS)}

#define INTERNAL_CHECK_cstring(ARGNAME, WORDS)\
if(!ARGNAME->isString()){THROWERROR(WORDS)}


#define V8ARGCHECK(TYPE, ARGNAME, WORDS)\
INTERNAL_CHECK_##TYPE

#define FUNCTION_FORWARD(NUMARGS, MODULE, FUNCTIONNAME, argtypes...)\
if(args.Length()<NUMARGS){THROWERROR("[##MODULE##] ##FUNCTIONNAME## Error: Called with less than ##NUMARGD## arguments.");}
*/


#endif
