/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_pld.h>
#include <acpi/acpigen.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <drivers/intel/gma/int15.h>
#include <option.h>
#include <smbios.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include "ec.h"
#include "onboard.h"

#define TP_TYPE_AUTO	0
#define TP_TYPE_ELAN	1
#define TP_TYPE_CYPRESS	2

void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	apm_control(APM_CNT_FINALIZE);
}

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

static void mainboard_fill_ssdt(const struct device *dev)
{
	if (!CONFIG(BOARD_GOOGLE_PEPPY))
		return;

	/* Get touchpad type option from CFR; enable both if not specified */
	unsigned int touchpad_type = get_uint_option("touchpad_type", TP_TYPE_AUTO);

	acpigen_write_scope("\\_SB.PCI0.I2C0");
	/* 0 = Enable both, 1 = Elan, 2 = Cypress */
	acpigen_write_name_integer("ETPD", touchpad_type != TP_TYPE_CYPRESS ? 1 : 0);
	acpigen_write_name_integer("CTPD", touchpad_type != TP_TYPE_ELAN ? 1 : 0);
	acpigen_pop_len(); /* Scope */

	/* SIM USB (PRT6) only when LTE/NGFF CFR option is enabled */
	if (get_uint_option("lte_ngff", 0)) {
		struct acpi_pld pld = {0};
		struct acpi_pld_group group = {0};

		acpi_pld_fill_usb(&pld, UPC_TYPE_PROPRIETARY, &group);
		/* Match prior ASL: GPLD(One) - SIM slot treated as visible */
		pld.visible = 1;

		acpigen_write_scope("\\_SB.PCI0.XHCI.HUB7.PRT6");
		acpigen_write_upc(UPC_TYPE_PROPRIETARY);
		acpigen_write_pld(&pld);
		acpigen_pop_len(); /* Scope */
	}
}

static int mainboard_smbios_data(struct device *dev, int *handle,
				 unsigned long *current)
{
	int len = 0;

	len += smbios_write_type41(
		current, handle,
		BOARD_LIGHTSENSOR_NAME,		/* name */
		BOARD_LIGHTSENSOR_IRQ,		/* instance */
		BOARD_LIGHTSENSOR_I2C_BUS,	/* segment */
		BOARD_LIGHTSENSOR_I2C_ADDR,	/* bus */
		0,				/* device */
		0,				/* function */
		SMBIOS_DEVICE_TYPE_OTHER);	/* device type */

	len += smbios_write_type41(
		current, handle,
		BOARD_TRACKPAD_NAME,		/* name */
		BOARD_TRACKPAD_IRQ,		/* instance */
		BOARD_TRACKPAD_I2C_BUS,		/* segment */
		BOARD_TRACKPAD_I2C_ADDR,	/* bus */
		0,				/* device */
		0,				/* function */
		SMBIOS_DEVICE_TYPE_OTHER);	/* device type */

	len += smbios_write_type41(
		current, handle,
		BOARD_TOUCHSCREEN_NAME,		/* name */
		BOARD_TOUCHSCREEN_IRQ,		/* instance */
		BOARD_TOUCHSCREEN_I2C_BUS,	/* segment */
		BOARD_TOUCHSCREEN_I2C_ADDR,	/* bus */
		0,				/* device */
		0,				/* function */
		SMBIOS_DEVICE_TYPE_OTHER);	/* device type */

	return len;
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_data = mainboard_smbios_data;
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_EDP, GMA_INT15_PANEL_FIT_CENTERING, GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

static void mainboard_chip_init(void *chip_info)
{
	struct device *dev;

	if (!CONFIG(BOARD_GOOGLE_PEPPY))
		return;

	/* PCIe RP2 (1c.1) for NGFF/LTE - present in overridetree, gated by CFR option */
	dev = pcidev_on_root(PCH_PCIE_DEV_SLOT, 1);
	if (dev)
		dev->enabled = get_uint_option("lte_ngff", 0);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
	.enable_dev = mainboard_enable,
};
