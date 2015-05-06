///////////////////////////////////////////////////////////////////////////////
/*

TurboSphere Plugin SDK

Copyright (c) 2012-2014 Martin McDonough.

All rights reserved.


Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

   3. The name of the author may not be used to endorse or promote products
    derived from this software without specific prior written permission.

Permission is granted to anyone to use this software for any purpose, including
 commercial applications, and to alter it and redistribute it freely, subject
 to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software in a
    product, an acknowledgment in the product documentation would be
    appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software. This notice may not be
    removed or altered from any source distribution.


THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
                                  OF SUCH DAMAGE.

/////
// What is in this file:

 - Type definitions for code that interacts with JS
 - Data validation routines for C++ representations of JS data
 - Forward declatarations of the required plugin functions. This is disabled
     when PLUGINNAME is not defined, which signifies that this file is being
     used by the TurboSphere engine binary itself.

*/
///////////////////////////////////////////////////////////////////////////////

#ifdef COMMON_PLUGIN_HEAD
#error You have included both the new and old plugin headers. This is not a good idea.
#endif
#ifndef COMMON_PLUGIN_NEW_HEAD
#define COMMON_PLUGIN_NEW_HEAD

#ifndef PLUGINNAME //In the engine
    #ifdef _MSC_VER
    #pragma warning No Plugin Name Specified. This is fine for the engine itself, bad for plugins.
    #else
    #warning No Plugin Name Specified. This is fine for the engine itself, bad for plugins.
    #endif
    #define PLUGINNAME "Engine"
    #define NOPLUGNAME 1
#endif
#include <jsapi.h>
#include <jsfriendapi.h>

#include <string>
#include <vector>
#include <mutex>
#include <cassert>

///////////////////////////////////////////////////////////////////////////////
// Plugin SDK

namespace Turbo{

    template<typename T = char>
    struct JSStringHolder{
        T *const string;
        JSContext *const ctx;
        
        JSStringHolder(JSContext *ctx_, T *string_): string(string_), ctx(ctx_){}
        
        ~JSStringHolder(){JS_free(ctx, string);}
    };
    
    /////
    // Basic TypeDefs    
    typedef const char *(*InitFunction)(JSContext *ctx, unsigned ID);
    typedef void (*CloseFunction)(JSContext *ctx);
    typedef int (*NumFunction)(JSContext *ctx);
    typedef JSNative *(*GetFuncFunction)(JSContext *ctx, int n);
    typedef void (*GetVarFunction)(JSContext *ctx, int n, JS::MutableHandleValue val);
    typedef const char *(*GetNameFunction)(JSContext *ctx, int n);
    
    inline void SetError(JSContext *ctx, const std::string err){
        // This gives us just a little more info on crash (lldb, for instance, can't handle printing `err').
        if(JS_IsExceptionPending(ctx)){
            printf("[Turbo] SetError Error an exception was already pending while trying to set the following exception:\n%s", err.c_str());
            JS_ReportPendingException(ctx);
            assert(false); // Break out.
        }
        JS::RootedValue error(ctx, STRING_TO_JSVAL(JS_NewStringCopyN(ctx, err.c_str(), err.length())));
        JS_SetPendingException(ctx, error);
    }
   
    inline void SetError(JSContext *ctx, const std::string err, JS::ObjectOpResult &result){
        // This is changing in the SM API. We don't really know the correct behaviour yet.
        // Just give it some failure.
        result.fail(1);
        
        SetError(ctx, err);
    }

    enum JSType{String = 1, Number, Bool, Object, Array, TypedArray, ArrayBuffer};
    
    inline bool CheckArg(JSContext *ctx, JS::HandleValue val, enum JSType type){
        switch(type){
            case JSType::String:
                return val.isString();
            case JSType::Number:
                return val.isNumber();
            case JSType::Bool:
                return val.isBoolean();
            case JSType::Object:
                return val.isObject();
            case JSType::Array:
            {
                JS::RootedValue obj(ctx, val); 
                return JS_IsArrayObject(ctx, obj);
            }
            case JSType::ArrayBuffer:
            {
                JS::RootedValue value(ctx, val);
                JS::RootedObject obj(ctx);
                if(!JS_ValueToObject(ctx, value, &obj))
                    return false;
                return JS_IsArrayBufferObject(obj);
            }
            case JSType::TypedArray:
            {
                JS::RootedValue value(ctx, val);
                JS::RootedObject obj(ctx);
                return (JS_ValueToObject(ctx, value, &obj) && JS_IsTypedArrayObject(obj));
            }
            default:
                return false;
        }
    }
    
    inline const char * const GetTypeName(enum JSType type){
        
        switch(type){
        #define TURBO_TYPE_CASE(TYPE)\
            case TYPE:\
                return #TYPE
                
        TURBO_TYPE_CASE(String);
        TURBO_TYPE_CASE(Number);
        TURBO_TYPE_CASE(Bool);
        TURBO_TYPE_CASE(Array);
        TURBO_TYPE_CASE(TypedArray);
        TURBO_TYPE_CASE(ArrayBuffer);
        TURBO_TYPE_CASE(Object);
        
        #undef TURBO_TYPE_CASE
        default:
            return nullptr;
        }
    }
    
    template<typename T>
    std::string BadArgTypeErrorMessage(unsigned i, enum JSType type, const T &func){
        return std::string("[" PLUGINNAME "] ") + func + " Error argument " + std::to_string(i) + " is not a " + GetTypeName(type);
    }
    
    
    template<int N>
    bool CheckSignature(JSContext *ctx, JS::CallArgs &args, const enum JSType type[N], const char *func, bool set_error = true){
        
        if(args.length()<N){
            SetError(ctx, std::string("[" PLUGINNAME "] ") + func + " Error called with fewer than " + std::to_string(N) + " arguments");
            return false;
        }
        for(int i = 0; i<N; i++){
            
            if(!CheckArg(ctx, args[i], type[i])){
                if(set_error){
                    SetError(ctx, BadArgTypeErrorMessage(i, type[i], func));
                }
                return false;
            }
        }
        
        return true;
    }
    
    inline bool CheckForSingleArg(JSContext *ctx, JS::CallArgs &args, enum JSType type, const char *func, int N=0, bool set_error = true){
        if(args.length()<N+1){
            if(set_error){
                SetError(ctx, std::string("[" PLUGINNAME "] ") + func + " Error called with fewer than " + std::to_string(N) + " arguments");
            }
            return false;
        }
        if(!CheckArg(ctx, args[N], type)){
            if(set_error){
                SetError(ctx, BadArgTypeErrorMessage(0, type, func));
            }
            return false;
        }
        return true;
    }
    
    template<class T> struct JSPrototype{

        struct proto_object {JSContext *ctx; JS::Heap<JSObject *> proto;};
        std::vector<struct proto_object> prototypes;
        std::mutex prototypes_mutex;
		JSClass clazz;
        unsigned num_constructor_args;
		
        JSPrototype(const char *class_name, JSNative construct = nullptr, unsigned nargs = 0, JSFinalizeOp finalizer = nullptr, JSSetterOp property = nullptr){
			clazz.flags = JSCLASS_HAS_PRIVATE;
            clazz.name = strdup(class_name);
            //prototypes.reserve(1);
            clazz.construct = construct;
            num_constructor_args = nargs;
            clazz.finalize = finalizer;
            clazz.setProperty = property;
            
        };

        ~JSPrototype(){
            free((void *)(clazz.name));
        };
        
        void initForContext(JSContext *ctx, JSPropertySpec *ps = nullptr, JSFunctionSpec *fs = nullptr, JSPropertySpec *static_ps = nullptr, JSFunctionSpec *static_fs = nullptr){
            
            prototypes_mutex.lock();
            
            JS::RootedObject global(ctx, JS::CurrentGlobalOrNull(ctx)), global_proto(ctx);
            JS_GetPrototype(ctx, global, &global_proto);
            
            prototypes.push_back(proto_object());
			prototypes.back().ctx = ctx;
            prototypes.back().proto = JS_InitClass(ctx, global, global_proto, &clazz, clazz.construct, num_constructor_args, ps, fs, static_ps, static_fs);
            
            prototypes_mutex.unlock();
        
        }
        
        void closeForContext(JSContext *ctx){
            for(typename std::vector<struct proto_object>::iterator iter = prototypes.begin(); iter!=prototypes.end(); iter++){
                if(iter->ctx==ctx){
                    prototypes.erase(iter);
                    return;
                }
            }
        }
        
        bool getPrototypeForContext(JSContext *ctx, JS::MutableHandleObject obj){
            
            prototypes_mutex.lock();
            
            for(typename std::vector<struct proto_object>::const_iterator citer = prototypes.cbegin(); citer!=prototypes.cend(); citer++){
                
                if(citer->ctx==ctx){
                    *obj = citer->proto;
                    prototypes_mutex.unlock();
                    return true;
                }
                
            }
            
            prototypes_mutex.unlock();
            return false;
        
        }
        
        JSObject *wrap(JSContext *ctx, T *that){
            prototypes_mutex.lock();
            for(typename std::vector<struct proto_object>::iterator iter = prototypes.begin(); iter!=prototypes.end(); iter++){
                
                if(iter->ctx==ctx){
                    JS::RootedObject global(ctx, JS::CurrentGlobalOrNull(ctx)), proto(ctx, iter->proto.get());
                    prototypes_mutex.unlock();
                    
                    JS::RootedObject obj(ctx, JS_NewObject(ctx, &clazz)), rooted_proto(ctx, iter->proto.get());
                    JS_SetPrototype(ctx, obj, rooted_proto);
                    JS_SetPrivate(obj.get(), that);
                    return obj;
                }
                
            }
            
            // This usually means you didn't do a initForContext
            assert(false);
            
            return nullptr;
            
        }
        
        T *unwrap(JSContext *ctx, JS::Value &a, JS::CallArgs *args){
            if((!a.isObject()) || a.isNull() || a.isUndefined())
                return nullptr;
                
            return unwrap(ctx, &a, args);
        }
        

        T *unwrap(JSContext *ctx, JS::Value *a, JS::CallArgs *args){
            if((!a->isObject()) || a->isNull() || a->isUndefined())
                return nullptr;
            return unwrap(ctx, a->toObjectOrNull(), args);
        }
        
        T *unwrap(JSContext *ctx, JSObject * a, JS::CallArgs *args){
            JS::RootedObject obj(ctx, a);
            return static_cast<T *>(JS_GetInstancePrivate(ctx, obj, &clazz, args));
        }
        
        T *unwrap(JSContext *ctx, JS::HandleObject a, JS::CallArgs *args){
            return static_cast<T *>(JS_GetInstancePrivate(ctx, a, &clazz, args));
        }
        
        T *unwrap(JSContext *ctx, JS::HandleValue a, JS::CallArgs *args){
            if((!a.isObject()) || a.isNull() || a.isUndefined())
                return nullptr;
            JS::RootedObject obj(ctx, a.toObjectOrNull());
            return static_cast<T *>(JS_GetInstancePrivate(ctx, obj, &clazz, args));
        }
        
        // For use in constructors and destructors. Does not check class.
        T *unsafeUnwrap(JSObject * a){return static_cast<T *>(JS_GetPrivate(a));}
        T *unsafeUnwrap(JS::Value *a){return static_cast<T *>(JS_GetPrivate(a->toObjectOrNull()));}
        T *unsafeUnwrap(JS::Value &a){return static_cast<T *>(unsafeUnwrap(&a));}
        T *unsafeUnwrap(JS::HandleValue a){return static_cast<T *>(unsafeUnwrap(a.toObjectOrNull()));}
        T *unsafeUnwrap(JS::MutableHandleValue a){return static_cast<T *>(unsafeUnwrap(a.toObjectOrNull()));}
        
        T *getSelf(JSContext *ctx, JS::Value *vp, JS::CallArgs *args){
            
            JS::RootedValue  that_val(ctx, JS_THIS(ctx, vp));
            JS::RootedObject that_obj(ctx);
            return unwrap(ctx, that_val.toObjectOrNull(), args);
        }
        
        bool instanceOf(JSContext *ctx, JS::HandleObject obj, JS::CallArgs *args){
            return JS_InstanceOf(ctx, obj, &clazz, args);
        }
        
        bool instanceOf(JSContext *ctx, JS::HandleValue val, JS::CallArgs *args){
            JS::RootedObject that_obj(ctx);
            if(!JS_ValueToObject(ctx, val, &that_obj))
                return false;
                
            return instanceOf(ctx, that_obj, args);
        }
        
    };

}
#ifdef NOPLUGNAME
    #undef PLUGINNAME
#endif

#endif
