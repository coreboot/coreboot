/* Taken from depthcharge: src/base/list.c */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/list.h>

// Initialize a circular list, with `head` being a placeholder head node.
static void list_init(struct list_node *head)
{
	if (!head->next) {
		assert(!head->prev);
		head->next = head->prev = head;
	}
}

void list_remove(struct list_node *node)
{
	/* Cannot remove the head node. */
	assert(node->prev && node->next);
	if (node->prev)
		node->prev->next = node->next;
	if (node->next)
		node->next->prev = node->prev;
}

void list_insert_after(struct list_node *node, struct list_node *after)
{
	/* Check uninitialized head node. */
	if (after->prev == NULL)
		list_init(after);
	node->next = after->next;
	node->prev = after;
	after->next = node;
	if (node->next)
		node->next->prev = node;
}

void list_insert_before(struct list_node *node, struct list_node *before)
{
	/* `before` cannot be an uninitialized head node. */
	assert(before->prev)
	node->prev = before->prev;
	node->next = before;
	before->prev = node;
	if (node->prev)
		node->prev->next = node;
}

void list_append(struct list_node *node, struct list_node *head)
{
	list_init(head);
	/* With a circular list, we just need to insert before the head. */
	list_insert_before(node, head);
}
