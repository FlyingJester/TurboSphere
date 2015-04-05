#pragma once
#include "FS.hpp"
#include <t5_datasource.h>

namespace RawFile {

    void initScript(JSContext *ctx);
    void closeScript(JSContext *ctx);

    extern Turbo::JSPrototype<t5::DataSource> raw_file_proto;
    bool OpenRawFile(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool RawFileSeek(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool RawFileTell(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool RawFileSize(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool RawFileRead(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool RawFileWrite(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool RawFileFlush(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool RawFileClose(JSContext *ctx, unsigned argc, JS::Value *vp);
}

namespace FS {
    bool GetFileList(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool GetDirectoryList(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool DeleteFile(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool DeleteDirectory(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool CreateDirectory(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool Rename(JSContext *ctx, unsigned argc, JS::Value *vp);
}
