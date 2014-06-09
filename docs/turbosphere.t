TurboSphere

NAME
    TurboSphere - A Plugin-Based, JavaScript Game Engine

SYNOPSIS
    turbosphere [gamedir|sgmfile]
    [-v|--version] [-h|--help]
    [-u|--usage] [-8flags|--v8=flags|--v8flags=flags]
    [--gamename=name] [--sgmname=sgm]
    [--gamefunc=game] [--mainscript=script]
    [--screenwidth=w] [--screenheight=h]
    [--compositing=bool] [--fullscreen=bool]
    [--scale=s] [--systemfont=rfn]
    [--systemttffont=ttf] [--systemarrow=imagefile]
    [--systemuparrow=imagefile] [--systemdownarrow=imagefile]
    [--systemsoundfont=sf2]

DESCRIPTION
    TurboSphere runs the Sphere/TurboSphere game located in gamedir, or the
    Sphere sgm or TurboSphere tsgm file specified as sgmfile.

OPTIONS
    -v
    --version
        Print version information and then exit. Information includes V8 info,
        JS info, and Sphere/TurboSphere API information.

    -h
    --help
        Prints basic help information and then exits.

    -u
    --usage
        Prints options and then exits.

    -8 flags
    --v8=flags
    --v8flags=flags
        Set flags to be passed to V8 on startup. These options should only be
        set if you know what you are doing. Most setups will just make all
        games break, or will cause hard crashes. The main use of this option
        is to give access to experimental V8 features, or to try and fix odd
        performance issues in V8. In the latter case, changing this option
        should only ever be used as a last resort.

    --gamename=name
        Set the game's display name. This is mainly useful for debugging.
        This is set in the sgm file by default.

    --sgmname=sgm
        Set the default file name to use as an SGM or TSGM file when
        TurboSphere is passed a directory to run.
        The default value is "game.sgm".

    --gamefunc=game
        Set the name of the function to call once the main script has been
        evaluated.
        The default value is "game".

    --mainscript=script
        Set the main script to be run.
        This is set in the sgm file by default.
    --screenwidth=w
        Set the screen width.
        This is set in the sgm file by default.

    --screenheight=h
        Set the screen height.
        This is set in the sgm file by default.

    --compositing=[y|n]
        Enable or disable screen compositing. Disabling compositing can improve
        performance on some of the older graphics cards that TurboSphere
        supports. Enabling compositing improves the performance of taking
        screenshots, and is necessary for some external programs (such as
        fraps) to capture TurboSphere output without causing hard crashes in
        TurboSphere.
        The default is set in engine.ini. If the value is not specified there,
        it defaults to on.

    --fullscreen=[y|n]
        Force fullscreen on or off.
        The default is set in engine.ini. If the value is not specified there,
        it defaults to off.

    --scale=s
        Set video scaling. On some platforms (OS X, some older graphics cards,
        older versions of configmanager) this value must be power of two.
        Setting this value too high will result terrible issues. If this causes
        hard crashes with values of 8 or more, then there is nothing I can do
        about it.
        A value of 0 will disable video output, if supported by the video
        plugin. If not, a value of 0 is synonymous with a value of 1.
        The default is set in engine.ini. If the value is not specified there,
        it defualts to 1.

    --systemfont=rfn
        Set the system bitmap font.
        The default is set in system.ini. If the value is not specified there,
        it defaults to 'system/system.rfn'.

    --systemttffont=ttf
        Set the system truetype font.
        The default is set in system.ini. If the value is not specified there,
        it defaults to 'system/DejaVuSans.ttf'.

    --systemarrow=imagefile
        Set the system arrow image.
        The default is set in system.ini. If the value is not specified there,
        it defaults to 'system/pointer.png'.

    --systemuparrow=imagefile
        Set the system up arrow image.
        The default is set in system.ini. If the value is not specified there,
        it defaults to 'system/up_arrow.png'.

    --systemdownarrow=imagefile
        Set the system down arrow image.
        The default is set in system.ini. If the value is not specified there,
        it defaults to 'system/down_arrow.png'.

    --systemsoundfont=sf2
        Set the system soundfont to use when rendering MIDI files. It must be
        an SF2 sound font.
        The default is set in system.ini. If the value is not specified there,
        it defaults to the builtin system soundfont (if available). If there is
        no soundfont available on the platform and this value is not set, then
        MIDI playback will (in the standard audio plugin) fail silently.
