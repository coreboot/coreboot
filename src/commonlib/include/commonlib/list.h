/* Taken from depthcharge: src/base/list.h */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __COMMONLIB_LIST_H__
#define __COMMONLIB_LIST_H__

#include <commonlib/helpers.h>
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
// Appends the node to the end of the list.
void list_append(struct list_node *node, struct list_node *head);

#define list_for_each(ptr, head, member)                                      \
	for ((ptr) = container_of((head).next, typeof(*(ptr)), member);       \
		(uintptr_t)ptr + (uintptr_t)offsetof(typeof(*(ptr)), member); \
		(ptr) = container_of((ptr)->member.next,                      \
			typeof(*(ptr)), member))

#endif /* __COMMONLIB_LIST_H__ */
