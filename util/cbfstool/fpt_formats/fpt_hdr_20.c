/* CSE FPT header version 0x20 */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <sys/types.h>

#include "cse_fpt.h"

struct fpt_hdr {
	uint8_t marker[4];		/* FPT_MARKER */
	uint32_t num_entries;
	uint8_t hdr_version;		/* FPT_HDR_VERSION_20 */
	uint8_t entry_version;		/* FPT_ENTRY_VERSION */
	uint8_t hdr_length;
	uint8_t hdr_checksum;
	uint8_t rsvd[20];
} __packed;

static bool match_version(struct buffer *buff)
{
	const uint8_t *data = buffer_get(buff);
	uint8_t version = read_at_le8(data, offsetof(struct fpt_hdr, hdr_version));

	return version == FPT_HDR_VERSION_20;
}

static bool validate_fpt_hdr(const struct fpt_hdr *h)
{
	if (memcmp(h->marker, FPT_MARKER, sizeof(h->marker))) {
		ERROR("Invalid FPT header marker!\n");
		return false;
	}

	if (h->hdr_version != FPT_HDR_VERSION_20) {
		ERROR("Invalid FPT header version(0x%x)!\n", h->hdr_version);
		return false;
	}

	if (h->entry_version != FPT_ENTRY_VERSION) {
		ERROR("Invalid FPT entry version(0x%x)!\n", h->entry_version);
		return false;
	}

	const uint8_t *data = (const uint8_t *)h;
	uint8_t checksum = 0;

	for (size_t i = 0; i < sizeof(*h); i++)
		checksum += data[i];

	if (checksum != 0) {
		ERROR("Invalid checksum (0x%x)!\n", h->hdr_checksum);
		return false;
	}

	return true;
}

static fpt_hdr_ptr read_fpt_hdr(struct buffer *buff)
{
	struct fpt_hdr *h = malloc(sizeof(*h));
	if (!h)
		return NULL;

	READ_MEMBER(buff, h->marker);
	READ_MEMBER(buff, h->num_entries);
	READ_MEMBER(buff, h->hdr_version);
	READ_MEMBER(buff, h->entry_version);
	READ_MEMBER(buff, h->hdr_length);
	READ_MEMBER(buff, h->hdr_checksum);
	READ_MEMBER(buff, h->rsvd);

	if (!validate_fpt_hdr(h)) {
		free(h);
		return NULL;
	}

	return h;
}

static void print_fpt_hdr(const fpt_hdr_ptr ptr)
{
	struct fpt_hdr *h = ptr;

	printf(" * FPT header\n");
	printf("%-25s: %.4s\n", "Marker", h->marker);
	printf("%-25s: %d\n", "Number of entries", h->num_entries);
	printf("%-25s: 0x%x\n", "Header version", h->hdr_version);
	printf("%-25s: 0x%x\n", "Entry version", h->entry_version);
	printf("%-25s: %d\n", "Header length", h->hdr_length);
	printf("%-25s: 0x%x\n", "Header checksum", h->hdr_checksum);
}

static size_t get_entry_count(const fpt_hdr_ptr ptr)
{
	struct fpt_hdr *h = ptr;

	return h->num_entries;
}

const struct fpt_hdr_ops fpt_hdr_20_ops = {
	.match_version = match_version,

	.read = read_fpt_hdr,
	.print = print_fpt_hdr,

	.get_entry_count = get_entry_count,
};
