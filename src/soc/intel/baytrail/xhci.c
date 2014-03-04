/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <reg_script.h>

#include <baytrail/iomap.h>
#include <baytrail/iosf.h>
#include <baytrail/pci_devs.h>
#include <baytrail/pmc.h>
#include <baytrail/ramstage.h>
#include <baytrail/xhci.h>

#include "chip.h"

struct reg_script usb3_phy_script[] = {
	/* USB3PHYInit() */
	REG_IOSF_RMW(IOSF_PORT_USHPHY, USHPHY_CDN_PLL_CONTROL,
		     ~0x00700000, 0x00500000),
	REG_IOSF_RMW(IOSF_PORT_USHPHY, USHPHY_CDN_VCO_START_CAL_POINT,
		     ~0x001f0000, 0x000A0000),
	REG_IOSF_RMW(IOSF_PORT_USHPHY, USHPHY_CCDRLF,
		     ~0x0000000f, 0x0000000b),
	REG_IOSF_RMW(IOSF_PORT_USHPHY, USHPHY_PEAKING_AMP_CONFIG_DIAG,
		     ~0x000000f0, 0x000000f0),
	REG_IOSF_RMW(IOSF_PORT_USHPHY, USHPHY_OFFSET_COR_CONFIG_DIAG,
		     ~0x000001c0, 0x00000000),
	REG_IOSF_RMW(IOSF_PORT_USHPHY, USHPHY_VGA_GAIN_CONFIG_DIAG,
		     ~0x00000070, 0x00000020),
	REG_IOSF_RMW(IOSF_PORT_USHPHY, USHPHY_REE_DAC_CONTROL,
		     ~0x00000002, 0x00000002),
	REG_IOSF_RMW(IOSF_PORT_USHPHY, USHPHY_CDN_U1_POWER_STATE_DEF,
		     ~0x00000000, 0x00040000),
	REG_SCRIPT_END
};

const struct reg_script xhci_init_script[] = {
	/* CommonXhciHcInit() */
	/* BAR + 0x0c[31:16] = 0x0200 */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x000c, 0x0000ffff, 0x02000000),
	/* BAR + 0x0c[7:0] = 0x0a */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x000c, 0xffffff00, 0x0000000a),
	/* BAR + 0x8094[23,21,14]=111b */
	REG_RES_OR32(PCI_BASE_ADDRESS_0,  0x8094, 0x00a04000),
	/* BAR + 0x8110[20,11,8,2]=1100b */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x8110, ~0x00000104, 0x00100800),
	/* BAR + 0x8144[8,7,6]=111b */
	REG_RES_OR32(PCI_BASE_ADDRESS_0,  0x8144, 0x000001c0),
	/* BAR + 0x8154[21,13,3]=010b */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x8154, ~0x00200008, 0x80002000),
	/* BAR + 0x816c[19:0]=1110x100000000111100b */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x816c, 0xfff08000, 0x000e0030),
	/* BAR + 0x8188[26,24]=11b */
	REG_RES_OR32(PCI_BASE_ADDRESS_0,  0x8188, 0x05000000),
	/* BAR + 0x8174=0x1000c0a*/
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x8174, 0xfe000000, 0x01000c0a),
	/* BAR + 0x854c[29]=0b */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x854c, ~0x20000000, 0),
	/* BAR + 0x8178[12:0]=0b */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x8178, ~0xffffe000, 0),
	/* BAR + 0x8164[7:0]=0xff */
	REG_RES_OR32(PCI_BASE_ADDRESS_0,  0x8164, 0x000000ff),
	/* BAR + 0x0010[10,9,5]=110b */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x0010, ~0x00000020, 0x00000600),
	/* BAR + 0x8058[20,16,8]=110b */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x8058, ~0x00000010, 0x00110000),
	/* BAR + 0x8060[25]=1b */
	REG_RES_OR32(PCI_BASE_ADDRESS_0,  0x8060, 0x02000000),
	/* BAR + 0x80e0[19,9,6]=001b, toggle bit 24=1 */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x80e0, ~0x00010020, 0x01000040),
	/* BAR + 0x80e0 toggle bit 24=0 */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x80e0, ~0x01000000, 0),
	/* BAR + 0x80f0[20]=0b */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x80f0, ~0x00100000, 0),
	/* BAR + 0x8008[19]=1b (to enable LPM) */
	REG_RES_OR32(PCI_BASE_ADDRESS_0,  0x8008, 0x00080000),
	/* BAR + 0x80fc[25]=1b */
	REG_RES_OR32(PCI_BASE_ADDRESS_0,  0x80fc, 0x02000000),
	/* 0x40/0x44 are written as bytes to avoid touching bit31 */
	/* D20:F0:40[21,20,18,10,9,8]=111001b (don't write byte3) */
	REG_PCI_RMW8(0x41, ~0x06, 0x01),
	/* Except [21,20,19,18]=0001b USB wake W/A is disable IIL1E */
	REG_PCI_RMW8(0x42, 0x3c, 0x04),
	/* D20:F0:44[19:14,10,9,7,3:0]=1 (don't write byte3) */
	REG_PCI_RMW8(0x44, 0x00, 0x8f),
	REG_PCI_RMW8(0x45, ~0xcf, 0xc6),
	REG_PCI_RMW8(0x46, ~0x0f, 0x0f),
	/* BAR + 0x8140 = 0xff00f03c */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x8140, 0, 0xff00f03c),
	REG_SCRIPT_END
};

const struct reg_script xhci_clock_gating_script[] = {
	/* ConfigureXhciClockGating() */
	/* D20:F0:40[21:19,18,10:8]=000,1,001 (don't write byte 3) */
	REG_PCI_RMW16(0x40, ~0x0300, 0x0010),
	REG_PCI_RMW8(0x42, ~0x38, 0x04),
	/* D20:F0:44[5:3]=001b */
	REG_PCI_RMW16(0x44, ~0x0030, 0x0008),
	/* D20:F0:A0[19:18]=01b */
	REG_PCI_RMW32(0xa0, ~0x00080000, 0x00040000),
	/* D20:F0:A4[15:0]=0x00 */
	REG_PCI_WRITE16(0xa4, 0x0000),
	/* D20:F0:B0[21:17,14:13]=0000000b */
	REG_PCI_RMW32(0xb0, ~0x00376000, 0x00000000),
	/* D20:F0:50[31:0]=0x0bce6e5f */
	REG_PCI_WRITE32(0x50, 0x0bce6e5f),
	REG_SCRIPT_END
};

/* Warm Reset a USB3 port */
static void xhci_reset_port_usb3(device_t dev, int port)
{
	struct reg_script reset_port_usb3_script[] = {
		REG_SCRIPT_SET_DEV(dev),
		/* Issue Warm Port Rest to the port */
		REG_RES_OR32(PCI_BASE_ADDRESS_0, XHCI_USB3_PORTSC(port),
			     XHCI_USB3_PORTSC_WPR),
		/* Wait up to 100ms for it to complete */
		REG_RES_POLL32(PCI_BASE_ADDRESS_0, XHCI_USB3_PORTSC(port),
			       XHCI_USB3_PORTSC_WRC, XHCI_USB3_PORTSC_WRC,
			       XHCI_RESET_TIMEOUT),
		/* Clear change status bits, do not set PED */
		REG_RES_RMW32(PCI_BASE_ADDRESS_0, XHCI_USB3_PORTSC(port),
			      ~XHCI_USB3_PORTSC_PED, XHCI_USB3_PORTSC_CHST),
		REG_SCRIPT_END
	};
	reg_script_run(reset_port_usb3_script);
}

/* Prepare ports to be routed to EHCI or XHCI */
static void xhci_route_all(device_t dev)
{
	struct reg_script xhci_route_all_script[] = {
		REG_SCRIPT_SET_DEV(dev),
		/* USB3 SuperSpeed Enable */
		REG_PCI_WRITE32(XHCI_USB3PR, BYTM_USB3_PORT_MAP),
		/* USB2 Port Route to XHCI */
		REG_PCI_WRITE32(XHCI_USB2PR, BYTM_USB2_PORT_MAP),
		REG_SCRIPT_END
	};
	u32 port_disabled;
	int port;

	printk(BIOS_INFO, "USB: Route ports to XHCI controller\n");

	/* Route ports to XHCI controller */
	reg_script_run(xhci_route_all_script);

	/* Reset enabled USB3 ports */
	port_disabled = pci_read_config32(dev, XHCI_USB3PDO);
	for (port = 0; port < BYTM_USB3_PORT_COUNT; port++) {
		if (port_disabled & (1 << port))
			continue;
		xhci_reset_port_usb3(dev, port);
	}
}

static void xhci_init(device_t dev)
{
	struct soc_intel_baytrail_config *config = dev->chip_info;
	struct reg_script xhci_hc_init[] = {
		REG_SCRIPT_SET_DEV(dev),
		/* Setup USB3 phy */
		REG_SCRIPT_NEXT(usb3_phy_script),
		/* Initialize host controller */
		REG_SCRIPT_NEXT(xhci_init_script),
		/* Initialize clock gating */
		REG_SCRIPT_NEXT(xhci_clock_gating_script),
		/* Set USB2 Port Routing Mask */
		REG_PCI_WRITE32(XHCI_USB2PRM, BYTM_USB2_PORT_MAP),
		/* Set USB3 Port Routing Mask */
		REG_PCI_WRITE32(XHCI_USB3PRM, BYTM_USB3_PORT_MAP),
		/*
		 * Disable ports if requested
		 */
		/* Open per-port disable control override */
		REG_IO_RMW16(ACPI_BASE_ADDRESS + UPRWC, ~0, UPRWC_WR_EN),
		REG_PCI_WRITE32(XHCI_USB2PDO, config->usb2_port_disable_mask),
		REG_PCI_WRITE32(XHCI_USB3PDO, config->usb3_port_disable_mask),
		/* Close per-port disable control override */
		REG_IO_RMW16(ACPI_BASE_ADDRESS + UPRWC, ~UPRWC_WR_EN, 0),
		REG_SCRIPT_END
	};

	/* Initialize XHCI controller */
	reg_script_run(xhci_hc_init);

	/* Route all ports to XHCI if requested */
	if (config->usb_route_to_xhci)
		xhci_route_all(dev);
}

static struct device_operations xhci_device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= xhci_init,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver baytrail_xhci __pci_driver = {
	.ops    = &xhci_device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device = XHCI_DEVID
};
