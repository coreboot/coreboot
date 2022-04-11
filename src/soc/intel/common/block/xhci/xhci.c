/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <drivers/usb/acpi/chip.h>
#include <intelblocks/acpi.h>
#include <intelblocks/xhci.h>
#include <soc/pci_devs.h>

#define XHCI_USB2	2
#define XHCI_USB3	3

#define XHCI_USBCMD	0x80
#define  USBCMD_HCRST	(1 << 1)

/* Current Connect Status */
#define XHCI_STATUS_CCS		(1 << 0)

static uint8_t *xhci_mem_base(void)
{
	uint32_t mem_base = pci_read_config32(PCH_DEV_XHCI, PCI_BASE_ADDRESS_0);

	/* Check if the controller is disabled or not present */
	if (mem_base == 0 || mem_base == 0xffffffff)
		return 0;

	return (uint8_t *)(mem_base & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK);
}

void xhci_host_reset(void)
{
	uint8_t *xhci_base = xhci_mem_base();
	if (!xhci_base)
		return;

	setbits8(xhci_base + XHCI_USBCMD, USBCMD_HCRST);
}

#if ENV_RAMSTAGE
static bool is_usb_port_connected(const struct xhci_usb_info *info,
			unsigned int port_type, unsigned int port_id)
{
	uintptr_t port_sts_reg;
	uint32_t port_status;
	const struct resource *res;

	/* Support only USB2 or USB3 ports */
	if (!(port_type == XHCI_USB2 || port_type == XHCI_USB3))
		return false;

	/* Mark out of bound port id as not connected */
	if ((port_type == XHCI_USB2 && port_id >= info->num_usb2_ports) ||
	    (port_type == XHCI_USB3 && port_id >= info->num_usb3_ports))
		return false;

	/* Calculate port status register address and read the status */
	res = probe_resource(PCH_DEV_XHCI, PCI_BASE_ADDRESS_0);
	/* If the memory BAR is not allocated for XHCI, leave the devices enabled */
	if (!res)
		return true;

	if (port_type == XHCI_USB2)
		port_sts_reg = (uintptr_t)res->base +
				info->usb2_port_status_reg + port_id * 0x10;
	else
		port_sts_reg = (uintptr_t)res->base +
				info->usb3_port_status_reg + port_id * 0x10;
	port_status = read32((void *)port_sts_reg);

	/* Ensure that the status is not all 1s */
	if (port_status == 0xffffffff)
		return false;

	return !!(port_status & XHCI_STATUS_CCS);
}

void usb_xhci_disable_unused(bool (*ext_usb_xhci_en_cb)(unsigned int port_type,
							unsigned int port_id))
{
	struct device *xhci, *hub = NULL, *port = NULL;
	const struct xhci_usb_info *info = soc_get_xhci_usb_info(PCH_DEVFN_XHCI);
	struct drivers_usb_acpi_config *config;
	bool enable;

	xhci = pcidev_path_on_root(PCH_DEVFN_XHCI);
	if (!xhci) {
		printk(BIOS_ERR, "%s: Could not locate XHCI device in DT\n", __func__);
		return;
	}

	while ((hub = dev_bus_each_child(xhci->link_list, hub)) != NULL) {
		while ((port = dev_bus_each_child(hub->link_list, port)) != NULL) {
			enable = true;
			config = config_of(port);
			if (config->type == UPC_TYPE_INTERNAL) {
				/* Probe the connect status of internal ports */
				enable = is_usb_port_connected(info, port->path.usb.port_type,
							       port->path.usb.port_id);
			} else if (ext_usb_xhci_en_cb) {
				/* Check the mainboard for the status of external ports */
				enable = ext_usb_xhci_en_cb(port->path.usb.port_type,
							    port->path.usb.port_id);
			}

			if (!enable) {
				printk(BIOS_INFO, "%s: Disabling USB Type%d Id%d\n",
					__func__, port->path.usb.port_type,
					port->path.usb.port_id);
				port->enabled = 0;
			}
		}
	}
}

__weak void soc_xhci_init(struct device *dev) { /* no-op */ }

static struct device_operations usb_xhci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= soc_xhci_init,
	.ops_pci		= &pci_dev_ops_pci,
	.scan_bus		= scan_static_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= soc_acpi_name,
#endif
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_XHCI,
	PCI_DID_INTEL_APL_XHCI,
	PCI_DID_INTEL_CNL_LP_XHCI,
	PCI_DID_INTEL_GLK_XHCI,
	PCI_DID_INTEL_SPT_LP_XHCI,
	PCI_DID_INTEL_SPT_H_XHCI,
	PCI_DID_INTEL_LWB_XHCI,
	PCI_DID_INTEL_LWB_XHCI_SUPER,
	PCI_DID_INTEL_UPT_H_XHCI,
	PCI_DID_INTEL_CNP_H_XHCI,
	PCI_DID_INTEL_ICP_LP_XHCI,
	PCI_DID_INTEL_CMP_LP_XHCI,
	PCI_DID_INTEL_CMP_H_XHCI,
	PCI_DID_INTEL_TGP_LP_XHCI,
	PCI_DID_INTEL_TGP_H_XHCI,
	PCI_DID_INTEL_MCC_XHCI,
	PCI_DID_INTEL_JSP_XHCI,
	PCI_DID_INTEL_ADP_P_XHCI,
	PCI_DID_INTEL_ADP_S_XHCI,
	PCI_DID_INTEL_ADP_M_XHCI,
	0
};

static const struct pci_driver pch_usb_xhci __pci_driver = {
	.ops	 = &usb_xhci_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices	 = pci_device_ids,
};
#endif
