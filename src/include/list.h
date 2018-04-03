/*
 * Copyright 2012 Google Inc.
 * Copyright 2018-present Facebook, Inc.
 *
 * Taken from depthcharge: src/base/list.h
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

#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>
#include <stdint.h>

struct list_node {
	struct list_node *next;
	struct list_node *prev;
};

// Remove list_node node from the doubly linked list it's a part of.
void list_remove(struct list_node *node);
// Insert list_node node after list_node after in a doubly linked list.
void list_insert_after(struct list_node *node, struct list_node *after);
// Insert list_node node before list_node before in a doubly linked list.
void list_insert_before(struct list_node *node, struct list_node *before);

#define list_for_each(ptr, head, member)                                \
	for ((ptr) = container_of((head).next, typeof(*(ptr)), member); \
		&((ptr)->member);                                       \
		(ptr) = container_of((ptr)->member.next,                \
			typeof(*(ptr)), member))

#endif /* __LIST_H__ */
