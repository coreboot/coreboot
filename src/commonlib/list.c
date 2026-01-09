/* Taken from depthcharge: src/base/list.c */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/list.h>

#define NEXT(ptr) ((ptr)->_internal_do_not_access.next)
#define PREV(ptr) ((ptr)->_internal_do_not_access.prev)

void _list_init(struct list_node *head)
{
	if (!NEXT(head)) {
		assert(!PREV(head));
		PREV(head) = NEXT(head) = head;
	}
}

void list_remove(struct list_node *node)
{
	/* Cannot remove the head node. */
	assert(PREV(node) && NEXT(node));
	NEXT(PREV(node)) = NEXT(node);
	PREV(NEXT(node)) = PREV(node);
}

void list_insert_after(struct list_node *node, struct list_node *after)
{
	/* Check uninitialized head node. */
	if (!PREV(after))
		_list_init(after);
	NEXT(node) = NEXT(after);
	PREV(node) = after;
	NEXT(after) = node;
	PREV(NEXT(node)) = node;
}

void list_insert_before(struct list_node *node, struct list_node *before)
{
	/* `before` cannot be an uninitialized head node. */
	assert(PREV(before));
	PREV(node) = PREV(before);
	NEXT(node) = before;
	PREV(before) = node;
	NEXT(PREV(node)) = node;
}

size_t list_length(const struct list_node *head)
{
	struct {
		struct list_node node;
	} const *ptr;
	size_t len = 0;

	list_for_each(ptr, *head, node)
		len++;

	return len;
}

#undef NEXT
#undef PREV
