
#ifdef USE_OGG

#include <ogg/ogg.h>
#include "ogg.hpp"

#include <algorithm>
#include <cmath>

namespace Cinnamon{

struct OggDemuxer::Implementation{
    ogg_sync_state sync_state;
    ogg_stream_state stream_state;
    ogg_page in_progress;
    uint8_t *remnants;
    size_t rem_len;
};

OggDemuxer::OggDemuxer(){

    // Construct guts
    guts.reset(new Implementation);

    // Init guts
    ogg_sync_init(&(guts->sync_state));
    ogg_stream_reset(&(guts->stream_state));
    guts->rem_len = 0;
    guts->remnants = nullptr;

}

struct Demuxer::packet_info OggDemuxer::Demux(uint8_t* restrict into, size_t len, const uint8_t * restrict from, size_t remaining){

    int err;
    struct packet_info info = {0ull, 0ull, 0ull, false, false, false, false, false};

    // Sooner or later we will need to do this.
    std::fill(into, into+std::min<size_t>(32, len), 0);
    uint8_t *data = reinterpret_cast<uint8_t *>(into)+32;

    // First, finish up any old business.
    if(guts->rem_len>0){

        info.size = std::min<size_t>(guts->rem_len, len);

        if(info.size<=len){
            info.ending = true;
            info.middle = true;
        }
        else{
            info.middle = true;
        }

        std::copy_n(data, info.size, guts->remnants);
        guts->rem_len-=info.size;

    }
    else{
        uint8_t * buffer = reinterpret_cast<uint8_t *>(ogg_sync_buffer(&(guts->sync_state), remaining));

        if(buffer==nullptr){
            //...
            // TODO
        }

        std::copy_n(from, remaining, buffer);
        err = ogg_sync_wrote(&(guts->sync_state), remaining);

        if(err==-1){
            //...
            // TODO
        }

        err = ogg_sync_pageout(&(guts->sync_state), &(guts->in_progress));

        if(err==1){ // Enough data has been fed in, we have output.
            err = ogg_stream_pagein(&(guts->stream_state), &(guts->in_progress));

            if(err==-1){
                //...
                // TODO
            }

            ogg_packet packet;

            err = ogg_stream_packetout(&(guts->stream_state), &packet);
            guts->remnants = packet.packet;

            info.size = std::min<size_t>(packet.bytes, len);
            size_t data_size = info.size;

            if(packet.bytes<=len){
                // All in all is all in all.
                info.ending = true;
                info.middle = true;
                info.beginning = true;
                guts->rem_len = 0;

            }
            else{
                info.beginning = true;
                guts->rem_len = packet.bytes-len;
            }

            info.start_of_stream = packet.b_o_s;
            info.end_of_stream   = packet.e_o_s;
            info.special   = packet.granulepos;
            info.packet_id = packet.packetno;

            std::copy_n(packet.packet, data_size, data);

        }
        else{
            info.size = 0;
        }

    }

    return info;

}


}

#endif // USE_OGG
