#ifndef COMMON_PLUGIN_HEAD
#define COMMON_PLUGIN_HEAD

#include "../../configmanager/opengame.h"
#define V8_USE_UNSAFE_HANDLES
#include "../../v8.h"
#undef V8_USE_UNSAFE_HANDLES

#define MIN_V8_ALIGN 2

#ifdef _MSC_VER

    #define __func__ __FUNCTION__

    #define MEMALIGN(X) _declspec(align(X))
#else

    #define MEMALIGN(X) __attribute__((aligned(X)))
#endif


#ifdef _WIN32

    //TODO: Fix these includes for SDL2.
//#error Fix these includes for SDL2!
    #include "../../SDL2/SDL.h"
    #include "../../SDL2/SDL_ttf.h"
    #include "../../SDL2/SDL_image.h"
    #include "../../SDL2/SDL_thread.h"

#else
    #include "../../SDL2/SDL.h"
    #include "../../SDL2/SDL_ttf.h"
    #include "../../SDL2/SDL_image.h"
    #include "../../SDL2/SDL_thread.h"
#endif
#ifndef MINMEMALIGN
#define MINMEMALIGN MEMALIGN(MIN_V8_ALIGN)
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

struct TS_PluginInfo {
	const char * name;
	const char * author;
	const char * description;
	const char * date;
	const char * version;
};

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

#define V8GETTERARGS v8::Local<v8::String> property, const v8::AccessorInfo &info
#define V8SETTERARGS v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info
#define V8FINALIZERARGS v8::Isolate* isolate, v8::Persistent<v8::Object>* object, void* parameter

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
    return v8::ThrowException(v8::String::New((std::string("[" PLUGINNAME "] ")+std::string(__func__)+std::string(name)).c_str()));

#define THROWERROR_RANGE(name)\
	return v8::ThrowException(v8::Exception::RangeError(v8::String::New((std::string("[" PLUGINNAME "] ")+std::string(__func__)+std::string(name)).c_str())));

#define THROWERROR_REFERENCE(name)\
	return v8::ThrowException(v8::Exception::ReferenceError(v8::String::New((std::string("[" PLUGINNAME "] ")+std::string(__func__)+std::string(name)).c_str())));

#define THROWERROR_SYNTAX(name)\
	return v8::ThrowException(v8::Exception::SyntaxError(v8::String::New((std::string("[" PLUGINNAME "] ")+std::string(__func__)+std::string(name)).c_str())));

#define THROWERROR_TYPE(name)\
	return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string("[" PLUGINNAME "] ")+std::string(__func__)+std::string(name)).c_str())));

#define THROWERROR_MSG(name)\
	return v8::ThrowException(v8::Exception::Error(v8::String::New((std::string("[" PLUGINNAME "] ")+std::string(__func__)+std::string(name)).c_str())));

#define CHECK_ARG_INT(index) \
    if (!args[index]->IsNumber()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string("[" PLUGINNAME "] ")+std::string(__func__)+std::string("Error: Argument " #index " is not an integer.")).c_str())));\
    }

#define CHECK_ARG_STR(index) \
    if (!args[index]->IsString()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string("[" PLUGINNAME "] ")+std::string(__func__)+std::string("Error: Argument " #index " is not a string.")).c_str())));\
    }

#define CHECK_ARG_OBJ(index) \
    if (!args[index]->IsObject()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string("[" PLUGINNAME "] ")+std::string(__func__)+std::string("Error: Argument " #index " is not an object.")).c_str())));\
    } \

#define CHECK_ARG_BOOL(index) \
    if (!args[index]->IsBoolean()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string("[" PLUGINNAME "] ")+std::string(__func__)+std::string("Error: Argument " #index " is not a boolean.")).c_str())));\
    } \

#define CHECK_ARG_ARRAY(index) \
    if (!args[index]->IsArray()) { \
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New((std::string("[" PLUGINNAME "] ")+std::string(__func__)+std::string("Error: Argument " #index " is not an array.")).c_str())));\
    } \


#endif

    #define QuitGracefully \
    printf("Exiting TurboSphere...\n");\
    exitContext();\
    QuitAll();

    void QuitAll();

#define FINALIZER(NAME) void TS_##NAME##Finalizer(v8::Handle<v8::Value> object, void* parameter)

//Note!
//This function requires you name your objects as:
//obj_nameInsttempl
//obj_nametempl
//TS_obj_nameFinalizer
//

#define DECLARE_OBJECT_TEMPLATES(JSOBJ) \
v8::Handle<v8::FunctionTemplate> JSOBJ##templ;\
v8::Handle<v8::ObjectTemplate> JSOBJ##Insttempl;\
v8::Handle<v8::ObjectTemplate> JSOBJ##proto

#define SET_CLASS_NAME(JSOBJ, NAME) \
JSOBJ##templ->SetClassName(v8::String::New(NAME))

#define EXTERN_OBJECT_TEMPLATES(JSOBJ) \
extern v8::Handle<v8::FunctionTemplate> JSOBJ##templ;\
extern v8::Handle<v8::ObjectTemplate> JSOBJ##Insttempl;\
extern v8::Handle<v8::ObjectTemplate> JSOBJ##proto


#define INIT_OBJECT_TEMPLATES(JSOBJ) \
    JSOBJ##templ    = v8::FunctionTemplate::New();\
	JSOBJ##Insttempl= JSOBJ##templ->InstanceTemplate();\
    JSOBJ##proto    = JSOBJ##templ->PrototypeTemplate()

#define CLOSE_OBJECT_TEMPLATES(JSOBJ) \
    JSOBJ##templ.Clear();\
	JSOBJ##Insttempl.Clear();\
    JSOBJ##proto.Clear();

#define SET_CLASS_ACCESSOR(JSOBJ, NAME, GETTER, SETTER)\
    JSOBJ##Insttempl->SetAccessor(v8::String::New(NAME), GETTER, SETTER)

#define ADD_TO_PROTO(JSOBJ, PROPNAME, CALL){\
    JSOBJ##proto->Set(PROPNAME, v8::FunctionTemplate::New(CALL));\
}

#define BEGIN_OBJECT_WRAP_CODE \
    v8::HandleScope temporary_scope;\
    //v8External external;

#define END_OBJECT_WRAP_CODE(JSOBJ, TO_WRAP){\
    JSOBJ##Insttempl->SetInternalFieldCount(2);\
    v8::Handle<v8::Function> JSOBJ##ctor = JSOBJ##templ->GetFunction();\
	v8::Handle<v8::Object> JSOBJ##obj = JSOBJ##ctor->NewInstance();\
    v8::Persistent<v8::Object> P##JSOBJ##obj = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), JSOBJ##obj);\
    P##JSOBJ##obj.MakeWeak((void *)TO_WRAP, TS_##JSOBJ##Finalizer);\
    P##JSOBJ##obj->SetAlignedPointerInInternalField(0, (void *)TO_WRAP);\
    P##JSOBJ##obj->SetInternalField(1, v8::Integer::NewFromUnsigned(0ULL));\
    P##JSOBJ##obj->GetConstructorName()=v8::String::New( #JSOBJ );\
    return temporary_scope.Close(P##JSOBJ##obj);\
}


#define END_OBJECT_WRAP_CODE_WITH_ID(JSOBJ, TO_WRAP, ID){\
    JSOBJ##Insttempl->SetInternalFieldCount(2);\
    v8::Handle<v8::Function> JSOBJ##ctor = JSOBJ##templ->GetFunction();\
	v8::Handle<v8::Object> JSOBJ##obj = JSOBJ##ctor->NewInstance();\
    v8::Persistent<v8::Object> P##JSOBJ##obj = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), JSOBJ##obj);\
    P##JSOBJ##obj.MakeWeak((void *)TO_WRAP, TS_##JSOBJ##Finalizer);\
    P##JSOBJ##obj->SetAlignedPointerInInternalField(0, (void *)TO_WRAP);\
    P##JSOBJ##obj->SetInternalField(1, v8::Integer::NewFromUnsigned(ID)); \
    P##JSOBJ##obj->GetConstructorName()=v8::String::New( #JSOBJ );\
    return temporary_scope.Close(P##JSOBJ##obj);\
}


#define RETURN_OBJECT_WRAP_CODE(JSOBJ, TO_WRAP)\
    JSOBJ##Insttempl->SetInternalFieldCount(1);\
    v8::Handle<v8::Function> JSOBJ##ctor = JSOBJ##templ->GetFunction();\
	v8::Handle<v8::Object> JSOBJ##obj = JSOBJ##ctor->NewInstance();\
    v8::Persistent<v8::Object> P##JSOBJ##obj = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), JSOBJ##obj);\
    P##JSOBJ##obj.MakeWeak((void *)TO_WRAP, TS_##JSOBJ##Finalizer);\
    P##JSOBJ##obj->SetAlignedPointerInInternalField(0, (void *)TO_WRAP);\
    P##JSOBJ##obj->GetConstructorName()=v8::String::New( #JSOBJ );\

#define GET_OBJECT_WRAP_CODE(JSOBJ) temporary_scope.Close(P##JSOBJ##obj)


#define GET_SELF(TYPE) static_cast<TYPE>(args.Holder()->GetAlignedPointerFromInternalField(0))

#define GET_ACCESSOR_SELF(TYPE) static_cast<TYPE>(info.Holder()->GetAlignedPointerFromInternalField(0))

    //v8::Local<v8::Object> self = info.Holder();
    //v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    //void* ptr = wrap->Value();

    //v8::Local<v8::Object> self = args.Holder();
    //SDL_Surface *sstatic_cast<TS_Image*>(v8::Local<v8::External>::Cast((args.Holder())->GetAlignedPointerInInternalField(0)))->CreateSurface();

//#define GET_SELF(TYPE) static_cast<TYPE>(v8::Local<v8::External>::Cast((args.Holder())->GetInternalField(0))->Value())

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
