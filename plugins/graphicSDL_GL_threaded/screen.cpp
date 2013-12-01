#include "main.h"
#include "screen.h"
#include "save.h"
#include <ctime>


static GLuint screenshottex;

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

void ScreenInit(void){
    glGenTextures(1, &screenshottex);
    *(GetScreenShotFlag())=false;

}

void ScreenClose(void){
    glDeleteTextures(1, &screenshottex);
    *(GetScreenShotFlag())=false;
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

void FlipScreen(void){
        //Old SDL 1.2 code.
    //SDL_GL_SwapBuffers();
    SDL_GL_SwapWindow(screen);
    if(*scr==true){
        printf("Performing screenshot.\n");
        *scr=false;
        glReadBuffer(GL_FRONT);
        int w = GetScreenWidth();
        int h = GetScreenHeight();
        //glBindTexture(GL_TEXTURE_2D, screenshottex);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        //glCopyTexImage2D(screenshottex, 0, GL_RGBA, 0, 0, w, h, 0);
        void * screenCopy = calloc(w*h, BPP);
        //glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenCopy);

        glReadPixels(0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, screenCopy);

        time_t t = time(NULL);
        struct tm times = *localtime(&t);

        char *timestr = (char *)malloc(15+18+7+1);

        sprintf(timestr, "%s/%04i_%02i_%02i_%02i_%02i_%02i_%04i%s", SCREENSHOT_FOLDER, times.tm_year, times.tm_mon+1, times.tm_mday, times.tm_hour, times.tm_min, times.tm_sec, SDL_GetTicks()%1000, DEFAULT_IMAGE_EXT);

        save_TGA(timestr, screenCopy, w, h, R8G8B8A8, SDL_GL_SAVETGA_GLNATIVE|SDL_GL_SAVETGA_COMPRESS|SDL_GL_SAVETGA_BGRA);

        free(screenCopy);

    }
    glClear(GL_COLOR_BUFFER_BIT);
}
