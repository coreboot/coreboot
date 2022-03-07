/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <acpi/acpi.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <drivers/uart/uart8250reg.h>

#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/spi.h>
#include "chip.h"
#include <acpi/acpigen.h>

static inline void add_mmio_resource(struct device *dev, int i, unsigned long addr,
				     unsigned long size)
{
	mmio_resource(dev, i, addr >> 10, size >> 10);
}

static void sc_add_mmio_resources(struct device *dev)
{
	add_mmio_resource(dev, 0xfeb, ABORT_BASE_ADDRESS, ABORT_BASE_SIZE);
	add_mmio_resource(dev, PBASE, PMC_BASE_ADDRESS, PMC_BASE_SIZE);
	add_mmio_resource(dev, IOBASE, IO_BASE_ADDRESS, IO_BASE_SIZE);
	add_mmio_resource(dev, IBASE, ILB_BASE_ADDRESS, ILB_BASE_SIZE);
	add_mmio_resource(dev, SBASE, SPI_BASE_ADDRESS, SPI_BASE_SIZE);
	add_mmio_resource(dev, MPBASE, MPHY_BASE_ADDRESS, MPHY_BASE_SIZE);
	add_mmio_resource(dev, PUBASE, PUNIT_BASE_ADDRESS, PUNIT_BASE_SIZE);
	add_mmio_resource(dev, RCBA, RCBA_BASE_ADDRESS, RCBA_BASE_SIZE);
}

/* Default IO range claimed by the LPC device. The upper bound is exclusive. */
#define LPC_DEFAULT_IO_RANGE_LOWER 0
#define LPC_DEFAULT_IO_RANGE_UPPER 0x1000

static inline int io_range_in_default(int base, int size)
{
	/* Does it start above the range? */
	if (base >= LPC_DEFAULT_IO_RANGE_UPPER)
		return 0;

	/* Is it entirely contained? */
	if (base >= LPC_DEFAULT_IO_RANGE_LOWER && (base + size) < LPC_DEFAULT_IO_RANGE_UPPER)
		return 1;

	/* This will return not in range for partial overlaps */
	return 0;
}

/*
 * Note: this function assumes there is no overlap with the default LPC device's
 * claimed range: LPC_DEFAULT_IO_RANGE_LOWER -> LPC_DEFAULT_IO_RANGE_UPPER.
 */
static void sc_add_io_resource(struct device *dev, int base, int size, int index)
{
	struct resource *res;

	if (io_range_in_default(base, size))
		return;

	res = new_resource(dev, index);
	res->base = base;
	res->size = size;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void sc_add_io_resources(struct device *dev)
{
	struct resource *res;

	/* Add the default claimed IO range for the LPC device. */
	res = new_resource(dev, 0);
	res->base = LPC_DEFAULT_IO_RANGE_LOWER;
	res->size = LPC_DEFAULT_IO_RANGE_UPPER - LPC_DEFAULT_IO_RANGE_LOWER;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* GPIO */
	sc_add_io_resource(dev, GPIO_BASE_ADDRESS, GPIO_BASE_SIZE, GBASE);

	/* ACPI */
	sc_add_io_resource(dev, ACPI_BASE_ADDRESS, ACPI_BASE_SIZE, ABASE);
}

static void sc_read_resources(struct device *dev)
{
	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add non-standard MMIO resources. */
	sc_add_mmio_resources(dev);

	/* Add IO resources. */
	sc_add_io_resources(dev);
}

/*
 * The UART hardware loses power while in suspend. Because of this the kernel
 * can hang because it doesn't re-initialize serial ports it is using for
 * consoles at resume time. The following function configures the UART
 * if the hardware is enabled though it may not be the correct baud rate
 * or configuration.  This is definitely a hack, but it helps the kernel
 * along.
 */
static void com1_configure_resume(struct device *dev)
{
	const uint16_t port = 0x3f8;

	/* Is the UART I/O port enabled? */
	if (!(pci_read_config32(dev, UART_CONT) & 1))
		return;

	/* Disable interrupts */
	outb(0x0, port + UART8250_IER);

	/* Enable FIFOs */
	outb(UART8250_FCR_FIFO_EN, port + UART8250_FCR);

	/* assert DTR and RTS so the other end is happy */
	outb(UART8250_MCR_DTR | UART8250_MCR_RTS, port + UART8250_MCR);

	/* DLAB on */
	outb(UART8250_LCR_DLAB | 3, port + UART8250_LCR);

	/* Set Baud Rate Divisor. 1 ==> 115200 Baud */
	outb(1, port + UART8250_DLL);
	outb(0, port + UART8250_DLM);

	/* Set to 3 for 8N1 */
	outb(3, port + UART8250_LCR);
}

static void sc_init(struct device *dev)
{
	int i;
	u8 *pr_base = (u8 *)(ILB_BASE_ADDRESS + 0x08);
	u16 *ir_base = (u16 *)(ILB_BASE_ADDRESS + 0x20);
	u32 *gen_pmcon1 = (u32 *)(PMC_BASE_ADDRESS + GEN_PMCON1);
	u32 *actl = (u32 *)(ILB_BASE_ADDRESS + ACTL);
	const struct baytrail_irq_route *ir = &global_baytrail_irq_route;
	struct soc_intel_baytrail_config *config = config_of(dev);

	/* Set up the PIRQ PIC routing based on static config. */
	for (i = 0; i < NUM_PIRQS; i++) {
		write8(pr_base + i, ir->pic[i]);
	}
	/* Set up the per device PIRQ routing base on static config. */
	for (i = 0; i < NUM_IR_DEVS; i++) {
		write16(ir_base + i, ir->pcidev[i]);
	}

	/* Route SCI to IRQ9 */
	write32(actl, (read32(actl) & ~SCIS_MASK) | SCIS_IRQ9);

	cmos_init(rtc_failure());

	if (config->disable_slp_x_stretch_sus_fail) {
		printk(BIOS_DEBUG, "Disabling slp_x stretching.\n");
		write32(gen_pmcon1, read32(gen_pmcon1) | DIS_SLP_X_STRCH_SUS_UP);

	} else {
		write32(gen_pmcon1, read32(gen_pmcon1) & ~DIS_SLP_X_STRCH_SUS_UP);
	}

	if (acpi_is_wakeup_s3())
		com1_configure_resume(dev);
}

/*
 * Common code for the south cluster devices.
 */

/* Set bit in function disable register to hide this device. */
static void sc_disable_devfn(struct device *dev)
{
	void *func_dis  = (void *)(PMC_BASE_ADDRESS + FUNC_DIS);
	void *func_dis2 = (void *)(PMC_BASE_ADDRESS + FUNC_DIS2);
	uint32_t mask  = 0;
	uint32_t mask2 = 0;

	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(MMC_DEV, MMC_FUNC):
		mask |= MMC_DIS;
		break;
	case PCI_DEVFN(SDIO_DEV, SDIO_FUNC):
		mask |= SDIO_DIS;
		break;
	case PCI_DEVFN(SD_DEV, SD_FUNC):
		mask |= SD_DIS;
		break;
	case PCI_DEVFN(SATA_DEV, SATA_FUNC):
		mask |= SATA_DIS;
		break;
	case PCI_DEVFN(XHCI_DEV, XHCI_FUNC):
		mask |= XHCI_DIS;
		/* Disable super speed PHY when XHCI is not available. */
		mask2 |= USH_SS_PHY_DIS;
		break;
	case PCI_DEVFN(LPE_DEV, LPE_FUNC):
		mask |= LPE_DIS;
		break;
	case PCI_DEVFN(MMC45_DEV, MMC45_FUNC):
		mask |= MMC45_DIS;
		break;
	case PCI_DEVFN(SIO_DMA1_DEV, SIO_DMA1_FUNC):
		mask |= SIO_DMA1_DIS;
		break;
	case PCI_DEVFN(I2C1_DEV, I2C1_FUNC):
		mask |= I2C1_DIS;
		break;
	case PCI_DEVFN(I2C2_DEV, I2C2_FUNC):
		mask |= I2C1_DIS;
		break;
	case PCI_DEVFN(I2C3_DEV, I2C3_FUNC):
		mask |= I2C3_DIS;
		break;
	case PCI_DEVFN(I2C4_DEV, I2C4_FUNC):
		mask |= I2C4_DIS;
		break;
	case PCI_DEVFN(I2C5_DEV, I2C5_FUNC):
		mask |= I2C5_DIS;
		break;
	case PCI_DEVFN(I2C6_DEV, I2C6_FUNC):
		mask |= I2C6_DIS;
		break;
	case PCI_DEVFN(I2C7_DEV, I2C7_FUNC):
		mask |= I2C7_DIS;
		break;
	case PCI_DEVFN(TXE_DEV, TXE_FUNC):
		mask |= TXE_DIS;
		break;
	case PCI_DEVFN(HDA_DEV, HDA_FUNC):
		mask |= HDA_DIS;
		break;
	case PCI_DEVFN(PCIE_PORT1_DEV, PCIE_PORT1_FUNC):
		mask |= PCIE_PORT1_DIS;
		break;
	case PCI_DEVFN(PCIE_PORT2_DEV, PCIE_PORT2_FUNC):
		mask |= PCIE_PORT2_DIS;
		break;
	case PCI_DEVFN(PCIE_PORT3_DEV, PCIE_PORT3_FUNC):
		mask |= PCIE_PORT3_DIS;
		break;
	case PCI_DEVFN(PCIE_PORT4_DEV, PCIE_PORT4_FUNC):
		mask |= PCIE_PORT4_DIS;
		break;
	case PCI_DEVFN(EHCI_DEV, EHCI_FUNC):
		mask |= EHCI_DIS;
		break;
	case PCI_DEVFN(SIO_DMA2_DEV, SIO_DMA2_FUNC):
		mask |= SIO_DMA2_DIS;
		break;
	case PCI_DEVFN(PWM1_DEV, PWM1_FUNC):
		mask |= PWM1_DIS;
		break;
	case PCI_DEVFN(PWM2_DEV, PWM2_FUNC):
		mask |= PWM2_DIS;
		break;
	case PCI_DEVFN(HSUART1_DEV, HSUART1_FUNC):
		mask |= HSUART1_DIS;
		break;
	case PCI_DEVFN(HSUART2_DEV, HSUART2_FUNC):
		mask |= HSUART2_DIS;
		break;
	case PCI_DEVFN(SPI_DEV, SPI_FUNC):
		mask |= SPI_DIS;
		break;
	case PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC):
		mask2 |= SMBUS_DIS;
		break;
	}

	if (mask != 0) {
		write32(func_dis, read32(func_dis) | mask);
		/* Ensure posted write hits */
		read32(func_dis);
	}

	if (mask2 != 0) {
		write32(func_dis2, read32(func_dis2) | mask2);
		/* Ensure posted write hits */
		read32(func_dis2);
	}
}

static inline void set_d3hot_bits(struct device *dev, int offset)
{
	uint32_t reg8;
	printk(BIOS_DEBUG, "Power management CAP offset 0x%x.\n", offset);
	reg8 = pci_read_config8(dev, offset + 4);
	reg8 |= 0x3;
	pci_write_config8(dev, offset + 4, reg8);
}

/*
 * Parts of the audio subsystem are powered by the HDA device. Thus, one cannot put HDA into
 * D3Hot. Instead, perform this workaround to make some of the audio paths work for LPE audio.
 */
static void hda_work_around(struct device *dev)
{
	void *gctl = (void *)(TEMP_BASE_ADDRESS + 0x8);

	/* Need to set magic register 0x43 to 0xd7 in config space. */
	pci_write_config8(dev, 0x43, 0xd7);

	/*
	 * Need to set bit 0 of GCTL to take the device out of reset.
	 * However, that requires setting up the 64-bit BAR.
	 */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, TEMP_BASE_ADDRESS);
	pci_write_config32(dev, PCI_BASE_ADDRESS_1, 0);
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_MEMORY);
	write32(gctl, read32(gctl) | 0x1);
	pci_write_config16(dev, PCI_COMMAND, 0);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, 0);
}

static int place_device_in_d3hot(struct device *dev)
{
	unsigned int offset;

	/*
	 * Parts of the HDA block are used for LPE audio as well.
	 * Therefore assume the HDA will never be put into D3Hot.
	 */
	if (dev->path.pci.devfn == PCI_DEVFN(HDA_DEV, HDA_FUNC)) {
		hda_work_around(dev);
		return 0;
	}

	offset = pci_find_capability(dev, PCI_CAP_ID_PM);

	if (offset != 0) {
		set_d3hot_bits(dev, offset);
		return 0;
	}

	/*
	 * For some reason some of the devices don't have the capability pointer set correctly.
	 * Work around this by hard coding the offset.
	 */
	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(MMC_DEV, MMC_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(SDIO_DEV, SDIO_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(SD_DEV, SD_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(MMC45_DEV, MMC45_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(LPE_DEV, LPE_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(SIO_DMA1_DEV, SIO_DMA1_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C1_DEV, I2C1_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C2_DEV, I2C2_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C3_DEV, I2C3_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C4_DEV, I2C4_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C5_DEV, I2C5_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C6_DEV, I2C6_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C7_DEV, I2C7_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(SIO_DMA2_DEV, SIO_DMA2_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(PWM1_DEV, PWM1_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(PWM2_DEV, PWM2_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(HSUART1_DEV, HSUART1_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(HSUART2_DEV, HSUART2_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(SPI_DEV, SPI_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(SATA_DEV, SATA_FUNC):
		offset = 0x70;
		break;
	case PCI_DEVFN(XHCI_DEV, XHCI_FUNC):
		offset = 0x70;
		break;
	case PCI_DEVFN(EHCI_DEV, EHCI_FUNC):
		offset = 0x70;
		break;
	case PCI_DEVFN(HDA_DEV, HDA_FUNC):
		offset = 0x50;
		break;
	case PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC):
		offset = 0x50;
		break;
	case PCI_DEVFN(TXE_DEV, TXE_FUNC):
		/* TXE cannot be placed in D3Hot. */
		return 0;
	case PCI_DEVFN(PCIE_PORT1_DEV, PCIE_PORT1_FUNC):
		offset = 0xa0;
		break;
	case PCI_DEVFN(PCIE_PORT2_DEV, PCIE_PORT2_FUNC):
		offset = 0xa0;
		break;
	case PCI_DEVFN(PCIE_PORT3_DEV, PCIE_PORT3_FUNC):
		offset = 0xa0;
		break;
	case PCI_DEVFN(PCIE_PORT4_DEV, PCIE_PORT4_FUNC):
		offset = 0xa0;
		break;
	}

	if (offset != 0) {
		set_d3hot_bits(dev, offset);
		return 0;
	}

	return -1;
}

/* Common PCI device function disable. */
void southcluster_enable_dev(struct device *dev)
{
	uint16_t reg16;

	if (!dev->enabled) {
		int slot = PCI_SLOT(dev->path.pci.devfn);
		int func = PCI_FUNC(dev->path.pci.devfn);
		printk(BIOS_DEBUG, "%s: Disabling device: %02x.%01x\n",
		       dev_path(dev), slot, func);

		/* Ensure memory, io, and bus master are all disabled */
		reg16 = pci_read_config16(dev, PCI_COMMAND);
		reg16 &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		pci_write_config16(dev, PCI_COMMAND, reg16);

		/* Place device in D3Hot */
		if (place_device_in_d3hot(dev) < 0) {
			printk(BIOS_WARNING,
			       "Could not place %02x.%01x into D3Hot. "
			       "Keeping device visible.\n", slot, func);
			return;
		}
		/* Disable this device if possible */
		sc_disable_devfn(dev);
	} else {
		/* Enable SERR */
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);
	}
}

static struct device_operations device_ops = {
	.read_resources		= sc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.write_acpi_tables	= acpi_write_hpet,
	.init			= sc_init,
	.enable			= southcluster_enable_dev,
	.scan_bus		= scan_static_bus,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver southcluster __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VID_INTEL,
	.device		= LPC_DEVID,
};

int __weak mainboard_get_spi_config(struct spi_config *cfg)
{
	return -1;
}

static void finalize_chipset(void *unused)
{
	void *bcr = (void *)(SPI_BASE_ADDRESS + BCR);
	void *gcs = (void *)(RCBA_BASE_ADDRESS + GCS);
	void *gen_pmcon2 = (void *)(PMC_BASE_ADDRESS + GEN_PMCON2);
	void *etr = (void *)(PMC_BASE_ADDRESS + ETR);
	uint8_t *spi = (uint8_t *)SPI_BASE_ADDRESS;
	struct spi_config cfg;

	/* Set the lock enable on the BIOS control register */
	write32(bcr, read32(bcr) | BCR_LE);

	/* Set BIOS lock down bit controlling boot block size and swapping */
	write32(gcs, read32(gcs) | BILD);

	/* Lock sleep stretching policy and set SMI lock */
	write32(gen_pmcon2, read32(gen_pmcon2) | SLPSX_STR_POL_LOCK | SMI_LOCK);

	/*  Set the CF9 lock */
	write32(etr, read32(etr) | CF9LOCK);

	if (mainboard_get_spi_config(&cfg) < 0) {
		printk(BIOS_DEBUG, "No SPI lockdown configuration.\n");
	} else {
		write16(spi + PREOP, cfg.preop);
		write16(spi + OPTYPE, cfg.optype);
		write32(spi + OPMENU0, cfg.opmenu[0]);
		write32(spi + OPMENU1, cfg.opmenu[1]);
		write16(spi + HSFSTS, read16(spi + HSFSTS) | FLOCKDN);
		write32(spi + UVSCC, cfg.uvscc);
		write32(spi + LVSCC, cfg.lvscc | VCL);
	}
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, finalize_chipset, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, finalize_chipset, NULL);
