#include "main.h"
#include "screen.h"
#include "save.h"
#include <ctime>
#include <cassert>

void(* FlipScreen)(void) = NULL;

static GLuint screenshottex;

GLuint screenFrameBuffer = 0;
GLuint screenColorBuffer = 0;

void * screenCopy;

void TS_SetClippingRectangle(TS_Segment *segment){
    if((segment->x1<=0)&&(segment->y1<=0)&&(segment->x2>=(int)GetScreenWidth())&&(segment->x2>=(int)GetScreenWidth())){
        glDisable(GL_SCISSOR_TEST);
        return;
    }

	static const int scaleSize = GetConfig()->scale;

    glEnable(GL_SCISSOR_TEST);

    glScissor(segment->x1, segment->y1, segment->x2*scaleSize, segment->y2*scaleSize);

}

TS_Segment TS_GetCLippingRectangle(void){
    if(!glIsEnabled(GL_SCISSOR_TEST)){
        return TS_Segment(0, 0, (int)GetScreenWidth(), (int)GetScreenHeight());
    }
    GLint params[4];
    glGetIntegerv(GL_SCISSOR_BOX, params);
    return TS_Segment(params[0], params[1], params[2], params[3]);
}

v8Function SetClippingRectangle(V8ARGS){
    if(args.Length()<4){
        THROWERROR(" Error: Called with fewer than 4 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);

    TS_Segment segment;

    segment.x1 = args[0]->Int32Value();
    segment.y1 = args[1]->Int32Value();
    segment.x2 = args[2]->Int32Value();
    segment.y2 = args[3]->Int32Value();

    TS_SetClippingRectangle(&segment);

    return v8::Undefined();
}

v8Function GetClippingRectangle(V8ARGS){

    v8::Local<v8::Object> cliprect = v8::Object::New();

    TS_Segment segment = TS_GetCLippingRectangle();

    cliprect->Set(v8::String::New("x"), v8::Integer::New(segment.x1));
    cliprect->Set(v8::String::New("y"), v8::Integer::New(segment.y1));
    cliprect->Set(v8::String::New("w"), v8::Integer::New(segment.x2));
    cliprect->Set(v8::String::New("h"), v8::Integer::New(segment.y2));

    return cliprect;
}

void FlagForScreenshot(void){
    *(GetScreenShotFlag())=true;
}

GLuint screenVertexBuffer = 0;

#define EXPLAIN_ERROR {\
    GLenum err = glGetError();\
    switch(err){\
    case GL_INVALID_ENUM:\
        printf(BRACKNAME " %s GL Error: Bad Enum.\n", __func__);\
        break;\
    case GL_INVALID_VALUE:\
        printf(BRACKNAME " %s GL Error: Bad Value.\n", __func__);\
        break;\
    case GL_INVALID_OPERATION:\
        printf(BRACKNAME " %s GL Error: Bad Operation.\n", __func__);\
        break;\
    default:\
        printf(BRACKNAME " %s GL Error: %x.\n", __func__, err);\
    }\
}\

void ScreenInit(void){
    glGenTextures(1, &screenshottex);
    *(GetScreenShotFlag())=false;
    TS_Config *TS_Conf = GetConfig();
    const int w = GetScreenWidth();
    const int h = GetScreenHeight();
    screenCopy = calloc(w*h, BPP);
    if(TS_Conf->compositing){
        printf(BRACKNAME " Info: Compositing is defaults to on.\n");
        FlipScreen = FlipScreenComposite;
        glEnable(GL_TEXTURE_2D);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(1, &screenFrameBuffer);
        glGenTextures(1, &screenColorBuffer);


        GLuint vertices[] = {0, 0, (GLuint)w, 0, (GLuint)w, (GLuint)h, 0, (GLuint)h, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 1, 1, 1, 1, 0, 0, 0};

        glGenBuffers(1, &screenVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, screenVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 80, vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        glBindTexture(GL_TEXTURE_2D, screenColorBuffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenCopy);


        glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);    glClearColor(0, 0, 0, 0);
        //ResetOrtho();
        glClear(GL_COLOR_BUFFER_BIT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenColorBuffer, 0);
        glClearColor(0, 0, 0, 255);

    }
    else{
        printf(BRACKNAME " Info: Compositing is defaults to off.\n");
        FlipScreen = FlipScreenDirect;
        TS_Conf->compositing = false;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(screen);
    glClear(GL_COLOR_BUFFER_BIT);

    EXPLAIN_ERROR

}

void ScreenClose(void){
    glDeleteTextures(1, &screenshottex);
    *(GetScreenShotFlag())=false;

    if(screenVertexBuffer!=0){
        glDeleteBuffers(1, &screenVertexBuffer);
        screenVertexBuffer = 0;

    }
    if(screenFrameBuffer!=0){
        glDeleteFramebuffers(1, &screenFrameBuffer);
        glDeleteTextures(1, &screenColorBuffer);
    }
    screenFrameBuffer=0;

    free(screenCopy);
}

bool * const GetScreenShotFlag(void){
    static bool * const s = (bool*)malloc(1);
    return s;
}

v8Function V8GetScreenWidth(V8ARGS){
    return v8::Number::New(GetScreenWidth());
}

v8Function V8GetScreenHeight(V8ARGS){
    return v8::Number::New(GetScreenHeight());
}

static bool * const scr = GetScreenShotFlag();

void FlipScreenComposite(void){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //Must be initialised by ScreenInit.
    //assert(screenFrameBuffer!=0);
    bool sh_same = true;
    if(TS_CurrentCompositeShader!=TS_CurrentShader){
        sh_same = false;
        glUseProgram(TS_CurrentCompositeShader);
    }

    glBindTexture(GL_TEXTURE_2D, screenColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, screenVertexBuffer);
    glVertexPointer(2, GL_INT, 0, NULL);

    //glVertexAttribPointer(CurrentColorAttrib, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (void *)(32));
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, (void *)(32));
    glTexCoordPointer(2, GL_INT, 0, (void *)(48));
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindTexture(GL_TEXTURE_2D, TS_EmptyTexture);

    if(!sh_same){
        glUseProgram(TS_CurrentShader);
    }
    //SDL_GL_SwapWindow(screen);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);
    SDL_GL_SwapWindow(screen);
    glClear(GL_COLOR_BUFFER_BIT);

    //v8::V8::LowMemoryNotification();
}

void FlipScreenDirect(void){

    SDL_GL_SwapWindow(screen);

    if(*scr==true){
        printf("Performing screenshot.\n");
        *scr=false;
        glReadBuffer(GL_FRONT);
        //glBindTexture(GL_TEXTURE_2D, screenshottex);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        //glCopyTexImage2D(screenshottex, 0, GL_RGBA, 0, 0, w, h, 0);
        //glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenCopy);

        const int w = GetScreenWidth();
        const int h = GetScreenHeight();

        glReadPixels(0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, screenCopy);

        time_t t = time(NULL);
        struct tm times = *localtime(&t);

        char *timestr = (char *)malloc(15+18+7+1);

        sprintf(timestr, "%s/%04i_%02i_%02i_%02i_%02i_%02i_%04i%s", SCREENSHOT_FOLDER, times.tm_year+1900, times.tm_mon+1, times.tm_mday, times.tm_hour, times.tm_min, times.tm_sec, SDL_GetTicks()%1000, DEFAULT_IMAGE_EXT);

        save_TGA(timestr, screenCopy, w, h, R8G8B8A8, SDL_GL_SAVETGA_GLNATIVE|SDL_GL_SAVETGA_COMPRESS|SDL_GL_SAVETGA_BGRA);

        //free(screenCopy);

    }
    glClear(GL_COLOR_BUFFER_BIT);
}


v8Function V8FlipScreen(V8ARGS){
    assert(FlipScreen!=NULL);
    FlipScreen();
    return v8::Undefined();
}
