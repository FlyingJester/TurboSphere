
#include "main.h"
#include "shader.h"
#include "../../t5.h"
#include "../../configmanager/opengame.h"
#include <string>
#include <cassert>
#include <cstring>

using std::string;

TS_Shader TS_LoadSystemShader(const char *file){

    assert(file!=NULL);

    TS_Directories *TS_Dirs = GetDirs();

    assert(TS_Dirs->systemshader!=NULL);
    assert(strnlen(TS_Dirs->systemshader, 8)>0);

    //Concatenate up the filename.

    size_t len1 = strlen(TS_Dirs->systemshader);
    size_t len2 = len1+strlen(file);

    char *fullfile = (char *)malloc(len2+1);

    fullfile[len2] = '\0';

    memcpy(fullfile, TS_Dirs->systemshader, len1+1);

    strcat(fullfile, file);

    assert(T5_IsDir(TS_Dirs->systemshader));
    assert(T5_IsFile(fullfile));

    //Load the specified shader manifest
    T5_file* shaderfile = T5_OpenFile(fullfile);

    assert(shaderfile!=NULL);

    const char *fragmentname = shaderfile->getValue("fragment");
    const char *vertexname = shaderfile->getValue("vertex");

    //Load the filetext of the shaders
    T5_FileText fragment_text   = T5_LoadFileAsText(string(TS_Dirs->systemshader).append(fragmentname).c_str());
    T5_FileText vertex_text     = T5_LoadFileAsText(string(TS_Dirs->systemshader).append(vertexname).c_str());

    printf("Frag Shader:\n%s\n\nVertex Shader:\n%s\n", fragment_text, vertex_text);

    //Build the program
    TS_Shader frag = TS_CreateShader(fragment_text, GL_FRAGMENT_SHADER);
    TS_Shader vert = TS_CreateShader(vertex_text, GL_VERTEX_SHADER);

    TS_Shader prog = TS_CreateProgram(frag, vert);

    if(prog==0)
        fprintf(stderr, BRACKNAME "%s Error: Could not create program from %s.\n", __func__, file);

    return 0;
}

TS_Shader TS_LoadShader(const char *file){
    return 0;
}

TS_Shader TS_CreateShader(const char *text, GLenum type){
    if(text==NULL){
        printf(BRACKNAME BRACKNAME " Error: Emtpy string given.\n");
        return 0;
    }
    TS_Shader shader = glCreateShader(type);
    if(shader==0){
        printf(BRACKNAME BRACKNAME " Error: Something went terribly wrong, the shader index was 0.\n");
    }
    GLint tsize = strlen(text)+2;

    glShaderSource(shader, 1, &text, &tsize);

    glCompileShader(shader);

    GLint shader_status;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_status);

    if (shader_status==GL_FALSE) {
        printf(BRACKNAME " Error: Failed to compile shader.\n");

        GLint log_size;
        GLint written = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
        char *log_text = (char *)malloc(log_size);
        glGetShaderInfoLog(shader, log_size, &written, log_text);
        if(written==0)
            printf(BRACKNAME " Error: No log was written.\n");
        if(written>log_size)
            printf(BRACKNAME " Error: Your OpenGL driver just wrote past the end of my buffer. I told it not to, really!\n");
        printf("%s\n", log_text);
        free(log_text);
        glDeleteShader(shader);

        return 0;
    }
    printf(BRACKNAME " Info: Shader compiled ok. ID number %i.\n", shader);
    return shader;

}

TS_ShaderProg TS_CreateProgram(TS_Shader frag, TS_Shader vert){
    if((glIsShader(frag)==GL_FALSE)||(glIsShader(vert)==GL_FALSE)){
        printf(BRACKNAME " Error: One or more shader was invalid\n\tFrag %s\tVert %s\n", (glIsShader(frag)==GL_TRUE)?"good":"bad", (glIsShader(vert)==GL_TRUE)?"good":"bad");
        //return 0;
    }

    GLint program_status;

    GLint fragstat, vertstat;

    glGetShaderiv(frag, GL_SHADER_TYPE, &fragstat);

    if(fragstat!=GL_FRAGMENT_SHADER)
        printf(BRACKNAME " Error: Invalid fragment shader.\n");

    glGetShaderiv(vert, GL_SHADER_TYPE, &vertstat);

    if(vertstat!=GL_VERTEX_SHADER)
        printf(BRACKNAME " Error: Invalid vertex shader.\n");

    if((fragstat!=GL_FRAGMENT_SHADER)||(vertstat!=GL_VERTEX_SHADER)){
        printf(BRACKNAME " Error: Bad shader(s). Exiting.\n");
        //return 0;

    }

    TS_ShaderProg prog = glCreateProgram();

    glAttachShader(prog, frag);
    glAttachShader(prog, vert);
    printf(BRACKNAME " Info: Linking Program.\n");
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &program_status);

    if(!program_status){
        printf(BRACKNAME " Error: Could not link program.\n");
        GLint log_size;
        char *log_text;

        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_size);
        log_text = (char *)malloc(log_size+1);
        glGetProgramInfoLog(prog, log_size, NULL, log_text);
        printf("%s\n", log_text);
        free(log_text);
        glDeleteProgram(prog);

        return 0;

    }
    printf(BRACKNAME " Info: Program linked ok.\n");
    return prog;

}
