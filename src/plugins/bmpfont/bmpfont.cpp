#define BMPFONT_INTERNAL 1
#include "bmpfont.hpp"
#include "bmpfontCore.h"
#include <cassert>
#include <cmath>
#include <string>
#include <SDL2/SDL.h>
#include <array>
#include <opengame.h>
#include <TSPR/dlopenwrap.h>
#include <t5.h>

#define Frmask 0x000000FF
#define Fgmask 0x0000FF00
#define Fbmask 0x00FF0000
#define Famask 0xFF000000

#define CHANNEL_MASKS Frmask, Fgmask, Fbmask, Famask
#define IMAGE_DEPTH 32

namespace bmpfont {

/*

struct RGBA_Pixels {
  uint32_t *pixels;
  unsigned w, h;
};

struct RGBA_string {
  RGBA_Pixels *pixelarray;
  unsigned length;
};
*/

struct StringSpec{
  unsigned length;
  unsigned w;
  unsigned h;

};

void WriteUTF8ToSurface(TS_bmpfont *aFont, SDL_Surface *aSurface, const char *aString, unsigned x, unsigned y){
    assert(aFont);
    assert(aSurface);
    struct RGBA_string lString;
    SDL_Surface *lSurf = nullptr;
    TS_BMPGetUTF8String(aFont, &lString, aString);

    SDL_Rect dest = {(short)x, (short)y, 0, 0};

    for(int i = 0; i<lString.length; i++){
      dest.w = lString.pixelarray[i].w;
      dest.h = lString.pixelarray[i].h;
      lSurf = SDL_CreateRGBSurfaceFrom(lString.pixelarray[i].pixels, dest.w, dest.h, IMAGE_DEPTH, dest.w<<2, CHANNEL_MASKS);

      SDL_BlitSurface(lSurf, nullptr, aSurface, &dest);

      SDL_FreeSurface(lSurf);
      dest.x+=lString.pixelarray[i].w;
      if(dest.x>aSurface->w)
        return;
    }
}

struct StringSpec GetUTF8StringSpecs(TS_bmpfont *aFont, const char *aString){
    struct RGBA_string lString;
    TS_BMPGetUTF8String(aFont, &lString, aString);
    struct StringSpec lSpec;
    lSpec.w = 0;
    lSpec.h = 0;
    for(int i = 0; i < lString.length; i++){
        lSpec.h = std::max<unsigned>(lSpec.h, lString.pixelarray[i].h);
        lSpec.w+= lString.pixelarray[i].w;
        //printf(BRACKNAME " %s Info: char %i has a width of %i.\n", __func__, i, lString.pixelarray[i].w);
    }
    lSpec.length = lString.length;
    return lSpec;
};

namespace script {

const unsigned NumFuncs = 2;

Turbo::JSFunction LoadFont(Turbo::JSArguments args);
Turbo::JSFunction GetSystemFont(Turbo::JSArguments args);

std::array<Turbo::JSCallback,    NumFuncs> FunctionList = {{LoadFont, GetSystemFont}};
std::array<Turbo::JSName,        NumFuncs> FunctionNameList = {{"Font", "GetSystemFont"}};

Turbo::JSObj<TS_bmpfont> BMPFontJSObj;

void BMPFontFinalizer(const v8::WeakCallbackData<v8::Object, TS_bmpfont> &args) {
    TS_DestroyFont(args.GetParameter());
    args.GetValue().Clear();
}



Turbo::JSFunction LoadFont(Turbo::JSArguments args){

    v8::String::Utf8Value lStr(args[0]);
    std::string lName = (std::string(GetDirs()->font) + std::string(*lStr));

    if(!t5::IsFile(lName.c_str())){
        Turbo::SetError(args, std::string(BRACKNAME " LoadFont Error: No file ").append(lName).c_str());
        return;
    }

    FILE *lFile = fopen(lName.c_str(), "rb");

    if(lFile==nullptr){
        Turbo::SetError(args, std::string(BRACKNAME " LoadFont Error: Could not open file ").append(lName).c_str());
        return;
    }

    TS_bmpfont *lFont = TS_BMPOpenFontCFile(lFile);
    if(!lFont){
        Turbo::SetError(args, std::string(BRACKNAME " LoadFont Error: Invalid Font ").append(lName).c_str());
        return;
    }

    Turbo::WrapObject(args, BMPFontJSObj, lFont);

}

inline Turbo::JSFunction NewFont(Turbo::JSArguments args){
    Turbo::WrapObject(args, BMPFontJSObj, TS_BMPCreateEmptyFont());
}

Turbo::JSFunction GetStringWidth(Turbo::JSArguments args){
    const int sig[] = {Turbo::String, 0};
    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
      return;

    TS_bmpfont* mFont = Turbo::GetMemberSelf <TS_bmpfont> (args);
    v8::String::Utf8Value lStr(args[0]);
    struct StringSpec lSpec = GetUTF8StringSpecs(mFont, *lStr);

    args.GetReturnValue().Set(lSpec.w);
}

Turbo::JSFunction GetStringHeight(Turbo::JSArguments args){
    const int sig[] = {Turbo::String, 0};
    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
      return;

    TS_bmpfont* mFont = Turbo::GetMemberSelf <TS_bmpfont> (args);
    v8::String::Utf8Value lStr(args[0]);
    struct StringSpec lSpec = GetUTF8StringSpecs(mFont, *lStr);
    args.GetReturnValue().Set(lSpec.h);
}

Turbo::JSFunction Font(Turbo::JSArguments args){
    if(args.Length()==0){
        NewFont(args);
        return;
    }

    const int sig[] = {Turbo::String, 0};
    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
      return;

    LoadFont(args);
}

Turbo::JSFunction GetSystemFont(Turbo::JSArguments args){
    printf(BRACKNAME " Info: Loading system font.\n");
    std::string lName = (std::string(GetDirs()->system) + std::string(GetConfig()->systemfont));

    if(!t5::IsFile(lName.c_str())){
        fprintf(stderr, BRACKNAME " %s Error: File %s does not exist.\n", __func__, lName.c_str());
        Turbo::SetError(args, std::string(BRACKNAME " GetSystemFont Error: No file ").append(lName).c_str());
        return;
    }


    FILE *lFile = fopen(lName.c_str(), "rb");

    if(lFile==nullptr){
        fprintf(stderr, BRACKNAME " %s Error: Could not open %s.\n", __func__, lName.c_str());
        Turbo::SetError(args, std::string(BRACKNAME " GetSystemFont Error: Could not open file ").append(lName).c_str());
        return;
    }

    TS_bmpfont *lFont = TS_BMPOpenFontCFile(lFile);
    if(!lFont){
        fprintf(stderr, BRACKNAME " %s Error: Could not open font %s.\n", __func__, lName.c_str());
        Turbo::SetError(args, std::string(BRACKNAME " GetSystemFont Error: Invalid Font ").append(lName).c_str());
        return;
    }

    printf(BRACKNAME " Info: opened system font.\n");
    Turbo::WrapObject(args, BMPFontJSObj, lFont);

}

Turbo::JSFunction DrawText(Turbo::JSArguments args){
    const int sig[] = {Turbo::Object, Turbo::Int, Turbo::Int, Turbo::String, 0};

    if(!Turbo::CheckArg::CheckSig(args, 4, sig, true))
      return;

    if(!BMPFontJSObj.IsA(args[0])){
        Turbo::SetError(args, BRACKNAME " DrawText Error: Argument 0 is not a valid BMPFont.");
        return;
    }

    TS_bmpfont *lFont = (TS_bmpfont*)(v8::Local<v8::Object>::Cast(args[0]))->GetAlignedPointerFromInternalField(0);

    SDL_Surface* mSurface = Turbo::GetMemberSelf <SDL_Surface> (args);

    v8::String::Utf8Value lStr(args[3]);

    WriteUTF8ToSurface(lFont, mSurface, *lStr, args[1]->Int32Value(), args[2]->Int32Value());

}

}
}

void Close(void){}

typedef void (*AddToSurface_t)(const char *aName, Turbo::JSCallback aFunc);

AddToSurface_t Sapphire_AddToSurface;

const char * Init(int ID){
    bmpfont::script::BMPFontJSObj = Turbo::JSObj<TS_bmpfont>();
    bmpfont::script::BMPFontJSObj.ID = ID << 16;
    bmpfont::script::BMPFontJSObj.Finalize = bmpfont::script::BMPFontFinalizer;
    bmpfont::script::BMPFontJSObj.SetTypeName("Font");
    bmpfont::script::BMPFontJSObj.AddToProto("getStringWidth",  bmpfont::script::GetStringWidth);
    bmpfont::script::BMPFontJSObj.AddToProto("getStringHeight", bmpfont::script::GetStringHeight);
    // Load Sapphire!

    fhandle SapphireHandle = DLOPENLIBRARY("Sapphire", "plugin", DL_LOCAL|DL_NOW);
    if(!SapphireHandle){
      fprintf(stderr, BRACKNAME " Init Error: Could not open Sapphire.\n");
      return nullptr;
    }
    // Load the func to attach V8 callbacks to the Surface prototype.
    Sapphire_AddToSurface = (AddToSurface_t)DLOPENFUNCTION(SapphireHandle, "TS_AddMethodToSurface");

    if(!Sapphire_AddToSurface){
      fprintf(stderr, BRACKNAME " Init Error: Could not open from 'TS_AddMethodToSurface' Sapphire.\n");
      return nullptr;
    }
    // Attach our text-drawing functions to Sapphire's surface.
    Sapphire_AddToSurface("drawText", bmpfont::script::DrawText);

    printf( BRACKNAME " Init Info: Successfully extended Surface prototype with Font functions.\n");
    // Leave Sapphire open, in case we opened before Sapphire did properly.
    return PLUGINNAME;
}

int GetNumFunctions(void){
    // Close Sapphire.

    return bmpfont::script::NumFuncs;
}

const Turbo::JSFunctionArray GetFunctions(void){
    return bmpfont::script::FunctionList.data();
}

const Turbo::JSNameArray GetFunctionNames(void){
    return bmpfont::script::FunctionNameList.data();
}

int GetNumVariables(void){return 0;}
const Turbo::JSValueArray GetVariables(void){return nullptr;}
const Turbo::JSNameArray GetVariableNames(void){return nullptr;}
