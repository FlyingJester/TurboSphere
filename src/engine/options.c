#include "options.h"
#include "version.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <TSPR/noreturn.h>

//Valid short option starters
char option_marks[] = {
    '-',
#ifdef _WIN32
    '/',
#endif
};

TS_NORETURN void print_help(void) {
    printf("TurboSphere\n\n\
This is the placeholder text for the help for TurboSphere. Complain bitterly\n\
\tthat this isn't done yet.\n");
    exit(EXIT_SUCCESS);
}

TS_NORETURN void print_version(void) {
    printf("%s\n", TS_GetLongVersion());
    exit(EXIT_SUCCESS);
}

// Note that this does NOT mean that the option is really a short  option,
// just that it can be interpreted as one. It may in fact be a long option.
inline int IsShort(char *opt) {
    for(size_t e = 0; e<sizeof(option_marks); e++) {
        if(opt[0] == option_marks[e]) {
            return 1;
        }
    }
    return 0;
}

// Returns how far in the actual option starts, or zero if it is not an option.
inline int IsLong(char *opt) {
#ifdef _WIN32
    if(opt[0]=='/')
        return 1;
#endif
    if((opt[0]=='-')&&(opt[1]=='-'))
        return 2;

    return 0;

}

void TS_CMD_ProcessStubbingOptions(int argc, char *argv[]) {
    for(int i = 0; i<argc; i++) {
        if(strlen(argv[i])<=1)
            continue;

        int isshort = IsShort(argv[i]);
        int islong  = IsLong(argv[i]);

        if(isshort) {
            switch(argv[i][1]) {
            case 'v':
                print_version();
#ifdef _WIN32
            case '?':
#endif
            case 'h':
                print_help();
            }
        }

        if(islong) {
            if(!memcmp(argv[i]+2, "version", strlen("version"))) {
                print_version();
            }
            if(!memcmp(argv[i]+2, "help", strlen("help"))) {
                print_help();
            }

        }
    }
}

unsigned TS_CMD_GetNumV8Options(int argc, char *argv[]) {
    int v8args = 0;
    for(int i = 0; i<argc; i++) {

        if(strcmp(argv[i], "--turbo")==0){
            return 1;
        }

        int isshort = IsShort(argv[i]);
        int islong  = IsLong(argv[i]);

        unsigned arglen = strlen(argv[i]);

        if(isshort) {
            if(argv[i][1]=='8') {
                unsigned off = 2;
                while((off<arglen)&&((argv[i][off]==' ')||(argv[i][off]=='\t')
                                     ||(argv[i][off]=='\n')||(argv[i][off]=='"'))) {
                    off++;
                }
                if(arglen==off) {
                    // What? You specify -8, but don't actually pass anything through it?
                    printf("[Engine] Warning: -8 specified on the CMD, but no args passed to V8.\n");
                    return 0;
                }

                int sp = 0;
                while(off<arglen) {
                    if((argv[i][off]==' ')||(argv[i][off]=='\t')||(argv[i][off]=='\n')) {
                        if(!sp) {
                            v8args++;
                            sp = 1;
                        }
                    }
                    else {
                        sp = 0;
                    }
                    off++;
                }
                return v8args;
            }
        }
        else if(islong){
            unsigned off = islong+2;
            if((argv[i][islong]=='v')&&(argv[i][islong+1]=='8')){
                while((off<arglen)&&((argv[i][off]==' ')||(argv[i][off]=='\t')||(argv[i][off]=='\n')))
                    off++;

                if(off==islong+2){ //We haven't moved. We can try checking if the long option was 'v8flags'.
                    if((argv[i][off]=='f')&&(argv[i][off+1]=='l')&&(argv[i][off+2]=='a')&&(argv[i][off+3]=='g')&&(argv[i][off+4]=='s'))
                        off+=5;
                }

                // Check for the equals or set signs
                if((argv[i][off]!='=')&&(argv[i][off]!=':')){
                    printf("[Engine] Warning: Mangled argument \"%s\".\n", argv[i]);
                    return 0; //Mangled argument.
                }

                off++; //For the = or set sign.

                //Trim out whitespace and quoting marks.
                while((off<arglen)&&((argv[i][off]==' ')||(argv[i][off]=='\t')||(argv[i][off]=='\n')||(argv[i][off]=='"')||(argv[i][off]=='\'')))
                    off++;

                int sp = 0;
                while(off<arglen) {
                    if((argv[i][off]==' ')||(argv[i][off]=='\t')||(argv[i][off]=='\n')) {
                        if(!sp) {
                            v8args++;
                            sp = 1;
                        }
                    }
                    else {
                        sp = 0;
                    }
                    off++;
                }
                return v8args;
            }
            else
                return 0;
        }
    }

    return v8args;

}

char *TS_CMD_ProcessV8Options(int argc, char *argv[]) {
    for(int i = 0; i<argc; i++){


        if(strcmp(argv[i], "--turbo")==0){
            char *opt = strdup("--harmony --use-strict --compiled_keyed_generic_loads --turbo_types --turbo_inlining --context_specialization --cpu_profiler_sampling_interval=10 --print_turbo_replay ");
            /*
            char * opt = malloc(strlen("--harmony --use-strict")+1);
            memcpy(opt, " --harmony --use-strict", strlen("--harmony --use-strict")+1);
            */
            printf("[Engine] Info: Using turbo preset options.\n");
            return opt;
        }

        int isshort = IsShort(argv[i]);
        int islong  = IsLong(argv[i]);

        const char *av = NULL;

        if(isshort){
            if(argv[i][1]=='8'){
                av = argv[i]+2;
            }
        }

        if(islong){
            if(!strcmp(argv[i]+islong, "v8"))
                av = argv[i]+islong+strlen("v8");
            else if(!strcmp(argv[i]+islong, "v8flags"))
                av = argv[i]+islong+strlen("v8flags");
        }

        if(av==NULL) // We aren't looking at a v8 flag argument.
            continue;

        // Trim off any whitespace or quote marks or set signs or equals signs.
        // But please, don't make me clean up set signs or equals signs when you use short options.
        while((av[0]==' ')||(av[0]=='\t')||(av[0]=='\n')||(av[0]=='"')||(av[0]=='\'')||(av[0]=='=')||(av[0]==':'))
            av++;

        unsigned avlen = strlen(av);
        //It can't be larger than that, likely one or two bytes smaller
        char *v8args = malloc(avlen+1);//Save room for a NULL.
        memcpy(v8args, av, avlen+1);
        char *v8end = v8args+avlen;

        // Use a NULL to cut off any junk on the end of v8args.
        while((v8end[0]==' ')||(v8end[0]=='\t')||(v8end[0]=='\n')||(v8end[0]=='"')||(v8end[0]=='\'')){
            v8end--;
        }
        ((char *)(v8end+1))[0] = '\0';

        return v8args;
    }
    return NULL;
}
