
#include "main.h"
#include "shader.h"
#include "../../t5.h"
#include "../../configmanager/opengame.h"
#include <string>
#include <cassert>
#include <cstring>

using std::string;

GLint CurrentColorAttrib = -1;
GLint CurrentVertexAttrib = -1;
GLint CurrentTexcoordAttrib = -1;

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

    //printf("Frag Shader:\n%s\n\nVertex Shader:\n%s\n", fragment_text, vertex_text);

    //Build the program
    TS_Shader frag = TS_CreateShader(fragment_text, GL_FRAGMENT_SHADER);
    TS_Shader vert = TS_CreateShader(vertex_text, GL_VERTEX_SHADER);

    TS_Shader prog = TS_CreateProgram(frag, vert);

    if(prog==0)
        fprintf(stderr, BRACKNAME "%s Error: Could not create program from %s.\n", __func__, file);

    T5_close(shaderfile);

    return prog;
}

TS_Shader TS_LoadShader(const char *file, GLuint *fragptr, GLuint *vertptr){

    assert(file!=NULL);

    TS_Directories *TS_Dirs = GetDirs();

    assert(TS_Dirs->shader!=NULL);
    assert(strnlen(TS_Dirs->shader, 8)>0);

    //Concatenate up the filename.

    size_t len1 = strlen(TS_Dirs->shader);
    size_t len2 = len1+strlen(file);

    char *fullfile = (char *)malloc(len2+1);

    fullfile[len2] = '\0';

    memcpy(fullfile, TS_Dirs->shader, len1+1);

    strcat(fullfile, file);

    assert(T5_IsDir(TS_Dirs->shader));
    assert(T5_IsFile(fullfile));

    //Load the specified shader manifest
    T5_file* shaderfile = T5_OpenFile(fullfile);

    assert(shaderfile!=NULL);

    const char *fragmentname = shaderfile->getValue("fragment");
    const char *vertexname = shaderfile->getValue("vertex");

    //Load the filetext of the shaders
    T5_FileText fragment_text   = T5_LoadFileAsText(string(TS_Dirs->shader).append(fragmentname).c_str());
    T5_FileText vertex_text     = T5_LoadFileAsText(string(TS_Dirs->shader).append(vertexname).c_str());

    //Build the program
    TS_Shader frag = TS_CreateShader(fragment_text, GL_FRAGMENT_SHADER);
    TS_Shader vert = TS_CreateShader(vertex_text, GL_VERTEX_SHADER);

    TS_Shader prog = TS_CreateProgram(frag, vert);

    if(prog==0)
        fprintf(stderr, BRACKNAME "%s Error: Could not create program from %s.\n", __func__, file);

    if(fragptr!=NULL)
        *fragptr = frag;
    if(vertptr!=NULL)
        *vertptr = vert;

    T5_close(shaderfile);

    return prog;
}

TS_Shader TS_CreateShader(const char *text, GLenum type, char **error_text){
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
        if(error_text)
            *error_text = log_text;
        else
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

v8Function CreateFragShader(V8ARGS){
    if(args.Length()<1)
        THROWERROR("[" PLUGINNAME "] CreateFragShader Error: Called with fewer than 1 argument.");
    CHECK_ARG_STR(0);

    v8::String::Utf8Value str(args[0]);
    const char *cstr = *str;
    size_t len = strlen(cstr)+1;
    char *source = (char *)malloc(len);
    memcpy(source, cstr, len);

    char *error = NULL;

    TS_Shader frag = TS_CreateShader(source, GL_FRAGMENT_SHADER, &error);
    if(!frag){
        free(source);
        const char *estr = NULL;
        if(error)
            estr = strdup(string("[" PLUGINNAME "] GLSL Fragment Shader Error: ").append(error).c_str());
        else
            estr = "[" PLUGINNAME "] GLSL Fragment Shader Error: Could not compile shader.";

        THROWERROR(estr);
    }

    BEGIN_OBJECT_WRAP_CODE;

    TS_ScriptShader *shader = new TS_ScriptShader();
    shader->name         = frag;
    shader->source       = source;
    shader->used_in_prog = 0;
    shader->type = GL_FRAGMENT_SHADER;

    END_OBJECT_WRAP_CODE_WITH_ID(ScriptShader, shader, ShaderID)

}

v8Function CreateVertShader(V8ARGS){
    if(args.Length()<1)
        THROWERROR("[" PLUGINNAME "] CreateVertShader Error: Called with fewer than 1 argument.");
    CHECK_ARG_STR(0);

    v8::String::Utf8Value str(args[0]);
    const char *cstr = *str;
    size_t len = strlen(cstr)+1;
    char *source = (char *)malloc(len);
    memcpy(source, cstr, len);

    char *error = NULL;

    TS_Shader frag = TS_CreateShader(source, GL_VERTEX_SHADER, &error);
    if(!frag){
        free(source);
        const char *estr = NULL;
        if(error)
            estr = strdup(string("[" PLUGINNAME "] GLSL Vertex Shader Error: ").append(error).c_str());
        else
            estr = "[" PLUGINNAME "] GLSL Vertex Shader Error: Could not compile shader.";

        THROWERROR(estr);
    }

    BEGIN_OBJECT_WRAP_CODE;

    TS_ScriptShader *shader = new TS_ScriptShader();
    shader->name         = frag;
    shader->source       = source;
    shader->used_in_prog = 0;
    shader->type = GL_VERTEX_SHADER;

    END_OBJECT_WRAP_CODE_WITH_ID(ScriptShader, shader, ShaderID);
}

v8Function LoadProgram(V8ARGS){
    if(args.Length()<1)
        THROWERROR("[" PLUGINNAME "] LoadProgram Error: Called with fewer than 1 argument.");
    CHECK_ARG_STR(0);

    v8::String::Utf8Value str(args[0]);
    const char *cstr = *str;

    BEGIN_OBJECT_WRAP_CODE;

    GLuint fragptr = 0;
    GLuint vertptr = 0;

    TS_ShaderProg prog = TS_LoadShader(cstr, &fragptr, &vertptr);
    if(!prog)
        THROWERROR(string("[" PLUGINNAME "] Error: Could not load shader ").append(cstr).c_str());

    TS_ShaderS *shader = new TS_ShaderS();

    TS_ScriptShader *frag = new TS_ScriptShader();
    TS_ScriptShader *vert = new TS_ScriptShader();

    frag->name = fragptr;
    vert->name = vertptr;

    frag->source = NULL;
    vert->source = NULL;

    frag->used_in_prog = 1;
    vert->used_in_prog = 1;

    shader->name = prog;
    shader->fragment = frag;
    shader->vertex = vert;

    shader->colorAttrib = glGetAttribLocation(shader->name, "Color");
    shader->vertexAttrib = glGetAttribLocation(shader->name, "Vertex");
    shader->texcoordAttrib = glGetAttribLocation(shader->name, "Texcoord");

    END_OBJECT_WRAP_CODE_WITH_ID(ScriptShaderProgram, shader, ShaderProgramID);

}

v8Function CreateProgram(V8ARGS){
    if(args.Length()<2)
        THROWERROR("[" PLUGINNAME "] CreateProgram Error: Called with fewer than 2 arguments.");

    for(int i = 0; i<args.Length(); i++){
        CHECK_ARG_TYPE(i, ShaderID, "shader");
    }

    GLuint frag = 0;
    GLuint vert = 0;

    v8::Local<v8::Object> obj0 = v8::Local<v8::Object>::Cast(args[0]);
    TS_ScriptShader *shader0 = (TS_ScriptShader*)obj0->GetAlignedPointerFromInternalField(0);
    v8::Local<v8::Object> obj1 = v8::Local<v8::Object>::Cast(args[1]);
    TS_ScriptShader *shader1 = (TS_ScriptShader*)obj1->GetAlignedPointerFromInternalField(1);

    if(shader1->type==shader0->type){
        if(shader1->type==GL_FRAGMENT_SHADER){
            THROWERROR_TYPE("[" PLUGINNAME "] CreateProgram Error: Requires at least one vertex shader. All shaders are fragment shaders.");
        }
        else if(shader1->type==GL_VERTEX_SHADER){
            THROWERROR_TYPE("[" PLUGINNAME "] CreateProgram Error: Requires at least one vertex shader. All shaders are fragment shaders.");
        }
        else{
            THROWERROR("[" PLUGINNAME "] CreateProgram Internal Error: Both shaders are of (the same) unknown GLenum type. Probably running on unsupported OpenGL version. Possibly heap corruption.");
        }
    }

    if((shader0->type!=GL_FRAGMENT_SHADER)&&(shader0->type!=GL_VERTEX_SHADER))
        THROWERROR("[" PLUGINNAME "] CreateProgram Internal Error: Argument 0 is a Shader of unknown GLenum type. Probably running on unsupported OpenGL version. Possibly heap corruption.");

    if(shader1->type==GL_FRAGMENT_SHADER){
        frag = shader1->name;
        vert = shader0->name;
    }
    else if(shader1->type==GL_VERTEX_SHADER){
        frag = shader0->name;
        vert = shader1->name;
    }
    else{
        THROWERROR("[" PLUGINNAME "] CreateProgram Internal Error: Argument 1 is a Shader of unknown GLenum type. Probably running on unsupported OpenGL version. Possibly heap corruption.");
    }

    BEGIN_OBJECT_WRAP_CODE;

    TS_ShaderProg prog = TS_CreateProgram(frag, vert);

    TS_ShaderS *shader = new TS_ShaderS();
    shader->name = prog;
    shader->colorAttrib = glGetAttribLocation(shader->name, "Color");
    shader->vertexAttrib = glGetAttribLocation(shader->name, "Vertex");
    shader->texcoordAttrib = glGetAttribLocation(shader->name, "Texcoord");
    shader->fragment = (shader0->type==GL_FRAGMENT_SHADER)?shader0:shader1;
    shader->vertex    = (shader0->type==GL_VERTEX_SHADER)?shader0:shader1;

    shader0->used_in_prog++;
    shader1->used_in_prog++;

    END_OBJECT_WRAP_CODE_WITH_ID(ScriptShaderProgram, shader, ShaderProgramID);

}

v8Function UseProgram(V8ARGS){
    if(args.Length()<1)
        THROWERROR("[" PLUGINNAME "] UseProgram Error: Called with fewer than 1 argument.");

    CHECK_ARG_TYPE(0, ShaderProgramID, "shaderProgram");

    v8::Local<v8::Object> shaderobj = v8::Local<v8::Object>::Cast(args[0]);
    TS_ShaderS *shader = (TS_ShaderS*)shaderobj->GetAlignedPointerFromInternalField(0);

    if(!glIsProgram(shader->name)){
        THROWERROR("[" PLUGINNAME "] UseProgram Internal Error: Argument 0 is of type shaderProgram, but does not have a valid OpenGL Shader Program handle. Probably running on unsupported OpenGL version. Possibly heap corruption.");
    }

    glUseProgram(shader->name);

    TS_CurrentShader = shader->name;

    CurrentColorAttrib      = shader->colorAttrib;
    CurrentVertexAttrib     = shader->vertexAttrib;
    CurrentTexcoordAttrib   = shader->texcoordAttrib;

    return v8::Undefined();

}

v8Function UseCompositeProgram(V8ARGS){
    if(args.Length()<1)
        THROWERROR("[" PLUGINNAME "] UseCompositeProgram Error: Called with fewer than 1 argument.");

    CHECK_ARG_TYPE(0, ShaderProgramID, "shaderProgram");

    v8::Local<v8::Object> shaderobj = v8::Local<v8::Object>::Cast(args[0]);
    TS_ShaderS *shader = (TS_ShaderS*)shaderobj->GetAlignedPointerFromInternalField(0);

    if(!glIsProgram(shader->name)){
        THROWERROR("[" PLUGINNAME "] UseCompositeProgram Internal Error: Argument 0 is of type shaderProgram, but does not have a valid OpenGL Shader Program handle. Probably running on unsupported OpenGL version. Possibly heap corruption.");
    }

    TS_CurrentCompositeShader = shader->name;

    CurrentColorAttrib      = shader->colorAttrib;
    CurrentVertexAttrib     = shader->vertexAttrib;
    CurrentTexcoordAttrib   = shader->texcoordAttrib;

    return v8::Undefined();

}

v8Function GetSystemProgram(V8ARGS){
    return v8::Undefined();
}

v8Function GetSystemCompositeProgram(V8ARGS){
    return v8::Undefined();

}

void TS_ScriptShaderFinalizer(V8FINALIZERARGS){
    TS_ScriptShader* ss = (TS_ScriptShader*)parameter;
    if(!ss->used_in_prog)
        glDeleteShader(ss->name);

    free((void *)(ss->source));

    delete ss;
    object->Dispose();
}

void TS_ScriptShaderProgramFinalizer(V8FINALIZERARGS){
    TS_ShaderS* ss = (TS_ShaderS*)parameter;

    if(ss->vertex){
        ss->vertex->used_in_prog--;

        if(!ss->vertex->used_in_prog){
            glDeleteShader(ss->vertex->name);
            free((void *)(ss->vertex->source));
        }
    }

    if(ss->fragment){
        ss->fragment->used_in_prog--;

        if(!ss->fragment->used_in_prog){
            glDeleteShader(ss->fragment->name);
            free((void *)(ss->fragment->source));
        }
    }

    glDeleteProgram(ss->name);

    delete ss;
    object->Dispose();
}
