#include "Galileo.hpp"
#include "Shader.hpp"
namespace Sapphire{
namespace Galileo{
    void Init(void *ctx, uint64_t ID){
        Shader::GetDefaultShader(ctx)->Bind();

    }
}
}
