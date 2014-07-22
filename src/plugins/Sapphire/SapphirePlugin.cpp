#include "SapphirePlugin.hpp"
#include "SapphireInit.hpp"
#include "Sapphire.hpp"
#include "script.hpp"


//#define L_WINDOW_TEST 1

#ifdef L_WINDOW_TEST
#include "Galileo/Group.hpp"
#include "Galileo/State.hpp"
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include "GLStart.hpp"
#include "Image.hpp"
#include <vector>

const char *L_NUMS[] = {
  "one",
  "two",
  "three",
  "four",
  "five"
};

#endif


const char * Init(int aID){

    Sapphire::Script::InitScript(aID);

    Sapphire::Init(aID);

    #ifdef L_WINDOW_TEST

    Sapphire::Galileo::Shader *lShader = Sapphire::Galileo::Shader::GetDefaultShader();
    Sapphire::Galileo::Group *lGroup = new Sapphire::Galileo::Group();

    SDL_Surface *lSurf = IMG_Load("system/pointer.png");

/*
Uint8 BitsPerPixel
Uint8 BytesPerPixel
Rmask
Gmask
Bmask
Amask
*/

    assert(lSurf);
    printf(BRACKNAME " Info: Opened a surface with the following properies: %i bits, %i bytes, %x r %x g %x b %x a \n",
    lSurf->format->BitsPerPixel, lSurf->format->BytesPerPixel, lSurf->format->Rmask, lSurf->format->Gmask, lSurf->format->Bmask, lSurf->format->Amask);
    SDL_Surface* iSurf = SDL_CreateRGBSurface(0,lSurf->w, lSurf->h,32, CHANNEL_MASKS);
    assert(iSurf);
    printf(BRACKNAME " Info: Created a surface with the following properies: %i bits, %i bytes, %x r %x g %x b %x a \n",
    iSurf->format->BitsPerPixel, iSurf->format->BytesPerPixel, iSurf->format->Rmask, iSurf->format->Gmask, iSurf->format->Bmask, iSurf->format->Amask);
    SDL_Rect lRect = {0, 0, lSurf->w, lSurf->h};

    SDL_LowerBlit(lSurf, &lRect, iSurf, &lRect);

    SDL_FreeSurface(lSurf);

    SDL_LockSurface(iSurf);
    Sapphire::Image *lImage = new Sapphire::Image(iSurf);
    SDL_UnlockSurface(iSurf);

    SDL_FreeSurface(iSurf);

   // SDL_FreeSurface(lSurf);

    std::vector<Sapphire::Galileo::Vertex> lVertices;
    lVertices.resize(4);

    lVertices[0] = Sapphire::Galileo::Vertex(-16,   0, 0.0f, 1.0f);
    lVertices[1] = Sapphire::Galileo::Vertex(128,   0, 1.0f, 1.0f);
    lVertices[2] = Sapphire::Galileo::Vertex(16,  144, 0.0f, 0.0f);
    lVertices[3] = Sapphire::Galileo::Vertex(144, 128, 1.0f, 0.0f);

//    lVertices[0].color. = 0;

    for(auto lIterator = lVertices.begin(); lIterator!=lVertices.end(); lIterator++){
        printf(BRACKNAME " Info: X:%i\tY:%i\tU:%f\tV:%f\n", lIterator->x, lIterator->y, lIterator->u, lIterator->v);
    }

    Sapphire::Galileo::Shape *lShape = new Sapphire::Galileo::Shape(lVertices, lImage);

    lShape->FillGL();

    lGroup->SetShader(lShader);

    lGroup->push(lShape);

    //sleep(1);

    for(int i = 0; i<10; i++){
        //printf(BRACKNAME "Test: That's %s.\n", L_NUMS[i]);
        lGroup->DrawAll(Sapphire::GL::RenderQueue());

        Sapphire::GL::RenderQueue()->push(new Sapphire::Galileo::FlipScreen(Sapphire::GL::GetRenderFrame()));

        //sleep(1);
    }

    SDL_Surface *sSurf =SDL_CreateRGBSurfaceFrom(lImage->LockImage(),  lImage->Width(), lImage->Height(),
                                                 32, lImage->Width()*4, CHANNEL_MASKS);

    SDL_SaveBMP(sSurf, "LoooL.bmp");
    fgetc(stdin);
    Sapphire::GL::RenderThread::StopThread();
    exit(EXIT_SUCCESS);

    #endif

    return PLUGINNAME;
}

void Close(void){

}

using namespace Sapphire;

int GetNumFunctions(void){
    return Script::NumFuncs;
}

const Turbo::JSFunctionArray GetFunctions(void){
    return Script::FunctionList.data();
}

const Turbo::JSNameArray GetFunctionNames(void){
    return Script::FunctionNameList.data();
}

int GetNumVariables(void){
    return Script::NumVars;
}

const Turbo::JSValueArray GetVariables(void){
    return Script::VariableList.data();
}

const Turbo::JSNameArray GetVariableNames(void){
    return Script::VariableNameList.data();
}
