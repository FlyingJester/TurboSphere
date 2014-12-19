#pragma once
#include <vector>
#include <string>

#ifdef __GNUC__
#define restrict __restrict__
#elif defined _MSC_VER
#define restrict __restrict
#else
#define restrict
#endif

namespace Cinnamon{


    enum error_t     : uint8_t {eSuccess, eFailure, eEndOfStream};
    enum format_t    : uint8_t {fAuto, fRaw, fVorbis};
    enum container_t : uint8_t {cAuto, cRaw, cOgg};
    enum style       : uint8_t {sFloating, sFloat = 0, sFixed, sSigned, sUnsigned};

    class Player {
    public:

        // These values are just requests. To get the actual results, use Style and ByteWidth.
        static Player *CreateNativePlayer(style type, int byte_width, int sample_per_second);

        virtual style Style()     const {return sFloating;}
        virtual int   ByteWidth() const {return 4;}

        virtual error_t Load(const uint8_t  *from, size_t num, int samples_per_second) = 0;
        virtual error_t Play() = 0;
        virtual error_t Stop() = 0;
        virtual error_t Pause() = 0;
        virtual error_t Restart() = 0;

        virtual const char * const ExplainError() const = 0;
    };

    class Demuxer {
    public:

        static bool SupportsContainer(container_t container);
        static bool SupportsMime(const std::string &mime);

        static Demuxer *ForContainer(container_t container);
        static Demuxer *ForMime(const std::string &mime);
        struct packet_info {size_t size; uint64_t packet_id, special; bool ending, middle, beginning, start_of_stream, end_of_stream;};
        virtual struct packet_info Demux(uint8_t *restrict into, size_t len, const uint8_t *restrict from, size_t remaining) = 0;
    };

    class Decoder {
    public:

        // Returns fAuto if it can't tell.
        static format_t WhichFormat(const uint8_t *to_test, size_t len);

        static bool SupportsFormat(format_t format);
        static bool SupportsMime(const std::string &mime);

        static Decoder *ForFormat(format_t format);
        static Decoder *ForMime(const std::string &mime);

        struct packet_info {size_t size; style type; uint32_t samples_per_second; uint8_t byte_width; uint64_t packet_id, special;};
        virtual struct packet_info Decode(uint8_t *restrict into, size_t len, const uint8_t *restrict from, size_t remaining) = 0;
    };

    typedef int(*load_function)(void *into, size_t len, void *arg);

    class Audio {
    protected:

        load_function load_func;
        void *load_callback_arg;
        load_function filter_func;
        void *filter_callback_arg;

    public:

        error_t SetLoadCallback(load_function cb){load_func = cb; return eSuccess;}
        error_t SetLoadArg(void *a){load_callback_arg = a; return eSuccess;}
        load_function GetLoadCallback() const {return load_func;}
        void *GetLoadArg() const {return load_callback_arg;}

        error_t SetFilterCallback(load_function cb){filter_func = cb; return eSuccess;}
        error_t SetFilterArg(void *a){filter_callback_arg = a; return eSuccess;}
        load_function GetFilterCallback() const {return filter_func;}
        void *GetFilterArg() const {return filter_callback_arg;}

        virtual error_t Load() = 0;
        virtual error_t Play() = 0;

    };

    class Stream : public Audio {
        std::vector<unsigned char> load_buffer;
        std::vector<unsigned char> demux_buffer;
        std::unique_ptr<Player> player;
        std::unique_ptr<Demuxer> demuxer;
        std::unique_ptr<Decoder> decoder;
    public:

        // Passing in nullptr results in an attempt to autodetect.
        Stream(Demuxer *aDemuxer, Decoder *aDecoder, Player *aPlayer);

        const Player *GetPlayer(){return player.get();}
        const Demuxer *GetDemuxer(){return demuxer.get();}
        const Decoder *GetDecoder(){return decoder.get();}

        error_t Load() override;
        error_t Play() override;

    };



}
