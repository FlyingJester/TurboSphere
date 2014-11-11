#include "EmbeddedJS.hpp"
#include <openscript.h>

namespace Sapphire {
namespace Embed {

void ExecuteEmbdeddedJS(v8::Isolate *aIso){
    auto lIter = EmbeddedJSSource.begin();
    int i = 0;
    while(lIter!=EmbeddedJSSource.end()){
        ExecuteString(v8::String::NewFromUtf8(aIso, *lIter), v8::String::NewFromUtf8(aIso, (std::string("Embed") + std::to_string(i)).c_str()), aIso, true);
        lIter++;
        i++;
    }
}

const char * const VanillaImageJS =
"/* Create a Sphere 1.x-style Image prototype. */" "\n"
"/* Check if TurboSphere's secret object exists, create it if it doesn't. */" "\n"
""
"if(typeof BlackMamba == \"undefined\")" "\n"
"\t var BlackMamba = {};" "\n"
""
""
""
""
"/* Create the old Image prototype. */" "\n"
""
"BlackMamba.Image = function(surface){" "\n"
"/* Check if the default shader exists, create it if it doesn't. */" "\n"
"\t if(typeof BlackMamba.DefaultShader == \"undefined\")" "\n"
"\t\t BlackMamba.DefaultShader = GetDefaultShaderProgram();" "\n"
"\t surface.width = 32; surface.height = 32;\n"
"\t this.image = new Image(surface);" "\n"
"\t this.shape = new Shape([{x:0, y:0}, {x:surface.width, y:0}, {x:surface.width, y:surface.height}, {x:0, y:surface.height}], this.image);" "\n"
"\t this.group = new Group(this.shape, BlackMamba.DefaultShader);" "\n"
"\t this.width = surface.width; this.height = surface.height;" "\n"
"\t this.group.rotX = this.width>>1;" "\n"
"\t this.group.rotY = this.height>>1;" "\n"
"\t/* Blitting functions: */" "\n"
"\t" "\n"
"\t this.blit = function(x, y){" "\n"
"\t\t this.group.setPosition(x, y);" "\n"
"\t\t this.group.draw();" "\n"
"\t }" "\n"
"\t this.rotateBlit = function(x, y, a){" "\n"
"\t\t this.group.angle = a;" "\n"
"\t\t this.group.setPosition(x, y);" "\n"
"\t\t this.group.draw();" "\n"
"\t\t this.group.angle = 0;" "\n"
"\t }" "\n"
"}" "\n"
"" "\n"
"BlackMamba.LoadImage = function(file){return new BlackMamba.Image(new Surface(file));}" "\n"
"" "\n"
"function LoadImage(file){ return BlackMamba.LoadImage(file);}" "\n"
"" "\n";

const std::array<const char * const, NUM_EMBEDDED_SCRIPTS> EmbeddedJSSource = {VanillaImageJS};

}
}
