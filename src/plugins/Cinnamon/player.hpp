#pragma once
#include "sound.hpp"
#include <vector>

namespace Cinnamon {
    
    class Player {
        
        void *dev;
        void *context;
    
        // Buffers that have been allocated for this sound, but are not in use or queued at the moment.
        std::vector<unsigned> buffers;
        
        std::vector<uint8_t> buffer;

        uint8_t byte_width;
        int format;
        uint32_t rate;
        
        void makeCurrent() const;
        
    public:
        
        friend class Sound;
        
        Player();
        bool init();
        
        // full_length is the total number of samples that exist in the sound. If this is zero, we assume that 'num' is
        // all the Sound will ever have. This value will only affect querying the length of the sound.
        Sound load(const int16_t *from, size_t num, int num_channels, int samples_per_second, size_t full_length = 0);
        void addToSound(Sound *sound, const int16_t *from, size_t num);
        void addToSound(Sound &sound, const int16_t *from, size_t num){addToSound(&sound, from, num);}
        
        static bool supportsFloat32();
        
    };
    
}
