/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/common/pmbase.h>
#include <southbridge/intel/common/pmutil.h>
#include "i82801jx.h"
#include "chip.h"

void i82801jx_lpc_setup(void)
{
	const pci_devfn_t d31f0 = PCI_DEV(0, 0x1f, 0);
	const struct device *dev = pcidev_on_root(0x1f, 0);
	const struct southbridge_intel_i82801jx_config *config;

	/* Configure serial IRQs.*/
	pci_write_config8(d31f0, D31F0_SERIRQ_CNTL, 0xd0);
	/*
	 * Enable some common LPC IO ranges:
	 * - 0x2e/0x2f, 0x4e/0x4f often SuperIO
	 * - 0x60/0x64, 0x62/0x66 often KBC/EC
	 * - 0x3f0-0x3f5/0x3f7 FDD
	 * - 0x378-0x37f and 0x778-0x77f LPT
	 * - 0x2f8-0x2ff COMB
	 * - 0x3f8-0x3ff COMA
	 * - 0x208-0x20f GAMEH
	 * - 0x200-0x207 GAMEL
	 */
	pci_write_config16(d31f0, D31F0_LPC_IODEC, 0x0010);
	pci_write_config16(d31f0, D31F0_LPC_EN, CNF2_LPC_EN | CNF1_LPC_EN
			   | MC_LPC_EN | KBC_LPC_EN | GAMEH_LPC_EN
			   | GAMEL_LPC_EN | FDD_LPC_EN | LPT_LPC_EN
			   | COMB_LPC_EN | COMA_LPC_EN);

	/* Set up generic decode ranges */
	if (!dev || !dev->chip_info)
		return;
	config = dev->chip_info;

	pci_write_config32(d31f0, D31F0_GEN1_DEC, config->gen1_dec);
	pci_write_config32(d31f0, D31F0_GEN2_DEC, config->gen2_dec);
	pci_write_config32(d31f0, D31F0_GEN3_DEC, config->gen3_dec);
	pci_write_config32(d31f0, D31F0_GEN4_DEC, config->gen4_dec);
}

void i82801jx_setup_bars(void)
{
	const pci_devfn_t d31f0 = PCI_DEV(0, 0x1f, 0);

	/* Set up RCBA. */
	pci_write_config32(d31f0, RCBA, CONFIG_FIXED_RCBA_MMIO_BASE | 1);

	/* Set up PMBASE. */
	pci_write_config32(d31f0, D31F0_PMBASE, DEFAULT_PMBASE | 1);
	/* Enable PMBASE. */
	pci_write_config8(d31f0, D31F0_ACPI_CNTL, 0x80);

	/* Set up GPIOBASE. */
	pci_write_config32(d31f0, D31F0_GPIO_BASE, DEFAULT_GPIOBASE);
	/* Enable GPIO. */
	pci_or_config8(d31f0, D31F0_GPIO_CNTL, 0x10);
}

#define TCO_BASE 0x60

void i82801jx_early_init(void)
{
	const pci_devfn_t d31f0 = PCI_DEV(0, 0x1f, 0);

	if (ENV_RAMINIT)
		enable_smbus();

	printk(BIOS_DEBUG, "Setting up static southbridge registers...");
	i82801jx_setup_bars();
	printk(BIOS_DEBUG, " done.\n");

	setup_pch_gpios(&mainboard_gpio_map);

	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	write_pmbase16(TCO_BASE + 0x8, (1 << 11));	/* halt timer */
	write_pmbase16(TCO_BASE + 0x4, (1 << 3));	/* clear timeout */
	write_pmbase16(TCO_BASE + 0x6, (1 << 1));	/* clear 2nd timeout */
	printk(BIOS_DEBUG, " done.\n");

	/* Enable IOAPIC */
	RCBA8(OIC) = 0x3;
	RCBA8(OIC);

	/* Initialize power management initialization
	   register early as it affects reboot behavior. */
	/* Bit 20 activates global reset of host and ME on cf9 writes of 0x6
	   and 0xe (required if ME is disabled but present), bit 31 locks it.
	   The other bits are 'must write'. */
	u8 reg8 = pci_read_config8(d31f0, 0xac);

	/* FIXME: It's a 8-bit variable!!! */
	reg8 |= (1 << 31) | (1 << 30) | (1 << 20) | (3 << 8);
	pci_write_config8(d31f0, 0xac, reg8);

	/* TODO: If RTC power failed, reset RTC state machine
		(set, then reset RTC 0x0b bit7) */

	/* TODO: Check power state bits in GEN_PMCON_2 (D31F0 0xa2)
		before they get cleared. */
}
