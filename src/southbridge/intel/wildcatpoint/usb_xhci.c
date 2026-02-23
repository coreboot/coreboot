/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/haswell/haswell.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <acpi/acpi.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <soc/xhci.h>

#ifdef __SIMPLE_DEVICE__
static u8 *usb_xhci_mem_base(pci_devfn_t dev)
{
	u32 mem_base = pci_read_config32(dev, PCI_BASE_ADDRESS_0);

	/* Check if the controller is disabled or not present */
	if (mem_base == 0 || mem_base == 0xffffffff)
		return 0;

	return (u8 *)(mem_base & ~0xf);
}

static int usb_xhci_port_count_usb3(pci_devfn_t dev)
{
	/* PCH-LP has 4 SS ports */
	return 4;
}

static void usb_xhci_reset_status_usb3(u8 *mem_base, int port)
{
	u8 *portsc = mem_base + XHCI_USB3_PORTSC(port);
	u32 status = read32(portsc);
	/* Do not set Port Enabled/Disabled field */
	status &= ~XHCI_USB3_PORTSC_PED;
	/* Clear all change status bits */
	status |= XHCI_USB3_PORTSC_CHST;
	write32(portsc, status);
}

static void usb_xhci_reset_port_usb3(u8 *mem_base, int port)
{
	u8 *portsc = mem_base + XHCI_USB3_PORTSC(port);
	write32(portsc, read32(portsc) | XHCI_USB3_PORTSC_WPR);
}

#define XHCI_RESET_DELAY_US	1000 /* 1ms */
#define XHCI_RESET_TIMEOUT	100  /* 100ms */

/*
 * 1) Wait until port is done polling
 * 2) If port is disconnected
 *  a) Issue warm port reset
 *  b) Poll for warm reset complete
 *  c) Write 1 to port change status bits
 */
static void usb_xhci_reset_usb3(pci_devfn_t dev, int all)
{
	u32 status, port_disabled;
	int timeout, port;
	int port_count = usb_xhci_port_count_usb3(dev);
	u8 *mem_base = usb_xhci_mem_base(dev);

	if (!mem_base || !port_count)
		return;

	/* Get mask of disabled ports */
	port_disabled = pci_read_config32(dev, XHCI_USB3PDO);

	/* Wait until all enabled ports are done polling */
	for (timeout = XHCI_RESET_TIMEOUT; timeout; timeout--) {
		int complete = 1;
		for (port = 0; port < port_count; port++) {
			/* Skip disabled ports */
			if (port_disabled & (1 << port))
				continue;
			/* Read port link status field */
			status = read32(mem_base + XHCI_USB3_PORTSC(port));
			status &= XHCI_USB3_PORTSC_PLS;
			if (status == XHCI_PLSR_POLLING)
				complete = 0;
		}
		/* Exit if all ports not polling */
		if (complete)
			break;
		udelay(XHCI_RESET_DELAY_US);
	}

	/* Reset all requested ports */
	for (port = 0; port < port_count; port++) {
		u8 *portsc = mem_base + XHCI_USB3_PORTSC(port);
		/* Skip disabled ports */
		if (port_disabled & (1 << port))
			continue;
		status = read32(portsc) & XHCI_USB3_PORTSC_PLS;
		/* Reset all or only disconnected ports */
		if (all || (status == XHCI_PLSR_RXDETECT ||
			    status == XHCI_PLSR_POLLING))
			usb_xhci_reset_port_usb3(mem_base, port);
		else
			port_disabled |= 1 << port;
	}

	/* Wait for warm reset complete on all reset ports */
	for (timeout = XHCI_RESET_TIMEOUT; timeout; timeout--) {
		int complete = 1;
		for (port = 0; port < port_count; port++) {
			/* Only check ports that were reset */
			if (port_disabled & (1 << port))
				continue;
			/* Check if warm reset is complete */
			status = read32(mem_base + XHCI_USB3_PORTSC(port));
			if (!(status & XHCI_USB3_PORTSC_WRC))
				complete = 0;
		}
		/* Check for warm reset complete in any port */
		if (complete)
			break;
		udelay(XHCI_RESET_DELAY_US);
	}

	/* Clear port change status bits */
	for (port = 0; port < port_count; port++)
		usb_xhci_reset_status_usb3(mem_base, port);
}

/* Handler for XHCI controller on entry to S3/S4/S5 */
void usb_xhci_sleep_prepare(pci_devfn_t dev, u8 slp_typ)
{
	u32 reg32;
	u8 *mem_base = usb_xhci_mem_base(dev);
	u8 is_broadwell = !!(cpu_family_model() == BROADWELL_FAMILY_ULT);

	if (!mem_base || slp_typ < ACPI_S3)
		return;

	/* Set D0 state */
	pci_update_config16(dev, XHCI_PWR_CTL_STS, ~XHCI_PWR_CTL_SET_MASK, XHCI_PWR_CTL_SET_D0);

	if (!is_broadwell) {
		/* This WA is only for lpt */

		/* Clear PCI 0xB0[14:13] */
		pci_and_config32(dev, 0xb0, ~((1 << 14) | (1 << 13)));

		/* Clear MMIO 0x816c[14,2] */
		reg32 = read32(mem_base + 0x816c);
		reg32 &= ~((1 << 14) | (1 << 2));
		write32(mem_base + 0x816c, reg32);

		/* Reset disconnected USB3 ports */
		usb_xhci_reset_usb3(dev, 0);

		/* Set MMIO 0x80e0[15] */
		reg32 = read32(mem_base + 0x80e0);
		reg32 |= (1 << 15);
		write32(mem_base + 0x80e0, reg32);
	} else {
		/*
		 * Clear port change status bits.  Clearing CSC alone seemed to
		 * fix wakeup from S3 if entering USB compliance state even if
		 * bit wasn't set on the port.
		 */
		int port;
		for (port = 0; port < usb_xhci_port_count_usb3(dev); port++)
			usb_xhci_reset_status_usb3(mem_base, port);
	}

	reg32 = read32(mem_base + 0x8154);
	reg32 &= ~(1 << 31);
	write32(mem_base + 0x8154, reg32);

	/* Set D3Hot state and enable PME */
	pci_or_config16(dev, XHCI_PWR_CTL_STS, XHCI_PWR_CTL_SET_D3);
	pci_or_config16(dev, XHCI_PWR_CTL_STS, XHCI_PWR_CTL_STATUS_PME);
	pci_or_config16(dev, XHCI_PWR_CTL_STS, XHCI_PWR_CTL_ENABLE_PME);
}
#else /* !__SIMPLE_DEVICE__ */

static void xhci_init(struct device *dev)
{
	struct resource *res = find_resource(dev, PCI_BASE_ADDRESS_0);
	u16 reg16;
	u32 reg32;

	/* Ensure controller is in D0 state */
	reg16 = pci_read_config16(dev, XHCI_PWR_CTL_STS);
	reg16 &= ~XHCI_PWR_CTL_SET_MASK;
	reg16 |= XHCI_PWR_CTL_SET_D0;
	pci_write_config16(dev, XHCI_PWR_CTL_STS, reg16);

	/* Disable Compliance Mode Entry */
	reg32 = read32(res2mmio(res, 0x80ec, 0));
	reg32 |= (1 << 0);
	write32(res2mmio(res, 0x80ec, 0), reg32);
}

static struct device_operations usb_xhci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.init			= xhci_init,
};

static const unsigned short pci_device_ids[] = {
	0x9c31, /* LynxPoint-LP */
	0x9cb1, /* WildcatPoint */
	0
};

static const struct pci_driver pch_usb_xhci __pci_driver = {
	.ops	 = &usb_xhci_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
#endif /* !__SIMPLE_DEVICE__ */
