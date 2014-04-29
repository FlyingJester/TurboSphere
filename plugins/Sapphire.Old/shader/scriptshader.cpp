#include "shader.h"
#include "../loadfunc/gl_extra.h"
#include "../frontend.h"
#include <cstdlib>
#include <cstring>
#include <string>

using std::string;

#ifdef _MSC_VER
#define STRDUP _STRDUP
#else
#define STRDUP strdup
#endif

Turbo::JSObj<TS_ScriptShader> ShaderObject;
Turbo::JSObj<TS_ShaderS> ShaderProgramObject;

void TS_ScriptShaderFinalizer(const v8::WeakCallbackData<v8::Object, TS_ScriptShader> &args);
void TS_ScriptShaderProgramFinalizer(const v8::WeakCallbackData<v8::Object, TS_ShaderS> &args);

void InitShader(uint32_t ID){
    ShaderObject                 = Turbo::JSObj<TS_ScriptShader>();
    ShaderObject.Finalize        = TS_ScriptShaderFinalizer;
    ShaderProgramObject          = Turbo::JSObj<TS_ShaderS>();
    ShaderProgramObject.Finalize = TS_ScriptShaderProgramFinalizer;
    ShaderObject.ID              = (ID<<16)|(0xBE11u);
    ShaderProgramObject.ID       = (ID<<16)|(0xF00Du);
}

Turbo::JSFunction CreateFragShader(Turbo::JSArguments args){

    int sig[] = {Turbo::String, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig))
        return;

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
            estr = STRDUP(string("[" PLUGINNAME "] GLSL Fragment Shader Error: ").append(error).c_str());
        else
            estr = STRDUP("[" PLUGINNAME "] GLSL Fragment Shader Error: Could not compile shader.");

        Turbo::SetError(args, estr);
        return;
    }

    TS_ScriptShader *shader = new TS_ScriptShader();
    shader->name         = frag;
    shader->source       = source;
    shader->used_in_prog = 0;
    shader->type = GL_FRAGMENT_SHADER;

    Turbo::WrapObject(args, ShaderObject,shader);

}

Turbo::JSFunction CreateVertShader(Turbo::JSArguments args){

    int sig[] = {Turbo::String, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig))
        return;

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
            estr = STRDUP(string("[" PLUGINNAME "] GLSL Vertex Shader Error: ").append(error).c_str());
        else
            estr = STRDUP("[" PLUGINNAME "] GLSL Vertex Shader Error: Could not compile shader.");

        Turbo::SetError(args, estr);
        return;
    }

    TS_ScriptShader *shader = new TS_ScriptShader();
    shader->name         = frag;
    shader->source       = source;
    shader->used_in_prog = 0;
    shader->type = GL_VERTEX_SHADER;

    Turbo::WrapObject(args, ShaderObject,shader);
}

Turbo::JSFunction LoadProgram(Turbo::JSArguments args){

    int sig[] = {Turbo::String, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig))
        return;

    v8::String::Utf8Value str(args[0]);
    const char *cstr = *str;

    GLuint fragptr = 0;
    GLuint vertptr = 0;

    TS_ShaderProg prog = TS_LoadShader(cstr, &fragptr, &vertptr);
    if(!prog){
        Turbo::SetError(args, STRDUP(string("[" PLUGINNAME "] Error: Could not load shader ").append(cstr).c_str()));
        return;
    }

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

    Turbo::WrapObject(args, ShaderProgramObject, shader);
}

Turbo::JSFunction CreateProgram(Turbo::JSArguments args){

    if(args.Length()<2){
        Turbo::SetError(args, STRDUP("[" PLUGINNAME "] CreateProgram Error: Called with fewer than 2 arguments."));
        return;
    }

    for(int i = 0; i<args.Length(); i++){
        if(!ShaderObject.IsA(args[1])){
            Turbo::SetError(args, STRDUP((std::string("[" PLUGINNAME "] CreateProgram Error: Argument ")+std::to_string(i)+std::string(" is not a Shader.")).c_str()), v8::Exception::TypeError);
            return;
        }
    }

    GLuint frag = 0;
    GLuint vert = 0;

    v8::Local<v8::Object> obj0 = v8::Local<v8::Object>::Cast(args[0]);
    TS_ScriptShader *shader0 = (TS_ScriptShader*)obj0->GetAlignedPointerFromInternalField(0);
    v8::Local<v8::Object> obj1 = v8::Local<v8::Object>::Cast(args[1]);
    TS_ScriptShader *shader1 = (TS_ScriptShader*)obj1->GetAlignedPointerFromInternalField(1);

    if(shader1->type==shader0->type){
        if(shader1->type==GL_FRAGMENT_SHADER){

            Turbo::SetError(args, STRDUP("[" PLUGINNAME "] CreateProgram Error: Requires at least one vertex shader. All shaders are fragment shaders."), v8::Exception::TypeError);

            //THROWERROR_TYPE("[" PLUGINNAME "] CreateProgram Error: Requires at least one vertex shader. All shaders are fragment shaders.");
        }
        else if(shader1->type==GL_VERTEX_SHADER){
            Turbo::SetError(args, STRDUP("[" PLUGINNAME "] CreateProgram Error: Requires at least one vertex shader. All shaders are fragment shaders."), v8::Exception::TypeError);
        }
        else{
            Turbo::SetError(args, STRDUP("[" PLUGINNAME "] CreateProgram Internal Error: Both shaders are of (the same) unknown GLenum type. Probably running on unsupported OpenGL version. Possibly heap corruption."));
        }
        return;
    }

    if((shader0->type!=GL_FRAGMENT_SHADER)&&(shader0->type!=GL_VERTEX_SHADER)){
        Turbo::SetError(args, STRDUP("[" PLUGINNAME "] CreateProgram Internal Error: Argument 0 is a Shader of unknown GLenum type. Probably running on unsupported OpenGL version. Possibly heap corruption."));
        return;
    }

    if(shader1->type==GL_FRAGMENT_SHADER){
        frag = shader1->name;
        vert = shader0->name;
    }
    else if(shader1->type==GL_VERTEX_SHADER){
        frag = shader0->name;
        vert = shader1->name;
    }
    else{
        Turbo::SetError(args, STRDUP("[" PLUGINNAME "] CreateProgram Internal Error: Argument 1 is a Shader of unknown GLenum type. Probably running on unsupported OpenGL version. Possibly heap corruption."));
        return;
    }

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

    Turbo::WrapObject(args, ShaderProgramObject, shader);
//    END_OBJECT_WRAP_CODE_WITH_ID(ScriptShaderProgram, shader, ShaderProgramID);

}

Turbo::JSFunction UseProgram(Turbo::JSArguments args){
    if(args.Length()<1){
        Turbo::SetError(args, STRDUP("[" PLUGINNAME "] UseProgram Error: Called with no arguments."));
        return;
    }

    if(!ShaderProgramObject.IsA(v8::Handle<v8::Object>::Cast(args[0]))){
        Turbo::SetError(args, STRDUP(std::string("[" PLUGINNAME "] UseProgram Error: Argument 0 is not a ShaderProgram.").c_str()), v8::Exception::TypeError);
        return;
    }
/*
    if(args.Length()<1)
        THROWERROR("[" PLUGINNAME "] UseProgram Error: Called with fewer than 1 argument.");

    CHECK_ARG_TYPE(0, ShaderProgramID, "shaderProgram");
*/
    v8::Local<v8::Object> shaderobj = v8::Local<v8::Object>::Cast(args[0]);
    TS_ShaderS *shader = (TS_ShaderS*)shaderobj->GetAlignedPointerFromInternalField(0);

    if(!glIsProgram(shader->name)){
        Turbo::SetError(args, STRDUP(std::string(("[" PLUGINNAME "] UseProgram Internal Error: Argument 0 is of type shaderProgram, but does not have a valid OpenGL Shader Program handle. Probably running on unsupported OpenGL version. Possibly heap corruption.")).c_str()), v8::Exception::TypeError);
        return;
    }

    glUseProgram(shader->name);

    TS_CurrentShader = shader->name;

    if((CurrentColorAttrib!=shader->colorAttrib)&&(CurrentColorAttrib!=shader->texcoordAttrib)&&(CurrentColorAttrib!=shader->vertexAttrib))
        glDisableVertexAttribArray(CurrentColorAttrib);

    if((CurrentVertexAttrib!=shader->colorAttrib)&&(CurrentVertexAttrib!=shader->texcoordAttrib)&&(CurrentVertexAttrib!=shader->vertexAttrib))
        glDisableVertexAttribArray(CurrentVertexAttrib);

    if((CurrentTexcoordAttrib!=shader->colorAttrib)&&(CurrentTexcoordAttrib!=shader->texcoordAttrib)&&(CurrentTexcoordAttrib!=shader->vertexAttrib))
        glDisableVertexAttribArray(CurrentTexcoordAttrib);

    CurrentColorAttrib      = shader->colorAttrib;
    CurrentVertexAttrib     = shader->vertexAttrib;
    CurrentTexcoordAttrib   = shader->texcoordAttrib;

    glEnableVertexAttribArray(CurrentColorAttrib);
    glEnableVertexAttribArray(CurrentVertexAttrib);
    glEnableVertexAttribArray(CurrentTexcoordAttrib);


}

Turbo::JSFunction UseCompositeProgram(Turbo::JSArguments args){

    if(args.Length()<1){
        Turbo::SetError(args, STRDUP("[" PLUGINNAME "] UseCompositeProgram Error: Called with no arguments."));
        return;
    }

    if(!ShaderProgramObject.IsA(args[0])){
        Turbo::SetError(args, STRDUP(std::string("[" PLUGINNAME "] UseCompositeProgram Error: Argument 0 is not a ShaderProgram.").c_str()), v8::Exception::TypeError);
        return;
    }

    v8::Local<v8::Object> shaderobj = v8::Local<v8::Object>::Cast(args[0]);
    TS_ShaderS *shader = (TS_ShaderS*)shaderobj->GetAlignedPointerFromInternalField(0);

    if(!glIsProgram(shader->name)){
        Turbo::SetError(args, STRDUP(std::string(("[" PLUGINNAME "] UseCompositeProgram Internal Error: Argument 0 is of type shaderProgram, but does not have a valid OpenGL Shader Program handle. Probably running on unsupported OpenGL version. Possibly heap corruption.")).c_str()), v8::Exception::TypeError);
        return;
    }

    TS_CurrentCompositeShader = shader->name;

    CurrentColorAttrib      = shader->colorAttrib;
    CurrentVertexAttrib     = shader->vertexAttrib;
    CurrentTexcoordAttrib   = shader->texcoordAttrib;
}

Turbo::JSFunction GetSystemProgram(Turbo::JSArguments args){
}

Turbo::JSFunction GetSystemCompositeProgram(Turbo::JSArguments args){

}

void TS_ScriptShaderFinalizer(const v8::WeakCallbackData<v8::Object, TS_ScriptShader> &args){
    //TS_ScriptShader* ss = (TS_ScriptShader*)parameter;
    if(!args.GetParameter()->used_in_prog)
        glDeleteShader(args.GetParameter()->name);

    free((void *)(args.GetParameter()->source));

    delete args.GetParameter();
    args.GetValue().Clear();
}

void TS_ScriptShaderProgramFinalizer(const v8::WeakCallbackData<v8::Object, TS_ShaderS> &args){
    //TS_ShaderS* ss = (TS_ShaderS*)parameter;

    if(args.GetParameter()->vertex){
        args.GetParameter()->vertex->used_in_prog--;

        if(!args.GetParameter()->vertex->used_in_prog){
            glDeleteShader(args.GetParameter()->vertex->name);
            free((void *)(args.GetParameter()->vertex->source));
        }
    }

    if(args.GetParameter()->fragment){
        args.GetParameter()->fragment->used_in_prog--;

        if(!args.GetParameter()->fragment->used_in_prog){
            glDeleteShader(args.GetParameter()->fragment->name);
            free((void *)(args.GetParameter()->fragment->source));
        }
    }

    glDeleteProgram(args.GetParameter()->name);

    delete args.GetParameter();
    args.GetValue().Clear();
}
