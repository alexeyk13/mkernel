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

/** \addtogroup lib_dlist dual-linked list
	\{
 */

#include "dlist.h"
#include "dbg.h"

/**
	\brief clear list
	\param dlist: address of list
	\retval: none
  */
void dlist_clear(DLIST** dlist)
{
	*dlist = NULL;
}

/**
	\brief check, if \b DLIST is empty
	\param dlist: address of list
	\retval: \b true if empty
  */
bool is_dlist_empty(DLIST** dlist)
{
	return *dlist == NULL;
}

/**
	\brief add item to \b DLIST head
	\param dlist: address of list
	\param entry: pointer to object to add
	\retval: none
  */
void dlist_add_head(DLIST** dlist, DLIST* entry)
{
	if (*dlist == NULL)
		entry->next = entry->prev = entry;
	else
	{
		entry->next = *dlist;
		entry->prev = (*dlist)->prev;
		(*dlist)->prev->next = entry;
		(*dlist)->prev = entry;
	}
	*dlist = entry;
}

/**
	\brief add item to \b DLIST tail
	\param dlist: address of list
	\param entry: pointer to object to add
	\retval: none
  */
void dlist_add_tail(DLIST** dlist, DLIST* entry)
{
	if (*dlist == NULL)
	{
		*dlist = entry;
		entry->next = entry->prev = entry;
	}
	else
	{
		entry->next = *dlist;
		entry->prev = (*dlist)->prev;
		(*dlist)->prev->next = entry;
		(*dlist)->prev = entry;
	}
}

/**
	\brief add item \b DLIST before existing item
	\param dlist: address of list
	\param before: pointer to existing object
	\param entry: pointer to object to add
	\retval: none
  */
void dlist_add_before(DLIST** dlist, DLIST* before, DLIST* entry)
{
	ASSERT(*dlist);
	ASSERT(before);
	if (before == *dlist)
		dlist_add_head(dlist, entry);
	else
	{
		entry->next = before;
		entry->prev = before->prev;
		before->prev->next = entry;
		before->prev = entry;
	}
}

/**
	\brief add item \b DLIST after existing item
	\param dlist: address of list
	\param after: pointer to existing object
	\param entry: pointer to object to add
	\retval: none
  */
void dlist_add_after(DLIST** dlist, DLIST* after, DLIST* entry)
{
	ASSERT(*dlist);
	ASSERT(after);

	entry->next = after->next;
	entry->prev = after;
	after->next->prev = entry;
	after->next = entry;
}

/**
	\brief remove head item from \b DLIST
	\param dlist: address of list
	\retval: none
  */
void dlist_remove_head(DLIST** dlist)
{
	ASSERT(*dlist);
	if((*dlist)->next == *dlist)
		*dlist = NULL;
	else
	{
		(*dlist)->prev->next = (*dlist)->next;
		(*dlist)->next->prev = (*dlist)->prev;
		*dlist = (*dlist)->next;
	}
}

/**
	\brief remove tail item from \b DLIST
	\param dlist: address of list
	\retval: none
  */
void dlist_remove_tail(DLIST** dlist)
{
	ASSERT(*dlist);
	if((*dlist)->next == *dlist)
		*dlist = NULL;
	else
	{
		(*dlist)->prev->prev->next = (*dlist);
		(*dlist)->prev = (*dlist)->prev->prev;
	}
}

/**
	\brief remove item from \b DLIST
	\details \b DLIST must contain item
	\param dlist: address of list
	\param entry: pointer to object to add
	\retval: none
  */
void dlist_remove(DLIST** dlist, DLIST* entry)
{
	if ((*dlist) == entry)
		dlist_remove_head(dlist);
	else
	{
		DLIST* head;
		head = *dlist;
		(*dlist) = entry;
		dlist_remove_head(dlist);
		(*dlist) = head;
	}
}

/**
	\brief go next
	\param dlist: address of list
	\retval: none
  */
void dlist_next(DLIST** dlist)
{
	if (!(*dlist == NULL))
		(*dlist) = (*dlist)->next;
}

/**
	\brief go previous
	\param dlist: address of list
	\retval: none
  */
void dlist_prev(DLIST** dlist)
{
	if (!(*dlist == NULL))
		(*dlist) = (*dlist)->prev;
}

/**
	\brief start enumeration
	\param dlist: address of list
	\param de: pointer to existing \ref DLIST_ENUM structure
	\retval: none
  */
void dlist_enum_start(DLIST** dlist, DLIST_ENUM* de)
{
	de->start = *dlist;
	de->cur = *dlist;
	de->has_more = !(*dlist == NULL);
}

/**
	\brief get next item
	\details enumeration must be prepared by \ref dlist_enum_start
	\param de: pointer to initialized \ref DLIST_ENUM structure
	\param cur: address of pointer to current item holder
	\retval: \b true if success, \b false after last item was fetched
  */
bool dlist_enum(DLIST_ENUM* de, DLIST** cur)
{
	if (!(de->has_more))
		return false;
	(*cur) = de->cur;
	de->cur = de->cur->next;
	de->has_more = (de->cur != de->start);
	return true;
}

/**
	\brief get next item with stolen bit flags
	\details enumeration must be prepared by \ref dlist_enum_start
	\param de: pointer to initialized \ref DLIST_ENUM structure
	\param cur: address of pointer to current item holder
	\retval: \b true if success, \b false after last item was fetched
  */
bool dlist_enum_aligned(DLIST_ENUM* de, DLIST** cur)
{
	if (!(de->has_more))
		return false;
	(*cur) = UNALIGN_DLIST(de->cur);
	de->cur = UNALIGN_DLIST(de->cur)->next;
	de->has_more = (de->cur != de->start);
	return true;
}

/**
	\brief remove item, while enumeration is in progress
	\param dlist: address of list
	\param de: pointer to initialized \ref DLIST_ENUM structure
	\param cur: pointer to item to remove
	\retval: none
  */
void dlist_remove_current_inside_enum(DLIST** dlist, DLIST_ENUM* de, DLIST* cur)
{
	if (cur == de->start)
		de->start = de->start->next;
	dlist_remove(dlist, cur);
	if (*dlist == NULL)
		de->has_more = false;
}

/**
	\brief check, if \b DLIST is contains entry
	\param dlist: address of list
	\param entry: pointer to object to check
	\retval: \b true if contains
  */
bool is_dlist_contains(DLIST** dlist, DLIST* entry)
{
	DLIST_ENUM de;
	DLIST* cur;
	dlist_enum_start(dlist, &de);
	while (dlist_enum(&de, &cur))
	{
		if (cur == entry)
			return true;
	}
	return false;
}

/** \} */ // end of lib_dlist group
