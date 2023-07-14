/* SPDX-License-Identifier: GPL-2.0-only */

#include "2api.h"
#include <arch/hlt.h>
#include <bl_uapp/bl_errorcodes_public.h>
#include <bl_uapp/bl_syscall_public.h>
#include <boot_device.h>
#include <cbfs.h>
#include <console/console.h>
#include <psp_verstage.h>
#include <security/vboot/misc.h>
#include <security/vboot/vbnv.h>

#define PSP_FW_HASH_FILE_NAME(slot, id) "apu/amdfw_" slot "_hash" id
/*
 * We can't pass pointer to hash table in the SPI.
 * The AMD PSP team specifically required that whole hash table
 * should be copied into memory before passing them to the PSP
 * to reduce window of TOCTOU.
 */
#define MAX_NUM_HASH_ENTRIES 64
static struct psp_fw_hash_table hash_table;
static struct psp_fw_entry_hash_256 hash_256[MAX_NUM_HASH_ENTRIES];
static struct psp_fw_entry_hash_384 hash_384[MAX_NUM_HASH_ENTRIES];

static struct psp_fw_hash_table_v2 hash_table_v2;
static struct psp_fw_entry_hash_256_v2 hash_256_v2[MAX_NUM_HASH_ENTRIES];
static struct psp_fw_entry_hash_384_v2 hash_384_v2[MAX_NUM_HASH_ENTRIES];

static void update_one_psp_fw_hash_table_v1(enum verstage_cmd_id cmd, uint8_t *spi_ptr)
{
	uint32_t len;

	memcpy(&hash_table, spi_ptr, offsetof(struct psp_fw_hash_table, fw_hash_256));

	if (hash_table.no_of_entries_256 > MAX_NUM_HASH_ENTRIES ||
			hash_table.no_of_entries_384 > MAX_NUM_HASH_ENTRIES) {
		printk(BIOS_ERR, "Too many entries in AMD Firmware hash table"
				 " (SHA256:%d, SHA384:%d)\n", hash_table.no_of_entries_256,
				 hash_table.no_of_entries_384);
		return;
	}

	if (hash_table.no_of_entries_256 == 0 && hash_table.no_of_entries_384 == 0) {
		printk(BIOS_ERR, "No entries in AMD Firmware hash table"
				 " (SHA256:%d, SHA384:%d)\n", hash_table.no_of_entries_256,
				 hash_table.no_of_entries_384);
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

	svc_set_fw_hash_table(cmd, &hash_table);
}

static void update_one_psp_fw_hash_table_v2(enum verstage_cmd_id cmd, uint8_t *spi_ptr)
{
	uint32_t len;

	memcpy(&hash_table_v2, spi_ptr, offsetof(struct psp_fw_hash_table_v2, fw_hash_256));

	if (hash_table_v2.no_of_entries_256 > MAX_NUM_HASH_ENTRIES ||
			hash_table_v2.no_of_entries_384 > MAX_NUM_HASH_ENTRIES) {
		printk(BIOS_ERR, "Too many entries in AMD Firmware hash table"
				 " (SHA256:%d, SHA384:%d)\n", hash_table_v2.no_of_entries_256,
				 hash_table_v2.no_of_entries_384);
		return;
	}

	if (hash_table_v2.no_of_entries_256 == 0 && hash_table_v2.no_of_entries_384 == 0) {
		printk(BIOS_ERR, "No entries in AMD Firmware hash table"
				 " (SHA256:%d, SHA384:%d)\n", hash_table_v2.no_of_entries_256,
				 hash_table_v2.no_of_entries_384);
		return;
	}

	spi_ptr += offsetof(struct psp_fw_hash_table_v2, fw_hash_256);

	hash_table_v2.fw_hash_256 = hash_256_v2;
	hash_table_v2.fw_hash_384 = hash_384_v2;
	len = sizeof(struct psp_fw_entry_hash_256_v2) * hash_table_v2.no_of_entries_256;
	memcpy(hash_256_v2, spi_ptr, len);

	spi_ptr += len;
	len = sizeof(struct psp_fw_entry_hash_384_v2) * hash_table_v2.no_of_entries_384;
	memcpy(hash_384_v2, spi_ptr, len);

	svc_set_fw_hash_table(cmd, &hash_table_v2);
}

static void update_one_psp_fw_hash_table(enum verstage_cmd_id cmd, const char *fname)
{
	void *hash_file = cbfs_map(fname, NULL);
	uint16_t version;

	if (!hash_file) {
		printk(BIOS_ERR, "AMD Firmware hash table %s not found\n", fname);
		/*
		 * If we don't supply hash table, the PSP will refuse to boot.
		 * So returning here is safe to do.
		 */
		return;
	}

	memcpy(&version, hash_file, sizeof(version));
	assert(version <= 2);
	switch (version) {
	case 1:
		update_one_psp_fw_hash_table_v1(cmd, hash_file);
		break;
	case 2:
		update_one_psp_fw_hash_table_v2(cmd, hash_file);
		break;
	default:
		printk(BIOS_ERR, "%s: Unexpected version %d\n", __func__, version);
	}
	cbfs_unmap(hash_file);
	rdev_munmap(boot_device_ro(), hash_file);
}

void update_psp_fw_hash_tables(void)
{
	struct vb2_context *ctx = vboot_get_context();

	if (vboot_is_firmware_slot_a(ctx)) {
		update_one_psp_fw_hash_table(CMD_SET_FW_HASH_TABLE_STAGE1,
					     PSP_FW_HASH_FILE_NAME("a", ""));
		update_one_psp_fw_hash_table(CMD_SET_FW_HASH_TABLE_STAGE2,
					     PSP_FW_HASH_FILE_NAME("a", "1"));
		update_one_psp_fw_hash_table(CMD_SET_FW_HASH_TABLE_TOS,
					     PSP_FW_HASH_FILE_NAME("a", "2"));
	} else {
		update_one_psp_fw_hash_table(CMD_SET_FW_HASH_TABLE_STAGE1,
					     PSP_FW_HASH_FILE_NAME("a", ""));
		update_one_psp_fw_hash_table(CMD_SET_FW_HASH_TABLE_STAGE2,
					     PSP_FW_HASH_FILE_NAME("a", "1"));
		update_one_psp_fw_hash_table(CMD_SET_FW_HASH_TABLE_TOS,
					     PSP_FW_HASH_FILE_NAME("a", "2"));
	}
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

void platform_report_mode(int developer_mode_enabled)
{
	printk(BIOS_INFO, "Reporting %s mode\n",
	       developer_mode_enabled ? "Developer" : "Normal");
	if (developer_mode_enabled)
		svc_set_platform_boot_mode(CHROME_BOOK_BOOT_MODE_DEVELOPER);
	else
		svc_set_platform_boot_mode(CHROME_BOOK_BOOT_MODE_NORMAL);
}

void report_prev_boot_status_to_vboot(void)
{
	uint32_t boot_status = 0;
	int ret;
	struct vb2_context *ctx = vboot_get_context();

	/* Already in recovery mode. No need to report previous boot status. */
	if (ctx->flags & VB2_CONTEXT_RECOVERY_MODE)
		return;

	ret = svc_get_prev_boot_status(&boot_status);
	if (ret != BL_OK || boot_status) {
		printk(BIOS_ERR, "PSPFW failure in previous boot: %d:%#8x\n", ret, boot_status);
		vbnv_init();
		vb2api_previous_boot_fail(ctx, VB2_RECOVERY_FW_VENDOR_BLOB,
					  boot_status ? (int)boot_status : ret);
	}
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
