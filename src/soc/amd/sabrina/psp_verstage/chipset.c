/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <bl_uapp/bl_syscall_public.h>
#include <console/console.h>
#include <psp_verstage.h>

uint32_t update_psp_bios_dir(uint32_t *psp_dir_offset, uint32_t *bios_dir_offset)
{
	return svc_update_psp_bios_dir(psp_dir_offset, bios_dir_offset);
}

uint32_t save_uapp_data(void *address, uint32_t size)
{
	return svc_save_uapp_data(address, size);
}

uint32_t get_bios_dir_addr(struct embedded_firmware *ef_table)
{
	return 0;
}

int platform_set_sha_op(enum vb2_hash_algorithm hash_alg,
			struct sha_generic_data *sha_op)
{
	if (hash_alg == VB2_HASH_SHA256) {
		sha_op->SHAType = SHA_TYPE_256;
		sha_op->DigestLen = 32;
	} else if (hash_alg == VB2_HASH_SHA384) {
		sha_op->SHAType = SHA_TYPE_384;
		sha_op->DigestLen = 48;
	} else {
		return -1;
	}
	return 0;
}


/* Functions below are stub functions for not-yet-implemented PSP features.
 * These functions should be replaced with proper implementations later.
 */

uint32_t svc_write_postcode(uint32_t postcode)
{
	return 0;
}

void platform_report_mode(int developer_mode_enabled)
{
	printk(BIOS_INFO, "Reporting %s mode\n",
	       developer_mode_enabled ? "Developer" : "Normal");
	if (developer_mode_enabled)
		svc_set_platform_boot_mode(CHROME_BOOK_BOOT_MODE_DEVELOPER);
	else
		svc_set_platform_boot_mode(CHROME_BOOK_BOOT_MODE_NORMAL);
}
