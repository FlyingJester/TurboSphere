#ifdef USE_VORBIS
#include <vorbis/codec.h>
#include "vorbis.hpp"

struct VorbisDecoder::Implementation{};

VorbisDecoder::VorbisDecoder();

int VorbisDecoder::Decode(uint8_t* restrict into, size_t len, uint8_t* restrict from, size_t remaining){

}

#endif // USE_VORBIS
