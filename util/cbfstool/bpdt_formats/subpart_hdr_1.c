/* Subpart directory header version 1 support */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <sys/types.h>

#include "cse_serger.h"

struct subpart_hdr {
	uint32_t signature;		/* SUBPART_SIGNATURE */
	uint32_t count;
	uint8_t hdr_version;		/* Header version = 1 */
	uint8_t entry_version;		/* Entry version = 1 */
	uint8_t length;
	uint8_t checksum;
	uint8_t name[4];
} __packed;

static void subpart_hdr_print(const subpart_hdr_ptr ptr)
{
	const struct subpart_hdr *hdr = ptr;

	printf("%-25s %.4s\n", "Signature", (const char *)&hdr->signature);
	printf("%-25s %-25d\n", "Count", hdr->count);
	printf("%-25s %-25d\n", "Header Version", hdr->hdr_version);
	printf("%-25s %-25d\n", "Entry Version", hdr->entry_version);
	printf("%-25s 0x%-23x\n", "Header Length", hdr->length);
	printf("%-25s 0x%-23x\n", "Checksum", hdr->checksum);
	printf("%-25s ", "Name");
	for (size_t i = 0; i < sizeof(hdr->name); i++)
		printf("%c", hdr->name[i]);
	printf("\n");
}

static subpart_hdr_ptr subpart_hdr_read(struct buffer *buff)
{
	struct subpart_hdr *hdr = malloc(sizeof(*hdr));

	if (!hdr)
		return NULL;

	READ_MEMBER(buff, hdr->signature);
	READ_MEMBER(buff, hdr->count);
	READ_MEMBER(buff, hdr->hdr_version);
	READ_MEMBER(buff, hdr->entry_version);
	READ_MEMBER(buff, hdr->length);
	READ_MEMBER(buff, hdr->checksum);
	READ_MEMBER(buff, hdr->name);

	return hdr;
}

static size_t subpart_get_count(const subpart_hdr_ptr ptr)
{
	const struct subpart_hdr *hdr = ptr;

	return hdr->count;
}

static void subpart_hdr_free(subpart_hdr_ptr ptr)
{
	free(ptr);
}

const struct subpart_hdr_ops subpart_hdr_1_ops = {
	.read = subpart_hdr_read,
	.print = subpart_hdr_print,
	.get_entry_count = subpart_get_count,
	.free = subpart_hdr_free,
};
