#include "player.hpp"

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alext.h>
#include <AL/alc.h>
#endif
#include <cassert>

/* Specifics of the OpenAL player:
	
	Buffers are managed user-side
	
	We only support 16-bit currently. A nice todo would be to add 8-bit and float32 support.
	Currently the addToSound functions for these formats are intentionally failed.
	
*/

namespace Cinnamon{

/////
// Player object.

void Player::makeCurrent() const{
    assert(context);
    alcMakeContextCurrent(static_cast<ALCcontext *>(context));
}

Player::Player()
  : dev(nullptr)
  , context(nullptr)
  , format(AL_FORMAT_STEREO16){
    
}

Player::~Player(){

}

bool Player::init(){
    dev = alcOpenDevice(nullptr);
    if(!dev)
        return false;

    context = alcCreateContext(static_cast<ALCdevice *>(dev), nullptr);
    if(!context)
        return false;

    makeCurrent();

    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_ORIENTATION, 0.0f, 0.0f, 0.0f);
    
    return true;
    
}

Sound Player::load(const int16_t *from, size_t num, int num_channels, int samples_per_second, size_t full_length){
    
    makeCurrent();
    
    Sound sound(*this);
    sound.num_channels = num_channels;
    sound.samples_per_second = samples_per_second;

    sound.length = static_cast<float>((full_length==0)?num:full_length)/samples_per_second;  
    
    switch(num_channels){
        case 1:
            sound.format = AL_FORMAT_MONO16;
            break;
        case 2:
            sound.format = AL_FORMAT_STEREO16;
            break;
        default:
            sound.format = format;
    }
    
    addToSound(sound, from, num);
    
    sound.gain = 1.0f;
    
    return sound;
}

Sound Player::load(const int8_t *from, size_t num, int num_channels, int samples_per_second, size_t full_length){
	assert(false);
}

Sound Player::load(const float *from, size_t num, int num_channels, int samples_per_second, size_t full_length){
	assert(false);
}

void Player::addToSound(Sound *sound, const int16_t *from, size_t num){

    makeCurrent();

    if(buffers.empty())
        sound->returnBuffers();

    if(buffers.empty()){
        const unsigned num_new_buffers = 8;
        
        ALuint new_buffers[num_new_buffers];
        alGenBuffers(num_new_buffers, new_buffers);
        
        for(unsigned i = 0; i<num_new_buffers; i++)
            buffers.push_back(new_buffers[i]);
    }

    assert(!buffers.empty());

    alBufferData(buffers.back(), sound->format, from, num, sound->samples_per_second);

    ALuint b = buffers.back();
    buffers.pop_back();

    alSourceQueueBuffers(sound->handle, 1, &b);
}

void  Player::addToSound(Sound *sound, const int8_t *from, size_t num){
	assert(false);
	
	int16_t *to = new int16_t[num];
	for(int i = 0; i<num; i++){
		const int32_t t = (from[i]*0xFFFF)/0xFF;
		to[i] = t;
	}
	addToSound(sound, to, num);
	delete[] to;
}

void  Player::addToSound(Sound *sound, const float *from, size_t num){
	assert(false);
	
	int16_t *to = new int16_t[num];
	for(int i = 0; i<num; i++){
		to[i] = from[i]*(0xFFFFFF);
	}
	addToSound(sound, to, num);
	delete[] to;
}

bool Player::supportsFloat32(){
    return false && alIsExtensionPresent("AL_EXT_FLOAT32");
}

bool Player::supportsInt16(){
	return true;
}

bool Player::supportsInt8(){
	return false;
}

/////
// Sound object
Sound::Sound(Player &p)
  : player(p) {
    player.makeCurrent();
    ALuint h;
    alGenSources(1, &h);
    handle = h;
    alSourcef(handle, AL_PITCH, 1.0f);
    alSourcef(handle, AL_GAIN, 1.0f);
    alSource3f(handle, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(handle, AL_VELOCITY, 0.0f, 0.0f, 0.0f);

    setLooping(false);
}


Sound::~Sound(){
    returnBuffers();
    player.makeCurrent();
    ALuint h = handle;
    alDeleteSources(1, &h);
}

void Sound::play() const{
    alSourcePlay(handle);
}

void Sound::pause() const{
    alSourcePause(handle);
}

void Sound::stop() const{
    alSourceStop(handle);
}

void Sound::rewind() const{
    alSourceRewind(handle);
}

void Sound::setVolume(float to){
    if(setVolume_(to))
        alSourcef(handle, AL_GAIN, gain);
}

void Sound::setLooping(bool loop){
    if(setLooping_(loop))
        alSourcei(handle, AL_LOOPING, loop?AL_TRUE:AL_FALSE);
}

void Sound::returnBuffers() const{
    ALint num_buffers;
    player.makeCurrent();
	
    alGetSourcei(handle, AL_BUFFERS_PROCESSED, &num_buffers);
    if(num_buffers!=0){
        player.buffers.resize(player.buffers.size()+num_buffers);
       
        for(int i = 0; i<num_buffers; i++){
            ALuint b;
            alSourceUnqueueBuffers(handle, 1, &b);
            (player.buffers.data()-i)[0] = b;
        }
        
    }
}

void Sound::setPan(float p){
    setPan_(p);
}

}
