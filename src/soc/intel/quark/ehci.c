/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <soc/reg_access.h>

/* USB Phy Registers */
#define USB2_GLOBAL_PORT  0x4001
#define USB2_PLL1         0x7F02
#define USB2_PLL2         0x7F03
#define USB2_COMPBG       0x7F04

/* EHCI Packet Buffer OUT/IN Thresholds, values in number of DWORDs */
#define EHCI_OUT_THRESHOLD_VALUE        0x7f
#define EHCI_IN_THRESHOLD_VALUE         0x7f

/* Platform init USB device interrupt masks */
#define V_IOH_USBDEVICE_D_INTR_MSK_UDC_REG    (0x0000007f)
#define V_IOH_USBDEVICE_EP_INTR_MSK_UDC_REG   \
	(B_IOH_USBDEVICE_EP_INTR_MSK_UDC_REG_OUT_EP_MASK \
	| B_IOH_USBDEVICE_EP_INTR_MSK_UDC_REG_IN_EP_MASK)

/* In order to configure the USB PHY to use clk120 (ickusbcoreclk) as PLL
 * reference clock and Port2 as a USB device port, the following sequence must
 * be followed
 */
static const struct reg_script ehci_init_script[] = {

	/* Set packet buffer OUT/IN thresholds */
	REG_MMIO_RMW32(R_IOH_EHCI_INSNREG01,
		~(B_IOH_EHCI_INSNREG01_OUT_THRESHOLD_MASK
			| B_IOH_EHCI_INSNREG01_IN_THRESHOLD_MASK),
		 (EHCI_OUT_THRESHOLD_VALUE
			<< B_IOH_EHCI_INSNREG01_OUT_THRESHOLD_BP)
		 | (EHCI_IN_THRESHOLD_VALUE
			<< B_IOH_EHCI_INSNREG01_IN_THRESHOLD_BP)),

	/* Sighting #4930631 PDNRESCFG [8:7] of USB2_GLOBAL_PORT = 11b.
	 * For port 0 & 1 as host and port 2 as device.
	 */
	REG_USB_RXW(USB2_GLOBAL_PORT, ~(BIT8 | BIT7 | BIT1), (BIT8 | BIT7)),

	/*
	 * Sighting #4930653 Required BIOS change on Disconnect vref to change
	 * to 600mV.
	 */
	REG_USB_RXW(USB2_COMPBG, ~(BIT10 | BIT9 | BIT8 | BIT7),
		(BIT10 | BIT7)),

	/* Sideband register write to USB AFE (Phy)
	 * (pllbypass) to bypass/Disable PLL before switch
	 */
	REG_USB_OR(USB2_PLL2, BIT29),

	/* Sideband register write to USB AFE (Phy)
	 * (coreclksel) to select 120MHz (ickusbcoreclk) clk source.
	 * (Default 0 to select 96MHz (ickusbclk96_npad/ppad))
	 */
	REG_USB_OR(USB2_PLL1, BIT1),

	/* Sideband register write to USB AFE (Phy)
	 * (divide by 8) to achieve internal 480MHz clock
	 * for 120MHz input refclk.  (Default: 4'b1000 (divide by 10) for 96MHz)
	 */
	REG_USB_RXW(USB2_PLL1, ~(BIT6 | BIT5 | BIT4 | BIT3), BIT6),

	/* Sideband register write to USB AFE (Phy)
	 * Clear (pllbypass)
	 */
	REG_USB_AND(USB2_PLL2, ~BIT29),

	/* Sideband register write to USB AFE (Phy)
	 * Set (startlock) to force the PLL FSM to restart the lock
	 * sequence due to input clock/freq switch.
	 */
	REG_USB_OR(USB2_PLL2, BIT24),
	REG_SCRIPT_END
};

static const struct reg_script usb_device_port_init_script[] = {

	/* Mask and clear controller interrupts */
	REG_MMIO_WRITE32(R_IOH_USBDEVICE_D_INTR_MSK_UDC_REG,
		V_IOH_USBDEVICE_D_INTR_MSK_UDC_REG),
	REG_MMIO_WRITE32(R_IOH_USBDEVICE_D_INTR_UDC_REG,
		V_IOH_USBDEVICE_D_INTR_MSK_UDC_REG),

	/* Mask and clear end point interrupts */
	REG_MMIO_WRITE32(R_IOH_USBDEVICE_EP_INTR_MSK_UDC_REG,
		V_IOH_USBDEVICE_EP_INTR_MSK_UDC_REG),
	REG_MMIO_WRITE32(R_IOH_USBDEVICE_EP_INTR_UDC_REG,
		V_IOH_USBDEVICE_EP_INTR_MSK_UDC_REG),
	REG_SCRIPT_END
};

static void init(struct device *dev)
{
	if ((dev->path.pci.devfn & 7) == EHCI_FUNC) {
		printk(BIOS_INFO, "Initializing USB PLLs\n");
		reg_script_run_on_dev(dev, ehci_init_script);
	} else {
		printk(BIOS_INFO, "Initializing USB device port\n");
		reg_script_run_on_dev(dev, usb_device_port_init_script);
	}
}

static struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= init,
};

static const struct pci_driver driver __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= EHCI_DEVID,
};
