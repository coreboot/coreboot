/*
 * Copyright 2012 Google Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "base/list.h"

void list_remove(ListNode *node)
{
	if (node->prev)
		node->prev->next = node->next;
	if (node->next)
		node->next->prev = node->prev;
}

void list_insert_after(ListNode *node, ListNode *after)
{
	node->next = after->next;
	node->prev = after;
	after->next = node;
	if (node->next)
		node->next->prev = node;
}

void list_insert_before(ListNode *node, ListNode *before)
{
	node->prev = before->prev;
	node->next = before;
	before->prev = node;
	if (node->prev)
		node->prev->next = node;
}
