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
// `before` must not be the placeholder head node.
void list_insert_before(struct list_node *node, struct list_node *before);
// Append the node to the end of the list.
void list_append(struct list_node *node, struct list_node *head);

/*
 * Explanation of `ptr` initialization:
 * 1. head.next != NULL: This means the list isn't empty. As the implementation ensures that
 *    list_init() is called when the very first element is added, we can safely assume that
 *    the list is circular, and hence set `ptr` to the 1st element.
 * 2. head.next == NULL: This means the list is empty, and list_init() hasn't been called.
 *    As the `head` arg might be const, we cannot simply call list_init() here. Instead, we set
 *    `ptr` to a special value such that `&(ptr->member) == &head`, causing the loop to
 *    terminate immediately.
 */
#define list_for_each(ptr, head, member) \
	for ((ptr) = container_of((head).next ?: &(head), typeof(*(ptr)), member); \
		(&((ptr)->member) != &(head)); \
		(ptr) = container_of((ptr)->member.next, \
			typeof(*(ptr)), member))

#endif /* __COMMONLIB_LIST_H__ */
