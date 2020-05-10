/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/smbus_host.h>
#include <southbridge/intel/common/pmclib.h>
#include <elog.h>
#include "pch.h"
#include "chip.h"

#if CONFIG(INTEL_LYNXPOINT_LP)
#include "lp_gpio.h"
#else
#include <southbridge/intel/common/gpio.h>
#endif

const struct rcba_config_instruction pch_early_config[] = {
	/* Enable IOAPIC */
	RCBA_SET_REG_16(OIC, 0x0100),
	/* PCH BWG says to read back the IOAPIC enable register */
	RCBA_READ_REG_16(OIC),

	RCBA_END_CONFIG,
};

int pch_is_lp(void)
{
	u8 id = pci_read_config8(PCH_LPC_DEV, PCI_DEVICE_ID + 1);
	return id == PCH_TYPE_LPT_LP;
}

static void pch_enable_bars(void)
{
	/* Setting up Southbridge. In the northbridge code. */
	pci_write_config32(PCH_LPC_DEV, RCBA, (uintptr_t)DEFAULT_RCBA | 1);

	pci_write_config32(PCH_LPC_DEV, PMBASE, DEFAULT_PMBASE | 1);
	/* Enable ACPI BAR */
	pci_write_config8(PCH_LPC_DEV, ACPI_CNTL, 0x80);

	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);

	/* Enable GPIO functionality. */
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);
}

static void pch_generic_setup(void)
{
	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	outw((1 << 11), DEFAULT_PMBASE | 0x60 | 0x08);	/* halt timer */
	printk(BIOS_DEBUG, " done.\n");
}

void pch_enable_lpc(void)
{
	const struct device *dev = pcidev_on_root(0x1f, 0);
	const struct southbridge_intel_lynxpoint_config *config = NULL;

	/* Set COM1/COM2 decode range */
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0010);

	/* Enable SuperIO + MC + COM1 + PS/2 Keyboard/Mouse */
	u16 lpc_config = CNF1_LPC_EN | CNF2_LPC_EN | GAMEL_LPC_EN |
		COMA_LPC_EN | KBC_LPC_EN | MC_LPC_EN;
	pci_write_config16(PCH_LPC_DEV, LPC_EN, lpc_config);

	/* Set up generic decode ranges */
	if (!dev)
		return;
	if (dev->chip_info)
		config = dev->chip_info;
	if (!config)
		return;

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, config->gen4_dec);
}

void __weak mainboard_config_superio(void)
{
}

int early_pch_init(const void *gpio_map,
		   const struct rcba_config_instruction *rcba_config)
{
	int wake_from_s3;

	pch_enable_bars();

#if CONFIG(INTEL_LYNXPOINT_LP)
	setup_pch_lp_gpios(gpio_map);
#else
	setup_pch_gpios(gpio_map);
#endif
	pch_generic_setup();

	/* Enable SMBus for reading SPDs. */
	enable_smbus();

	/* Early PCH RCBA settings */
	pch_config_rcba(pch_early_config);

	/* Mainboard RCBA settings */
	pch_config_rcba(rcba_config);

	wake_from_s3 = southbridge_detect_s3_resume();

	elog_boot_notify(wake_from_s3);

	/* Report if we are waking from s3. */
	return wake_from_s3;
}
