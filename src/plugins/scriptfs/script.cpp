#include "script.hpp"
#include <t5.h>
#include <string>

#include "v8-hack.hpp"

#include <opengame.h>

#define TURBO_TRYRAWFILE_ISERROR(err) (err!=RawFileError::RF_NoError)

#define TURBO_TRYRAWFILE_ERROR(err)\
     if(TURBO_TRYRAWFILE_ISERROR(err)){\
        const char *err_s = (std::string(__func__) + std::string(" Error: Could not open file: ") + std::string(ExplainRawFileError(err)) ).c_str();\
    }


Turbo::JSFunction GetFileList(Turbo::JSArguments args){

}

Turbo::JSFunction RemoveFile(Turbo::JSArguments args){

}

Turbo::JSFunction RenameFile(Turbo::JSArguments args){

}

Turbo::JSFunction CopyFile(Turbo::JSArguments args){

}


namespace scriptfs {

    static std::string sRawfilePath = "";

    Turbo::JSObj<RawFileHolder> RawFileObj;

    void RawFileSize(Turbo::JSAccessorProperty aProp, Turbo::JSAccessorGetterInfo aInfo){
        const RawFileHolder *mRawFile = Turbo::GetAccessorSelf<RawFileHolder>(aInfo);
        assert(mRawFile);

        unsigned long s = 0;

        rawfile_getSize(mRawFile->A, &s);

        aInfo.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), s));
    }

    Turbo::JSFunction RawFileGetSize(Turbo::JSArguments args){
        const RawFileHolder *mRawFile = Turbo::GetMemberSelf<RawFileHolder>(args);
        assert(mRawFile!=nullptr);

        unsigned long s = 0;

        rawfile_getSize(mRawFile->A, &s);

        args.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), s));
    }


    Turbo::JSFunction RawFileRead(Turbo::JSArguments args){

        const int sig[2] = {Turbo::Int, 0};
        unsigned long len, left, at, total;
        enum RawFileError err;
        const RawFileHolder *mRawFile = Turbo::GetMemberSelf<RawFileHolder>(args);
        assert(mRawFile!=nullptr);

        if(!Turbo::CheckArg::CheckSig(args, 1, sig))
          return;

        len = args[0]->IntegerValue();

        err = rawfile_getPosition(mRawFile->A, &at);
        if(TURBO_TRYRAWFILE_ISERROR(err)){
          TURBO_TRYRAWFILE_ERROR(err);
          return;
        }

        err = rawfile_getSize(mRawFile->A, &total);
        if(TURBO_TRYRAWFILE_ISERROR(err)){
          TURBO_TRYRAWFILE_ERROR(err);
          return;
        }

        left = total-at;

        len = std::min<unsigned long>(left, len);

        v8::Local<v8::ArrayBuffer> buffer = v8::ArrayBuffer::New(args.GetIsolate(), len);


        {
            v8::HandleScope scope(args.GetIsolate());

            char *r_buffer = (char *)malloc(len);
            err = rawfile_read(mRawFile->A, r_buffer, len);

            printf("Reading %i bytes. Buffer holds %s.\n", len, r_buffer);

            v8::Local<v8::Uint8Array> typed_buffer = v8::Uint8Array::New(buffer, 0, len);

            for(int i = 0; i<len; i++){
                typed_buffer->Set(i, v8::Uint32::New(args.GetIsolate(), r_buffer[i]));
            }

            free(r_buffer);

        /*
            v8::ArrayBuffer::Contents contents = buffer->Externalize();

            assert(contents.ByteLength()==len);
            err = rawfile_read(mRawFile->A, contents.Data(), len);

            buffer->Reinternalize(contents);
        */
        }

        args.GetReturnValue().Set(buffer);

    }

    Turbo::JSFunction RawFileWrite(Turbo::JSArguments args){

        const int sig[] = {Turbo::TypedArray, Turbo::Int, 0};

        RawFileHolder *mRawFile = Turbo::GetMemberSelf<RawFileHolder>(args);
        assert(mRawFile!=nullptr);

        if(!Turbo::CheckArg::CheckSig(args, 1, sig))
          return;

        v8::TypedArray *ar = v8::TypedArray::Cast(*(args[0]));

        if(ar->Length()==0)
          return;

        v8::ArrayBuffer::Contents lContent = ar->Buffer()->Externalize();

        if(lContent.ByteLength()==0)
          return;
        unsigned long len = lContent.ByteLength();

        if(Turbo::CheckArg::CheckSig(args, 2, sig, false))
          len = std::min<unsigned long>(len, args[1]->IntegerValue());

        rawfile_write(mRawFile->A, lContent.Data(), len);
    }

    Turbo::JSFunction OpenRawFile(Turbo::JSArguments args);

}

Turbo::JSFunction scriptfs::OpenRawFile(Turbo::JSArguments args){

    const int sig[3] = {Turbo::String, Turbo::Bool, 0};
    RawFileError err;
    bool writeable = false;

    if(!Turbo::CheckArg::CheckSig(args, 1, sig))
      return;

    if(args.Length()>1){
        if(!Turbo::CheckArg::CheckSig(args, 2, sig)){
            return;
        }

        writeable = args[1]->BooleanValue();
    }

    v8::String::Utf8Value lPath(args[0]);

    const std::string lCombinedPath = scriptfs::sRawfilePath + std::string(*lPath);

    struct RawFile *rFile = AllocRawFile();
    err = ::OpenRawFile(rFile, lCombinedPath.c_str(), writeable);

    if(TURBO_TRYRAWFILE_ISERROR(err)){
      TURBO_TRYRAWFILE_ERROR(err);
    }
    else
      Turbo::WrapObject(args, scriptfs::RawFileObj, new RawFileHolder(rFile));
    }

void InitRawFile(int ID){
    scriptfs::RawFileObj = Turbo::JSObj<RawFileHolder> ();
    scriptfs::RawFileObj.ID = ID << 16;
    scriptfs::RawFileObj.Finalize = Turbo::Finalizer<RawFileHolder>;
    scriptfs::RawFileObj.SetTypeName("RawFile");
    scriptfs::RawFileObj.AddAccessor("size",   scriptfs::RawFileSize, nullptr);
    scriptfs::RawFileObj.AddToProto("getSize", scriptfs::RawFileGetSize);
    scriptfs::RawFileObj.AddToProto("read", scriptfs::RawFileRead);

    scriptfs::sRawfilePath = GetDirs()->save;

}
