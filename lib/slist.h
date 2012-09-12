/*
	M-Kernel - embedded RTOS
	Copyright (c) 2011-2012, Alexey Kramarenko
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
		list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the documentation
		and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SLIST_H
#define SLIST_H

/** \addtogroup lib_slist single-linked list
	single-linked list routines

	single-linked list is used mainly for system objects,
	however, can be used for user lists.

	\ref SLIST is just a header, added to custom structure
	as a first element. Using cast it's possible to use
	any structure as single-linked lists of custom objects.

	\{
 */

#include "dbg.h"

typedef struct _SLIST
{
	struct _SLIST * next;
}SLIST;

/**
	\brief add item after existing
	\param after: pointer to item, after which we need to add
	\param item: pointer to item to add
	\retval: none
  */
static inline void slist_add_after(SLIST* after, SLIST* item)
{
	ASSERT(after);
	ASSERT(item);
	item->next = after->next;
	after->next = item;
}

/**
	\brief add head item
	\param list: address of list
	\param item: pointer to item to add
	\retval: none
  */
static inline void slist_add_head(SLIST** list, SLIST* item)
{
	ASSERT(item);
	item->next = (*list);
	*list = item;
}

/**
	\brief remove after item
	\param after: pointer to item, after which we need to add
	\retval: none
  */
static inline void slist_remove_after(SLIST* after)
{
	ASSERT(after);
	ASSERT(after->next);
	after->next = after->next->next;
}

/**
	\brief remove head item
	\param list: address of list
	\retval: none
  */
static inline void slist_remove_head(SLIST** list)
{
	ASSERT(*list);
	*list =(*list)->next;
}

/** \} */ // end of lib_slist group

#endif // SLIST_H

