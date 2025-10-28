/* libavl
 * - manipulates AVL trees.
 *
 * Copyright (c) 2003
 * German Federal Agency for Cartography and Geodesy (BKG)
 *
 * Developed for Networked Transport of RTCM via Internet Protocol (NTRIP)
 * for streaming GNSS data over the Internet.
 *
 * Designed by Informatik Centrum Dortmund http://www.icd.de
 *
 * The BKG disclaims any liability nor responsibility to any person or entity
 * with respect to any loss or damage caused, or alleged to be caused,
 * directly or indirectly by the use and application of the NTRIP technology.
 *
 * For latest information and updates, access:
 * http://igs.ifag.de/index_ntrip.htm
 *
 * Georg Weber
 * BKG, Frankfurt, Germany, June 2003-06-13
 * E-mail: euref-ip@bkg.bund.de
 *
 * Based on the GNU General Public License published Icecast 1.3.12
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#if !avl_h
#define avl_h 1

/* This stack size allows for AVL trees for between 5,704,880 and
   4,294,967,295 nodes, depending on order of insertion.  If you
   increase this it will require recoding some functions that assume
   one long is big enough for a bitmap. */
#ifndef AVL_MAX_HEIGHT
#define AVL_MAX_HEIGHT  32
#endif

#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif

#ifndef __USE_BSD
#define __USE_BSD
#endif

#ifndef __USE_GNU
#define __USE_GNU
#endif


#include "threads.h"

extern void internal_lock_mutex(mutex_t *mutex);
extern void internal_unlock_mutex(mutex_t *mutex);


/* Structure for a node in an AVL tree. */
typedef struct avl_node
  {
    void *data;     /* Pointer to data. */
    struct avl_node *link[2]; /* Subtrees. */
    signed char bal;    /* Balance factor. */
    char cache;     /* Used during insertion. */
    signed char pad[2];   /* Unused.  Reserved for threaded trees. */
  }
avl_node;

/* Used for traversing an AVL tree. */
typedef struct avl_traverser
  {
    int init;     /* Initialized? */
    int nstack;     /* Top of stack. */
    const avl_node *p;    /* Used for traversal. */
    const avl_node *stack[AVL_MAX_HEIGHT];/* Descended trees. */
  }
avl_traverser;

/* Function types. */
#if !AVL_FUNC_TYPES
#define AVL_FUNC_TYPES 1
typedef int (*avl_comparison_func) (const void *a, const void *b, void *param);
typedef void (*avl_node_func) (void *data, void *param);
typedef void *(*avl_copy_func) (void *data, void *param);
#endif

/* Structure which holds information about an AVL tree. */
typedef struct avl_tree
  {
#if PSPP
    struct arena **owner; /* Arena to store nodes. */
#endif
    avl_node root;    /* Tree root node. */
    avl_comparison_func cmp;  /* Used to compare keys. */
    int count;      /* Number of nodes in the tree. */
    void *param;    /* Arbitary user data. */
  mutex_t mutex; /* to protect the tree */
  }
avl_tree;

#if PSPP
#define MAYBE_ARENA struct arena **owner,
#else
#define MAYBE_ARENA /* nothing */
#endif

/* General functions. */
avl_tree *avl_create (MAYBE_ARENA avl_comparison_func, void *param);
avl_tree *avl_create_nl (MAYBE_ARENA avl_comparison_func, void *param);
void avl_destroy (avl_tree *, avl_node_func);
void avl_free (avl_tree *);
int avl_count (const avl_tree *);
avl_tree *avl_copy (MAYBE_ARENA avl_tree *, avl_copy_func);

/* Walk the tree. */
void avl_walk (avl_tree *, avl_node_func, void *param);
void *avl_traverse (avl_tree *, avl_traverser *);

/* Search for a given item. */
void **avl_probe (avl_tree *, void *);
void *avl_delete (avl_tree *, const void *);
void *avl_find ( avl_tree *, const void *);
void *avl_find_close ( avl_tree *, const void *);

#if __GCC__ >= 2
extern inline void *
avl_insert (avl_tree *tree, void *item)
{
  void **p = avl_probe (tree, item);
  return (*p == item) ? NULL : *p;
}

extern inline void *
avl_replace (avl_tree *tree, void *item)
{
  void **p = avl_probe (tree, item);
  if (*p == item)
    return NULL;
  else
    {
      void *r = *p;
      *p = item;
      return r;
    }
}
#else /* not gcc */
void *avl_insert (avl_tree *tree, void *item);
void *avl_replace (avl_tree *tree, void *item);
#endif /* not gcc */

/* Easy assertions on insertion & deletion. */
#ifndef NDEBUG
#define avl_force_insert(A, B)      \
  do          \
    {         \
            void *r = avl_insert (A, B);  \
      assert (r == NULL);     \
    }         \
  while (0)
void *avl_force_delete (avl_tree *, void *);
#else
#define avl_force_insert(A, B)      \
  avl_insert (A, B)
#define avl_force_delete(A, B)      \
  avl_delete (A, B)
#endif

#endif /* avl_h */
