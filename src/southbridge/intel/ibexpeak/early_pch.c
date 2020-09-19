/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <northbridge/intel/ironlake/ironlake.h>
#include <southbridge/intel/ibexpeak/pch.h>
#include <southbridge/intel/common/gpio.h>

static void early_gpio_init(void)
{
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE | 1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);

	setup_pch_gpios(&mainboard_gpio_map);
}

static void pch_default_disable(void)
{
	/* Must set BIT0 (hides performance counters PCI device).
	   coreboot enables the Rate Matching Hub which makes the UHCI PCI
	   devices disappear, so BIT5-12 and BIT28 can be set to hide those. */
	RCBA32(FD) = (1 << 28) | (0xff << 5) | 1;

	/* Set reserved bit to 1 */
	RCBA32(FD2) = 1;
}

void ibexpeak_setup_bars(void)
{
	printk(BIOS_DEBUG, "Setting up static southbridge registers...");
	pci_write_config32(PCI_DEV(0, 0x1f, 0), RCBA, (uintptr_t)DEFAULT_RCBA | 1);

	pci_write_config32(PCI_DEV(0, 0x1f, 0), PMBASE, DEFAULT_PMBASE | 1);
	/* Enable ACPI BAR */
	pci_write_config8(PCH_LPC_DEV, ACPI_CNTL, ACPI_EN);

	printk(BIOS_DEBUG, " done.\n");

	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	/* No reset */
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);
	/* halt timer */
	outw((1 << 11), DEFAULT_PMBASE | 0x60 | 0x08);
	/* halt timer */
	outw(inw(DEFAULT_PMBASE | 0x60 | 0x06) | 2, DEFAULT_PMBASE | 0x60 | 0x06);
	printk(BIOS_DEBUG, " done.\n");
}

void early_pch_init(void)
{
	early_gpio_init();
	enable_smbus();
	/* TODO, make this configurable */
	pch_setup_cir(IRONLAKE_MOBILE);
	southbridge_configure_default_intmap();
	pch_default_disable();
	early_usb_init(mainboard_usb_ports);
}
