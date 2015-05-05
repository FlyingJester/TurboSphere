#pragma once
#include <cstdint>

namespace Cinnamon {

    class Player;
    
    class Sound{
        Player &player;
        uintptr_t handle;
        
        void returnBuffers() const;
        Sound(Player &p);
        
        float length, gain, pan;
        unsigned char num_channels;
        unsigned samples_per_second, format;
        bool looping;
        
        bool setLooping_(bool loop);
        bool setVolume_(float to);
		bool setPan_(float p);
		
    public:
        
        friend class Player;
        
        Sound(Sound &&s);
        Sound(const Sound &s);
        ~Sound();
        void setLooping(bool loop);
        bool getLooping() const {return looping;}
        
        float getLength() const {return length;}
        
        void play() const;
        void pause() const;
        void stop() const;
        void rewind() const;
        void setVolume(float to);
        float getVolume() const;
        float getPan() const {return pan;}
        void setPan(float p);
		
		unsigned getChannels() const {return num_channels;}
		unsigned getSamplesPerSecond() const {return samples_per_second;}
		
    };
    
}
