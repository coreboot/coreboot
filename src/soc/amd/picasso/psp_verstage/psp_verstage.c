/* SPDX-License-Identifier: GPL-2.0-only */

#include "psp_verstage.h"

#include <bl_uapp/bl_syscall_public.h>
#include <boot_device.h>
#include <console/console.h>
#include <security/vboot/misc.h>
#include <security/vboot/symbols.h>
#include <security/vboot/vboot_common.h>
#include <arch/stages.h>
#include <stdarg.h>
#include <stdio.h>

#define RUN_PSP_SVC_TESTS 0

extern char _bss_start, _bss_end;
static struct mem_region_device boot_dev =
		MEM_REGION_DEV_RO_INIT(NULL, CONFIG_ROM_SIZE);

void __weak verstage_mainboard_early_init(void) {}
void __weak verstage_mainboard_init(void) {}

static void reboot_into_recovery(struct vb2_context *ctx, uint32_t subcode)
{
	subcode += PSP_VBOOT_ERROR_SUBCODE;
	svc_write_postcode(subcode);

	vb2api_fail(ctx, VB2_RECOVERY_RO_UNSPECIFIED, (int)subcode);
	vboot_save_data(ctx);

	svc_debug_print("Rebooting into recovery\n");
	vboot_reboot();
}

/*
 * Tell the PSP where to load the rest of the firmware from
 */
static uint32_t update_boot_region(struct vb2_context *ctx)
{
	struct psp_ef_table *ef_table;
	uint32_t psp_dir_addr, bios_dir_addr;
	uint32_t *psp_dir_in_spi, *bios_dir_in_spi;

	/* Continue booting from RO */
	if (ctx->flags & VB2_CONTEXT_RECOVERY_MODE) {
		printk(BIOS_ERR, "In recovery mode. Staying in RO.\n");
		return 0;
	}

	if (vboot_is_firmware_slot_a(ctx)) {
		printk(BIOS_SPEW, "Using FMAP RW_A region.\n");
		ef_table = (struct psp_ef_table *)((CONFIG_PICASSO_FW_A_POSITION &
						SPI_ADDR_MASK) + (uint32_t)boot_dev.base);
	} else {
		printk(BIOS_SPEW, "Using FMAP RW_B region.\n");
		ef_table = (struct psp_ef_table *)((CONFIG_PICASSO_FW_B_POSITION &
						SPI_ADDR_MASK) + (uint32_t)boot_dev.base);
	}

	if (ef_table->signature != EMBEDDED_FW_SIGNATURE) {
		printk(BIOS_ERR, "Error: ROMSIG address is not correct.\n");
		return POSTCODE_ROMSIG_MISMATCH_ERROR;
	}

	psp_dir_addr = ef_table->psp_table;
	bios_dir_addr = ef_table->bios1_entry;
	psp_dir_in_spi = (uint32_t *)((psp_dir_addr & SPI_ADDR_MASK) +
			(uint32_t)boot_dev.base);
	bios_dir_in_spi = (uint32_t *)((bios_dir_addr & SPI_ADDR_MASK) +
			(uint32_t)boot_dev.base);
	if (*psp_dir_in_spi != PSP_COOKIE) {
		printk(BIOS_ERR, "Error: PSP Directory address is not correct.\n");
		return POSTCODE_PSP_COOKIE_MISMATCH_ERROR;
	}
	if (*bios_dir_in_spi != BDT1_COOKIE) {
		printk(BIOS_ERR, "Error: BIOS Directory address is not correct.\n");
		return POSTCODE_BDT1_COOKIE_MISMATCH_ERROR;
	}

	if (svc_update_psp_bios_dir((void *)&psp_dir_addr,
			(void *)&bios_dir_addr, DIR_OFFSET_SET)) {
		printk(BIOS_ERR, "Error: Updated BIOS Directory could not be set.\n");
		return POSTCODE_UPDATE_PSP_BIOS_DIR_ERROR;
	}

	return 0;
}

/*
 * Save workbuf (and soon memory console and timestamps) to the bootloader to pass
 * back to coreboot.
 */
static uint32_t save_buffers(struct vb2_context **ctx)
{
	uint32_t retval;
	uint32_t buffer_size = DEFAULT_WORKBUF_TRANSFER_SIZE;
	uint32_t max_buffer_size;

	/*
	 * This should never fail, but if it does, we should still try to
	 * save the buffer. If that fails, then we should go to recovery mode.
	 */
	if (svc_get_max_workbuf_size(&max_buffer_size)) {
		post_code(POSTCODE_DEFAULT_BUFFER_SIZE_NOTICE);
		printk(BIOS_NOTICE, "Notice: using default transfer buffer size.\n");
		max_buffer_size = DEFAULT_WORKBUF_TRANSFER_SIZE;
	}
	printk(BIOS_DEBUG, "\nMaximum buffer size: %d bytes\n", max_buffer_size);

	retval = vb2api_relocate(_vboot2_work, _vboot2_work, buffer_size, ctx);
	if (retval != VB2_SUCCESS) {
		printk(BIOS_ERR, "Error shrinking workbuf. Error code %#x\n", retval);
		buffer_size = VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE;
		post_code(POSTCODE_WORKBUF_RESIZE_WARNING);
	}

	if (buffer_size > max_buffer_size) {
		printk(BIOS_ERR, "Error: Workbuf is larger than max buffer size.\n");
		post_code(POSTCODE_WORKBUF_BUFFER_SIZE_ERROR);
		return POSTCODE_WORKBUF_BUFFER_SIZE_ERROR;
	}

	retval = svc_save_uapp_data(UAPP_COPYBUF_CHROME_WORKBUF, (void *)_vboot2_work,
			buffer_size);
	if (retval) {
		printk(BIOS_ERR, "Error: Could not save workbuf. Error code 0x%08x\n",
				retval);
		return POSTCODE_WORKBUF_SAVE_ERROR;
	}

	return 0;
}

void Main(void)
{
	uint32_t retval;
	struct vb2_context *ctx = NULL;

	/*
	 * Do not use printk() before console_init()
	 * Do not use post_code() before verstage_mainboard_init()
	 */
	svc_write_postcode(POSTCODE_ENTERED_PSP_VERSTAGE);
	svc_debug_print("Entering verstage on PSP\n");
	memset(&_bss_start, '\0', &_bss_end - &_bss_start);

	svc_write_postcode(POSTCODE_CONSOLE_INIT);
	console_init();

	svc_write_postcode(POSTCODE_EARLY_INIT);
	retval = verstage_soc_early_init();
	if (retval) {
		svc_debug_print("verstage_soc_early_init failed\n");
		reboot_into_recovery(NULL, retval);
	}
	svc_debug_print("calling verstage_mainboard_early_init\n");

	verstage_mainboard_early_init();

	svc_write_postcode(POSTCODE_LATE_INIT);
	sb_enable_legacy_io();
	verstage_soc_init();
	verstage_mainboard_init();

	post_code(POSTCODE_VERSTAGE_MAIN);

	verstage_main();

	post_code(POSTCODE_SAVE_BUFFERS);
	retval = save_buffers(&ctx);
	if (retval)
		goto err;

	post_code(POSTCODE_UPDATE_BOOT_REGION);
	retval = update_boot_region(ctx);
	if (retval)
		goto err;

	post_code(POSTCODE_UNMAP_SPI_ROM);
	if (boot_dev.base) {
		if (svc_unmap_spi_rom((void *)boot_dev.base))
			printk(BIOS_ERR, "Error unmapping SPI rom\n");
	}

	post_code(POSTCODE_UNMAP_FCH_DEVICES);
	unmap_fch_devices();

	post_code(POSTCODE_LEAVING_VERSTAGE);

	printk(BIOS_DEBUG, "Leaving verstage on PSP\n");
	svc_exit(retval);

err:
	reboot_into_recovery(ctx, retval);
}

const struct region_device *boot_device_ro(void)
{
	uintptr_t *addr;

	addr = map_spi_rom();
	mem_region_device_ro_init(&boot_dev, (void *)addr, CONFIG_ROM_SIZE);

	return &boot_dev.rdev;
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
