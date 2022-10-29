/* BPDT version 1.6 support */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <sys/types.h>

#include "cse_serger.h"

struct bpdt_header {
	uint32_t signature;		/* BPDT_SIGNATURE */
	uint16_t descriptor_count;
	uint16_t version;		/* Layout 1.6 = 1 */
	uint16_t reserved;
	uint8_t  whole_checksum;
	uint8_t  rom_checksum;
	uint32_t ifwi_version;
	struct {
		uint16_t major;
		uint16_t minor;
		uint16_t build;
		uint16_t hotfix;
	} fit_tool_version;
} __packed;

struct cse_layout {
	uint8_t rom_bypass[16];
	uint32_t data_offset;
	uint32_t data_size;
	uint32_t bp1_offset;
	uint32_t bp1_size;
	uint32_t bp2_offset;
	uint32_t bp2_size;
	uint32_t bp3_offset;
	uint32_t bp3_size;
	uint32_t reserved[16];
	uint8_t checksum;
} __packed;

static bool match_version(const struct buffer *buff)
{
	const uint8_t *data = buffer_get(buff);
	const uint32_t sig = read_le32(data);
	const uint16_t version = read_at_le16(data, offsetof(struct bpdt_header, version));

	if (sig != BPDT_SIGNATURE) {
		ERROR("Invalid BPDT signature(0x%x)!\n", sig);
		return false;
	}

	return version == BPDT_VERSION_1_6;
}

static bpdt_hdr_ptr create_bpdt_hdr(void)
{
	struct bpdt_header *h = malloc(sizeof(*h));

	if (!h)
		return NULL;

	h->signature = BPDT_SIGNATURE;
	h->descriptor_count = 0;
	h->version = BPDT_VERSION_1_6;
	h->reserved = 0;
	/* TODO(b/202549343): Need to calculate checksum */
	h->whole_checksum = 0;
	h->rom_checksum = 0;
	h->ifwi_version = 0;
	h->fit_tool_version.major = 0;
	h->fit_tool_version.minor = 0;
	h->fit_tool_version.build = 0;
	h->fit_tool_version.hotfix = 0;

	return h;
}

static void print_bpdt_hdr(const bpdt_hdr_ptr ptr)
{
	struct bpdt_header *h = ptr;

	printf(" * BPDT header\n");
	printf("%-25s 0x%-23.8x\n", "Signature", h->signature);
	printf("%-25s %-25d\n", "Descriptor count", h->descriptor_count);
	printf("%-25s %d (Layout 1.6)\n", "BPDT Version", h->version);
	printf("%-25s 0x%-23x\n", "Reserved", h->reserved);
	printf("%-25s 0x%-23x\n", "Whole Checksum", h->whole_checksum);
	printf("%-25s 0x%-23x\n", "ROM Checksum", h->rom_checksum);
	printf("%-25s 0x%-23x\n", "IFWI Version", h->ifwi_version);
	printf("%-25s %d.%d.%d.%d(%.2x.%.2x.%.2x.%.2x)\n", "FIT Tool Version",
	       h->fit_tool_version.major, h->fit_tool_version.minor,
	       h->fit_tool_version.build, h->fit_tool_version.hotfix,
	       h->fit_tool_version.major, h->fit_tool_version.minor,
	       h->fit_tool_version.build, h->fit_tool_version.hotfix);
}

static bpdt_hdr_ptr read_bpdt_hdr(struct buffer *buff)
{
	struct bpdt_header *h = malloc(sizeof(*h));

	if (!h)
		return NULL;

	READ_MEMBER(buff, h->signature);
	READ_MEMBER(buff, h->descriptor_count);
	READ_MEMBER(buff, h->version);
	READ_MEMBER(buff, h->reserved);
	READ_MEMBER(buff, h->whole_checksum);
	READ_MEMBER(buff, h->rom_checksum);
	READ_MEMBER(buff, h->ifwi_version);
	READ_MEMBER(buff, h->fit_tool_version);

	return h;
}

static int write_bpdt_hdr(struct buffer *buff, const bpdt_hdr_ptr ptr)
{
	struct bpdt_header *h = ptr;

	if (buffer_size(buff) < sizeof(struct bpdt_header)) {
		ERROR("Not enough size in buffer for BPDT header!\n");
		return -1;
	}

	WRITE_MEMBER(buff, h->signature);
	WRITE_MEMBER(buff, h->descriptor_count);
	WRITE_MEMBER(buff, h->version);
	WRITE_MEMBER(buff, h->reserved);
	WRITE_MEMBER(buff, h->whole_checksum);
	WRITE_MEMBER(buff, h->rom_checksum);
	WRITE_MEMBER(buff, h->ifwi_version);
	WRITE_MEMBER(buff, h->fit_tool_version);

	return 0;
}

static size_t get_bpdt_entry_count(const bpdt_hdr_ptr ptr)
{
	return ((const struct bpdt_header *)ptr)->descriptor_count;
}

static void inc_bpdt_entry_count(bpdt_hdr_ptr ptr)
{
	struct bpdt_header *h = ptr;
	h->descriptor_count++;
}

static cse_layout_ptr create_cse_layout(const struct region *r)
{
	struct cse_layout *l = malloc(sizeof(*l));

	if (!l)
		return NULL;

	l->data_offset = r[DP].offset;
	l->data_size = r[DP].size;
	l->bp1_offset = r[BP1].offset;
	l->bp1_size = r[BP1].size;
	l->bp2_offset = r[BP2].offset;
	l->bp2_size = r[BP2].size;
	l->bp3_offset = r[BP3].offset;
	l->bp3_size = r[BP3].size;
	l->checksum = 0;		/* unused */

	return l;
}

static void print_cse_layout(const cse_layout_ptr ptr)
{
	struct cse_layout *l = ptr;

	printf(" * CSE Layout\n\n");
	printf("ROM Bypass: ");
	for (size_t i = 0; i < sizeof(l->rom_bypass); i++)
		printf("0x%x ", l->rom_bypass[i]);
	printf("\n");
	printf("Data partition offset: 0x%x\n", l->data_offset);
	printf("Data partition size: 0x%x\n", l->data_size);
	printf("BP1 offset: 0x%x\n", l->bp1_offset);
	printf("BP1 size: 0x%x\n", l->bp1_size);
	printf("BP2 offset: 0x%x\n", l->bp2_offset);
	printf("BP2 size: 0x%x\n", l->bp2_size);
	printf("BP3 offset: 0x%x\n", l->bp3_offset);
	printf("BP3 size: 0x%x\n", l->bp3_size);
	printf("Checksum: 0x%x\n", l->checksum);
}

static cse_layout_ptr read_cse_layout(struct buffer *buff)
{
	struct cse_layout *l = malloc(sizeof(*l));

	if (!l)
		return NULL;

	READ_MEMBER(buff, l->rom_bypass);
	READ_MEMBER(buff, l->data_offset);
	READ_MEMBER(buff, l->data_size);
	READ_MEMBER(buff, l->bp1_offset);
	READ_MEMBER(buff, l->bp1_size);
	READ_MEMBER(buff, l->bp2_offset);
	READ_MEMBER(buff, l->bp2_size);
	READ_MEMBER(buff, l->bp3_offset);
	READ_MEMBER(buff, l->bp3_size);
	READ_MEMBER(buff, l->reserved);
	READ_MEMBER(buff, l->checksum);

	return l;
}

static int write_cse_layout(struct buffer *buff, const cse_layout_ptr ptr)
{
	struct cse_layout *l = ptr;

	if (buffer_size(buff) < sizeof(struct cse_layout)) {
		ERROR("Not enough size in buffer for CSE layout!\n");
		return -1;
	}

	WRITE_MEMBER(buff, l->rom_bypass);
	WRITE_MEMBER(buff, l->data_offset);
	WRITE_MEMBER(buff, l->data_size);
	WRITE_MEMBER(buff, l->bp1_offset);
	WRITE_MEMBER(buff, l->bp1_size);
	WRITE_MEMBER(buff, l->bp2_offset);
	WRITE_MEMBER(buff, l->bp2_size);
	WRITE_MEMBER(buff, l->bp3_offset);
	WRITE_MEMBER(buff, l->bp3_size);
	WRITE_MEMBER(buff, l->reserved);
	WRITE_MEMBER(buff, l->checksum);

	return 0;
}

static void update_checksum(bpdt_hdr_ptr ptr, struct bpdt_entry *e)
{
	(void)ptr;
	(void)e;

	/* TODO(b/202549343) */
	ERROR("Update checksum is not supported for 1.6!\n");
}

static bool validate_checksum(bpdt_hdr_ptr ptr, struct bpdt_entry *e)
{
	(void)e;
	(void)ptr;

	/* TODO(b/202549343) */
	ERROR("Validate checksum is not supported for 1.6!\n");

	return true;
}

const struct bpdt_ops bpdt_1_6_ops = {
	.match_version = match_version,

	.create_hdr = create_bpdt_hdr,
	.print_hdr = print_bpdt_hdr,
	.read_hdr = read_bpdt_hdr,
	.write_hdr = write_bpdt_hdr,

	.get_entry_count = get_bpdt_entry_count,
	.inc_entry_count = inc_bpdt_entry_count,

	.create_layout = create_cse_layout,
	.print_layout = print_cse_layout,
	.read_layout = read_cse_layout,
	.write_layout = write_cse_layout,

	.update_checksum = update_checksum,
	.validate_checksum = validate_checksum,

	.subpart_hdr_version = SUBPART_HDR_VERSION_1,
	.subpart_entry_version = SUBPART_ENTRY_VERSION_1,
};
