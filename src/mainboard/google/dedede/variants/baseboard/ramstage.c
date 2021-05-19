/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <drivers/usb/acpi/chip.h>
#include <fw_config.h>
#include <gpio.h>
#include <soc/pci_devs.h>

#define LTE_USB_PORT_ID 3
#define LTE_USB_PORT_TYPE 2

void update_lte_device(struct acpi_gpio *lte_reset_gpio, struct acpi_gpio *lte_enable_gpio)
{
	struct device *xhci, *hub = NULL, *port = NULL;
	struct drivers_usb_acpi_config *config;

	xhci = pcidev_path_on_root(PCH_DEVFN_XHCI);
	if (!xhci) {
		printk(BIOS_ERR, "%s: Could not locate XHCI device in DT\n", __func__);
		return;
	}

	while ((hub = dev_bus_each_child(xhci->link_list, hub)) != NULL) {
		while ((port = dev_bus_each_child(hub->link_list, port)) != NULL) {
			if (!port->chip_info || port->path.usb.port_id != LTE_USB_PORT_ID)
				continue;

			if (fw_config_probe(FW_CONFIG(DB_PORTS, DB_PORTS_LTE_HDMI)) ||
				fw_config_probe(FW_CONFIG(DB_PORTS, DB_PORTS_1C_LTE)) ||
				fw_config_probe(FW_CONFIG(DB_PORTS, DB_PORTS_1A_HDMI_LTE))) {

				config = port->chip_info;
				config->type = UPC_TYPE_INTERNAL;
				if (port->path.usb.port_type == LTE_USB_PORT_TYPE) {
					config->has_power_resource = 1;
					memcpy(&config->reset_gpio, &lte_reset_gpio,
								sizeof(config->reset_gpio));
					config->reset_off_delay_ms = 20;
					memcpy(&config->enable_gpio, &lte_enable_gpio,
								sizeof(config->enable_gpio));
					config->enable_delay_ms = 20;
				}
			}
		}
	}
}
