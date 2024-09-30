/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <intelblocks/xhci.h>
#include <soc/soc_chip.h>
#include <static.h>

#define XHCI_USB2_PORT_STATUS_REG	0x480
#define XHCI_USB3_PORT_STATUS_REG	0x500
#define XHCI_USB2_PORT_NUM		8
#define XHCI_USB3_PORT_NUM		6

#define XHCI_PMCTRL			0x80A4
/* BIT[7:4] LFPS periodic sampling off time for USB3 Ports */
#define PMCTRL_LFPS_OFFTIME_SHIFT	4
#define PMCTRL_LFPS_OFFTIME_MAX		0xF

static const struct xhci_usb_info usb_info = {
	.usb2_port_status_reg = XHCI_USB2_PORT_STATUS_REG,
	.num_usb2_ports = XHCI_USB2_PORT_NUM,
	.usb3_port_status_reg = XHCI_USB3_PORT_STATUS_REG,
	.num_usb3_ports = XHCI_USB3_PORT_NUM,
};

const struct xhci_usb_info *soc_get_xhci_usb_info(pci_devfn_t xhci_dev)
{
	/* Jasper Lake only has one XHCI controller */
	return &usb_info;
}

static void set_xhci_lfps_sampling_offtime(struct device *dev, uint8_t time_ms)
{
	void *addr;
	const struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);

	if (!res)
		return;

	if (time_ms > PMCTRL_LFPS_OFFTIME_MAX) {
		printk(BIOS_ERR,
			"XHCI: The maximum LFPS sampling OFF time is %u ms, "
			"cannot set it to %u ms\n",
			PMCTRL_LFPS_OFFTIME_MAX, time_ms);

		return;
	}

	addr = (void *)(uintptr_t)(res->base + XHCI_PMCTRL);
	clrsetbits32(addr,
		PMCTRL_LFPS_OFFTIME_MAX << PMCTRL_LFPS_OFFTIME_SHIFT,
		time_ms << PMCTRL_LFPS_OFFTIME_SHIFT);
	printk(BIOS_DEBUG,
		"XHCI: Updated LFPS sampling OFF time to %u ms\n", time_ms);
}

void soc_xhci_init(struct device *dev)
{
	const config_t *config = config_of_soc();

	/* Set xHCI LFPS period sampling off time */
	set_xhci_lfps_sampling_offtime(dev,
		config->xhci_lfps_sampling_offtime_ms);
}
