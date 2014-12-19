#include <string>
#include <functional>
#include <limits>
#include <cctype>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include "libapi.hpp"
#include "demuxer/raw.hpp"
#include "decoder/raw.hpp"

const int SampleFrequency = 441000;

struct WaveState{

    const enum t {sine, sawtooth, square} type;

    const unsigned samples_per_second;
    const Cinnamon::Player *player;
    const unsigned frequency;
    unsigned long long length; // in samples.

    float i; // Ending phase.

    unsigned long long at; // Number of samples

};

template<typename T>
T clamp(T in){
    return in*std::numeric_limits<T>::max();
}

template<> float clamp<float>(float in){return std::max(-1.0f, std::min(1.0f, in));}
template<> double clamp<double>(double in){return std::max(-1.0, std::min(1.0, in));}
template<> long double clamp< long double>( long double in){return std::max<long double>(-1.0, std::min<long double>(1.0, in));}

template<typename T>
int SineWaveWriter(void *into, size_t len, void *arg){
    struct WaveState *state = static_cast<struct WaveState *>(arg);

    assert(sizeof(T)==state->player->ByteWidth());

    T *data = static_cast<T *>(into);

    int i = 0;
    while(i<len/sizeof(T)){
        data[i++] = clamp<T>(std::sin(state->i));
        state->i += (state->frequency*M_PI)/(state->samples_per_second);
        while(state->i>M_PI) state->i -= M_PI;
    }

    state->at+=i;

    return i*sizeof(T);
}

int main(int argc, char *argv[]){
    if((argc<3)||(strcmp(argv[1], "-h")==0)||(strcmp(argv[1], "--help")==0)){
        puts("usage: <frequency> <length>\n\nFrequency in Hertz, Length in seconds.");
        return EXIT_FAILURE;
    }

    unsigned frequency = strtol(argv[1], 0, 10), length = strtol(argv[2], 0, 10);

    WaveState::t type = WaveState::sine;

    if(argc>3){
        std::string str = argv[3];
        std::transform(str.cbegin(), str.cend(), str.begin(), tolower);

        if((str==std::string("sine"))||str==std::string("sin")){
            type = WaveState::sine;
        }
        else if(str==std::string("square")){
            type = WaveState::square;
        }
        else if((str==std::string("saw"))||str==std::string("sawtooth")){
            type = WaveState::sawtooth;
        }

    }

    std::unique_ptr<Cinnamon::Stream> wave_sound(new Cinnamon::Stream(new Cinnamon::RawDemuxer(), new Cinnamon::RawDecoder(), Cinnamon::Player::CreateNativePlayer(Cinnamon::sFloat, 4, SampleFrequency)));

    struct WaveState state = {type, SampleFrequency, wave_sound->GetPlayer(), frequency, length*SampleFrequency, 0};

    Cinnamon::load_function func = nullptr;

    switch(wave_sound->GetPlayer()->Style()){
    case Cinnamon::sSigned:
        if(wave_sound->GetPlayer()->ByteWidth()==1)
            func = SineWaveWriter<int8_t>;
        else if(wave_sound->GetPlayer()->ByteWidth()==2)
            func = SineWaveWriter<int16_t>;
        else if(wave_sound->GetPlayer()->ByteWidth()==4)
            func = SineWaveWriter<int32_t>;
        else if(wave_sound->GetPlayer()->ByteWidth()==8)
            func = SineWaveWriter<int64_t>;
    break;
    case Cinnamon::sUnsigned:
        if(wave_sound->GetPlayer()->ByteWidth()==1)
            func = SineWaveWriter<uint8_t>;
        else if(wave_sound->GetPlayer()->ByteWidth()==2)
            func = SineWaveWriter<uint16_t>;
        else if(wave_sound->GetPlayer()->ByteWidth()==4)
            func = SineWaveWriter<uint32_t>;
        else if(wave_sound->GetPlayer()->ByteWidth()==8)
            func = SineWaveWriter<uint64_t>;
    break;
    case Cinnamon::sFloating:
        if(wave_sound->GetPlayer()->ByteWidth()==4)
            func = SineWaveWriter<float>;
        else if(wave_sound->GetPlayer()->ByteWidth()==8)
            func = SineWaveWriter<double>;
        else if(wave_sound->GetPlayer()->ByteWidth()==10)
            func = SineWaveWriter<long double>;
    break;
    case Cinnamon::sFixed:
    break;
    }

    assert(func);

    wave_sound->SetLoadCallback(func);
    wave_sound->SetLoadArg(&state);

    wave_sound->Load();
    wave_sound->Play();
    while(state.at<state.length){

        wave_sound->Load();

    }

    printf("Put %llu\n", state.at);


    do{
    }while(getc(stdin)!='q');

}
