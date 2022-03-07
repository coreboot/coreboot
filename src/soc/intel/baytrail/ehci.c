/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <reg_script.h>

#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/ehci.h>

#include "chip.h"

static const struct reg_script ehci_init_script[] = {
	/* Enable S0 PLL shutdown
	 * D29:F0:7A[12,10,7,6,4,3,2,1]=11111111b */
	REG_PCI_OR16(0x7a, 0x14de),
	/* Enable SB local clock gating
	 * D29:F0:7C[14,3,2]=111b (14 set in clock gating step) */
	REG_PCI_OR32(0x7c, 0x0000000c),
	REG_PCI_OR32(0x8c, 0x00000001),
	/* Enable dynamic clock gating 0x4001=0xCE */
	REG_IOSF_RMW(IOSF_PORT_USBPHY, 0x4001, 0xFFFFFF00, 0xCE),
	/* Magic RCBA register set sequence */
	/* RCBA + 0x200=0x1 */
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x200, 0x00000001),
	/* RCBA + 0x204=0x2 */
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x204, 0x00000002),
	/* RCBA + 0x208=0x0 */
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x208, 0x00000000),
	/* RCBA + 0x240[4,3,2,1,0]=00000b */
	REG_MMIO_RMW32(RCBA_BASE_ADDRESS + 0x240, ~0x0000001f, 0),
	/* RCBA + 0x318[9,8,6,5,4,3,2,1,0]=000000111b */
	REG_MMIO_RMW32(RCBA_BASE_ADDRESS + 0x318, ~0x00000378, 0x00000007),
	/* RCBA + 0x31c[3,2,1,0]=0011b */
	REG_MMIO_RMW32(RCBA_BASE_ADDRESS + 0x31c, ~0x0000000c, 0x00000003),
	REG_SCRIPT_END
};

static const struct reg_script ehci_clock_gating_script[] = {
	/* Enable SB local clock gating */
	REG_PCI_OR32(0x7c, 0x00004000),
	/* RCBA + 0x284=0xbe (step B0+) */
	REG_MMIO_WRITE32(RCBA_BASE_ADDRESS + 0x284, 0x000000be),
	REG_SCRIPT_END
};

static const struct reg_script ehci_disable_script[] = {
	/* Clear Run/Stop Bit */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, USB2CMD, ~USB2CMD_RS, 0),
	/* Wait for HC Halted */
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, USB2STS,
		       USB2STS_HCHALT, USB2STS_HCHALT, 10000),
	/* Disable Interrupts */
	REG_PCI_OR32(EHCI_CMD_STS, INTRDIS),
	/* Disable Asynchronous and Periodic Scheduler */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, USB2CMD,
		      ~(USB2CMD_ASE | USB2CMD_PSE), 0),
	/* Disable port wake */
	REG_PCI_RMW32(EHCI_SBRN_FLA_PWC, ~(PORTWKIMP | PORTWKCAPMASK), 0),
	/* Set Function Disable bit in RCBA */
	REG_MMIO_OR32(RCBA_BASE_ADDRESS + RCBA_FUNC_DIS, RCBA_EHCI_DIS),
	REG_SCRIPT_END
};

static const struct reg_script ehci_hc_reset[] = {
	REG_RES_OR16(PCI_BASE_ADDRESS_0, USB2CMD, USB2CMD_HCRESET),
	REG_SCRIPT_END
};

static void usb2_phy_init(struct device *dev)
{
	struct soc_intel_baytrail_config *config = config_of(dev);
	u32 usb2_comp_bg = (config->usb2_comp_bg == 0 ?
			    0x4700 : config->usb2_comp_bg);
	struct reg_script usb2_phy_script[] = {
		/* USB3PHYInit() */
		REG_IOSF_WRITE(IOSF_PORT_USBPHY, USBPHY_COMPBG,
			       usb2_comp_bg),
		/* Per port phy settings, set in devicetree.cb */
		REG_IOSF_WRITE(IOSF_PORT_USBPHY, USBPHY_PER_PORT_LANE0,
			       config->usb2_per_port_lane0),
		REG_IOSF_WRITE(IOSF_PORT_USBPHY,
			       USBPHY_PER_PORT_RCOMP_HS_PULLUP0,
			       config->usb2_per_port_rcomp_hs_pullup0),
		REG_IOSF_WRITE(IOSF_PORT_USBPHY, USBPHY_PER_PORT_LANE1,
			       config->usb2_per_port_lane1),
		REG_IOSF_WRITE(IOSF_PORT_USBPHY,
			       USBPHY_PER_PORT_RCOMP_HS_PULLUP1,
			       config->usb2_per_port_rcomp_hs_pullup1),
		REG_IOSF_WRITE(IOSF_PORT_USBPHY, USBPHY_PER_PORT_LANE2,
			       config->usb2_per_port_lane2),
		REG_IOSF_WRITE(IOSF_PORT_USBPHY,
			       USBPHY_PER_PORT_RCOMP_HS_PULLUP2,
			       config->usb2_per_port_rcomp_hs_pullup2),
		REG_IOSF_WRITE(IOSF_PORT_USBPHY, USBPHY_PER_PORT_LANE3,
			       config->usb2_per_port_lane3),
		REG_IOSF_WRITE(IOSF_PORT_USBPHY,
			       USBPHY_PER_PORT_RCOMP_HS_PULLUP3,
			       config->usb2_per_port_rcomp_hs_pullup3),
		REG_SCRIPT_END
	};
	reg_script_run(usb2_phy_script);
}

static void ehci_init(struct device *dev)
{
	struct soc_intel_baytrail_config *config = config_of(dev);
	struct reg_script ehci_hc_init[] = {
		/* Controller init */
		REG_SCRIPT_NEXT(ehci_init_script),
		/* Enable clock gating */
		REG_SCRIPT_NEXT(ehci_clock_gating_script),
		/*
		 * Disable ports if requested
		 */
		/* Open per-port disable control override */
		REG_IO_RMW16(ACPI_BASE_ADDRESS + UPRWC, ~0, UPRWC_WR_EN),
		REG_PCI_WRITE8(EHCI_USB2PDO, config->usb2_port_disable_mask),
		/* Close per-port disable control override */
		REG_IO_RMW16(ACPI_BASE_ADDRESS + UPRWC, ~UPRWC_WR_EN, 0),
		REG_SCRIPT_END
	};

	/* Don't reset controller in S3 resume path */
	if (!acpi_is_wakeup_s3())
		reg_script_run_on_dev(dev, ehci_hc_reset);

	/* Disable controller if ports are routed to XHCI */
	if (config->usb_route_to_xhci) {
		/* Disable controller */
		reg_script_run_on_dev(dev, ehci_disable_script);

		/* Hide device with southcluster function */
		dev->enabled = 0;
		southcluster_enable_dev(dev);
	} else {
		/* Initialize EHCI controller */
		reg_script_run_on_dev(dev, ehci_hc_init);
	}

	/* Setup USB2 PHY based on board config */
	usb2_phy_init(dev);
}

static struct device_operations ehci_device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ehci_init,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver baytrail_ehci __pci_driver = {
	.ops    = &ehci_device_ops,
	.vendor	= PCI_VID_INTEL,
	.device = EHCI_DEVID
};
