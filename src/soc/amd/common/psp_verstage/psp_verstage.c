/* SPDX-License-Identifier: GPL-2.0-only */

#include "psp_verstage.h"

#include <amdblocks/acpimmio.h>
#include <bl_uapp/bl_errorcodes_public.h>
#include <bl_uapp/bl_syscall_public.h>
#include <boot_device.h>
#include <cbfs.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <fmap.h>
#include <pc80/mc146818rtc.h>
#include <soc/iomap.h>
#include <soc/psp_transfer.h>
#include <security/tpm/tspi.h>
#include <security/tpm/tss.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/misc.h>
#include <security/vboot/symbols.h>
#include <security/vboot/vboot_common.h>
#include <arch/stages.h>
#include <stdarg.h>
#include <stdio.h>
#include <timestamp.h>

extern char _bss_start, _bss_end;

void __weak verstage_mainboard_init(void) {}

static void reboot_into_recovery(struct vb2_context *ctx, uint32_t subcode)
{
	subcode += PSP_VBOOT_ERROR_SUBCODE;
	svc_write_postcode(subcode);

	/*
	 * If there's an error but the PSP_verstage is already booting to RO,
	 * don't reset the system.  It may be that the error is fatal, but if
	 * the system is stuck, don't intentionally force it into a reboot loop.
	 */
	if (ctx->flags & VB2_CONTEXT_RECOVERY_MODE) {
		printk(BIOS_ERR, "Already in recovery mode. Staying in RO.\n");
		return;
	}

	svc_debug_print("Rebooting into recovery\n");
	vboot_fail_and_reboot(ctx, VB2_RECOVERY_RO_UNSPECIFIED, (int)subcode);
}

static uint32_t check_cmos_recovery(void)
{
	/* Only reset if cmos is valid */
	if (vbnv_cmos_failed())
		return 0;

	/* If the byte is set, clear it, then return error to reboot */
	if (cmos_read(CMOS_RECOVERY_BYTE) == CMOS_RECOVERY_MAGIC_VAL) {
		cmos_write(0x00, CMOS_RECOVERY_BYTE);
		printk(BIOS_DEBUG, "Reboot into recovery requested by coreboot\n");
		return POSTCODE_CMOS_RECOVERY;
	}

	return 0;
}

/*
 * Tell the PSP where to load the rest of the firmware from
 */
static uint32_t update_boot_region(struct vb2_context *ctx)
{
	struct embedded_firmware *ef_table;
	uint32_t psp_dir_addr, bios_dir_addr;
	uint32_t *psp_dir_in_spi, *bios_dir_in_spi;
	const char *fname;
	const char *hash_fname;
	void *amdfw_location;
	void *boot_dev_base = rdev_mmap_full(boot_device_ro());

	/* Continue booting from RO */
	if (ctx->flags & VB2_CONTEXT_RECOVERY_MODE) {
		printk(BIOS_ERR, "In recovery mode. Staying in RO.\n");
		return 0;
	}

	if (vboot_is_firmware_slot_a(ctx)) {
		fname = "apu/amdfw_a";
		hash_fname = "apu/amdfw_a_hash";
	} else {
		fname = "apu/amdfw_b";
		hash_fname = "apu/amdfw_b_hash";
	}

	amdfw_location = cbfs_map(fname, NULL);
	if (!amdfw_location) {
		printk(BIOS_ERR, "AMD Firmware table not found.\n");
		return POSTCODE_AMD_FW_MISSING;
	}
	ef_table = (struct embedded_firmware *)amdfw_location;
	if (ef_table->signature != EMBEDDED_FW_SIGNATURE) {
		printk(BIOS_ERR, "ROMSIG address is not correct.\n");
		return POSTCODE_ROMSIG_MISMATCH_ERROR;
	}

	psp_dir_addr = ef_table->combo_psp_directory;
	bios_dir_addr = get_bios_dir_addr(ef_table);
	psp_dir_in_spi = (uint32_t *)((psp_dir_addr & SPI_ADDR_MASK) +
			(uint32_t)boot_dev_base);
	if (*psp_dir_in_spi != PSP_COOKIE) {
		printk(BIOS_ERR, "PSP Directory address is not correct.\n");
		return POSTCODE_PSP_COOKIE_MISMATCH_ERROR;
	}

	if (bios_dir_addr) {
		bios_dir_in_spi = (uint32_t *)((bios_dir_addr & SPI_ADDR_MASK) +
				(uint32_t)boot_dev_base);
		if (*bios_dir_in_spi != BHD_COOKIE) {
			printk(BIOS_ERR, "BIOS Directory address is not correct.\n");
			return POSTCODE_BHD_COOKIE_MISMATCH_ERROR;
		}
	}

	/* EFS2 uses relative address and PSP isn't happy with that */
	if (ef_table->efs_gen.gen == EFS_SECOND_GEN &&
			!CONFIG(PSP_SUPPORTS_EFS2_RELATIVE_ADDR)) {
		psp_dir_addr = FLASH_BASE_ADDR + (psp_dir_addr & SPI_ADDR_MASK);
		bios_dir_addr = FLASH_BASE_ADDR + (bios_dir_addr & SPI_ADDR_MASK);
	}

	if (update_psp_bios_dir(&psp_dir_addr, &bios_dir_addr)) {
		printk(BIOS_ERR, "Updated BIOS Directory could not be set.\n");
		return POSTCODE_UPDATE_PSP_BIOS_DIR_ERROR;
	}

	if (CONFIG(SEPARATE_SIGNED_PSPFW))
		update_psp_fw_hash_table(hash_fname);

	return 0;
}

/*
 * Save workbuf (and soon memory console and timestamps) to the bootloader to pass
 * back to coreboot.
 */
static uint32_t save_buffers(void)
{
	uint32_t retval;
	uint32_t buffer_size;
	struct transfer_info_struct buffer_info = {0};

	buffer_size =
		(uint32_t)((uintptr_t)_etransfer_buffer - (uintptr_t)_transfer_buffer);

	buffer_info.console_offset = (uint32_t)((uintptr_t)_preram_cbmem_console -
				(uintptr_t)_transfer_buffer);
	buffer_info.timestamp_offset = (uint32_t)((uintptr_t)_timestamp -
				(uintptr_t)_transfer_buffer);
	buffer_info.fmap_offset = (uint32_t)((uintptr_t)_fmap_cache -
				(uintptr_t)_transfer_buffer);

	buffer_info.magic_val = TRANSFER_MAGIC_VAL;
	buffer_info.struct_bytes = sizeof(buffer_info);
	buffer_info.buffer_size = buffer_size;
	buffer_info.workbuf_offset = (uint32_t)((uintptr_t)_fmap_cache -
					(uintptr_t)_vboot2_work);

	memcpy(_transfer_buffer, &buffer_info, sizeof(buffer_info));

	retval = save_uapp_data((void *)_transfer_buffer, buffer_size);
	if (retval) {
		printk(BIOS_ERR, "Could not save workbuf. Error code 0x%08x\n", retval);
		return POSTCODE_WORKBUF_SAVE_ERROR;
	}

	return 0;
}

/*
 * S0i3 resume in PSP verstage is a special case. The FSDL is restoring mostly
 * everything, so do the minimum necessary here. Unlike normal boot, subsequent
 * coreboot stages are not run after s0i3 verstage.
 * If the TPM is reset in S0i3, it must be re-initialized here.
 */
static void psp_verstage_s0i3_resume(void)
{
	uint32_t rv;

	post_code(POSTCODE_VERSTAGE_S0I3_RESUME);

	printk(BIOS_DEBUG, "Entering PSP verstage S0i3 resume\n");

	if (!CONFIG(PSP_INIT_TPM_ON_S0I3_RESUME))
		return;

	rv = tpm_setup(true);
	if (rv != TPM_SUCCESS) {
		printk(BIOS_ERR, "tpm_setup failed rv:%d\n", rv);
		reboot_into_recovery(vboot_get_context(), POSTCODE_INIT_TPM_FAILED);
	}

	rv = tlcl_disable_platform_hierarchy();
	if (rv != TPM_SUCCESS) {
		printk(BIOS_ERR, "tlcl_disable_platform_hierarchy failed rv:%d\n", rv);
		reboot_into_recovery(vboot_get_context(), POSTCODE_INIT_TPM_FAILED);
	}
}

void Main(void)
{
	uint32_t retval;
	struct vb2_context *ctx = NULL;
	void *boot_dev_base;
	uint32_t bootmode;

	/*
	 * Do not use printk() before console_init()
	 * Do not use post_code() before verstage_mainboard_init()
	 * Do not use svc_write_postcode before verstage_soc_espi_init() if PSP uses ESPI
	 * to write postcodes.
	 */
	timestamp_init(timestamp_get());
	if (!CONFIG(PSP_POSTCODES_ON_ESPI))
		svc_write_postcode(POSTCODE_ENTERED_PSP_VERSTAGE);
	svc_debug_print("Entering verstage on PSP\n");
	memset(&_bss_start, '\0', &_bss_end - &_bss_start);

	if (!CONFIG(PSP_POSTCODES_ON_ESPI))
		svc_write_postcode(POSTCODE_CONSOLE_INIT);
	console_init();

	if (CONFIG(PSP_INCLUDES_HSP))
		report_hsp_secure_state();

	if (!CONFIG(PSP_POSTCODES_ON_ESPI))
		svc_write_postcode(POSTCODE_EARLY_INIT);
	retval = verstage_soc_early_init();
	if (retval) {
		/*
		 * If verstage_soc_early_init fails, cmos is probably not
		 * accessible, so rebooting into recovery is not an option.
		 * Just reboot and hope for the best.
		 */
		svc_write_postcode(POSTCODE_EARLY_INIT_ERROR);
		svc_debug_print("verstage_soc_early_init failed! -- rebooting\n");
		vboot_reboot();
	}

	printk(BIOS_DEBUG, "calling verstage_mainboard_espi_init\n");
	verstage_mainboard_espi_init();

	printk(BIOS_DEBUG, "calling verstage_soc_espi_init\n");
	verstage_soc_espi_init();

	printk(BIOS_DEBUG, "calling verstage_mainboard_tpm_init\n");
	/* mainboard_tpm_init may check board_id, so make sure espi is ready first */
	verstage_mainboard_tpm_init();

	printk(BIOS_DEBUG, "calling verstage_soc_aoac_init\n");
	verstage_soc_aoac_init();

	printk(BIOS_DEBUG, "calling verstage_soc_i2c_init\n");
	verstage_soc_i2c_init();

	/*
	 * S0i3 resume in PSP verstage is a special case, handle it separately.
	 * Make sure TPM i2c is ready first.
	 */
	svc_get_boot_mode(&bootmode);
	if (bootmode == PSP_BOOT_MODE_S0i3_RESUME) {
		psp_verstage_s0i3_resume();

		post_code(POSTCODE_SAVE_BUFFERS);
		retval = save_buffers();
		if (retval)
			post_code(retval);

		post_code(POSTCODE_UNMAP_FCH_DEVICES);
		unmap_fch_devices();

		post_code(POSTCODE_LEAVING_VERSTAGE);
		svc_exit(0);
	}

	printk(BIOS_DEBUG, "calling verstage_mainboard_early_init\n");
	verstage_mainboard_early_init();

	svc_write_postcode(POSTCODE_LATE_INIT);
	fch_io_enable_legacy_io();

	printk(BIOS_DEBUG, "calling verstage_soc_spi_init\n");
	verstage_soc_spi_init();

	verstage_mainboard_init();

	post_code(POSTCODE_VERSTAGE_MAIN);
	if (CONFIG(SEPARATE_SIGNED_PSPFW))
		report_prev_boot_status_to_vboot();

	vboot_run_logic();

	ctx = vboot_get_context();
	retval = check_cmos_recovery();
	if (retval)
		reboot_into_recovery(ctx, retval);

	platform_report_mode(vboot_developer_mode_enabled());

	post_code(POSTCODE_UPDATE_BOOT_REGION);

	/*
	 * Since psp_verstage doesn't load next stage we never call
	 * any cbfs API on RO path. However we still need to initialize
	 * RO CBFS MCACHE manually to pass it in transfer_buffer.
	 * In RW path, MCACHE build will be skipped for RO region since
	 * we already built here.
	 */
	cbfs_get_boot_device(true);

	retval = update_boot_region(ctx);
	if (retval)
		reboot_into_recovery(ctx, retval);

	post_code(POSTCODE_SAVE_BUFFERS);
	retval = save_buffers();
	if (retval)
		reboot_into_recovery(ctx, retval);


	post_code(POSTCODE_UNMAP_SPI_ROM);
	boot_dev_base = rdev_mmap_full(boot_device_ro());
	if (boot_dev_base) {
		if (svc_unmap_spi_rom((void *)boot_dev_base))
			printk(BIOS_ERR, "Error unmapping SPI rom\n");
	}

	post_code(POSTCODE_UNMAP_FCH_DEVICES);
	unmap_fch_devices();

	post_code(POSTCODE_LEAVING_VERSTAGE);

	printk(BIOS_DEBUG, "Leaving verstage on PSP\n");
	svc_exit(retval);
}

/*
 * The stage_entry function is not used directly, but stage_entry() is marked as an entry
 * point in arm/arch/header.h, so if stage_entry() isn't present and calling Main(), all
 * the verstage code gets dropped by the linker.  Slightly hacky, but mostly harmless.
 */
void stage_entry(uintptr_t stage_arg)
{
	Main();
}
