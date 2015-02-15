#include "formats.h"
#include <stdlib.h>
#include <assert.h>
#include <opus/opus.h>

#define CINNAMON_OPUS_BUFFER_LEN 960*6
#define CINNAMON_OPUS_MAX_PAYLOAD 1600

struct Cinnamon_OpusDecoder{
    int err, rate, channels, was_lost;
    OpusDecoder *decoder;
    uint8_t buffer[CINNAMON_OPUS_BUFFER_LEN];
};

struct Cinnamon_OpusDecoder *Cinnamon_CreateOpusDecoder(int rate, int channels){
    struct Cinnamon_OpusDecoder *ret = 
        malloc(sizeof(struct Cinnamon_OpusDecoder *));
    ret->err = OPUS_OK;
    ret->was_lost = 0;
    
    /* if `rate' is a known efficient audio rate for opus, use it. */
    if( (rate==8000) ||
        (rate==12000) ||
        (rate==16000) ||
        (rate==24000) ||
        (rate==48000))
        ret->rate = rate;
    /* if `rate' is not efficient, use a similar but more efficient rate. */
    else if(rate<10000)
        ret->rate = 8000;
    else if(rate<20000)
        ret->rate = 16000;
    else if(rate<30000)
        ret->rate = 24000;
    else
        ret->rate = 48000;
    
    assert(ret->rate);
    
    ret->channels = channels;
    
    ret->decoder = opus_decoder_create(ret->rate, ret->channels, &ret->err);
    
    if(ret->err!=OPUS_OK){
        free(ret);
        return NULL;
    }
    
    return ret;
}

void Cinnamon_DestroyOpusDecoder(struct Cinnamon_OpusDecoder *d){
    opus_decoder_destroy(d->decoder);
    free(d);
}

/* Based on the opus_demo.c in XIPH's libopus, and opusfile */
int Cinnamon_OpusRead(struct Cinnamon_OpusDecoder *decoder, void **to,
                unsigned long *out_len, const void *from, unsigned long in_len){
    const uint32_t * const data_uint = from;
    int output_samples = 0, was_zero = *out_len==0;
    
    assert(decoder);
    assert(decoder->decoder);
    assert(from);
    
    if(in_len<8)
        return 0;

    if(*out_len){
        const unsigned needed = CINNAMON_OPUS_MAX_PAYLOAD*decoder->channels;
        assert(to);
        assert(*to);
        
        if(*out_len < needed){
            *out_len = needed;
            *to = realloc(*to, *out_len);
        }
    }

    if(data_uint[0]==0)
        opus_decoder_ctl(decoder->decoder, 
            OPUS_GET_LAST_PACKET_DURATION(&output_samples));
    else
        output_samples = CINNAMON_OPUS_BUFFER_LEN;
    
    output_samples = opus_decode(decoder->decoder,
        (data_uint[0]==0)?(NULL):(decoder->buffer), data_uint[0],
        *to, output_samples, 0);

    *out_len = output_samples;

    {
        uint32_t final;
        opus_decoder_ctl(decoder->decoder, OPUS_GET_FINAL_RANGE(&final));

        /* Sanity check */
        if((final!=0) && (data_uint[0]!=0) && (data_uint[1]!=0) && (!decoder->was_lost) && (final!=data_uint[1])){
            if(was_zero){ /* The caller may not handle the output buffer. */
                free(*to);
                *out_len = 0;
            }
            return 0;
        }
    }

    decoder->was_lost = data_uint[0]==0;

    return 1;
    
}
