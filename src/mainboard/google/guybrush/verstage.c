/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/acpimmio.h>
#include <amdblocks/gpio_banks.h>
#include <arch/io.h>
#include <baseboard/variants.h>
#include <security/vboot/vboot_common.h>

static void setup_gpio(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;

	if (CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)) {
		gpios = variant_early_gpio_table(&num_gpios);
		gpio_configure_pads(gpios, num_gpios);
	}
}

void verstage_mainboard_early_init(void)
{
	setup_gpio();

	/*
	 * TODO : Make common function in cezanne code and just call it
	 * when PCI access is fixed in the PSP (b/186602472).
	 * For now the PSP doesn't configure LPC so it should be fine.
	 */
	if (CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)) {
		uint32_t dword;
		printk(BIOS_DEBUG, "Verstage configure eSPI\n");
		dword = pm_io_read32(0x90);
		dword |= 1 << 16;
		pm_io_write32(0x90, dword);

		dword = pm_io_read32(0x74);
		dword |= 3 << 10;
		pm_io_write32(0x74, dword);
	}
}
