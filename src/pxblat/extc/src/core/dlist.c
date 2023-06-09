/* dlist.c - Doubly-linked list routines.
 *
 * This file is copyright 2002 Jim Kent, but license is hereby
 * granted for all use - public, private or commercial. */
#include "common.h"
#include "dlist.h"


void dlListInit(struct dlList *dl)
/* Initialize list to be empty */
{
dl->head = (struct dlNode *)(&dl->nullMiddle);
dl->nullMiddle = NULL;
dl->tail = (struct dlNode *)(&dl->head);
}

struct dlList *newDlList()
/* Return a new doubly linked list. */
{
struct dlList *dl;
AllocVar(dl);
dl->head = (struct dlNode *)(&dl->nullMiddle);
dl->tail = (struct dlNode *)(&dl->head);
return dl;
}

void dlListReset(struct dlList *dl)
/* Reset a list to the empty state (does not free values)  */
{
struct dlNode *node, *next;
for (node = dl->head; node->next != NULL; node = next)
    {
    next = node->next;
    freeMem(node);
    }
dl->head = (struct dlNode *)(&dl->nullMiddle);
dl->nullMiddle = NULL;
dl->tail = (struct dlNode *)(&dl->head);
}

void freeDlList(struct dlList **pList)
/* Free up a doubly linked list and it's nodes (but not the node values). */
{
struct dlList *list = *pList;
if (list != NULL)
    {
    dlListReset(list);
    freez(pList);
    }
}

void freeDlListAndVals(struct dlList **pList)
/* Free all values in doubly linked list and the list itself.  (Just calls
 * freeMem on all values. */
{
struct dlList *list = *pList;
if (list != NULL)
    {
    struct dlNode *node;
    for (node = list->head; node->next != NULL; node = node->next)
        freeMem(node->val);
    freeDlList(pList);
    }
}


void dlInsertBetween(struct dlNode *before, struct dlNode *after, struct dlNode *newNode)
{
before->next = newNode;
newNode->prev = before;
newNode->next = after;
after->prev = newNode;
}

void dlAddBefore(struct dlNode *anchor, struct dlNode *newNode)
/* Add a node to list before anchor member. */
{
dlInsertBetween(anchor->prev, anchor, newNode);
}

void dlAddAfter(struct dlNode *anchor, struct dlNode *newNode)
/* Add a node to list after anchor member. */
{
dlInsertBetween(anchor, anchor->next, newNode);
}

void dlAddHead(struct dlList *list, struct dlNode *newNode)
/* Add a node to head of list. */
{
struct dlNode *head = list->head;
dlInsertBetween(head->prev, head, newNode);
}

void dlAddTail(struct dlList *list, struct dlNode *newNode)
/* Add a node to tail of list. */
{
struct dlNode *tail = list->tail;
dlInsertBetween(tail, tail->next, newNode);
}

struct dlNode *dlAddValBefore(struct dlNode *anchor, void *val)
/* Create a node containing val and add to list before anchor member. */
{
struct dlNode *node = AllocA(struct dlNode);
node->val = val;
dlAddBefore(anchor, node);
return node;
}

struct dlNode *dlAddValAfter(struct dlNode *anchor, void *val)
/* Create a node containing val and add to list after anchor member. */
{
struct dlNode *node = AllocA(struct dlNode);
node->val = val;
dlAddAfter(anchor, node);
return node;
}

struct dlNode *dlAddValHead(struct dlList *list, void *val)
/* Create a node containing val and add to head of list. */
{
struct dlNode *node = AllocA(struct dlNode);
node->val = val;
dlAddHead(list, node);
return node;
}

struct dlNode *dlAddValTail(struct dlList *list, void *val)
/* Create a node containing val and add to tail of list. */
{
struct dlNode *node = AllocA(struct dlNode);
node->val = val;
dlAddTail(list, node);
return node;
}

void dlRemove(struct dlNode *node)
/* Removes a node from list. Node is not freed. */
{
struct dlNode *before = node->prev;
struct dlNode *after = node->next;
before->next = after;
after->prev = before;
node->prev = NULL;
node->next = NULL;
}

void dlRemoveHead(struct dlList *list)
/* Removes head from list. Node is not freed. */
{
dlRemove(list->head);
}

void dlRemoveTail(struct dlList *list)
/* Remove tail from list. Node is not freed. */
{
dlRemove(list->tail);
}

struct dlNode *dlPopHead(struct dlList *list)
/* Remove first node from list and return it. */
{
struct dlNode *node = list->head;
if (node->next == NULL)
    return NULL;
dlRemove(node);
return node;
}

struct dlNode *dlPopTail(struct dlList *list)
/* Remove last node from list and return it. */
{
struct dlNode *node = list->tail;
if (node->prev == NULL)
    return NULL;
dlRemove(node);
return node;
}

void dlDelete(struct dlNode **nodePtr)
/* Removes a node from list and frees it. */
{
struct dlNode *node = *nodePtr;
if (node != NULL)
    {
    dlRemove(node);
    freeMem(node);
    }
}

int dlCount(struct dlList *list)
/* Return length of list. */
{
return slCount(list->head) - 1;
}


struct dlSorter
/* Helper structure for sorting dlNodes preserving order */
    {
    struct dlNode *node;
    };

static int (*compareFunc)(const void *elem1, const void *elem2);
/* Node comparison pointer, just used by dlSortNodes and helpers. */

static int dlNodeCmp(const void *elem1, const void *elem2)
/* Compare two dlSorters indirectly, by calling compareFunc. */
{
struct dlSorter *a = (struct dlSorter *)elem1;
struct dlSorter *b = (struct dlSorter *)elem2;

int ret = compareFunc(&a->node->val, &b->node->val);
// printf("dlNodeCmp: ret %d\n", ret);
// return compareFunc(&a->node->val, &b->node->val);
return ret;
}


struct cBlock2
/* A gapless part of a chain. */
    {
    struct cBlock2 *next;	/* Next in list. */
    int tStart,tEnd;		/* Range covered in target. */
    int qStart,qEnd;		/* Range covered in query. */
    int score;	 	 	/* Score of block. */
    void *data;			/* Some associated data pointer. */
    };


struct kdLeaf2
/* A leaf in our kdTree. */
    {
    struct kdLeaf2 *next;	/* Next in list. */
    struct cBlock2 *cb;	        /* Start position and score from user. */
    struct kdBranch *bestPred;	/* Best predecessor. */
    double totalScore;		/* Total score of chain up to here. */
    bool hit;			/* This hit? Used by system internally. */
    };



void print_dlist2(struct dlList *list)
{

struct dlNode *node = list->head;
struct kdLeaf2 *leaf = NULL;

for (; !dlEnd(node); node = node->next)
    {
    leaf = node->val;
    printf("leaf->cb->qStart %d, leaf->cb->tStart %d, leaf->cb->qEnd %d, leaf->cb->tEnd %d, leaf->totalScore %f\n",
           leaf->cb->qStart, leaf->cb->tStart, leaf->cb->qEnd, leaf->cb->tEnd, leaf->totalScore);


    }
}

void print_dlnode(struct dlNode *node)
{

  struct kdLeaf2 *leaf = NULL;
  leaf = node->val;

  // HACK: fix bugs in cpp binding indeed I do not know why <05-20-23>
  char buf[1024];
  sprintf(buf, "leaf->cb->qStart %d, leaf->cb->tStart %d, leaf->cb->qEnd %d, leaf->cb->tEnd %d, leaf->totalScore %f\n",
          leaf->cb->qStart, leaf->cb->tStart, leaf->cb->qEnd, leaf->cb->tEnd, leaf->totalScore);

}

void print_sorter(struct dlSorter *sorter, int len)
{

  for (int i = 0; i < len; i++)
    {
      printf("sorter[%d] \n", i);
      print_dlnode(sorter[i].node);
    }

}


// BLAT compiled on the mac Clang version 11.0.0 build 33.17 (gcc version 4.2.1)
// failed unless compiler optimization is set to level 1 when compiling dlSort().
// Clang pragma supports turning optimization off, but not setting it to a specific level.
#if defined(__clang__)
#pragma clang optimize off
#endif

void dlSort(struct dlList *list,
	int (*compare )(const void *elem1,  const void *elem2))
/* Sort a singly linked list with Qsort and a temporary array.
 * The arguments to the compare function in real, non-void, life
 * are pointers to pointers of the type that is in the val field of
 * the nodes of the list. */
{
int len = dlCount(list);

if (len > 1)
    {
    /* Move val's onto an array, sort, and then put back into list. */
    struct dlSorter *sorter = needLargeMem(len * sizeof(sorter[0])), *s;
    struct dlNode *node;
    int i;

    for (i=0, node = list->head; i<len; ++i, node = node->next)
	{
	s = &sorter[i];
	s->node = node;
  // print_dlnode(node);
	}

    // printf("before sort\n");
    // print_sorter(sorter, len);

    compareFunc = compare;
    qsort(sorter, len, sizeof(sorter[0]), dlNodeCmp);

    // printf("after sort\n");
    // print_sorter(sorter, len);

    dlListInit(list);

    for (i=0; i<len; ++i){
      // printf("\ncurrent list\n");
        // print_dlist2(list);
      // printf("add node\n");
      print_dlnode(sorter[i].node);
	    dlAddTail(list, sorter[i].node);
    }

    freeMem(sorter);
    }
}
#if defined(__clang__)
#pragma clang optimize on
#endif


boolean dlEmpty(struct dlList *list)
/* Return TRUE if list is empty. */
{
return dlIsEmpty(list);
}

struct dlNode *dlGetBeforeHead(struct dlList *list)
/* Get the node before the head of the list */
{
if (dlEmpty(list))
    return list->head;
else
    return list->head->prev;
}

struct dlNode *dlGetAfterTail(struct dlList *list)
/* Get the node after the tail of the list */
{
if (dlEmpty(list))
    return list->tail;
else
    return list->tail->next;
}

void *dlListToSlList(struct dlList *dList)
/* Return slList from dlList. */
{
struct slList *list = NULL, *el;
struct dlNode *node;

for (node = dList->tail; node->prev != NULL; node = node->prev)
    {
    el = node->val;
    slAddHead(&list, el);
    }
return list;
}

void dlCat(struct dlList *a, struct dlList *b)
/* Move items from b to end of a. */
{
struct dlNode *node;
while ((node = dlPopHead(b)) != NULL)
    dlAddTail(a, node);
}

struct dlNode *dlValInList(struct dlList *list, void *val)
/* Return node on list if any that has associated val. */
{
struct dlNode *node;
for (node = list->head; !dlEnd(node); node = node->next)
    if (node->val == val)
        return node;
return NULL;
}
