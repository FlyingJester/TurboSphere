#pragma once

namespace Cinnamon {

    class Player;
    
    class Sound{
        Player &player;
        unsigned handle;
        
        void returnBuffers() const;
        Sound(Player &p);
        
        float length;
        
    public:
        
        friend class Player;
        
        Sound(Sound &&s);
        Sound(const Sound &s);
        ~Sound();
        void setLooping(bool loop);
        
        float getLength(){return length;}
        
    };
    
}