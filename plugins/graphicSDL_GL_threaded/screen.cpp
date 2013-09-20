#include "main.h"
#include "screen.h"
#include "save.h"

static GLuint screenshottex;

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
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}
