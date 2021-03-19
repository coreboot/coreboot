/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <bootblock_common.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <delay.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>

void bootblock_mainboard_early_init(void)
{
	size_t num_gpios;
	uint32_t dword;
	const struct soc_amd_gpio *gpios;

	if (!CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)) {
		gpios = variant_early_gpio_table(&num_gpios);
		program_gpios(gpios, num_gpios);
	}

	printk(BIOS_DEBUG, "Bootblock configure eSPI\n");

	dword = pci_read_config32(SOC_LPC_DEV, 0x78);
	dword &= 0xFFFFF9F3;
	dword |= 0x200;
	pci_write_config32(SOC_LPC_DEV, 0x78, dword);
	pci_write_config32(SOC_LPC_DEV, 0x44, 0);
	pci_write_config32(SOC_LPC_DEV, 0x48, 0);

	dword = pm_read32(0x90);
	dword |= 1 << 16;
	pm_write32(0x90, dword);

	dword = pm_read32(0x74);
	dword |= 3 << 10;
	pm_write32(0x74, dword);

	if (CONFIG(GPIO_SIGN_OF_LIFE)) {
		for (int x = 0; x < 20; x++) {
			gpio_set(GPIO_31, 1);
			mdelay(10);
			gpio_set(GPIO_31, 0);
			mdelay(10);
		}
	}
}
