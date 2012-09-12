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

#ifndef DLIST_H
#define DLIST_H

/** \addtogroup lib_dlist dual-linked list
	dual-linked list routines

	dual-linked list is used mainly for system objects,
	however, can be used for user lists.

	\ref DLIST is just a header, added to custom structure
	as a first element. Using cast it's possible to use
	any structure as dual-linked lists of custom objects.

	For memory-critical apllications it's possible to use
	dual-linked lists also as a containers for up to 2 bit flags
	(for 32 bit system)
	\{
 */

/*
	circular dual-linked list
 */

#include "types.h"

/*
	dual-linked list type
	aligned function suffix requires dlist items align to sizeof(int)
	aligned dlist usage is a trick to add some flags to list
*/

/**
	\brief unalign \ref DLIST
	\details if \ref DLIST is used as bit-field container, you must call
	this function before any DLIST routines
	\param ptr: DLIST pointer
	\retval: unaligned DLIST
  */
#define UNALIGN_DLIST(ptr)					((DLIST*)(((unsigned int)(ptr)) & ~(WORD_SIZE - 1)))

/** \} */ // end of lib_dlist group

typedef struct _DLIST
{
	struct _DLIST * prev;
	struct _DLIST * next;
}DLIST;

typedef struct
{
	DLIST* start;
	DLIST* cur;
	bool has_more;
} DLIST_ENUM;


void dlist_clear(DLIST** dlist);
bool is_dlist_empty(DLIST** dlist);
//add to the list top, change head pointer
void dlist_add_head(DLIST** dlist, DLIST* entry);
//add to the tail. change only if dlist is empty
void dlist_add_tail(DLIST** dlist, DLIST* entry);
//add before "before". change only if before is first. cannot be called on empty list
void dlist_add_before(DLIST** dlist, DLIST* before, DLIST* entry);
//add after "after". never changed. cannot be called on empty list
void dlist_add_after(DLIST** dlist, DLIST* after, DLIST* entry);
//remove item from head. always change.
void dlist_remove_head(DLIST** dlist);
//remove item from tail. if item is head, change
void dlist_remove_tail(DLIST** dlist);
//remove item. if item is head, change
void dlist_remove(DLIST** dlist, DLIST* entry);

void dlist_next(DLIST** dlist);
void dlist_prev(DLIST** dlist);

void dlist_enum_start(DLIST** dlist, DLIST_ENUM* de);
bool dlist_enum(DLIST_ENUM* de, DLIST** cur);
bool dlist_enum_aligned(DLIST_ENUM* de, DLIST** cur);
void dlist_remove_current_inside_enum(DLIST** dlist, DLIST_ENUM* de, DLIST* cur);
bool is_dlist_contains(DLIST** dlist, DLIST* entry);

#endif // DLIST_H
