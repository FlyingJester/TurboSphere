#include "Shader.hpp"

namespace Sapphire {
namespace Galileo{

    Shader *Shader::GetDefaultShader(void *ctx){
        return new Shader(nullptr, 0);
    }

    Shader::Shader(TS_GameConfig *config, int aProgram){
    
    }
    
    Shader::~Shader(){}

    void Shader::Bind(void){}

}
}
