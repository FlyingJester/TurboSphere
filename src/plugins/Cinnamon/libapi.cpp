
#include <cassert>
#include "libapi.hpp"
#include "player/openal.hpp"

namespace Cinnamon {

bool SupportsContainer(container_t container){
    switch (container) {
    case cAuto:
        return true;
    case cRaw:
    #ifdef USE_RAW
        return true;
    #else
        return false;
    #endif
    case cOgg:
    #ifdef USE_OGG
        return true;
    #else
        return false;
    #endif
    default:
        return false;
    }
}
#undef CASE_DEF


bool SupportsMime(const std::string &mime){

#ifdef USE_RAW
    if((mime==std::string("audio/L8")||(mime==std::string("audio/L16"))||(mime==std::string("audio/L32")))
        return true;
#endif
#ifdef USE_OGG
    if(mime==std::string("audio/ogg"))
       return true;
#endif

    return false;
}

Demuxer *Demuxer::ForContainer(container_t container){
    return nullptr;
}

Demuxer *Demuxer::ForMime(const std::string &mime){
    return nullptr;
}

bool Decoder::SupportsFormat(format_t format){
    switch (format) {
    case fAuto:
        return true;
    case fRaw:
    #ifdef USE_RAW
        return true;
    #else
        return false;
    #endif
    case fVorbis:
    #ifdef USE_VORBIS
        return true;
    #else
        return false;
    #endif
    default:
        return false;
    }
}

bool Decoder::SupportsMime(const std::string &mime){

#ifdef USE_RAW
    if(std::string((mime=="audio/L8"))||(mime==std::string("audio/L16"))||(mime==std::string("audio/L32")))
        return true;
#endif
#ifdef USE_VORBIS
    if(mime==std::string("audio/vorbis"))
        return true;
#ifdef USE_OGG
    // It is evil we got here at all. But if we have both Ogg and Vorbis, I suppose we support audio/ogg.
    if(mime==std::string("audio/ogg"))
       return true;
#endif
#endif
    return false;
}

Decoder *Decoder::ForFormat(format_t format){
    return nullptr;
}

Decoder *Decoder::ForMime(const std::string &mime){
    return nullptr;
}

format_t Decoder::WhichFormat(const uint8_t *to_test, size_t len){

    if((to_test[0]==1) && (strncmp(reinterpret_cast<const char *>(to_test), "vorbis", len)==0))
        return fVorbis;

    return fAuto;

}

Stream::Stream(Demuxer *aDemuxer, Decoder *aDecoder, Player *aPlayer){

    player.reset(aPlayer);
    demuxer.reset(aDemuxer);
    decoder.reset(aDecoder);

    load_buffer.resize(0xFFFF);
    demux_buffer.resize(0xFFFF);

}

error_t Stream::Play() {return player->Play();}

error_t Stream::Load() {

    assert(GetLoadCallback());

    // Load some data.
    // Unlike most stages, this MUST yield some output.
    int num_loaded = GetLoadCallback()(load_buffer.data(), load_buffer.size(), GetLoadArg());
    if(num_loaded<0) return eFailure;
    if(num_loaded<1) return eEndOfStream;

    // Demux it.
    Demuxer::packet_info dempacket = demuxer->Demux(demux_buffer.data(), demux_buffer.size(), load_buffer.data(), num_loaded);

    // Decode it.
    Decoder::packet_info decpacket = decoder->Decode(load_buffer.data(), load_buffer.size(), demux_buffer.data(), dempacket.size);

    // Load it.
    player->Load(load_buffer.data(), decpacket.size, decpacket.samples_per_second);

    return eSuccess;
}

Player *Player::CreateNativePlayer(style type, int byte_width, int samples_per_second){

    #if (defined OS_X) || (defined __APPLE__)
        return new OpenALPlayer(type, byte_width, samples_per_second);
    #endif

}

}
