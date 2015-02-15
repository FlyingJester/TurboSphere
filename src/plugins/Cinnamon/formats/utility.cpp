#include "utility.hpp"
#include <array>
#include <cctype>

namespace Cinnamon {

    
static bool CompareExtStrings(const std::string &that, const char * const ext){
    const unsigned ext_len = strlen(ext);
    
    if(that.size()!=ext_len)
        return false;
    
    for(unsigned i = 0; i<ext_len; i++){
        if(tolower(that[i])!=tolower(ext[i]))
            return false;
    }
    return true;
}

struct format_pair{const char * const ext; enum Cinnamon_Formats format;};

typedef std::array<struct format_pair, 1> extensions_array_t;

const extensions_array_t format_extensions = {{
    {"opus", OpusFormat}
}};

enum Cinnamon_Formats FormatFromExtension(const std::string &that){
    
    for(extensions_array_t::const_iterator i = format_extensions.cbegin(); i!=format_extensions.cend(); i++)
        if(CompareExtStrings(that, i->ext)) return i->format;
    
    return BadFormat;
}

}
