#pragma once
#include "sound.hpp"
#include <vector>
#include <cstdint>

namespace Cinnamon {
    
    class Player {
        
        void *dev;
        void *context;
    
        // Buffers that have been allocated for this sound, but are not in use or queued at the moment.
        std::vector<uintptr_t> buffers;
        
        std::vector<uint8_t> buffer;

        uint8_t byte_width;
        int format;
        uint32_t rate;
        
        void makeCurrent() const;
        
		void addBuffers();
		void ensureBuffers();
		
    public:
        
        friend class Sound;
        
        Player();
		~Player();
		
        bool init();
        
        // full_length is the total number of samples that exist in the sound. If this is zero, we assume that 'num' is
        // all the Sound will ever have. This value will only affect querying the length of the sound.
		//
		// The Player::supports* must return true to use the corresponding load function.
        Sound load(const int16_t *from, size_t num, int num_channels, int samples_per_second, size_t full_length = 0);
        Sound load(const int8_t *from, size_t num, int num_channels, int samples_per_second, size_t full_length = 0);
        Sound load(const float *from, size_t num, int num_channels, int samples_per_second, size_t full_length = 0);
		
		// The Player::supports* must return true to use the corresponding add function.
        void addToSound(Sound *sound, const int16_t *from, size_t num);
        void addToSound(Sound *sound, const int8_t *from, size_t num);
        void addToSound(Sound *sound, const float *from, size_t num);
		
		template<typename T>
		void addToSound(Sound &sound, const T *from, size_t num){addToSound(&sound, from, num);}
		
		// The player is guaranteed to support either Int16 or Int8.
		// Float32 will not be exposed unless it is preferred.
        static bool supportsFloat32();
		static bool supportsInt16();
		static bool supportsInt8();
        
    };
    
}
