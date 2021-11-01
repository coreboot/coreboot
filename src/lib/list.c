/* Taken from depthcharge: src/base/list.c */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <list.h>

void list_remove(struct list_node *node)
{
	if (node->prev)
		node->prev->next = node->next;
	if (node->next)
		node->next->prev = node->prev;
}

void list_insert_after(struct list_node *node, struct list_node *after)
{
	node->next = after->next;
	node->prev = after;
	after->next = node;
	if (node->next)
		node->next->prev = node;
}

void list_insert_before(struct list_node *node, struct list_node *before)
{
	node->prev = before->prev;
	node->next = before;
	before->prev = node;
	if (node->prev)
		node->prev->next = node;
}

void list_append(struct list_node *node, struct list_node *head)
{
	while (head->next)
		head = head->next;

	list_insert_after(node, head);
}
