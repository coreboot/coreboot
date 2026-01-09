/* Taken from depthcharge: src/base/list.h */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __COMMONLIB_LIST_H__
#define __COMMONLIB_LIST_H__

#include <commonlib/helpers.h>
#include <stdbool.h>
#include <stdint.h>

struct list_node {
	struct _internal_do_not_access_list_node {
		struct list_node *next;
		struct list_node *prev;
	} _internal_do_not_access;
};

// These macros do NOT belong to the public API.
#define NEXT(ptr) ((ptr)->_internal_do_not_access.next)
#define PREV(ptr) ((ptr)->_internal_do_not_access.prev)

/* Initialize a circular list, with `head` being a placeholder head node. */
void _list_init(struct list_node *head);

// Remove list_node node from the doubly linked list it's a part of.
void list_remove(struct list_node *node);
// Insert list_node node after list_node after in a doubly linked list.
void list_insert_after(struct list_node *node, struct list_node *after);
// Insert list_node node before list_node before in a doubly linked list.
// `before` must not be the placeholder head node.
void list_insert_before(struct list_node *node, struct list_node *before);
// Append the node to the end of the list.
static inline void list_append(struct list_node *node, struct list_node *head)
{
	_list_init(head);
	/* With a circular list, we just need to insert before the head. */
	list_insert_before(node, head);
}

// Return if the list is empty.
static inline bool list_is_empty(const struct list_node *head)
{
	return !NEXT(head) || NEXT(head) == head;
}

// Get next node.
static inline struct list_node *list_next(const struct list_node *node,
					  const struct list_node *head)
{
	return NEXT(node) == head ? NULL : NEXT(node);
};

// Get prev node.
static inline struct list_node *list_prev(const struct list_node *node,
					  const struct list_node *head)
{
	return PREV(node) == head ? NULL : PREV(node);
};

// Get first node.
static inline struct list_node *list_first(const struct list_node *head)
{
	return list_next(head, head);
}

// Get last node.
static inline struct list_node *list_last(const struct list_node *head)
{
	return list_prev(head, head);
}

// Get the number of list elements.
size_t list_length(const struct list_node *head);

/*
 * Explanation of `ptr` initialization:
 * 1. head.next != NULL: This means the list isn't empty. As the implementation ensures that
 *    _list_init() is called when the very first element is added, we can safely assume that
 *    the list is circular, and hence set `ptr` to the 1st element.
 * 2. head.next == NULL: This means the list is empty, and _list_init() hasn't been called.
 *    As the `head` arg might be const, we cannot simply call _list_init() here. Instead, we set
 *    `ptr` to a special value such that `&(ptr->member) == &head`, causing the loop to
 *    terminate immediately.
 */
#define list_for_each(ptr, head, member) \
	for ((ptr) = container_of((head)._internal_do_not_access.next ?: &(head), typeof(*(ptr)), member); \
		&((ptr)->member) != &(head); \
		(ptr) = container_of((ptr)->member._internal_do_not_access.next, \
			typeof(*(ptr)), member))

#undef NEXT
#undef PREV

#endif /* __COMMONLIB_LIST_H__ */
