#pragma once

namespace Cinnamon {

    class Player;
    
    class Sound{
        Player &player;
        unsigned handle;
        
        void returnBuffers() const;
        Sound(Player &p);
        
        float length, gain;
        unsigned char num_channels;
        unsigned samples_per_second, format;
        bool looping;
        
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
    };
    
}
