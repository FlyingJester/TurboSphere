#include "player.hpp"

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <cassert>

/* Specifics of the DirectSound player:
	
	Buffers are not reused user-side.
	
	We only support 8-bit samples. This is the default for DirectSound.
	
*/

#ifdef _MSC_VER
#define bytes_from_bits(IN) (IN>>3)
#else
constexpr int bytes_from_bits(int in){
	static_assert(in%8==0, "Bits must be divisible by 8");
	return in>>3;
}
#endif

namespace Cinnamon {

Player::Player()
  : dev(nullptr)
  , context(nullptr)
  , format(0){
    
}

void Player::makeCurrent() const {
	static bool already_set = false;
	if(already_set) return;
	already_set = true;
	static_cast<IDirectSound8 *>(dev)->SetCooperativeLevel(GetDesktopWindow(), DSSCL_NORMAL);
}

bool Player::init(){
	if(DirectSoundCreate8(nullptr, (IDirectSound8 **)(&dev), nullptr)!=DS_OK)
        return false;
	
	//DSCAPS caps = {sizeof struct DSCAPS};
	//dsnd->GetCaps(&caps);
	
	//format = caps.dwFlags;
	
    makeCurrent();

    return true;    
}

Player::~Player(){
	free(dev);
}

Sound Player::load(const int16_t *from, size_t num, int num_channels, int samples_per_second, size_t full_length){
	makeCurrent();
	Sound sound(*this);
	addToSound(sound, from, num);
	return sound;
}

Sound Player::load(const int8_t *from, size_t num, int num_channels, int samples_per_second, size_t full_length){
	makeCurrent();
	Sound sound(*this);
	addToSound(sound, from, num);
	return sound;
}


Sound Player::load(const float *from, size_t num, int num_channels, int samples_per_second, size_t full_length){
	assert(false);
	return Sound(*this);
}

template<int bits>
inline LPDIRECTSOUNDBUFFER8 PrepareBuffer(IDirectSound8 *dsnd, Sound *sound, size_t n_samples){
	WAVEFORMATEX format;
	DSBUFFERDESC description;
	LPDIRECTSOUNDBUFFER lBuffer = nullptr;
	LPDIRECTSOUNDBUFFER8 lInterface;
	
	memset(&format, 0, sizeof(WAVEFORMATEX));
	format.wFormatTag = WAVE_FORMAT_PCM; 
	format.nChannels = sound->getChannels(); 
	format.nSamplesPerSec = sound->getSamplesPerSecond(); 
	format.nBlockAlign = 4; 
	format.nAvgBytesPerSec = sound->getSamplesPerSecond()<<2; 
	format.wBitsPerSample = bits;
	
	memset(&description, 0, sizeof(DSBUFFERDESC)); 
	description.dwSize = sizeof(DSBUFFERDESC); 
	description.dwFlags = DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_GLOBALFOCUS; 
	description.dwBufferBytes = n_samples*bytes_from_bits(bits);
	description.lpwfxFormat = &format; 
	
	if(!SUCCEEDED(dsnd->CreateSoundBuffer(&description, &lBuffer, NULL))){
		return nullptr;
	}
	
	HRESULT r = lBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)(&lInterface));
	lBuffer->Release();
	
	return SUCCEEDED(r)?lInterface:nullptr;
}

inline void WriteToBuffer(LPDIRECTSOUNDBUFFER8 buffer, const void *aFrom, size_t num_bytes){
	// All measurements in bytes, not samples
	unsigned char *to;
	const unsigned char *from = reinterpret_cast<const unsigned char*>(aFrom);
	void *vto;
	unsigned written = 0;
	while(written<num_bytes){
		DWORD len;
		buffer->Lock(0, 0, &vto, &len, nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
		to = (unsigned char*)vto;
		const size_t to_write = min(len, num_bytes-written);
		memcpy(to, from+written, to_write);
		buffer->Unlock(to, len, NULL, 0);
		written+=to_write;
	}
}

void Player::addToSound(Sound *sound, const int8_t *from, size_t num){
	WriteToBuffer((LPDIRECTSOUNDBUFFER8)(sound->handle), from, num);
}


void Player::addToSound(Sound *sound, const int16_t *from, size_t num){
	WriteToBuffer((LPDIRECTSOUNDBUFFER8)(sound->handle), from, num<<1);
}


void Player::addToSound(Sound *sound, const float *from, size_t num){
	assert(false);
}

bool Player::supportsFloat32(){
    return false;
}

bool Player::supportsInt16(){
	return true;
}

bool Player::supportsInt8(){
	return true;
}

/////
// Sound object
Sound::Sound(Player &p)
  : player(p) {
	setLooping(false);
	
}

/////
// Sound object

Sound::~Sound(){

}

void Sound::play() const{
	((LPDIRECTSOUNDBUFFER8)(handle))->Play(0, 0, looping?DSBPLAY_LOOPING:0);
}

void Sound::pause() const{
	stop();
}

void Sound::stop() const{
	((LPDIRECTSOUNDBUFFER8)(handle))->Stop();
}

void Sound::rewind() const{
	stop();
	play();
}

void Sound::setVolume(float to){
	static_assert(DSBVOLUME_MAX > DSBVOLUME_MIN, "MAX is less than MIN. Fix me!");
	if(setVolume_(to)){
		const LONG new_volume = to*static_cast<float>(DSBVOLUME_MAX - DSBVOLUME_MIN) + DSBVOLUME_MIN;
		((LPDIRECTSOUNDBUFFER8)(handle))->SetVolume(new_volume);
	}
}

void Sound::setLooping(bool loop){
	if(setLooping_(loop)){
		
	}
}

void Sound::setPan(float p){
	
	static_assert((DSBPAN_LEFT<0) && (DSBPAN_RIGHT>0), "Left and right should be swapped. Fix me!");

	if(setPan_(p)){
		if(p<0.0f){
			((LPDIRECTSOUNDBUFFER8)(handle))->SetPan(-p*DSBPAN_LEFT);
		}
		else if(p>0.0f){
			((LPDIRECTSOUNDBUFFER8)(handle))->SetPan(p*DSBPAN_RIGHT);
		}
		else ((LPDIRECTSOUNDBUFFER8)(handle))->SetPan(0);
	}
}

void Sound::returnBuffers() const{

}

}