#include "flipscreen.h"
#include "../backend.h"
#include "../loadfunc/gl_extra.h"
#include "../shader/shader.h"

GLuint CompoArray = 0;

#define NumComp 3

typedef struct {
    GLuint Array;
    size_t componentcount;
    GLuint Buffers[NumComp];
    GLuint AssignTo[NumComp];
    GLsizei Sizes[NumComp];
    int Counts[NumComp];
    GLenum Types[NumComp];

    size_t dimensions[2]; //for screen in pixels.
    RGBA *screenbuffer;

    GLuint FrameBuffer;
    GLuint ScreenTexture;

} CompositingData_t;

CompositingData_t *CompositingData = NULL;

inline void GenerateCompositingData(CompositingData_t *d){
    d->componentcount = NumComp;
    glGenFramebuffers(1, &d->FrameBuffer);
    glGenTextures(1, &d->ScreenTexture);
    glGenBuffers(NumComp, d->Buffers);
}

void FlipScreenCompositeInit(int w, int h){
    printf("[" PLUGINNAME "] Info: Setting Up Screen Compositing.\n");

    SwapBuffers = FlipScreenComposite;

    /////
    // Initialize structure that holds GL names and settings.

    CompositingData = malloc(sizeof(CompositingData_t));

    CompositingData->screenbuffer = malloc(sizeof(RGBA)*w*h);

    CompositingData->componentcount = 3;

    CompositingData->Sizes[0] = sizeof(GLuint);
    CompositingData->Sizes[1] = sizeof(GLfloat);
    CompositingData->Sizes[2] = sizeof(GLfloat);

    CompositingData->Counts[0] = 8;
    CompositingData->Counts[1] = 16;
    CompositingData->Counts[2] = 8;
    CompositingData->Types[0] = GL_UNSIGNED_INT;
    CompositingData->Types[1] = GL_FLOAT;
    CompositingData->Types[2] = GL_FLOAT;

    CompositingData->AssignTo[0] = CurrentVertexAttrib;
    CompositingData->AssignTo[1] = CurrentColorAttrib;
    CompositingData->AssignTo[2] = CurrentTexcoordAttrib;

    /////
    // Set function pointers

    /////
    // Fill a VBO with fillscreen vertex coords, tex coords, and full color.

    {
        GLuint vertices[] = {0, 0, (GLuint)w, 0, (GLuint)w, (GLuint)h, 0, (GLuint)h};
        GLfloat colors[] = {
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        };

        GLfloat texcoords[] = {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};

        void *data[] = {(void *)vertices, (void *)colors, (void *)texcoords};

        glGenBuffers(CompositingData->componentcount, CompositingData->Buffers);

        /////
        // Make a Vertex Array that captures this.

        glGenVertexArrays(1, &CompositingData->Array);
        glBindVertexArray(CompositingData->Array);

        //Fill it with data.
        for(int i = 0; i<CompositingData->componentcount; i++){

            glEnableVertexAttribArray(CompositingData->AssignTo[i]);
            glBindBuffer(GL_ARRAY_BUFFER, CompositingData->Buffers[i]);
            glBufferData(GL_ARRAY_BUFFER, CompositingData->Counts[i]*CompositingData->Sizes[i], data[i], GL_STATIC_DRAW);

            glVertexAttribPointer(CompositingData->AssignTo[i], CompositingData->Counts[i]>>2, CompositingData->Types[i], GL_FALSE, 0, NULL);


        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindVertexArray(0);

    /////
    // Create the compositing framebuffer.

    glGenFramebuffers(1, &CompositingData->FrameBuffer);

    // Texture to hold it's color data.

    glGenTextures(1, &CompositingData->ScreenTexture);

    glBindTexture(GL_TEXTURE_2D, CompositingData->ScreenTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /////
    // Assign it to a screen copy
    //
    // TODO: Make this copy and the one from FJ-GL compatible, so that we only need one.
    //

    /////
    // Setup for first use.

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, CompositingData->screenbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, CompositingData->FrameBuffer);

    // Make the alpha channel fall-through the framebuffer.
    glClearColor(0, 0, 0, 0);
    //Make it look nice at startup.
    glClear(GL_COLOR_BUFFER_BIT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CompositingData->ScreenTexture, 0);
    glClearColor(0, 0, 0, 255);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void FlipScreenCompositeClose(void){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteVertexArrays(1, &CompositingData->Array);
    glDeleteFramebuffers(1, &CompositingData->FrameBuffer);
    glDeleteBuffers(CompositingData->componentcount, CompositingData->Buffers);
    CompoArray = 0;

    free(CompositingData->screenbuffer);
    free(CompositingData);
    CompositingData = NULL;
}

void FlipScreenComposite(void){


/*

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //Must be initialised by ScreenInit.
    //assert(screenFrameBuffer!=0);
    bool sh_same = true;
    if(TS_CurrentCompositeShader!=TS_CurrentShader){
        sh_same = false;
        glUseProgram(TS_CurrentCompositeShader);
    }


    glBindVertexArray(composArray);
    glEnableVertexAttribArray(CurrentColorAttrib);
    glEnableVertexAttribArray(CurrentVertexAttrib);
    glEnableVertexAttribArray(CurrentTexcoordAttrib);
    glBindTexture(GL_TEXTURE_2D, screenColorBuffer);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
*/
    /*


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
    */
/*
    glBindTexture(GL_TEXTURE_2D, TS_EmptyTexture);

    if(!sh_same){
        glUseProgram(TS_CurrentShader);
    }
    //SDL_GL_SwapWindow(screen);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);
    SDL_GL_SwapWindow(screen);
    glClear(GL_COLOR_BUFFER_BIT);
*/
    //v8::V8::LowMemoryNotification();
}
