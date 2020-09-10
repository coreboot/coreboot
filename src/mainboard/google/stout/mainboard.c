/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_ops.h>
#include <drivers/intel/gma/int15.h>
#include <acpi/acpi.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <ec/quanta/it8518/ec.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "ec.h"
#include "onboard.h"

void mainboard_suspend_resume(void)
{
	/* Stout EC needs to be put back in ACPI mode */
	ec_write_cmd(EC_CMD_NOTIFY_ACPI_ENTER);
}

static void mainboard_init(struct device *dev)
{
	struct device *ethernet_dev = NULL;

	/* Initialize the Embedded Controller */
	stout_ec_init();

	/*
	 * Battery life time - LAN PCIe should enter ASPM L1 to save
	 * power when LAN connection is idle.
	 * enable CLKREQ: LAN pci config space 0x81h=01
	 */
	ethernet_dev = dev_find_device(STOUT_NIC_VENDOR_ID,
				       STOUT_NIC_DEVICE_ID, dev);

	if (ethernet_dev != NULL)
		pci_write_config8(ethernet_dev, 0x81, 0x01);
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_DEFAULT, GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
