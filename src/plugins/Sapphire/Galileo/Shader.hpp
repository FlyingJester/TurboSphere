#pragma once

#include <string>
#include <vector>
#include <map>
#include <color.h>

namespace Sapphire {
namespace Galileo{


class Shader {

public:
  static const std::string ShaderPositionName;
  static const std::string ShaderTextureUVName;
  static const std::string ShaderColorName;
  static const std::string ShaderOffsetUniformName;
  typedef std::map<std::string, int> GLSLValueMap;
protected:
    int mProgram;
public:

    Shader(int aProgram);
    ~Shader();

    // Map of Vertex Attribute names and their locations.
    GLSLValueMap mAttributes;
    // Map of Uniform names and their locations.
    GLSLValueMap mUniforms;

    void Bind(void);

    int GetName(){ return mProgram; }

};


}
}
