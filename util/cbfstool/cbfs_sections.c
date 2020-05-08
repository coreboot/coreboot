/* track which sections of the image will contain CBFSes */
/* SPDX-License-Identifier: GPL-2.0-only */

#include "cbfs_sections.h"
#include "common.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct descriptor_node {
	const struct flashmap_descriptor *val;
	struct descriptor_node *next;
};

static struct descriptor_list {
	struct descriptor_node *head;
	struct descriptor_node *tail;
} cbfs_sections;

static bool seen_primary_section = false;

static void descriptor_list_prepend(struct descriptor_list *list,
					struct descriptor_node *new_head)
{
	assert(list);
	assert(new_head);

	new_head->next = list->head;
	list->head = new_head;
	if (!list->tail)
		list->tail = new_head;
}

static void descriptor_list_append(struct descriptor_list *list,
					struct descriptor_node *new_tail)
{
	assert(list);
	assert(new_tail);

	if (list->tail)
		list->tail->next = new_tail;
	list->tail = new_tail;
	if (!list->head)
		list->head = new_tail;
}

/* Implementation of cbfs module's callback; invoked during fmd file parsing */
bool fmd_process_flag_cbfs(const struct flashmap_descriptor *node)
{
	struct descriptor_node *list_node;

	if (node->list_len != 0)
		return false;

	list_node = (struct descriptor_node *)malloc(sizeof(*list_node));
	if (!list_node) {
		ERROR("Cannot allocate CBFS flag node!\n");
		return false;
	}
	list_node->val = node;
	list_node->next = NULL;

	if (strcmp(node->name, SECTION_NAME_PRIMARY_CBFS) == 0) {
		descriptor_list_prepend(&cbfs_sections, list_node);
		seen_primary_section = true;
	} else {
		descriptor_list_append(&cbfs_sections, list_node);
	}

	return true;
}

cbfs_section_iterator_t cbfs_sections_iterator(void)
{
	return cbfs_sections.head;
}

bool cbfs_sections_iterator_advance(cbfs_section_iterator_t *it)
{
	assert(it);
	if (!*it)
		return false;

	*it = (*it)->next;
	return true;
}

const struct flashmap_descriptor *cbfs_sections_iterator_deref(
						cbfs_section_iterator_t it)
{
	assert(it);
	return it->val;
}

bool cbfs_sections_primary_cbfs_accounted_for(void)
{
	return seen_primary_section;
}

void cbfs_sections_cleanup(void)
{
	for (struct descriptor_node *cur = cbfs_sections.head, *next = NULL;
							cur; cur = next) {
		next = cur->next;
		free(cur);
	}
	cbfs_sections.head = NULL;
	cbfs_sections.tail = NULL;
}
