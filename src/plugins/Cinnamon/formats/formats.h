#pragma once

#ifdef __cplusplus
extern "C" {
#endif

static int Cinnamon_DefaultRate = 48000;
enum {Cinnamon_NullChannels, Cinnamon_Mono = 1, Cinnamon_Monoraul = 1, Cinnamon_Stereo = 2};

struct Cinnamon_OpusDecoder;
struct Cinnamon_OpusDecoder *Cinnamon_CreateOpusDecoder(int rate, int channels);
void Cinnamon_DestroyOpusDecoder(struct Cinnamon_OpusDecoder *d);
int Cinnamon_OpusRead(struct Cinnamon_OpusDecoder *decoder, void **to, 
    unsigned long *out_len, const void *from, unsigned long in_len);

enum Cinnamon_Formats {BadFormat, OpusFormat};

#ifdef __cplusplus
}
#endif
