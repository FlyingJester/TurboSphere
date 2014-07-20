#include "Shader.hpp"
#include <cstdio>

#include <cassert>
#ifdef OS_X
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif // OS_X

#include <screen.h>

namespace Sapphire {

  namespace embed {
    static const char * const FragShaderSource = "\
    #version 410\n\
    \
    in vec4 color_v;\n\
    in vec2 tex_v;\n\
    out vec4 fragColour;\n\
    \
    uniform sampler2D texDat;\n\
    \
    void main(void)\n\
    {\n\
        fragColour = color_v * texture(texDat, tex_v);\n\
    }\
    ";

    static const char * const VertexShaderSource = "\
    #version 410\n\
    \
    uniform vec2 TS_Offset;\n\
    \
    in vec2 TS_TextureUV;\n\
    in vec4 TS_Position;\n\
    in vec4 TS_Color;\n\
    \
    out vec4 color_v;\n\
    out vec2 tex_v;\n\
    \
    uniform vec2 TS_ScreenSize;\n\
    \
    void main (void)\n\
    {\n\
        tex_v = TS_TextureUV;\n\
        color_v = TS_Color;\n\
        vec4 TS_NewPos = TS_Position*2.0;\n\
        gl_Position = (vec4(TS_Offset.x*2.0, TS_Offset.y*(-2.0), 0.0, 0.0) +(vec4(TS_NewPos.x - TS_ScreenSize.x, -TS_NewPos.y + (TS_ScreenSize.y), TS_Position.ba)))/vec4(TS_ScreenSize, 1.0, 1.0);\n\
    }\
    ";
  }

namespace SDL_GL {


GLuint CreateShader(const char *text, GLenum type){
    if(text==NULL){
        printf("[Shader] Error: Emtpy string given.\n");
        return 0;
    }
    GLuint shader = glCreateShader(type);
    if(shader==0){
        fprintf(stderr, "[Shader] Error: Something went terribly wrong, the shader index was 0.\n");
    }
    GLsizei tsize = (GLsizei)strlen(text)+2;

    glShaderSource(shader, 1, &text, &tsize);

    glCompileShader(shader);

    GLint shader_status;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_status);

    if (shader_status==GL_FALSE) {
        printf("[Shader] Error: Failed to compile shader.\n");

        GLint log_size;
        GLint written = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
        char *log_text = new char[log_size];
        glGetShaderInfoLog(shader, log_size, &written, log_text);
        if(written==0)
            printf("[Shader] Error: No log was written.\n");
        if(written>log_size)
            printf("[Shader] Error: Your OpenGL driver just wrote past the end of my buffer. I told it not to, really!\n");
        printf("%s\n", log_text);
        delete[] log_text;
        glDeleteShader(shader);

        return 0;
    }
    printf("[Shader] Info: Shader compiled ok. ID number %i.\n", shader);
    return shader;

}

GLuint CreateProgram(GLuint frag, GLuint vert){
    if((glIsShader(frag)==GL_FALSE)||(glIsShader(vert)==GL_FALSE)){
        fprintf(stderr, "[Shader] Error: One or more shader was invalid\n\tFrag %s\tVert %s\n",
        (glIsShader(frag)==GL_TRUE)?"good":"bad", (glIsShader(vert)==GL_TRUE)?"good":"bad");
        return 0;
    }

    GLint program_status;

    GLint fragstat, vertstat;

    glGetShaderiv(frag, GL_SHADER_TYPE, &fragstat);

    if(fragstat!=GL_FRAGMENT_SHADER)
        fprintf(stderr, "[Shader] Error: Invalid fragment shader.\n");

    glGetShaderiv(vert, GL_SHADER_TYPE, &vertstat);

    if(vertstat!=GL_VERTEX_SHADER)
        fprintf(stderr, "[Shader] Error: Invalid vertex shader.\n");

    if((fragstat!=GL_FRAGMENT_SHADER)||(vertstat!=GL_VERTEX_SHADER)){
        fprintf(stderr, "[Shader] Error: Bad shader(s). Exiting.\n");
        return 0;

    }

    GLuint prog = glCreateProgram();

    glAttachShader(prog, frag);
    glAttachShader(prog, vert);
    fprintf(stderr, "[Shader] Info: Linking Program.\n");
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &program_status);

    if(!program_status){
        fprintf(stderr, "[Shader] Error: Could not link program.\n");
        GLint log_size;
        char *log_text;

        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_size);
        log_text = new char[log_size+1];
        glGetProgramInfoLog(prog, log_size, NULL, log_text);
        fprintf(stderr, "%s\n", log_text);
        delete[] log_text;
        glDeleteProgram(prog);

        return 0;

    }
    fprintf(stderr, "[Shader] Info: Program linked ok.\n");
    return prog;

}

GLuint DefaultFragShader(void){
    return CreateShader(embed::FragShaderSource, GL_FRAGMENT_SHADER);
}

GLuint DefaultVertShader(void){
    return CreateShader(embed::VertexShaderSource, GL_VERTEX_SHADER);
}

GLuint CreateDefaultProgram(void){
    return CreateProgram(DefaultFragShader(), DefaultVertShader());
}

}


namespace Galileo{


ShaderParamChange::ShaderParamChange()
  : mLocation(-1)
  , mCount(0)
  , mData(nullptr)
  , mCallback(nullptr)
{
}

ShaderParamChange::ShaderParamChange(int aLocation, size_t aCount, const void *aData,
                      callback_t aCallback, size_t s)
  : mLocation(aLocation)
  , mCount(aCount)
  , mData(malloc(mCount))
  , mCallback(aCallback)
{
    memcpy(mData, aData, mCount*s);
}

ShaderParamChange::~ShaderParamChange(){
    free(mData);
}

const std::string Shader::ShaderPositionName = "TS_Position";
const std::string Shader::ShaderTextureUVName= "TS_TextureUV";
const std::string Shader::ShaderColorName    = "TS_Color";
const std::string Shader::ShaderOffsetUniformName    = "TS_Offset";
const std::string Shader::ShaderScreenSizeUniformName    = "TS_ScreenSize";
__thread std::vector<int> *Shader::EnabledAttributes = nullptr;
__thread Shader *Shader::BoundShader = nullptr;

Shader *Shader::GetDefaultShader(void){

    GLuint lDefaultProg = SDL_GL::CreateDefaultProgram();
    if((lDefaultProg)!=0){

        Shader *lShader = new Shader(lDefaultProg);

        lShader->AddAttribute(ShaderPositionName);
        lShader->AddAttribute(ShaderTextureUVName);
        lShader->AddAttribute(ShaderColorName);
        lShader->AddUniform(ShaderOffsetUniformName);

        return lShader;
    }
    else
      return nullptr;
}

Shader::Shader(int aProgram)
  : mProgram(aProgram) {
    mAttributes = GLSLValueMap();
    mUniforms   = GLSLValueMap();

    glUseProgram(mProgram);
    // Add the default uniforms and attribs, if the shader defines them.
    AddUniform(ShaderOffsetUniformName);
    AddUniform(ShaderScreenSizeUniformName);
    AddAttribute(ShaderPositionName);
    AddAttribute(ShaderTextureUVName);
    AddAttribute(ShaderColorName);

    glUniform2f(mUniforms[ShaderScreenSizeUniformName], GetScreenWidth(), GetScreenHeight());

}

Shader::~Shader() {}

/*
// Map of Vertex Attribute names and their locations.
GLSLValueMap mAttributes;
// Map of Uniform names and their locations.
GLSLValueMap mUniforms;
*/
// Checks for the specified name in the shader program.
// Returns whether or not the name exists.
// If it exists, it will be added to the appropriate map.
bool Shader::AddUniform(const std::string &aName){
    int lLocation = glGetUniformLocation(mProgram, aName.c_str());
    assert(lLocation>=0);
    if((lLocation)>=0){
        mUniforms[aName.c_str()] = lLocation;
        return true;
    }
    else
        return false;
}

bool Shader::AddAttribute(const std::string &aName){
    int lLocation = glGetAttribLocation(mProgram, aName.c_str());
    assert(lLocation>=0);
    if((lLocation)>=0){
        mAttributes[aName.c_str()] = lLocation;
        mAttributeNumbers.push_back(lLocation);
        return true;
    }
    else
        return false;
}

void Shader::Bind(void){

    assert(mProgram);
    glUseProgram(mProgram);

    //for(auto lIter = EnabledAttributes->begin(); lIter!=EnabledAttributes->end(); lIter++){
    //    glDisableVertexAttribArray(*lIter);
   // }

    //EnabledAttributes->clear();

    for(auto lIter = mAttributes.begin(); lIter!=mAttributes.end(); lIter++){
        assert(lIter->second>=0);
        glEnableVertexAttribArray(lIter->second);
        //EnabledAttributes->push_back(lIter->second);
    }

    /*

    // We must initialize our thread-local enabled attribute vector if it is still null.
    // We also can take some decisive action if we know this is the first run.
    if(EnabledAttributes==nullptr){
        EnabledAttributes = new std::vector<int> ();

        for(auto lIter = mAttributes.begin(); lIter!=mAttributes.end(); lIter++){
            glEnableVertexAttribArray(lIter->second);
        }

        EnabledAttributes = &mAttributeNumbers;

        return;
    }

    std::vector<int> lNew; //New vertex attributes that need to be added.

    // Activate any vertex attribute locations that weren't activated for the previous shader.
    for(auto lIter = mAttributes.begin(); lIter!=mAttributes.end(); lIter++){
        auto lMatch = std::find(EnabledAttributes->begin(), EnabledAttributes->end(), lIter->second);
        if(lMatch==EnabledAttributes->end()){
            glEnableVertexAttribArray(lIter->second);
            lNew.push_back(lIter->second);
        }
    }

    // Deactivate and erase any attibs that are no longer used.
    for(auto lIter = EnabledAttributes->rbegin(); lIter!=EnabledAttributes->rend(); lIter++){

        auto lMatch = std::find_if(mAttributes.begin(), mAttributes.end(),
          [&] (const std::pair<std::string, int> &aIter) { return aIter.second == (*lIter); } );

        if(lMatch==mAttributes.end()){
            glDisableVertexAttribArray(*lIter);
            EnabledAttributes->erase(lIter.base());
            continue;
        }
    }

    // Push all new required attributes into Enabled Attributes.
    EnabledAttributes->insert(EnabledAttributes->rbegin().base(), lNew.begin(), lNew.end());
    */
    BoundShader = this;

}

}
}
