#include "sound.hpp"
#include "player.hpp"

#include <cmath>
#include <cassert>

namespace Cinnamon {

Sound::Sound(Sound &&s)
  : player(s.player) 
  , handle(s.handle) 
  , length(s.length) 
  , num_channels(s.num_channels) 
  , samples_per_second(s.samples_per_second) 
  , format(s.format){
	s.handle = 0u;
	setVolume(s.getVolume());
	setLooping(s.getVolume());
}

Sound::Sound(const Sound &s)
  : player(s.player) 
  , handle(s.handle)
  , length(s.length) 
  , num_channels(s.num_channels) 
  , samples_per_second(s.samples_per_second) 
  , format(s.format){
	setVolume(s.getVolume());
	setLooping(s.getVolume());
}

float Sound::getVolume() const {
	return sqrtf(gain);
}

bool Sound::setLooping_(bool loop){
	if(looping!=loop){
		looping = loop;
		return true;
	}
	else return false;
}

bool Sound::setVolume_(float to){
	// Double the epsilon we will really need.
	if(fabs(getVolume()-to)>(1.0f/512.0f)){
		gain = fmin(1.0f, fmax(0.0f, to));
		gain*=gain;
		return true;
	}
	else return false;
}

bool Sound::setPan_(float p){
	if(fabs(getVolume()-p)>(1.0f/512.0f)){
		pan = p;
		return true;
	}
	else return false;
}

}

