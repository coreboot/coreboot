/* SPDX-License-Identifier: GPL-2.0-only */

#include "pci_xhci.h"
#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/xhci.h>
#include <stdlib.h>

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

static void xhci_count_ports(void *context, const struct xhci_supported_protocol *data)
{
	struct port_counts *counts = context;

	switch (data->major_rev) {
	case 3:
		counts->super_speed += data->port_count;
		return;
	case 2:
		counts->high_speed += data->port_count;
		return;
	default:
		printk(BIOS_INFO, "%s: Unknown USB Version: %#x\n", __func__, data->major_rev);
		return;
	}
}

static bool xhci_port_exists(const struct device *dev, const struct usb_path *path)
{
	/* Cache the counts so we don't have to iterate on each invocation. */
	static struct {
		const struct device *dev;
		struct port_counts counts;
	} cache;

	if (cache.dev != dev) {
		cache.counts.high_speed = 0;
		cache.counts.super_speed = 0;
		cache.dev = dev;

		xhci_for_each_supported_usb_cap(dev, &cache.counts, xhci_count_ports);
	}

	/* port_ids are 0 based */
	switch (path->port_type) {
	case 3:
		return path->port_id < cache.counts.super_speed;
	case 2:
		return path->port_id < cache.counts.high_speed;
	default:
		printk(BIOS_INFO, "%s: Unknown USB Version: %#x\n", __func__, path->port_type);
		return false;
	}
}

static const struct device *get_xhci_dev(const struct device *dev)
{
	while (dev && dev->ops != &xhci_pci_ops) {
		if (dev->path.type == DEVICE_PATH_ROOT)
			return NULL;

		dev = dev->bus->dev;
	}

	return dev;
}

static const char *xhci_acpi_name(const struct device *dev)
{
	char *name;
	unsigned int port_id;
	const char *pattern;
	const struct device *xhci_dev;

	/* Generate ACPI names for the usb_acpi driver */
	if (dev->path.type == DEVICE_PATH_USB) {
		/* Ports index start at 1 */
		port_id = dev->path.usb.port_id + 1;

		switch (dev->path.usb.port_type) {
		case 0:
			return "RHUB";
		case 2:
			pattern = "HS%02d";
			break;
		case 3:
			pattern = "SS%02d";
			break;
		default:
			printk(BIOS_INFO, "%s: Unknown USB Version: %#x\n", __func__,
			       dev->path.usb.port_type);
			return NULL;
		}

		xhci_dev = get_xhci_dev(dev);
		if (!xhci_dev)
			die("%s: xHCI controller not found for %s\n", __func__, dev_path(dev));

		/*
		 * We only want to return an ACPI name for a USB port if the controller
		 * physically has the port. This has the desired side effect of making the
		 * usb_acpi driver skip generating an ACPI node for a device which has
		 * no port. This prevents writing an invalid SSDT table which the OS then
		 * complains about.
		 */
		if (!xhci_port_exists(xhci_dev, &dev->path.usb)) {
			printk(BIOS_WARNING, "%s: %s does not exist on xHC ", __func__,
			       dev_path(dev));
			/* dev_path uses a static buffer :( */
			printk(BIOS_WARNING, "%s\n", dev_path(xhci_dev));

			return NULL;
		}

		name = malloc(ACPI_NAME_BUFFER_SIZE);
		snprintf(name, ACPI_NAME_BUFFER_SIZE, pattern, port_id);
		name[4] = '\0';

		return name;
	} else if (dev->ops == &xhci_pci_ops) {
		return dev->name;
	}

	printk(BIOS_ERR, "%s: Unknown device %s\n", __func__, dev_path(dev));

	return NULL;
}

static void xhci_generate_port_acpi(void *context, const struct xhci_supported_protocol *data)
{
	const char *format;
	char buf[16];
	struct port_counts *counts = context;
	unsigned int *dev_num;

	xhci_print_supported_protocol(data);

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

	xhci_for_each_supported_usb_cap(dev, &counts, xhci_generate_port_acpi);

	acpigen_pop_len();
}

static void xhci_fill_ssdt(const struct device *dev)
{
	int gpe;
	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);

	if (!scope || !name)
		return;

	printk(BIOS_DEBUG, "xHCI SSDT generation\n");

	acpigen_write_scope(scope);
	acpigen_write_device(name);

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
	PCI_DID_AMD_FAM17H_MODEL18H_XHCI0,
	PCI_DID_AMD_FAM17H_MODEL18H_XHCI1,
	PCI_DID_AMD_FAM17H_MODEL20H_XHCI0,
	PCI_DID_AMD_FAM17H_MODEL60H_XHCI,
	PCI_DID_AMD_FAM17H_MODELA0H_XHCI0,
	PCI_DID_AMD_FAM17H_MODELA0H_XHCI1,
	PCI_DID_AMD_FAM17H_MODELA0H_XHCI2,
	0
};

static const struct pci_driver xhci_pci_driver __pci_driver = {
	.ops = &xhci_pci_ops,
	.vendor = PCI_VID_AMD,
	.devices = amd_pci_device_ids,
};
