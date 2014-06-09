#ifndef SAPPHIRE_SURFACE_THREAD_HEAD
#define SAPPHIRE_SURFACE_THREAD_HEAD

/////
// A doubly-linked list with lockless access and deletion.
//
// Written based on the conceptual implementation of concurrent access and
// deletion in Python 2.7, and the linked list implementation in the Linux
// kernel (specifically Kernel version 3.14). Code was taken from neither.
//

struct ListItem{

    void *payload;

    // An atomic, the exact type dependant on if we have C++11 here or not.
    // Only for internal use.
    void *next_atm;
    void *prev_atm;
    void *_ref;

};

#ifdef __cplusplus
extern "C" {
#endif

//Prep a struct ListItem
void CreateListItem(struct ListItem *_l);

void DestroyListItem(struct ListItem *_l);

// Atomically mod the reference variable.
void IncListItemRef(struct ListItem *_l);
void DecListItemRef(struct ListItem *_l);

// Atomically retrieve the reference count of the list item.
unsigned GetListItemRef(struct ListItem *_l);

/////
// Used in GC'ing the surface thread's list.
//
// Mark the list item as near death.
//
// If Force is 1, then the list item will be marked near death
//   even if there were still refs (generally a bad thing).
//
// If Force is 0, then the return value is 0 if the list item
//   was marked near death, or 1 if there were still refs on it.
int InvListItemRef(struct ListItem *_l, int force);

// Atomically set the raw pointers in the list item.
void SetListItemNext(struct ListItem *_l, struct ListItem *set_to);
void SetListItemPrev(struct ListItem *_l, struct ListItem *set_to);

// Atomically get the raw pointers in the list item.
void *GetListItemNext(struct ListItem *_l);
void *GetListItemPrev(struct ListItem *_l);

#ifdef __cplusplus
}
#endif

#endif
