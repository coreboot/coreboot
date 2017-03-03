/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <bootstate.h>
#include <console/console.h>
#include <intelblocks/cse.h>
#include <soc/pci_devs.h>
#include <stdint.h>

#define PCI_ME_HFSTS1	0x40
#define PCI_ME_HFSTS2	0x48
#define PCI_ME_HFSTS3	0x60
#define PCI_ME_HFSTS4	0x64
#define PCI_ME_HFSTS5	0x68
#define PCI_ME_HFSTS6	0x6c

#define MKHI_GROUP_ID_MCA			0x0a
#define READ_FILE				0x02
#define   READ_FILE_FLAG_DEFAULT		(1 << 0)
#define   READ_FILE_FLAG_HASH			(1 << 1)
#define   READ_FILE_FLAG_EMULATED		(1 << 2)
#define   READ_FILE_FLAG_HW			(1 << 3)

#define MCA_MAX_FILE_PATH_SIZE			64

#define FUSE_LOCK_FILE				"/fpf/intel/SocCfgLock"

static int8_t g_fuse_status;

/*
 * Read file from CSE internal filesystem.
 * size is maximum length of provided buffer buff, which is updated with actual
 * size of the file read. flags indicate whether real file or fuse is used.
 * Returns 1 on success and 0 otherwise.
 */
static int read_cse_file(const char *path, void *buff, size_t *size,
						size_t offset, uint32_t flags)
{
	int res;
	size_t reply_size;

	union mkhi_header {
		uint32_t data;
		struct {
			uint32_t group_id: 8;
			uint32_t command: 7;
			uint32_t is_response: 1;
			uint32_t reserved: 8;
			uint32_t result: 8;
		} __attribute__ ((packed)) fields;
	};

	struct mca_command {
		union mkhi_header mkhi_hdr;
		char file_name[MCA_MAX_FILE_PATH_SIZE];
		uint32_t offset;
		uint32_t data_size;
		uint8_t flags;
	} __attribute__ ((packed)) msg;

	struct mca_response {
		union mkhi_header mkhi_hdr;
		uint32_t data_size;
		uint8_t buffer[128];
	} __attribute__ ((packed)) rmsg;

	if (sizeof(rmsg.buffer) < *size) {
		printk(BIOS_ERR, "internal buffer is too small\n");
		return 0;
	}

	strncpy(msg.file_name, path, sizeof(msg.file_name));
	msg.mkhi_hdr.fields.group_id = MKHI_GROUP_ID_MCA;
	msg.mkhi_hdr.fields.command = READ_FILE;
	msg.flags = flags;
	msg.data_size = *size;
	msg.offset = offset;

	res = heci_send(&msg, sizeof(msg), BIOS_HOST_ADDR, HECI_MKHI_ADDR);

	if (!res) {
		printk(BIOS_ERR, "failed to send HECI message\n");
		return 0;
	}

	reply_size = sizeof(rmsg);
	res = heci_receive(&rmsg, &reply_size);

	if (!res) {
		printk(BIOS_ERR, "failed to receive HECI reply\n");
		return 0;
	}

	if (rmsg.data_size > *size) {
		printk(BIOS_ERR, "reply is too large\n");
		return 0;
	}

	memcpy(buff, rmsg.buffer, rmsg.data_size);
	*size = rmsg.data_size;

	return 1;
}

static void fpf_blown(void *unused)
{
	int8_t fuse;
	size_t sz = sizeof(fuse);

	if (read_cse_file(FUSE_LOCK_FILE, &fuse, &sz, 0, READ_FILE_FLAG_HW)) {
		g_fuse_status = fuse;
		return;
	}
	g_fuse_status = -1;
}

static uint32_t dump_status(int index, int reg_addr)
{
	uint32_t reg = pci_read_config32(HECI1_DEV, reg_addr);

	printk(BIOS_DEBUG, "CSE FWSTS%d: 0x%08x\n", index, reg);

	return reg;
}

static void dump_cse_state(void *unused)
{
	uint32_t fwsts1;

	fwsts1 = dump_status(1, PCI_ME_HFSTS1);
	dump_status(2, PCI_ME_HFSTS2);
	dump_status(3, PCI_ME_HFSTS3);
	dump_status(4, PCI_ME_HFSTS4);
	dump_status(5, PCI_ME_HFSTS5);
	dump_status(6, PCI_ME_HFSTS6);

	/* Minimal decoding is done here in order to call out most important
	   pieces. Manufacturing mode needs to be locked down prior to shipping
	   the product so it's called out explicitly. */
	printk(BIOS_DEBUG, "ME: Manufacturing Mode      : %s\n",
		(fwsts1 & (1 << 0x4)) ? "YES" : "NO");

	printk(BIOS_DEBUG, "ME: FPF status              : ");
	switch (g_fuse_status) {
	case 0:
		printk(BIOS_DEBUG, "unfused");
		break;
	case 1:
		printk(BIOS_DEBUG, "fused");
		break;
	default:
	case -1:
		printk(BIOS_DEBUG, "unknown");
	}
	printk(BIOS_DEBUG, "\n");
}
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, fpf_blown, NULL);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, dump_cse_state, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, dump_cse_state, NULL);
