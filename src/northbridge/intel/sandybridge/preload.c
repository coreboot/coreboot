/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <bootstate.h>
#include <drivers/intel/gma/opregion.h>
#include <program_loading.h>

static void preload_vbt_and_dsdt(void *unused)
{
	/*
	 * Preload the Video BIOS Table (VBT) and the DSDT in
	 * parallel with the payload. Started some time after BS_PRE_DEVICE
	 * to make sure that the microcode has been loaded.
	 *
	 * TODO: Implement a queue for managing preloads more efficiently.
	 */
	cbfs_preload(mainboard_vbt_filename());
	cbfs_preload(CONFIG_CBFS_PREFIX "/dsdt.aml");
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, preload_vbt_and_dsdt, NULL);

static void preload_payload_and_ucode(void *unused)
{
	/*
	 * Payload is large and needs to go first.
	 * The microcode is smaller and can be loaded in parallel as it needs
	 * to be ready in BS_DEV_INIT.
	 */
	payload_preload();
	cbfs_preload("cpu_microcode_blob.bin");
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, preload_payload_and_ucode, NULL);
