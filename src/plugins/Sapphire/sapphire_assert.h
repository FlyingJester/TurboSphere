#ifndef SAPPHIRE_ASSERT_HEAD
#define SAPPHIRE_ASSERT_HEAD

#include <stdbool.h>

#define Sapphire_Assert(EXP){\
        int msg_box_err = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Sapphire Assertion Failed", "Assertion " #EXP " failed at line " __LINE__ ", " __FILE__".", NULL);\
        assert(!msg_box_err);\
        abort();\
}

#endif
