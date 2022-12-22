/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <arch/hlt.h>
#include <bl_uapp/bl_errorcodes_public.h>
#include <bl_uapp/bl_syscall_public.h>
#include <cbfs.h>
#include <console/console.h>
#include <psp_verstage.h>

/*
 * We can't pass pointer to hash table in the SPI.
 * The AMD PSP team specifically required that whole hash table
 * should be copied into memory before passing them to the PSP
 * to reduce window of TOCTOU.
 */
#define MAX_NUM_HASH_ENTRIES 128
static struct psp_fw_hash_table hash_table;
static struct psp_fw_entry_hash_256 hash_256[MAX_NUM_HASH_ENTRIES];
static struct psp_fw_entry_hash_384 hash_384[MAX_NUM_HASH_ENTRIES];

void update_psp_fw_hash_table(const char *fname)
{
	uint8_t *spi_ptr = (uint8_t *)cbfs_map(fname, NULL);
	uint32_t len;

	if (!spi_ptr) {
		printk(BIOS_ERR, "AMD Firmware hash table %s not found\n", fname);
		/*
		 * If we don't supply hash table, the PSP will refuse to boot.
		 * So returning here is safe to do.
		 */
		return;
	}

	memcpy(&hash_table, spi_ptr, offsetof(struct psp_fw_hash_table, fw_hash_256));

	if (hash_table.no_of_entries_256 > MAX_NUM_HASH_ENTRIES ||
			hash_table.no_of_entries_384 > MAX_NUM_HASH_ENTRIES) {
		printk(BIOS_ERR, "Too many entries in AMD Firmware hash table"
				 " (SHA256:%d, SHA384:%d)\n",
				 hash_table.no_of_entries_256, hash_table.no_of_entries_384);
		return;
	}

	if (hash_table.no_of_entries_256 == 0 &&
			hash_table.no_of_entries_384 == 0) {
		printk(BIOS_ERR, "No entries in AMD Firmware hash table"
				 " (SHA256:%d, SHA384:%d)\n",
				 hash_table.no_of_entries_256, hash_table.no_of_entries_384);
		return;
	}

	spi_ptr += offsetof(struct psp_fw_hash_table, fw_hash_256);

	hash_table.fw_hash_256 = hash_256;
	hash_table.fw_hash_384 = hash_384;
	len = sizeof(struct psp_fw_entry_hash_256) * hash_table.no_of_entries_256;
	memcpy(hash_256, spi_ptr, len);

	spi_ptr += len;
	len = sizeof(struct psp_fw_entry_hash_384) * hash_table.no_of_entries_384;
	memcpy(hash_384, spi_ptr, len);

	svc_set_fw_hash_table(&hash_table);
}

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

void report_hsp_secure_state(void)
{
	uint32_t hsp_secure_state;
	int ret;

	ret = svc_get_hsp_secure_state(&hsp_secure_state);
	if (ret != BL_OK) {
		printk(BIOS_ERR, "Error reading HSP Secure state: %d\n", ret);
		hlt();
	}

	printk(BIOS_INFO, "HSP Secure state: %#8x\n", hsp_secure_state);
}
