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
#include <v8.h>

#include <TSPR/stacktrace.h>

/////
// Containers for JSAccessors
//
#include <tuple>
#include <forward_list>

#include <string>
#include <cstdarg>
#include <cassert>

///////////////////////////////////////////////////////////////////////////////
// New and shiny stuff

namespace Turbo{

    /////
    // Basic TypeDefs
    typedef const v8::FunctionCallbackInfo<v8::Value> & JSArguments;
    typedef void JSFunction;
    typedef JSFunction(*JSCallback)(JSArguments);
    typedef JSCallback*  JSFunctionArray;
    typedef const char*  JSName;
    typedef JSName       JSFunctionName;
    typedef JSName       JSVariableName;
    typedef const char** JSNameArray;
    typedef const char*  InitFunction;

    typedef void * CallbackPointer;

    /////
    // Internal Typdefs
    //
    typedef v8::Handle<v8::String> JSString;
    typedef v8::Local<v8::String> JSAccessorProperty;
    typedef const v8::PropertyCallbackInfo<v8::Value>& JSAccessorInfo;
    typedef JSAccessorInfo JSAccessorGetterInfo;
    typedef const v8::PropertyCallbackInfo<void>& JSAccessorSetterInfo;
    typedef v8::Local<v8::Value> JSValue;
    typedef JSValue *JSValueArray;

    /////
    // A helper for GetFunctions/Variables and names
    //
    /////
    // More safely stores script data (functions and variables) in a format
    //   easily used by Engine.
    //
    template <class A>
    class ScriptObjectList{
    public:
        ScriptObjectList(size_t n = 0){
            num = n;
            contains = new A[num];
            names    = new Turbo::JSName[num];
        }
        ~ScriptObjectList(){
            delete []contains;
            delete []names;
        }

        const A *GetA(void){
            return contains;
        }

        size_t GetNum(void){
            return num;
        }
        const Turbo::JSNameArray GetNames(void){
            return names;
        }

        void Set(A a, Turbo::JSName name, size_t at){
            assert(at<num);
            contains[at] = a;
            names[at] = name;
        }

    private:
        A *contains;
        size_t num;
        Turbo::JSNameArray names;
    };

    inline void SetError(JSArguments args, const char *err, v8::Local<v8::Value> (&v8ExceptionType)(v8::Handle<v8::String>) = v8::Exception::Error){
        v8::Isolate *iso = v8::Isolate::GetCurrent();
        args.GetReturnValue().Set( v8ExceptionType(v8::String::NewFromUtf8(iso, err)));
    }

    enum JSType{Int = 1, Uint, String, Number, Bool, Array, Object};
    class JSAccessor{
    public:
        JSString name;
        void (*Getter)(JSAccessorProperty, JSAccessorInfo);
        void (*Setter)(JSAccessorProperty, JSValue, JSAccessorInfo);
    };

    template<class T> class JSObj{
        public:
        v8::Handle<v8::FunctionTemplate> Template;
        v8::Handle<v8::ObjectTemplate> InstanceTemplate;
        v8::Handle<v8::ObjectTemplate> Prototype;
        v8::Handle<v8::Function> Constructor;

        JSObj<T>(){

            Template         = v8::FunctionTemplate::New(v8::Isolate::GetCurrent());
            Prototype        = Template->PrototypeTemplate();
            InstanceTemplate = Template->InstanceTemplate();
            InstanceTemplate->SetInternalFieldCount(2);
            Constructor      = Template->GetFunction();

        };

        ~JSObj(){

            Template.Clear();
            InstanceTemplate.Clear();
            Prototype.Clear();

            if(!TypeName.IsEmpty())
                TypeName.Clear();

        };

        template<class A>
        bool IsA(v8::Handle<A> Tobj){
            auto obj = v8::Handle<v8::Object>::Cast(Tobj);
            return (ID&&(!((obj->InternalFieldCount()<2)||(ID!=obj->GetInternalField(1)->Uint32Value()))));

        }

        JSString TypeName;
        void (*Finalize)(const v8::WeakCallbackData<v8::Object, T> &args);

        JSAccessor *accessors; //Null-terminated.

        unsigned long long ID;

        void SetTypeName(const char *name){
            if(!TypeName.IsEmpty())
                TypeName.Clear();
            TypeName = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), name);
            Template->SetClassName(TypeName);
        }

        void AddToProto(const char *name , v8::FunctionCallback call){
            auto iso = v8::Isolate::GetCurrent();
            InstanceTemplate->Set(v8::String::NewFromUtf8(iso, name), v8::FunctionTemplate::New(iso, call));
            //Prototype->Set(v8::String::NewFromUtf8(iso, name), v8::FunctionTemplate::New(iso, call));
        }

        void AddAccessor(const char *name, v8::AccessorGetterCallback Getter, v8::AccessorSetterCallback Setter){
            InstanceTemplate->SetAccessor(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), name), Getter, Setter);
            Prototype->SetAccessor(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), name), Getter, Setter);
        }

    };

    /////
    // Data Validation
    namespace CheckArg{

        inline bool String(JSArguments args, int i, const char *funcname){
            if (!args[i]->IsString()) {
                char argnumprnt[] = {(char)i, '\0'};
                std::string err = std::string("[" PLUGINNAME "] ").append(funcname).append(" Error: Argument ").append((char *)argnumprnt).append(" is not a Boolean.");
                args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), err.c_str())));
                return false;
            }
        return true;
        }

        inline bool Int(JSArguments args, int i, const char *funcname){

            if (!args[i]->IsInt32()) {
                char argnumprnt[] = {(char)i, '\0'};
                std::string err = std::string("[" PLUGINNAME "] ").append(funcname).append(" Error: Argument ").append((char *)argnumprnt).append(" is not a Boolean.");
                args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), err.c_str())));
                return false;
            }
        return true;
        }

        inline bool Bool(JSArguments args, int i, const char *funcname){


            if (!args[i]->IsBoolean()) {
                char argnumprnt[] = {(char)i, '\0'};

                std::string err = std::string("[" PLUGINNAME "] ").append(funcname).append(" Error: Argument ").append((char *)argnumprnt).append(" is not a Boolean.");
                args.GetReturnValue().Set( v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), err.c_str())));
                return false;
            }
        return true;
        }

        inline bool CheckSig(JSArguments args, int num, const int *argtypes/*<- Null terminated*/, bool error = true){

            const char *prevF = NULL;
            auto iso = v8::Isolate::GetCurrent();


            if(args.Length()<num){
                if(!error)
                    return false;
                TS_Stack_PreviousFunctionName(prevF);
                const char *err = (std::string("[" PLUGINNAME "] ").append(prevF).append(" Error: Called with fewer than ")+std::to_string(num)+std::string(" arguments.")).c_str();
                args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(iso, err)));
                return false;
            }
            unsigned char argnumprnt [2] = {'0', '\0'};

            std::string err;

            for(int i = 0; i<num; i++){
                *argnumprnt = (unsigned char)'0'+(unsigned char)i;
                assert(argtypes[i]!=NULL);

                switch(argtypes[i]){

                    case (JSType::String):
                        if (args[i]->IsString())
                            break;
                        TS_Stack_PreviousFunctionName(prevF);

                        err = std::string("[" PLUGINNAME "] ").append(prevF).append(" Error: Argument ").append((char *)argnumprnt).append(" is not a String.");
                        if(error)
                            args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(iso, err.c_str())));
                    return false;

                    case (JSType::Int):
                        if (args[i]->IsInt32())
                            break;
                        TS_Stack_PreviousFunctionName(prevF);

                        err = std::string("[" PLUGINNAME "] ").append(prevF).append(" Error: Argument ").append((char *)argnumprnt).append(" is not an Integer.");

                        if(error)
                            args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(iso, err.c_str())));
                    return false;

                    case Uint:
                        if (args[i]->IsUint32())
                            break;
                        TS_Stack_PreviousFunctionName(prevF);

                        err = std::string("[" PLUGINNAME "] ").append(prevF).append(" Error: Argument ").append((char *)argnumprnt).append(" is not an Integer.");

                        if(error)
                            args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(iso, err.c_str())));
                    return false;

                    case Number:
                        if (args[i]->IsNumber())
                            break;
                        TS_Stack_PreviousFunctionName(prevF);

                        err = std::string("[" PLUGINNAME "] ").append(prevF).append(" Error: Argument ").append((char *)argnumprnt).append(" is not a Number.");

                        if(error)
                            args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(iso, err.c_str())));
                    return false;

                    case JSType::Bool:
                        if (args[i]->IsBoolean())
                            break;
                        TS_Stack_PreviousFunctionName(prevF);

                       err = std::string("[" PLUGINNAME "] ").append(prevF).append(" Error: Argument ").append((char *)argnumprnt).append(" is not a Boolean.");

                        if(error)
                            args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(iso, err.c_str())));
                    return false;

                    case Array:
                        if (args[i]->IsArray())
                            break;
                        TS_Stack_PreviousFunctionName(prevF);

                        err = std::string("[" PLUGINNAME "] ").append(prevF).append(" Error: Argument ").append((char *)argnumprnt).append(" is not an Array.");

                        if(error)
                            args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(iso, err.c_str())));
                    return false;

                    case Object:
                        if (!args[i]->IsUndefined())
                            break;
                        TS_Stack_PreviousFunctionName(prevF);

                        err = std::string("[" PLUGINNAME "] ").append(prevF).append(" Error: Argument ").append((char *)argnumprnt).append(" is undefined.");

                        if(error)
                            args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(iso, err.c_str())));
                    return false;
                    default:
                    return true;
                }
            }
            return true;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Wrapping Objects

    template<class T, class A> inline void WrapObject(A args, const JSObj<T> &JSo, T *obj){

        //
        auto iso = v8::Isolate::GetCurrent();


        /////
        // Create a JS object that holds an ID number and a pointer to the object
        assert(obj != NULL);
        //JSo.InstanceTemplate->SetInternalFieldCount(2);
        v8::Handle<v8::Object> returnobj = JSo.InstanceTemplate->NewInstance();//JSo.Constructor->NewInstance();//v8::Object::New(iso);// = JSo.Constructor->NewInstance();
        assert(JSo.Template->HasInstance(returnobj));
        returnobj->SetAlignedPointerInInternalField(0, obj);
        returnobj->SetInternalField(1, v8::Integer::NewFromUnsigned(iso, JSo.ID));
        //args.GetReturnValue().Set(returnobj);
        //return;

        printf("%s\n", *(v8::String::Utf8Value(JSo.TypeName)));

        /////
        // Set the weak reference callback

        v8::Persistent<v8::Object> preturnobj(iso, returnobj);
        preturnobj.SetWeak(obj, JSo.Finalize);

        //
        args.GetReturnValue().Set(preturnobj);

    }

    /////
    // Unwrapping objects

    template<class T>
    T *GetMemberSelf(JSArguments args) {
        return static_cast<T*>(args.Holder()->GetAlignedPointerFromInternalField(0));
    }

    template<class T, typename A>
    T *GetAccessorSelf(v8::PropertyCallbackInfo<A> info) {
        return static_cast<T*>(info.Holder()->GetAlignedPointerFromInternalField(0));
    }

    template<class T, class A>
    T* GetSelf(const A &container){
        return static_cast<T*>(container.Holder()->GetAlignedPointerFromInternalField(0));
    }

    template<class A>
    void *GetSelf(const A &container){
        return container.Holder()->GetAlignedPointerFromInternalField(0);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Legacy and Old-fashioned stuff

/////
// Memory alignment

#define MIN_MEM_ALIGN 2

#ifdef _MSC_VER

    #define __func__ __FUNCTION__

    #define MEMALIGN(X) _declspec(align(X))
#else

    #define MEMALIGN(X) __attribute__((aligned(X)))
#endif

#define MINMEMALIGN MEMALIGN(MIN_MEM_ALIGN)



#ifdef NOPLUGNAME
    #undef PLUGINNAME
#endif

#endif