#include "surface_thread.h"
#include <SDL2/SDL.h>
#include <assert.h>

//
//

void CollectListItem(struct ListItem *li){
    assert(li);
    struct ListItem *prev = GetListItemPrev(li);
    struct ListItem *next = GetListItemNext(li);
    if(prev==NULL){
        while(GetListItemRef(li)){
            SDL_Delay(5);
        }
    }

    { // Stitch the list to ensure no new refs to this item.
        // Make the previous item point to the item after this one.
        if(prev)
            SetListItemNext(prev, next);
        // Make the next item point to the item before this one.
        if(next)
            SetListItemPrev(next, prev);
    }

    while(InvListItemRef(li, 0)){}

    DestroyListItem(li);

}
