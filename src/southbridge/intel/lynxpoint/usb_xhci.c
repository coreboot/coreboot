/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include "pch.h"

static u32 usb_xhci_mem_base(device_t dev)
{
	u32 mem_base = pci_read_config32(dev, PCI_BASE_ADDRESS_0);

	/* Check if the controller is disabled or not present */
	if (mem_base == 0 || mem_base == 0xffffffff)
		return 0;

	return mem_base & ~0xf;
}

static int usb_xhci_port_count_usb3(device_t dev)
{
	if (pch_is_lp()) {
		/* LynxPoint-LP has 4 SS ports */
		return 4;
	} else {
		/* LynxPoint-H can have 0, 2, 4, or 6 SS ports */
		u32 mem_base = usb_xhci_mem_base(dev);
		u32 fus = read32(mem_base + XHCI_USB3FUS);
		fus >>= XHCI_USB3FUS_SS_SHIFT;
		fus &= XHCI_USB3FUS_SS_MASK;
		switch (fus) {
		case 3: return 0;
		case 2: return 2;
		case 1: return 4;
		case 0: default: return 6;
		}
	}
	return 0;
}

static void usb_xhci_reset_status_usb3(u32 mem_base, int port)
{
	u32 portsc = mem_base + XHCI_USB3_PORTSC(port);
	write32(portsc, read32(portsc) | XHCI_USB3_PORTSC_CHST);
}

static void usb_xhci_reset_port_usb3(u32 mem_base, int port)
{
	u32 portsc = mem_base + XHCI_USB3_PORTSC(port);
	write32(portsc, read32(portsc) | XHCI_USB3_PORTSC_WPR);
}

#ifdef __SMM__

#define XHCI_RESET_DELAY_US	1000 /* 1ms */
#define XHCI_RESET_TIMEOUT	100  /* 100ms */

/*
 * 1) Wait until port is done polling
 * 2) If port is disconnected
 *  a) Issue warm port reset
 *  b) Poll for warm reset complete
 *  c) Write 1 to port change status bits
 */
static void usb_xhci_reset_usb3(device_t dev, int all)
{
	u32 status, port_disabled;
	int timeout, port;
	int port_count = usb_xhci_port_count_usb3(dev);
	u32 mem_base = usb_xhci_mem_base(dev);

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
		u32 portsc = mem_base + XHCI_USB3_PORTSC(port);
		/* Skip disabled ports */
		if (port_disabled & (1 << port))
			continue;
		status = read32(portsc) & XHCI_USB3_PORTSC_PLS;
		/* Reset all or only disconnected ports */
		if (all || status == XHCI_PLSR_RXDETECT)
			usb_xhci_reset_port_usb3(mem_base, port);
		else
			port_disabled |= 1 << port; /* No reset */
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
void usb_xhci_sleep_prepare(device_t dev, u8 slp_typ)
{
	u16 reg16;
	u32 reg32;
	u32 mem_base = usb_xhci_mem_base(dev);

	if (!mem_base || slp_typ < 3)
		return;

	if (pch_is_lp()) {
		/* Set D0 state */
		reg16 = pci_read_config16(dev, XHCI_PWR_CTL_STS);
		reg16 &= ~PWR_CTL_SET_MASK;
		reg16 |= PWR_CTL_SET_D0;
		pci_write_config16(dev, XHCI_PWR_CTL_STS, reg16);

		/* Clear PCI 0xB0[14:13] */
		reg32 = pci_read_config32(dev, 0xb0);
		reg32 &= ~((1 << 14) | (1 << 13));
		pci_write_config32(dev, 0xb0, reg32);

		/* Clear MMIO 0x816c[14,2] */
		reg32 = read32(mem_base + 0x816c);
		reg32 &= ~((1 << 14) | (1 << 2));
		write32(mem_base + 0x816c, reg32);

		/* Set MMIO 0x80e0[15] */
		reg32 = read32(mem_base + 0x80e0);
		reg32 |= (1 << 15);
		write32(mem_base + 0x80e0, reg32);
	}

	/* Set D3Hot state and enable PME */
	pci_or_config16(dev, XHCI_PWR_CTL_STS, PWR_CTL_SET_D3);
	pci_or_config16(dev, XHCI_PWR_CTL_STS, PWR_CTL_ENABLE_PME);
}

/* Route all ports to XHCI controller */
void usb_xhci_route_all(void)
{
	u32 port_mask, route;
	u16 reg16;

	/* Skip if EHCI is already disabled */
	if (RCBA32(FD) & PCH_DISABLE_EHCI1)
		return;

	/* Set D0 state */
	reg16 = pci_read_config16(PCH_XHCI_DEV, XHCI_PWR_CTL_STS);
	reg16 &= ~PWR_CTL_SET_MASK;
	reg16 |= PWR_CTL_SET_D0;
	pci_write_config16(PCH_XHCI_DEV, XHCI_PWR_CTL_STS, reg16);

	/* Set USB3 superspeed enable */
	port_mask = pci_read_config32(PCH_XHCI_DEV, XHCI_USB3PRM);
	route = pci_read_config32(PCH_XHCI_DEV, XHCI_USB3PR);
	route &= ~XHCI_USB3PR_SSEN;
	route |= XHCI_USB3PR_SSEN & port_mask;
	pci_write_config32(PCH_XHCI_DEV, XHCI_USB3PR, route);

	/* Route USB2 ports to XHCI controller */
	port_mask = pci_read_config32(PCH_XHCI_DEV, XHCI_USB2PRM);
	route = pci_read_config32(PCH_XHCI_DEV, XHCI_USB2PR);
	route &= ~XHCI_USB2PR_HCSEL;
	route |= XHCI_USB2PR_HCSEL & port_mask;
	pci_write_config32(PCH_XHCI_DEV, XHCI_USB2PR, route);

	/* Disable EHCI controller */
	usb_ehci_disable(PCH_EHCI1_DEV);

	/* LynxPoint-H has a second EHCI controller */
	if (!pch_is_lp())
		usb_ehci_disable(PCH_EHCI2_DEV);

	/* Reset and clear port change status */
	usb_xhci_reset_usb3(PCH_XHCI_DEV, 1);
}

#else /* !__SMM__ */

static void usb_xhci_clock_gating(device_t dev)
{
	u32 reg32;
	u16 reg16;

	/* IOBP 0xE5004001[7:6] = 11b */
	pch_iobp_update(0xe5004001, ~0, (1 << 7)|(1 << 6));

	reg32 = pci_read_config32(dev, 0x40);
	reg32 &= ~(1 << 23); /* unsupported request */

	if (pch_is_lp()) {
		/* D20:F0:40h[18,17,8] = 111b */
		reg32 |= (1 << 18) | (1 << 17) | (1 << 8);
		/* D20:F0:40h[21,20,19] = 110b to enable XHCI Idle L1 */
		reg32 &= ~(1 << 19);
		reg32 |= (1 << 21) | (1 << 20);
	} else {
		/* D20:F0:40h[21,20,18,17,8] = 11111b */
		reg32 |= (1 << 21)|(1 << 20)|(1 << 18)|(1 << 17)|(1 << 8);
	}

	/* Avoid writing upper byte as it is write-once */
	pci_write_config16(dev, 0x40, (u16)(reg32 & 0xffff));
	pci_write_config8(dev, 0x40 + 2, (u8)((reg32 >> 16) & 0xff));

	/* D20:F0:44h[9,7,3] = 111b */
	reg16 = pci_read_config16(dev, 0x44);
	reg16 |= (1 << 9) | (1 << 7) | (1 << 3);
	pci_write_config16(dev, 0x44, reg16);

	reg32 = pci_read_config32(dev, 0xa0);
	if (pch_is_lp()) {
		/* D20:F0:A0h[18] = 1 */
		reg32 |= (1 << 18);
	} else {
		/* D20:F0:A0h[6] = 1 */
		reg32 |= (1 << 6);
	}
	pci_write_config32(dev, 0xa0, reg32);

	/* D20:F0:A4h[13] = 0 */
	reg32 = pci_read_config32(dev, 0xa4);
	reg32 &= ~(1 << 13);
	pci_write_config32(dev, 0xa4, reg32);
}

/* Re-enable ports that are disabled */
static void usb_xhci_enable_ports_usb3(device_t dev)
{
#if CONFIG_FINALIZE_USB_ROUTE_XHCI
	int port;
	u32 portsc, status, disabled;
	u32 mem_base = usb_xhci_mem_base(dev);
	int port_count = usb_xhci_port_count_usb3(dev);

	if (!mem_base || !port_count)
		return;

	/* Get port disable override map */
	disabled = pci_read_config32(dev, XHCI_USB3PDO);

	for (port = 0; port < port_count; port++) {
		/* Skip overridden ports */
		if (disabled & (1 << port))
			continue;
		portsc = mem_base + XHCI_USB3_PORTSC(port);
		status = read32(portsc) & XHCI_USB3_PORTSC_PLS;

		switch (status) {
		case XHCI_PLSR_RXDETECT:
			/* Clear change status */
			printk(BIOS_DEBUG, "usb_xhci reset port %d\n", port);
			usb_xhci_reset_status_usb3(mem_base, port);
			break;
		case XHCI_PLSR_DISABLED:
		default:
			/* Transition to enabled */
			printk(BIOS_DEBUG, "usb_xhci enable port %d\n", port);
			usb_xhci_reset_port_usb3(mem_base, port);
			status = read32(portsc);
			status &= ~XHCI_USB3_PORTSC_PLS;
			status |= XHCI_PLSW_ENABLE | XHCI_USB3_PORTSC_LWS;
			write32(portsc, status);
			break;
		}
	}
#endif
}

static void usb_xhci_init(device_t dev)
{
	u32 reg32;
	u16 reg16;
	u32 mem_base = usb_xhci_mem_base(dev);

	/* D20:F0:74h[1:0] = 00b (set D0 state) */
	reg16 = pci_read_config16(dev, XHCI_PWR_CTL_STS);
	reg16 &= ~PWR_CTL_SET_MASK;
	reg16 |= PWR_CTL_SET_D0;
	pci_write_config16(dev, XHCI_PWR_CTL_STS, reg16);

	/* Enable clock gating first */
	usb_xhci_clock_gating(dev);

	reg32 = read32(mem_base + 0x8144);
	if (pch_is_lp()) {
		/* XHCIBAR + 8144h[8,7,6] = 111b */
		reg32 |= (1 << 8) | (1 << 7) | (1 << 6);
	} else {
		/* XHCIBAR + 8144h[8,7,6] = 100b */
		reg32 &= ~((1 << 7) | (1 << 6));
		reg32 |= (1 << 8);
	}
	write32(mem_base + 0x8144, reg32);

	if (pch_is_lp()) {
		/* XHCIBAR + 816Ch[19:0] = 000f0038h */
		reg32 = read32(mem_base + 0x816c);
		reg32 &= ~0x000fffff;
		reg32 |= 0x000f0038;
		write32(mem_base + 0x816c, reg32);

		/* D20:F0:B0h[17,14,13] = 100b */
		reg32 = pci_read_config32(dev, 0xb0);
		reg32 &= ~((1 << 14) | (1 << 13));
		reg32 |= (1 << 17);
		pci_write_config32(dev, 0xb0, reg32);
	}

	reg32 = pci_read_config32(dev, 0x50);
	if (pch_is_lp()) {
		/* D20:F0:50h[28:0] = 0FCE2E5Fh */
		reg32 &= ~0x1fffffff;
		reg32 |= 0x0fce2e5f;
	} else {
		/* D20:F0:50h[26:0] = 07886E9Fh */
		reg32 &= ~0x07ffffff;
		reg32 |= 0x07886e9f;
	}
	pci_write_config32(dev, 0x50, reg32);

	/* D20:F0:44h[31] = 1 (Access Control Bit) */
	reg32 = pci_read_config32(dev, 0x44);
	reg32 |= (1 << 31);
	pci_write_config32(dev, 0x44, reg32);

	/* D20:F0:40h[31,23] = 10b (OC Configuration Done) */
	reg32 = pci_read_config32(dev, 0x40);
	reg32 &= ~(1 << 23); /* unsupported request */
	reg32 |= (1 << 31);
	pci_write_config32(dev, 0x40, reg32);

#if CONFIG_HAVE_ACPI_RESUME
	/* Enable ports that are disabled before returning to OS */
	if (acpi_slp_type == 3)
		usb_xhci_enable_ports_usb3(dev);
#endif
}

static void usb_xhci_set_subsystem(device_t dev, unsigned vendor,
				   unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations lops_pci = {
	.set_subsystem = &usb_xhci_set_subsystem,
};

static struct device_operations usb_xhci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_xhci_init,
	.ops_pci		= &lops_pci,
};

static const unsigned short pci_device_ids[] = { 0x8c31, /* LynxPoint-H */
						 0x9c31, /* LynxPoint-LP */
						 0 };

static const struct pci_driver pch_usb_xhci __pci_driver = {
	.ops	 = &usb_xhci_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
#endif /* !__SMM__ */
