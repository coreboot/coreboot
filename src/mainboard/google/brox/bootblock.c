/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <bootmode.h>
#include <intelblocks/lpc_lib.h>
#include <reset.h>
#include <security/vboot/vboot_common.h>

void bootblock_mainboard_early_init(void)
{
	const struct pad_config *pads;
	size_t num;
	pads = variant_early_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

void bootblock_mainboard_init(void)
{
	if (platform_is_resuming()) {
		printk(BIOS_EMERG, "ACPI S3 resume is not supported on this platform\n");
		/* Prepare for reboot to clear the sleep state such that the board
		   is not stuck in reboot loop. */
		vboot_platform_prepare_reboot();
		board_reset();
	}
	variant_update_descriptor();

	if (CONFIG(EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_RANGE))
		lpc_open_mmio_window(CONFIG_EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_BASE,
			CONFIG_EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_SIZE);
}

void __weak variant_update_descriptor(void) {}
