/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/rcba.h>
#include <soc/serialio.h>
#include <soc/spi.h>
#include <southbridge/intel/lynxpoint/iobp.h>

u8 pch_revision(void)
{
	return pci_read_config8(PCH_DEV_LPC, PCI_REVISION_ID);
}

u16 pch_type(void)
{
	return pci_read_config16(PCH_DEV_LPC, PCI_DEVICE_ID);
}

/* Return 1 if PCH type is WildcatPoint */
int pch_is_wpt(void)
{
	return ((pch_type() & 0xfff0) == 0x9cc0) ? 1 : 0;
}

/* Return 1 if PCH type is WildcatPoint ULX */
int pch_is_wpt_ulx(void)
{
	u16 lpcid = pch_type();

	switch (lpcid) {
	case PCH_WPT_BDW_Y_SAMPLE:
	case PCH_WPT_BDW_Y_PREMIUM:
	case PCH_WPT_BDW_Y_BASE:
		return 1;
	}

	return 0;
}

u32 pch_read_soft_strap(int id)
{
	u32 fdoc;

	fdoc = SPIBAR32(SPIBAR_FDOC);
	fdoc &= ~0x00007ffc;
	SPIBAR32(SPIBAR_FDOC) = fdoc;

	fdoc |= 0x00004000;
	fdoc |= id * 4;
	SPIBAR32(SPIBAR_FDOC) = fdoc;

	return SPIBAR32(SPIBAR_FDOD);
}

#ifndef __SIMPLE_DEVICE__

/* Put device in D3Hot Power State */
static void pch_enable_d3hot(struct device *dev)
{
	pci_or_config32(dev, PCH_PCS, PCH_PCS_PS_D3HOT);
}

/* RCBA function disable and posting read to flush the transaction */
static void rcba_function_disable(u32 reg, u32 bit)
{
	RCBA32_OR(reg, bit);
	RCBA32(reg);
}

/* Set bit in Function Disable register to hide this device */
void pch_disable_devfn(struct device *dev)
{
	switch (dev->path.pci.devfn) {
	case PCH_DEVFN_ADSP: /* Audio DSP */
		rcba_function_disable(FD, PCH_DISABLE_ADSPD);
		break;
	case PCH_DEVFN_XHCI: /* XHCI */
		rcba_function_disable(FD, PCH_DISABLE_XHCI);
		break;
	case PCH_DEVFN_SDMA: /* DMA */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS0, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCH_DEVFN_I2C0: /* I2C0 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS1, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCH_DEVFN_I2C1: /* I2C1 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS2, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCH_DEVFN_SPI0: /* SPI0 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS3, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCH_DEVFN_SPI1: /* SPI1 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS4, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCH_DEVFN_UART0: /* UART0 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS5, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCH_DEVFN_UART1: /* UART1 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS6, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCH_DEVFN_ME: /* MEI #1 */
		rcba_function_disable(FD2, PCH_DISABLE_MEI1);
		break;
	case PCH_DEVFN_ME_2: /* MEI #2 */
		rcba_function_disable(FD2, PCH_DISABLE_MEI2);
		break;
	case PCH_DEVFN_ME_IDER: /* IDE-R */
		rcba_function_disable(FD2, PCH_DISABLE_IDER);
		break;
	case PCH_DEVFN_ME_KT: /* KT */
		rcba_function_disable(FD2, PCH_DISABLE_KT);
		break;
	case PCH_DEVFN_SDIO: /* SDIO */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS7, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCH_DEVFN_GBE: /* Gigabit Ethernet */
		rcba_function_disable(BUC, PCH_DISABLE_GBE);
		break;
	case PCH_DEVFN_HDA: /* HD Audio Controller */
		rcba_function_disable(FD, PCH_DISABLE_HD_AUDIO);
		break;
	case PCI_DEVFN(PCH_DEV_SLOT_PCIE, 0): /* PCI Express Root Port 1 */
	case PCI_DEVFN(PCH_DEV_SLOT_PCIE, 1): /* PCI Express Root Port 2 */
	case PCI_DEVFN(PCH_DEV_SLOT_PCIE, 2): /* PCI Express Root Port 3 */
	case PCI_DEVFN(PCH_DEV_SLOT_PCIE, 3): /* PCI Express Root Port 4 */
	case PCI_DEVFN(PCH_DEV_SLOT_PCIE, 4): /* PCI Express Root Port 5 */
	case PCI_DEVFN(PCH_DEV_SLOT_PCIE, 5): /* PCI Express Root Port 6 */
	case PCI_DEVFN(PCH_DEV_SLOT_PCIE, 6): /* PCI Express Root Port 7 */
	case PCI_DEVFN(PCH_DEV_SLOT_PCIE, 7): /* PCI Express Root Port 8 */
		rcba_function_disable(FD,
			PCH_DISABLE_PCIE(PCI_FUNC(dev->path.pci.devfn)));
		break;
	case PCH_DEVFN_EHCI: /* EHCI #1 */
		rcba_function_disable(FD, PCH_DISABLE_EHCI1);
		break;
	case PCH_DEVFN_LPC: /* LPC */
		rcba_function_disable(FD, PCH_DISABLE_LPC);
		break;
	case PCH_DEVFN_SATA: /* SATA #1 */
		rcba_function_disable(FD, PCH_DISABLE_SATA1);
		break;
	case PCH_DEVFN_SMBUS: /* SMBUS */
		rcba_function_disable(FD, PCH_DISABLE_SMBUS);
		break;
	case PCH_DEVFN_SATA2: /* SATA #2 */
		rcba_function_disable(FD, PCH_DISABLE_SATA2);
		break;
	case PCH_DEVFN_THERMAL: /* Thermal Subsystem */
		rcba_function_disable(FD, PCH_DISABLE_THERMAL);
		break;
	}
}

static void broadwell_pch_enable_dev(struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PCI)
		return;

	if (dev->ops && dev->ops->enable)
		return;

	/* These devices need special enable/disable handling */
	switch (PCI_SLOT(dev->path.pci.devfn)) {
	case PCH_DEV_SLOT_PCIE:
	case PCH_DEV_SLOT_EHCI:
	case PCH_DEV_SLOT_HDA:
		return;
	}

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(dev));

		/* Ensure memory, io, and bus master are all disabled */
		pci_and_config16(dev, PCI_COMMAND,
				~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO));

		/* Disable this device if possible */
		pch_disable_devfn(dev);
	} else {
		/* Enable SERR */
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);
	}
}

struct chip_operations soc_intel_broadwell_pch_ops = {
	CHIP_NAME("Intel Broadwell PCH")
	.enable_dev = &broadwell_pch_enable_dev,
};

#endif
