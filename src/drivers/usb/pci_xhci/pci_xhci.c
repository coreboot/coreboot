/* SPDX-License-Identifier: GPL-2.0-only */

#include "pci_xhci.h"
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/xhci.h>
#include <stdlib.h>
#include <string.h>

#define PCI_XHCI_CLASSCODE	0x0c0330 /* USB3.0 xHCI controller */

static unsigned int controller_count;
static const struct device_operations xhci_pci_ops;

struct port_counts {
	unsigned int high_speed;
	unsigned int super_speed;
};

__weak enum cb_err pci_xhci_get_wake_gpe(const struct device *dev, int *gpe)
{
	*gpe = -1;
	return CB_SUCCESS;
}

static const char *xhci_acpi_name(const struct device *dev)
{
	char *name;
	unsigned int port_id;

	if (dev->path.type == DEVICE_PATH_USB) {
		/* Ports index start at 1 */
		port_id = dev->path.usb.port_id + 1;

		switch (dev->path.usb.port_type) {
		case 0:
			return "RHUB";
		case 2:
			name = malloc(ACPI_NAME_BUFFER_SIZE);
			snprintf(name, ACPI_NAME_BUFFER_SIZE, "HS%02d", port_id);
			name[4] = '\0';

			return name;
		case 3:
			name = malloc(ACPI_NAME_BUFFER_SIZE);
			snprintf(name, ACPI_NAME_BUFFER_SIZE, "SS%02d", port_id);
			name[4] = '\0';
			return name;
		}
	} else if (dev->ops == &xhci_pci_ops) {
		return dev->name;
	}

	printk(BIOS_ERR, "%s: Unknown device %s\n", __func__, dev_path(dev));
	return NULL;
}

static void handle_xhci_ext_cap(void *context, const struct xhci_ext_cap *cap)
{
	const struct xhci_supported_protocol *data;
	const char *format;
	char buf[16];
	struct port_counts *counts = context;
	unsigned int *dev_num;

	if (cap->cap_id != XHCI_ECP_CAP_ID_SUPP)
		return;

	data = &cap->supported_protocol;

	xhci_print_supported_protocol(data);

	if (memcmp(data->name, "USB ", 4)) {
		printk(BIOS_INFO, "%s: Unknown Protocol: %.*s\n", __func__,
		       (int)sizeof(data->name), data->name);
		return;
	}

	if (data->major_rev == 3) {
		format = "SS%02d";
		dev_num = &counts->super_speed;
	} else if (data->major_rev == 2) {
		format = "HS%02d";
		dev_num = &counts->high_speed;
	} else {
		printk(BIOS_INFO, "%s: Unknown USB Version: %#x\n", __func__, data->major_rev);
		return;
	}

	for (unsigned int i = 0; i < data->port_count; ++i) {
		snprintf(buf, sizeof(buf), format, ++(*dev_num));
		acpigen_write_device(buf);
		acpigen_write_name_byte("_ADR", data->port_offset + i);
		acpigen_pop_len();
	}
}

static void xhci_add_devices(const struct device *dev)
{
	/* Used by the callback to track how many ports have been seen. */
	struct port_counts counts = {0, 0};

	acpigen_write_device("RHUB");
	acpigen_write_name_integer("_ADR", 0x00000000);

	xhci_for_each_ext_cap(dev, &counts, handle_xhci_ext_cap);

	acpigen_pop_len();
}

static void xhci_fill_ssdt(const struct device *dev)
{
	int gpe;

	printk(BIOS_DEBUG, "xHCI SSDT generation\n");

	acpigen_write_scope(acpi_device_scope(dev));
	acpigen_write_device(acpi_device_name(dev));

	acpigen_write_ADR_pci_device(dev);
	acpigen_write_name_string("_DDN", "xHC - Extensible Host Controller");
	acpigen_write_STA(acpi_device_status(dev));

	if (pci_xhci_get_wake_gpe(dev, &gpe) == CB_SUCCESS) {
		printk(BIOS_DEBUG, "%s: Got GPE %d for %s\n", __func__, gpe, dev_path(dev));
	} else {
		printk(BIOS_ERR, "%s: Error getting GPE for : %s\n", __func__, dev_path(dev));
		gpe = -1;
	}

	if (gpe > 0) {
		acpigen_write_PRW(gpe, SLP_TYP_S3);
		acpigen_write_name_integer("_S0W", ACPI_DEVICE_SLEEP_D0);
		acpigen_write_name_integer("_S3W", ACPI_DEVICE_SLEEP_D3_COLD);
		acpigen_write_name_integer("_S4W", ACPI_DEVICE_SLEEP_D3_COLD);
	}

	xhci_add_devices(dev);

	acpigen_pop_len();
	acpigen_pop_len();
}

static void xhci_enable(struct device *dev)
{
	char *name;
	uint32_t class = pci_read_config32(dev, PCI_CLASS_REVISION);
	/* Class code, the upper 3 bytes of PCI_CLASS_REVISION. */
	class >>= 8;

	if (class != PCI_XHCI_CLASSCODE) {
		printk(BIOS_ERR, "Incorrect xHCI class code: %#x\n", class);
		dev->enabled = 0;
		return;
	}

	name = malloc(ACPI_NAME_BUFFER_SIZE);
	snprintf(name, ACPI_NAME_BUFFER_SIZE, "XHC%d", controller_count++);
	dev->name = name;
}

static const struct device_operations xhci_pci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= pci_dev_init,
	.scan_bus		= scan_static_bus,
	.enable			= xhci_enable,
	.ops_pci		= &pci_dev_ops_pci,
	.acpi_fill_ssdt		= xhci_fill_ssdt,
	.acpi_name		= xhci_acpi_name,
};

static const unsigned short amd_pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_FAM17H_MODEL18H_XHCI0,
	PCI_DEVICE_ID_AMD_FAM17H_MODEL18H_XHCI1,
	PCI_DEVICE_ID_AMD_FAM17H_MODEL20H_XHCI0,
	0
};

static const struct pci_driver xhci_pci_driver __pci_driver = {
	.ops = &xhci_pci_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.devices = amd_pci_device_ids,
};
