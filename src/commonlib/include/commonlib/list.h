/* Taken from depthcharge: src/base/list.h */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __COMMONLIB_LIST_H__
#define __COMMONLIB_LIST_H__

#include <commonlib/helpers.h>
#include <stdbool.h>
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
// Append the node to the end of the list.
void list_append(struct list_node *node, struct list_node *head);

// Return if the list is empty.
static inline bool list_is_empty(const struct list_node *head)
{
	return !head->next;
}

// Get next node.
static inline const struct list_node *list_next(const struct list_node *node,
						const struct list_node *head)
{
	return node->next;
};

// Get prev node.
static inline const struct list_node *list_prev(const struct list_node *node,
						const struct list_node *head)
{
	return node->prev == head ? NULL : node->prev;
};

// Get first node.
static inline const struct list_node *list_first(const struct list_node *head)
{
	return list_is_empty(head) ? NULL : head->next;
}

// Get last node.
const struct list_node *list_last(const struct list_node *head);

// Get the number of list elements.
size_t list_length(const struct list_node *head);

#define list_for_each(ptr, head, member)                                      \
	for ((ptr) = container_of((head).next, typeof(*(ptr)), member);       \
		(uintptr_t)ptr + (uintptr_t)offsetof(typeof(*(ptr)), member); \
		(ptr) = container_of((ptr)->member.next,                      \
			typeof(*(ptr)), member))

#endif /* __COMMONLIB_LIST_H__ */
