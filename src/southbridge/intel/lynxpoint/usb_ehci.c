/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ehci.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include "iobp.h"
#include "pch.h"

#ifdef __SIMPLE_DEVICE__

void usb_ehci_disable(pci_devfn_t dev)
{
	/* Set 0xDC[0]=1 */
	pci_or_config32(dev, 0xdc, (1 << 0));

	/* Set D3Hot state and disable PME */
	pci_update_config16(dev, EHCI_PWR_CTL_STS, ~(PWR_CTL_ENABLE_PME | PWR_CTL_SET_MASK),
			    PWR_CTL_SET_D3);

	/* Clear memory and bus master */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, 0);

	pci_and_config16(dev, PCI_COMMAND,
			~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO));

	/* Disable device */
	switch (dev) {
	case PCH_EHCI1_DEV:
		RCBA32_OR(FD, PCH_DISABLE_EHCI1);
		break;
	case PCH_EHCI2_DEV:
		RCBA32_OR(FD, PCH_DISABLE_EHCI2);
		break;
	}
}

/* Handler for EHCI controller on entry to S3/S4/S5 */
void usb_ehci_sleep_prepare(pci_devfn_t dev, u8 slp_typ)
{
	u32 reg32;
	u8 *bar0_base;
	u16 pwr_state;
	u16 pci_cmd;

	/* Check if the controller is disabled or not present */
	bar0_base = (u8 *)pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	if (bar0_base == 0 || bar0_base == (u8 *)0xffffffff)
		return;
	pci_cmd = pci_read_config16(dev, PCI_COMMAND);

	switch (slp_typ) {
	case ACPI_S4:
	case ACPI_S5:
		/* Check if controller is in D3 power state */
		pwr_state = pci_read_config16(dev, EHCI_PWR_CTL_STS);
		if ((pwr_state & PWR_CTL_SET_MASK) == PWR_CTL_SET_D3) {
			/* Put in D0 */
			u32 new_state = pwr_state & ~PWR_CTL_SET_MASK;
			new_state |= PWR_CTL_SET_D0;
			pci_write_config16(dev, EHCI_PWR_CTL_STS, new_state);

			/* Make sure memory bar is set */
			pci_write_config32(dev, PCI_BASE_ADDRESS_0, (u32)bar0_base);

			/* Make sure memory space is enabled */
			pci_write_config16(dev, PCI_COMMAND, pci_cmd |
				   PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
		}

		/*
		 * If Run/Stop (bit0) is clear in USB2.0_CMD:
		 * - Clear Async Schedule Enable (bit5) and
		 * - Clear Periodic Schedule Enable (bit4) and
		 * - Set Run/Stop (bit0)
		 */
		reg32 = read32(bar0_base + EHCI_USB_CMD);
		if (reg32 & EHCI_USB_CMD_RUN) {
			reg32 &= ~(EHCI_USB_CMD_PSE | EHCI_USB_CMD_ASE);
			reg32 |= EHCI_USB_CMD_RUN;
			write32(bar0_base + EHCI_USB_CMD, reg32);
		}

		/* Check for Port Enabled in PORTSC(0) (RMH) */
		reg32 = read32(bar0_base + EHCI_PORTSC(0));
		if (reg32 & EHCI_PORTSC_ENABLED) {
			/* Set suspend bit in PORTSC if not already set */
			if (!(reg32 & EHCI_PORTSC_SUSPEND)) {
				reg32 |= EHCI_PORTSC_SUSPEND;
				write32(bar0_base + EHCI_PORTSC(0), reg32);
			}

			/* Delay 25ms !! */
			udelay(25 * 1000);

			/* Clear Run/Stop bit */
			reg32 = read32(bar0_base + EHCI_USB_CMD);
			reg32 &= EHCI_USB_CMD_RUN;
			write32(bar0_base + EHCI_USB_CMD, reg32);
		}

		/* Restore state to D3 if that is what it was at the start */
		if ((pwr_state & PWR_CTL_SET_MASK) == PWR_CTL_SET_D3) {
			/* Restore pci command reg */
			pci_write_config16(dev, PCI_COMMAND, pci_cmd);

			/* Enable D3 */
			pci_write_config16(dev, EHCI_PWR_CTL_STS, pwr_state);
		}
	}
}

#else /* !__SIMPLE_DEVICE__ */

static void usb_ehci_clock_gating(struct device *dev)
{
	/* IOBP 0xE5004001[7:6] = 11b */
	pch_iobp_update(0xe5004001, ~0, (1 << 7) | (1 << 6));

	/* Dx:F0:DCh[5,2,1] = 111b
	 * Dx:F0:DCh[0] = 1b when EHCI controller is disabled */
	pci_or_config32(dev, 0xdc, (1 << 5) | (1 << 2) | (1 << 1));

	/* Dx:F0:78h[1:0] = 11b */
	pci_or_config32(dev, 0x78, (1 << 1) | (1 << 0));
}

static void usb_ehci_init(struct device *dev)
{
	printk(BIOS_DEBUG, "EHCI: Setting up controller.. ");

	usb_ehci_clock_gating(dev);

	/* Disable Wake on Disconnect in RMH */
	RCBA32_OR(0x35b0, 0x00000022);

	printk(BIOS_DEBUG, "done.\n");
}

static void usb_ehci_set_subsystem(struct device *dev, unsigned int vendor,
				   unsigned int device)
{
	u8 access_cntl;

	access_cntl = pci_read_config8(dev, 0x80);

	/* Enable writes to protected registers. */
	pci_write_config8(dev, 0x80, access_cntl | 1);

	pci_dev_set_subsystem(dev, vendor, device);

	/* Restore protection. */
	pci_write_config8(dev, 0x80, access_cntl);
}

static struct pci_operations lops_pci = {
	.set_subsystem	= &usb_ehci_set_subsystem,
};

static struct device_operations usb_ehci_ops = {
	.read_resources		= pci_ehci_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_ehci_init,
	.ops_pci		= &lops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_LPT_LP_EHCI,
	PCI_DID_INTEL_LPT_H_EHCI_1,
	PCI_DID_INTEL_LPT_H_EHCI_2,
	0
};

static const struct pci_driver pch_usb_ehci __pci_driver = {
	.ops	 = &usb_ehci_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};

#endif /* !__SIMPLE_DEVICE__ */
