/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <reg_script.h>

#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/lpc.h>
#include <soc/pattrs.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/xhci.h>

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
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x8058, ~0x00000100, 0x00110000),
	/* BAR + 0x8060[25]=1b */
	REG_RES_OR32(PCI_BASE_ADDRESS_0,  0x8060, 0x02000000),
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

const struct reg_script xhci_init_boot_script[] = {
	/* Setup USB3 phy */
	REG_SCRIPT_NEXT(usb3_phy_script),
	/* Initialize host controller */
	REG_SCRIPT_NEXT(xhci_init_script),
	/* BAR + 0x80e0[16,9,6]=001b, toggle bit 24=1 */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x80e0, ~0x00010200, 0x01000040),
	/* BAR + 0x80e0 toggle bit 24=0 */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x80e0, ~0x01000000, 0),
	REG_SCRIPT_END
};

const struct reg_script xhci_init_resume_script[] = {
	/* Setup USB3 phy */
	REG_SCRIPT_NEXT(usb3_phy_script),
	/* Initialize host controller */
	REG_SCRIPT_NEXT(xhci_init_script),
	/* BAR + 0x80e0[16,9,6]=001b, leave bit 24=0 to prevent HC reset */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x80e0, ~0x01010200, 0x00000040),
	REG_SCRIPT_END
};

const struct reg_script xhci_clock_gating_script[] = {
	/* ConfigureXhciClockGating() */
	/* D20:F0:40[21:19,18,10:8]=000,1,001 (don't write byte 3) */
	REG_PCI_RMW16(0x40, ~0x0600, 0x0100),
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
static void xhci_reset_port_usb3(struct device *dev, int port)
{
	struct reg_script reset_port_usb3_script[] = {
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
	reg_script_run_on_dev(dev, reset_port_usb3_script);
}

/* Prepare ports to be routed to EHCI or XHCI */
static void xhci_route_all(struct device *dev)
{
	static const struct reg_script xhci_route_all_script[] = {
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
	reg_script_run_on_dev(dev, xhci_route_all_script);

	if (acpi_is_wakeup_s3())
		return;

	/* Reset enabled USB3 ports */
	port_disabled = pci_read_config32(dev, XHCI_USB3PDO);
	for (port = 0; port < BYTM_USB3_PORT_COUNT; port++) {
		if (port_disabled & (1 << port))
			continue;
		xhci_reset_port_usb3(dev, port);
	}
}

static void xhci_init(struct device *dev)
{
	struct soc_intel_baytrail_config *config = config_of(dev);
	struct reg_script xhci_hc_init[] = {
		/* Initialize clock gating */
		REG_SCRIPT_NEXT(xhci_clock_gating_script),
		/* Finalize XHCC1 and XHCC2 */
		REG_PCI_RMW32(0x44, ~0x00000000, 0x83c00000),
		REG_PCI_RMW32(0x40, ~0x00800000, 0x80000000),
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

	/* Initialize XHCI controller for boot or resume path */
	if (acpi_is_wakeup_s3())
		reg_script_run_on_dev(dev, xhci_init_resume_script);
	else
		reg_script_run_on_dev(dev, xhci_init_boot_script);

	/* C0 steppings change iCLK/USB PLL VCO settings from 5 to 7 */
	if (pattrs_get()->stepping == STEP_C0) {
		uint32_t reg =  iosf_ushphy_read(USHPHY_CDN_PLL_CONTROL);
		reg |= 0x00700000;
		iosf_ushphy_write(USHPHY_CDN_PLL_CONTROL, reg);
	}

	/* Finalize Initialization */
	reg_script_run_on_dev(dev, xhci_hc_init);

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
	.vendor	= PCI_VID_INTEL,
	.device = XHCI_DEVID
};
