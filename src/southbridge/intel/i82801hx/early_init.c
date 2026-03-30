/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel ICH8-M (82801HBM) Southbridge - Early Initialization
 *
 * Reference: coreboot ICH9 (i82801ix) early_init.c
 */

#include <stdint.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <southbridge/intel/i82801hx/i82801hx.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/common/pmutil.h>
#include <southbridge/intel/common/pmbase.h>
#include "chip.h"

void i82801hx_lpc_setup(void)
{
	/* Configure serial IRQ: continuous mode, 21 frames */
	pci_write_config8(D31F0, D31F0_SERIRQ_CNTL, 0xd0);

	/* LPC I/O decode ranges */
	pci_write_config16(D31F0, D31F0_LPC_IODEC, 0x0010);

	/*
	 * Enable LPC I/O decode for:
	 *   CNF2 (0x4E/0x4F) - SuperIO config
	 *   CNF1 (0x2E/0x2F) - SuperIO config
	 *   MC   (0x62/0x66) - Embedded Controller
	 *   KBC  (0x60/0x64) - Keyboard Controller
	 *   COMB (0x2F8)      - Serial port
	 *   COMA (0x3F8)      - Serial port
	 */
	pci_write_config16(D31F0, D31F0_LPC_EN,
			   LPC_EN_CNF2 | LPC_EN_CNF1 | LPC_EN_MC | LPC_EN_KBC | LPC_EN_COMB |
				   LPC_EN_COMA);

	/* Generic decode ranges from chip config (devicetree). */
	const struct device *dev = pcidev_on_root(0x1f, 0);
	const struct southbridge_intel_i82801hx_config *config;

	if (dev && dev->chip_info) {
		config = dev->chip_info;
		pci_write_config32(D31F0, D31F0_GEN1_DEC, config->gen1_dec);
		pci_write_config32(D31F0, D31F0_GEN2_DEC, config->gen2_dec);
		pci_write_config32(D31F0, D31F0_GEN3_DEC, config->gen3_dec);
		pci_write_config32(D31F0, D31F0_GEN4_DEC, config->gen4_dec);
	}
}

/* SATA mode is configured by the ramstage sata.c driver. */
int i82801hx_detect_s3_resume(void)
{
	uint16_t pm1_cnt;
	uint8_t gen_pmcon_3;

	/* Check GEN_PMCON_3 for power failure (would invalidate S3) */
	gen_pmcon_3 = pci_read_config8(D31F0, D31F0_GEN_PMCON_3);
	if (gen_pmcon_3 & (1 << 1)) /* PWR_FLR - power failure */
		return 0;

	/* Read PM1_CNT to check sleep type */
	pm1_cnt = inw(DEFAULT_PMBASE + PM1_CNT);

	/* SLP_TYP field is bits [12:10] */
	uint8_t slp_typ = (pm1_cnt >> 10) & 7;

	/* SLP_TYP = 5 means S3 (STR) */
	return (slp_typ == 5) ? 1 : 0;
}

#define TCO_BASE 0x60

void i82801hx_early_init(void)
{
	/* Set up RCBA (Root Complex Base Address, D31:F0 offset 0xF0) */
	pci_write_config32(D31F0, D31F0_RCBA, RCBA_BASE | 1);

	/* Set up PMBASE (Power Management Base, D31:F0 offset 0x40) */
	pci_write_config32(D31F0, D31F0_PMBASE, DEFAULT_PMBASE);
	/* Enable ACPI I/O decode (D31:F0 offset 0x44 bit 7) */
	pci_write_config8(D31F0, D31F0_ACPI_CNTL, 0x80);

	/* Set up GPIOBASE (D31:F0 offset 0x48) */
	pci_write_config32(D31F0, D31F0_GPIO_BASE, DEFAULT_GPIOBASE);
	/* Enable GPIO I/O decode (D31:F0 offset 0x4C bit 4) */
	pci_or_config8(D31F0, D31F0_GPIO_CNTL, 0x10);

	setup_pch_gpios(&mainboard_gpio_map);

	/*
	 * Reset TCO watchdog.
	 * If the watchdog fires before we get here, the system will
	 * keep rebooting. Clear TCO status to prevent this.
	 */
	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(RCBA_GCS) = RCBA32(RCBA_GCS) | (1 << 5); /* No reset */
	write_pmbase16(TCO_BASE + 0x8, (1 << 11));      /* halt timer */
	write_pmbase16(TCO_BASE + 0x4, (1 << 3));       /* clear timeout */
	write_pmbase16(TCO_BASE + 0x6, (1 << 1));       /* clear 2nd timeout */
	printk(BIOS_DEBUG, " done.\n");

	/*
	 * Enable upper 128 bytes of CMOS (RCBA offset 0x3400).
	 * Bit 2 enables the extended CMOS range.
	 */
	RCBA32(RCBA_RC) = (1 << 2);
}
