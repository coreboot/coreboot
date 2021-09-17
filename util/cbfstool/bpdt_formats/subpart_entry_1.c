/* Subpart directory entry version 1 support */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <sys/types.h>

#include "cse_serger.h"

#define SUBPART_OFFSET_SHIFT		0
#define SUBPART_OFFSET_MASK		0x1ffffff
#define SUBPART_OFFSET(x)		(((x) >> SUBPART_OFFSET_SHIFT) & SUBPART_OFFSET_MASK)
#define SUBPART_COMPRESSED_SHIFT	25
#define SUBPART_COMPRESSED_MASK	1
#define SUBPART_COMPRESSED(x)		\
	(((x) >> SUBPART_COMPRESSED_SHIFT) & SUBPART_COMPRESSED_MASK)

struct subpart_entry {
	uint8_t name[12];
	uint32_t offset_bytes;
	uint32_t length;
	uint32_t rsvd2;
} __packed;

static void subpart_read_entry(struct buffer *buff, struct subpart_entry *e)
{
	READ_MEMBER(buff, e->name);
	READ_MEMBER(buff, e->offset_bytes);
	READ_MEMBER(buff, e->length);
	READ_MEMBER(buff, e->rsvd2);
}

static void subpart_print_entry(const struct subpart_entry *e, size_t index)
{
	printf("%-25zd%-25.12s0x%-23x%-25c0x%-23x0x%-23x\n", index,
		e->name, SUBPART_OFFSET(e->offset_bytes),
		SUBPART_COMPRESSED(e->offset_bytes) ? 'Y' : 'N',
		e->length, e->rsvd2);
}

static void subpart_print_entries(struct buffer *buff, size_t count)
{
	struct subpart_entry *e = malloc(count * sizeof(*e));

	if (!e)
		return;

	for (size_t i = 0; i < count; i++)
		subpart_read_entry(buff, &e[i]);

	printf("%-25s%-25s%-25s%-25s%-25s%-25s\n", "Entry #", "Name", "Offset",
	       "Huffman Compressed?", "Length", "Rsvd");

	printf("====================================================================="
	       "=====================================================================\n");

	for (size_t i = 0; i < count; i++)
		subpart_print_entry(&e[i], i + 1);

	printf("====================================================================="
	       "=====================================================================\n");

	free(e);
}

const struct subpart_entry_ops subpart_entry_1_ops = {
	.print = subpart_print_entries,
};
