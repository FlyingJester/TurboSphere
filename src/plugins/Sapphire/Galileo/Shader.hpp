#pragma once

#include <string>
#include <vector>
#include <utility>
#include <color.h>
#include <opengame.h>
#include "Shape.hpp"

#ifdef OS_X

#ifdef OPENGL_2
#include <OpenGL/gl.h>
#elif defined(OPENGL_4) || defined(OPENGL_3)
#include <OpenGL/gl3.h>
#endif

#else
#include <GL/gl.h>
#endif

namespace Sapphire {
namespace Galileo{

class ShaderParamChange : public Sapphire::Galileo::GL::Operation {
public:
    typedef void (*callback_t)(GLint, GLsizei, const void *);
protected:
    int mLocation;
    size_t mCount;
    void *mData;
    callback_t mCallback;

public:
    ShaderParamChange();
    ShaderParamChange(int aLocation, size_t aCount, const void *aData,
                      callback_t aCallback, size_t s);
    virtual ~ShaderParamChange();

    virtual int Draw(){
        mCallback(mLocation, mCount, mData);
        return 0;
    }

    virtual bool IsPersistent(void){
        return false;
    }


};

class Shader {
public:
    enum shader_type {fragment = GL_FRAGMENT_SHADER, vertex = GL_VERTEX_SHADER, geometry = GL_GEOMETRY_SHADER};
private:
    const std::string mSource;
    const shader_type mType;
    int mHandle;

public:
    Shader(const std::string &str, shader_type t)
      : mSource(str)
      , mType(t)
      , mHandle(-1){
      
    }
    ~Shader();
        
    bool Compile(std::string &err);

    static Shader *GetDefault(void *ctx, shader_type t);

    int GetHandle() const { return mHandle; }
    shader_type GetType() const { return mType; }

};

class ShaderProgram : public Sapphire::Galileo::GL::Operation {
    std::vector<std::shared_ptr<Shader> > shaders;
public:
    static const std::string ShaderPositionName;
    static const std::string ShaderTextureUVName;
    static const std::string ShaderColorName;
    static const std::string ShaderOffsetUniformName;
    static const std::string ShaderRotOffsetUniformName;
    static const std::string ShaderAngleUniformName;
    static const std::string ShaderScreenSizeUniformName;
  
    struct value_double { const std::string str; int a; };
    typedef std::vector<struct value_double> GLSLValueMap;
    //typedef std::map<std::string, int> GLSLValueMap;
protected:
    int mProgram;
    std::vector<int> mAttributeNumbers;
public:

    static ShaderProgram *GetDefault(void *ctx);

    // Note that because this takes a preset program handle, you do NOT need to call link().
    ShaderProgram(TS_GameConfig *config, int aProgram);
    // You MUST call link when using this.
    // All shaders in shaderv must already be compiled.
    ShaderProgram(TS_GameConfig *config, unsigned shaderc, std::shared_ptr<Shader> shaderv[]);
    ~ShaderProgram();

    bool Link(std::string &err);

    // Map of Vertex Attribute names and their locations.
    GLSLValueMap mAttributes;
    // Map of Uniform names and their locations.
    GLSLValueMap mUniforms;

    // Checks for the specified name in the shader program.
    // Returns whether or not the name exists.
    // If it exists, it will be added to the appropriate map.
    bool AddUniform(const std::string &aName);
    bool AddAttribute(const std::string &aName);

    inline int FindUniform(const std::string &a){
        for(GLSLValueMap::const_iterator i = mUniforms.cbegin(); i!=mUniforms.cend(); i++){
            if(i->str==a) return i->a;
        }
        return -1;
    }
    
    inline int FindAttribute(const std::string &a){
        for(GLSLValueMap::const_iterator i = mAttributes.cbegin(); i!=mAttributes.cend(); i++){
            if(i->str==a) return i->a;
        }
        return -1;
    }
    
    

    void Bind(void);

    virtual int Draw(){
        Bind();
        return 0;
    }

    int GetName(){ return mProgram; }

#ifdef _MSC_VER
	__declspec( thread ) static ShaderProgram *BoundShader;
#else
    static __thread ShaderProgram *BoundShader;
#endif

};


}
}
