/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <cbfs.h>

static void print_blob_version(void *arg)
{
	char *version;
	size_t size;

	version = cbfs_map("amdfw_manifest", &size);

	if (!version) {
		printk(BIOS_WARNING, "Failed to get amdfw_manifest\n");
		return;
	}

	printk(BIOS_INFO, "AMDFW blobs version:\n");
	printk(BIOS_INFO, "%.*s", (int)size, version);

	cbfs_unmap(version);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, print_blob_version, NULL);
