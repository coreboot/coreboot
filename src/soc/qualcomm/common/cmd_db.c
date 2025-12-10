/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <string.h>
#include <soc/cmd_db.h>
#include <types.h>
#include <string.h>
#include <endian.h>

/**
 * struct entry_header: header for each entry in cmd_db
 *
 * @id: resource's identifier
 * @priority: unused
 * @addr: the address of the resource
 * @len: length of the data
 * @offset: offset from :@data_offset, start of the data
 */
struct entry_header {
	u8 id[8];
	u32 priority[NUM_PRIORITY];
	u32 addr;
	u16 len;
	u16 offset;
} __packed;
/**
 * struct rsc_hdr: resource header information
 *
 * @slv_id: id for the resource
 * @header_offset: entry's header at offset from the end of the cmd_db_header
 * @data_offset: entry's data at offset from the end of the cmd_db_header
 * @cnt: number of entries for HW type
 * @version: MSB is major, LSB is minor
 * @reserved: reserved for future use.
 */
struct rsc_hdr {
	u16 slv_id;
	u16 header_offset;
	u16 data_offset;
	u16 cnt;
	u16 version;
	u16 reserved[3];
} __packed;

/**
 * struct cmd_db_header: The DB header information
 *
 * @version: The cmd db version
 * @magic: constant expected in the database
 * @header: array of resources
 * @checksum: checksum for the header. Unused.
 * @reserved: reserved memory
 * @data: driver specific data
 */
struct cmd_db_header {
	u32 version;
	u8 magic[4];
	struct rsc_hdr header[MAX_SLV_ID];
	u32 checksum;
	u32 reserved;
	u8 data[];
} __packed;

/**
 * DOC: Description of the Command DB database.
 *
 * At the start of the command DB memory is the cmd_db_header structure.
 * The cmd_db_header holds the version, checksum, magic key as well as an
 * array for header for each slave (depicted by the rsc_header). Each h/w
 * based accelerator is a 'slave' (shared resource) and has slave id indicating
 * the type of accelerator. The rsc_header is the header for such individual
 * slaves of a given type. The entries for each of these slaves begin at the
 * rsc_hdr.header_offset. In addition each slave could have auxiliary data
 * that may be needed by the driver. The data for the slave starts at the
 * entry_header.offset to the location pointed to by the rsc_hdr.data_offset.
 *
 * Drivers have a stringified key to a slave/resource. They can query the slave
 * information and get the slave id and the auxiliary data and the length of the
 * data. Using this information, they can format the request to be sent to the
 * h/w accelerator and request a resource state.
 */

static const u8 CMD_DB_MAGIC[] = { 0xdb, 0x30, 0x03, 0x0c };

static struct cmd_db_header *cmd_db_header;

static bool cmd_db_magic_matches(const struct cmd_db_header *header)
{
	return memcmp(header->magic, CMD_DB_MAGIC, sizeof(CMD_DB_MAGIC)) == 0;
}

static inline const void *rsc_to_entry_header(const struct rsc_hdr *hdr)
{
	u16 offset = hdr->header_offset;
	return cmd_db_header->data + offset;
}

static inline void *rsc_offset(const struct rsc_hdr *hdr,
			       const struct entry_header *ent)
{
	u16 offset = hdr->data_offset;
	u16 loffset = ent->offset;

	return cmd_db_header->data + offset + loffset;
}

enum cb_err cmd_db_ready(void)
{
	if (cmd_db_header == NULL)
		return CB_ERR;
	else if (!cmd_db_magic_matches(cmd_db_header))
		return CB_ERR;

	return CB_SUCCESS;
}

static enum cb_err cmd_db_get_header(const char *id,
				     const struct entry_header **eh,
				     const struct rsc_hdr **rh)
{
	const struct rsc_hdr *rsc_hdr;
	const struct entry_header *ent;
	enum cb_err ret;
	int i, j;
	u8 query[sizeof(ent->id)];
	size_t id_len;

	ret = cmd_db_ready();
	if (ret != CB_SUCCESS)
		return ret;

	memset(query, 0, sizeof(query));
	id_len = strlen(id);
	if (id_len > sizeof(query))
		id_len = sizeof(query);
	memcpy(query, id, id_len);

	for (i = 0; i < MAX_SLV_ID; i++) {
		rsc_hdr = &cmd_db_header->header[i];
		if (!rsc_hdr->slv_id)
			break;

		ent = rsc_to_entry_header(rsc_hdr);
		for (j = 0; j < rsc_hdr->cnt; j++, ent++) {
			if (memcmp(ent->id, query, sizeof(ent->id)) == 0) {
				if (eh)
					*eh = ent;
				if (rh)
					*rh = rsc_hdr;
				return CB_SUCCESS;
			}
		}
	}

	return CB_ERR;
}

u32 cmd_db_read_addr(const char *id)
{
	enum cb_err ret;
	const struct entry_header *ent;

	ret = cmd_db_get_header(id, &ent, NULL);
	if (ret != CB_SUCCESS)
		return 0;

	return ent->addr;
}

const void *cmd_db_read_aux_data(const char *id, size_t *len)
{
	enum cb_err ret;
	const struct entry_header *ent;
	const struct rsc_hdr *rsc_hdr;

	ret = cmd_db_get_header(id, &ent, &rsc_hdr);
	if (ret != CB_SUCCESS)
		return NULL;

	if (len)
		*len = ent->len;

	return rsc_offset(rsc_hdr, ent);
}

bool cmd_db_match_resource_addr(u32 addr1, u32 addr2)
{
	if (addr1 == addr2)
		return true;
	else if (SLAVE_ID(addr1) == CMD_DB_HW_VRM &&
		 VRM_ADDR(addr1) == VRM_ADDR(addr2))
		return true;

	return false;
}

enum cmd_db_hw_type cmd_db_read_slave_id(const char *id)
{
	enum cb_err ret;
	const struct entry_header *ent;
	u32 addr;

	ret = cmd_db_get_header(id, &ent, NULL);
	if (ret != CB_SUCCESS)
		return CMD_DB_HW_INVALID;

	addr = ent->addr;
	return (addr >> SLAVE_ID_SHIFT) & SLAVE_ID_MASK;
}

bool cmd_db_is_standalone(void)
{
	enum cb_err ret = cmd_db_ready();
	u32 standalone;

	if (ret != CB_SUCCESS)
		return false;

	standalone = cmd_db_header->reserved & CMD_DB_STANDALONE_MASK;
	return standalone != 0;
}

enum cb_err cmd_db_init(uintptr_t base, size_t size)
{
	if (base == 0 || size == 0) {
		printk(BIOS_ERR, "CMD_DB: Invalid base address or size\n");
		return CB_ERR;
	}

	/* Map the command DB memory region */
	cmd_db_header = (struct cmd_db_header *)base;

	if (!cmd_db_magic_matches(cmd_db_header)) {
		printk(BIOS_ERR, "CMD_DB: Invalid Command DB Magic\n");
		cmd_db_header = NULL;
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "CMD_DB: Initialized at 0x%lx, size: 0x%zx\n",
	       base, size);

	if (cmd_db_is_standalone())
		printk(BIOS_DEBUG, "CMD_DB: Running in standalone mode\n");

	return CB_SUCCESS;
}
