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
        
        Sound load(const uint8_t *from, size_t num, int samples_per_second); // Returns a handle
        void play(Sound *sound);
        
        static bool supportsFloat32();
        
    };
    
}
