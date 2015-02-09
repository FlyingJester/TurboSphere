#ifndef USE_XPM
#error
#endif

#include <vector>
#include <array>
#include <t5.h>

namespace Sapphire{
namespace Save{

namespace XPM{

    const int num_xpm_chars = 70;

    std::array<char, num_xpm_chars> chars;

    size_t MapColors(std::vector<uint32_t> &colors, const SDL_Surface *surf){
        uint32_t *pix = reinterpret_cast<uint32_t *>(surf->pixels),
          *pix_end = &(pix[surf->pitch*surf->h]);

        std::for_each(pix, pix_end, [&](const uint32_t &color){
            uint32_t color_new = color|Famask;
            if(std::find(colors.cbegin(), colors.cend(), color_new)==colors.cend())
                colors.push_back(color_new);
        });

        return colors.size();
    }

}


SaveStatus XPMInitFunction(void){

    XPM::chars[0] = '_';
    XPM::chars[1] = '-';
    XPM::chars[2] = '+';
    XPM::chars[3] = '=';
    XPM::chars[4] = '<';
    XPM::chars[5] = '>';
    XPM::chars[6] = '|';
    XPM::chars[7] = '*';

    for(int i = 0; i<10; i++){
        XPM::chars[i+8] = '0' + i;
    }

    for(int i = 0; i<26; i++){
        XPM::chars[i+18] = 'a'+i;
        XPM::chars[i+18+26] = 'A'+i;
    }




    return ssSuccess;
}

SaveStatus XPMSaveFunction(SDL_Surface *aToSave, const std::string &aPath){

    std::vector<uint32_t> colors;

    int num_colors = XPM::MapColors(colors, aToSave);
    int color_length = (num_colors<0xFFFFFFFF)?((num_colors<0xFFFFFF)?((num_colors<0xFFFF)?1:2):3):4;

    const std::string header =  "\"" +
                                std::to_string(aToSave->w)   + " " +
                                std::to_string(aToSave->h)   + " " +
                                std::to_string(num_colors)   + " " +
                                std::to_string(color_length) + " " +
                                std::string("\"\n");

    t5::DataSource *file = t5::DataSource::FromPath(t5::DataSource::eWrite|t5::DataSource::eNewIf, aPath.c_str());
    file->WriteS(header.c_str());

 //   union {uint32_t i; char c[4];} color;
    union {uint32_t i; char c[4];} name; name.i = 0;

    std::for_each(colors.begin(), colors.end(), [&](const uint32_t &in){
        file->Put('"');
//        std::string color_s =
        file->Put('"');
    });


    return ssSuccess;
}



}
}

