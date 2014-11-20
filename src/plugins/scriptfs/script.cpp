#include "script.hpp"
#include <t5.h>
#include <string>

#include <opengame.h>

#define TURBO_TRYRAWFILE_ISERROR(ERR) (ERR!=RawFileError::RF_NoError)

#define TURBO_TRYRAWFILE_ERROR(ARGS, ERR)\
     if(TURBO_TRYRAWFILE_ISERROR(ERR)){\
        Turbo::SetError(ARGS, (std::string(__func__) + std::string(" Error: Could not open file: ") + std::string(ExplainRawFileError(ERR)) ).c_str());\
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
          TURBO_TRYRAWFILE_ERROR(args, err);
          return;
        }

        err = rawfile_getSize(mRawFile->A, &total);
        if(TURBO_TRYRAWFILE_ISERROR(err)){
          TURBO_TRYRAWFILE_ERROR(args, err);
          return;
        }

        left = total-at;

        len = std::min<unsigned long>(left, len);

        v8::Local<v8::ArrayBuffer> buffer = v8::ArrayBuffer::New(args.GetIsolate(), len);


        {
            v8::HandleScope scope(args.GetIsolate());

            void *r_buffer = malloc(len);
            err = rawfile_read(mRawFile->A, r_buffer, len);

            printf("Reading %i bytes.\n", len);

            int to = len/4;
            v8::Local<v8::Uint32Array> typed_buffer = v8::Uint32Array::New(buffer, 0, to);

            for(int i = 0; i<to; i++){
                typed_buffer->Set(i, v8::Uint32::New(args.GetIsolate(), static_cast<uint32_t *>(r_buffer)[i]));
            }

            v8::Local<v8::Uint8Array> typed_buffer_2 = v8::Uint8Array::New(buffer, 0, len);
            for(int i = to*4; i<len; i++){
                typed_buffer_2->Set(i, v8::Uint32::New(args.GetIsolate(), static_cast<uint8_t *>(r_buffer)[i]));
            }

            printf("Copied %i bytes.\n", len, r_buffer);

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

inline bool IsPathSeperator(char c){
    return (c=='/')
#ifdef _WIN32
    || (c=='\\')
#endif
    ;
}

bool ReplacePathStringIfFound(std::string &in, std::string replace, const char *with){
    size_t c = in.rfind(replace);
    if(c!=std::string::npos){
        in = std::string(with) + in.substr(c+replace.size()+1);
        return true;
    }

    return false;
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

    std::string lCombinedPath = scriptfs::sRawfilePath + std::string(*lPath);

    if(ReplacePathStringIfFound(lCombinedPath, "#~", GetDirs()->system))
        printf(BRACKNAME " %s Info: Replaced #~, with %s. Now we have %s.\n", __func__, GetDirs()->system, lCombinedPath.c_str());
    if(ReplacePathStringIfFound(lCombinedPath, "~", GetDirs()->root))
        printf(BRACKNAME " %s Info: Replaced ~ with %s. Now we have %s.\n", __func__, GetDirs()->root, lCombinedPath.c_str());

    struct RawFile *rFile = AllocRawFile();
    err = ::OpenRawFile(rFile, lCombinedPath.c_str(), writeable);

    if(TURBO_TRYRAWFILE_ISERROR(err)){
      TURBO_TRYRAWFILE_ERROR(args, err);
      return;
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
