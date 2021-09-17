/* CSE FPT header version 0x21 */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <sys/types.h>

#include "cse_fpt.h"

struct fpt_hdr {
	uint8_t marker[4];		/* FPT_MARKER */
	uint32_t num_entries;
	uint8_t hdr_version;		/* FPT_HDR_VERSION_21 */
	uint8_t entry_version;		/* FPT_ENTRY_VERSION */
	uint8_t hdr_length;
	uint8_t redundancy;
	uint8_t reserved[8];
	uint32_t checksum;
	struct {
		uint16_t major;
		uint16_t minor;
		uint16_t build;
		uint16_t hotfix;
	} fit_tool_version;
} __packed;

static bool match_version(struct buffer *buff)
{
	const uint8_t *data = buffer_get(buff);
	uint8_t version = read_at_le8(data, offsetof(struct fpt_hdr, hdr_version));

	return version == FPT_HDR_VERSION_21;
}

static bool validate_fpt_hdr(const struct fpt_hdr *h)
{
	if (memcmp(h->marker, FPT_MARKER, sizeof(h->marker))) {
		ERROR("Invalid FPT header marker!\n");
		return false;
	}

	if (h->hdr_version != FPT_HDR_VERSION_21) {
		ERROR("Invalid FPT header version(0x%x)!\n", h->hdr_version);
		return false;
	}

	if (h->entry_version != FPT_ENTRY_VERSION) {
		ERROR("Invalid FPT entry version(0x%x)!\n", h->entry_version);
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
	READ_MEMBER(buff, h->redundancy);
	READ_MEMBER(buff, h->reserved);
	READ_MEMBER(buff, h->checksum);
	READ_MEMBER(buff, h->fit_tool_version);

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
	printf("%-25s: 0x%x\n", "Redundancy", h->redundancy);
	printf("%-25s: ", "Reserved");
	for (size_t i = 0; i < ARRAY_SIZE(h->reserved); i++)
		printf("0x%x ", h->reserved[i]);
	printf("\n");
	printf("%-25s: 0x%x\n", "Checksum", h->checksum);
	printf("%-25s: %d.%d.%d.%d(%.2x.%.2x.%.2x.%.2x)\n", "FIT Tool Version",
	       h->fit_tool_version.major, h->fit_tool_version.minor,
	       h->fit_tool_version.build, h->fit_tool_version.hotfix,
	       h->fit_tool_version.major, h->fit_tool_version.minor,
	       h->fit_tool_version.build, h->fit_tool_version.hotfix);
}

static size_t get_entry_count(const fpt_hdr_ptr ptr)
{
	struct fpt_hdr *h = ptr;

	return h->num_entries;
}

const struct fpt_hdr_ops fpt_hdr_21_ops = {
	.match_version = match_version,

	.read = read_fpt_hdr,
	.print = print_fpt_hdr,

	.get_entry_count = get_entry_count,
};
