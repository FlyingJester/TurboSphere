#include "Blits.h"
#include "../Sapphire.hpp"

#define DECLARE_ERROR_LABEL(NAME)\
    static char * NAME = calloc(1, strlen(BRACKNAME) + strlen(__func__) + strlen(" Error: ") + 2);\
    do{\
        if((*NAME)=='\0'){\
            strcpy(NAME, BRACKNAME);\
            strcat(NAME, " ");\
            strcat(NAME, __func__);\
            strcat(NAME, " Error: ");\
        }\
    }while(0)

#define PRINT_SDL_ERROR(FN_NAME_PREFIX) do{\
            TS_StdErr_Write(FN_NAME_PREFIX);\
            TS_StdErr_Write(SDL_GetError());\
            TS_StdErr_Write("\n");}while(0)

#define SET_BLEND_MODE_SWITCH(ERR, S, BM, FN_NAME_PREFIX)\
        ERR = SDL_SetSurfaceBlendMode(S, BM);\
        if(ERR){\
          PRINT_SDL_ERROR(FN_NAME_PREFIX);\
          return tssFailure;\
        }\
        break

enum TS_SurfError TS_SurfaceBlitSurface(struct TS_SurfaceArgs *arg){
    if(args==NULL)
      return tssInvalidArg;
    if(args->mTo==NULL)
      return tssInvalidSurf;
    if(args->mFrom==NULL)
      return tssInvalidSurf|tssInvalidArg;

    DECLARE_ERROR_LABEL(pfName);

    SDL_Rect srcrect = {args->ax,args->ay,args->aw,args->ah};
    SDL_Rect dstrect = {args->x, args->y, args->w, args->h};

    // If we aren't overriding the blend mode, just do the blit.

    if(args->s.mBlendMode==tsbNone){
        if(SDL_BlitSurface(args->mFrom,&srcrect,args->mTo, &dstrect)){
            PRINT_SDL_ERROR(pfName);
            return tssFailure;
        }
        return tssSuccess;
    }

    //We need to record the blend mode, override it, blit, and then change it back.

    SDL_BlendMode BM;
    int err = SDL_GetSurfaceBlendMode(args->mTo,&BM);
    if(err){
      PRINT_SDL_ERROR(pfName);
      return tssFailure;
    }

    switch(args->s.mBlendMode){
      case tsbBlend:
          SET_BLEND_MODE_SWITCH(err, args->mTo, SDL_BLENDMODE_BLEND, pfName);
      case tsbReplace:
          SET_BLEND_MODE_SWITCH(err, args->mTo, SDL_BLENDMODE_NONE, pfName);
      case tsbAdd:
          SET_BLEND_MODE_SWITCH(err, args->mTo, SDL_BLENDMODE_ADD, pfName);
      case tsbMultiply:
          SET_BLEND_MODE_SWITCH(err, args->mTo, SDL_BLENDMODE_MOD, pfName);
      default:
          {
              char lN[] = {(args->s.mBlendMode/10)%10, (args->s.mBlendMode)%10, '\0'};

              TS_StdErr_Write(pfName);
              TS_StdErr_Write(" Warning: FIXME! Unimplemented blendmode ");
              TS_StdErr_Write(lN);
              TS_StdErr_Write(". Falling back to BLEND.\n");

          }
          SET_BLEND_MODE_SWITCH(err, args->mTo, SDL_BLENDMODE_BLEND, pfName);
    };

    if(args->a.s.mScale){
        err = SDL_BlitScaled(args->a.s.mFrom,&srcrect,args->mTo, &dstrect);
    }
    else{
        err = SDL_BlitSurface(args->a.s.mFrom,&srcrect,args->mTo, &dstrect);
    }

    if(err)
      PRINT_SDL_ERROR(pfName);

    err = SDL_SetSurfaceBlendMode(args->mTo, BM);
    if(err){
      PRINT_SDL_ERROR(pfName);
      return tssFailure;
    }
    return tssSuccess

};

enum TS_SurfError TS_SurfaceTransformBlitSurface(struct TS_SurfaceArgs *arg);
