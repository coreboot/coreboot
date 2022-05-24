/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <acpi/acpi.h>
#include <arch/ioapic.h>
#include <bootstate.h>
#include "chip.h"
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/lpc_lib.h>
#include <pc80/isa-dma.h>
#include <pc80/i8254.h>
#include <pc80/i8259.h>
#include <soc/acpi.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/spi.h>
#include <spi-generic.h>
#include <stdint.h>
#include <southbridge/intel/common/spi.h>

static void sc_set_serial_irqs_mode(struct device *dev, enum serirq_mode mode)
{
	u8 *ilb_base = (u8 *)(pci_read_config32(dev, IBASE) & ~0xf);

	switch (mode) {
	case SERIRQ_CONTINUOUS:
		break;

	case SERIRQ_OFF:
		write32(ilb_base + ILB_OIC, read32(ilb_base + ILB_OIC) & ~SIRQEN);
		break;

	case SERIRQ_QUIET:
	default:
		write8(ilb_base + SCNT, read8(ilb_base + SCNT) & ~SCNT_MODE);
		break;
	}
}

static inline void add_mmio_resource(struct device *dev, int i, unsigned long addr,
				     unsigned long size)
{
	mmio_resource_kb(dev, i, addr >> 10, size >> 10);
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
	add_mmio_resource(dev, 0xfff, 0xffffffff - (CONFIG_COREBOOT_ROMSIZE_KB * KiB) + 1,
			(CONFIG_COREBOOT_ROMSIZE_KB * KiB));	/* BIOS ROM */

	add_mmio_resource(dev, 0xfec, IO_APIC_ADDR, 0x00001000); /* IOAPIC */
}

/* Default IO range claimed by the LPC device. The upper bound is exclusive. */
#define LPC_DEFAULT_IO_RANGE_LOWER 0
#define LPC_DEFAULT_IO_RANGE_UPPER 0x1000

static void sc_enable_serial_irqs(struct device *dev)
{
	u8 *ilb_base = (u8 *)(pci_read_config32(dev, IBASE) & ~0xF);

	printk(BIOS_SPEW, "Enable serial irq\n");
	write32(ilb_base + ILB_OIC, read32(ilb_base + ILB_OIC) | SIRQEN);
	write8(ilb_base + SCNT, read8(ilb_base + SCNT) | SCNT_MODE);
}

/*
 * Write PCI config space IRQ assignments. PCI devices have the INT_LINE (0x3c) and INT_PIN
 * (0x3d) registers which report interrupt routing information to operating systems and drivers.
 * The INT_PIN register is generally read only and reports which interrupt pin A - D it uses.
 * The INT_LINE register is configurable and reports which IRQ (generally the PIC IRQs 1 - 15)
 * it will use. This needs to take interrupt pin swizzling on devices that are downstream on
 * a PCI bridge into account.
 *
 * This function will loop through all enabled PCI devices and program the INT_LINE register
 * with the correct PIC IRQ number for the INT_PIN that it uses.  It then configures each
 * interrupt in the PIC to be level triggered.
 */
static void write_pci_config_irqs(void)
{
	struct device *irq_dev;
	struct device *targ_dev;
	uint8_t int_line = 0;
	uint8_t original_int_pin = 0;
	uint8_t new_int_pin = 0;
	uint16_t current_bdf = 0;
	uint16_t parent_bdf = 0;
	uint8_t pirq = 0;
	uint8_t device_num = 0;
	const struct soc_irq_route *ir = &global_soc_irq_route;

	if (ir == NULL) {
		printk(BIOS_WARNING, "Can't write PCI IRQ assignments "
			"because 'global_braswell_irq_route' structure does not exist\n");
		return;
	}

	/*
	 * Loop through all enabled devices and program their INT_LINE, INT_PIN registers from
	 * values taken from the Interrupt Route registers in the ILB
	 */
	printk(BIOS_DEBUG, "PCI_CFG IRQ: Write PIRQ assignments\n");
	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {

		if ((irq_dev->path.type != DEVICE_PATH_PCI) ||
			(!irq_dev->enabled))
			continue;

		current_bdf = irq_dev->path.pci.devfn |
			irq_dev->bus->secondary << 8;

		/*
		 * Step 1: Get the INT_PIN and device structure to look for
		 * in the pirq_data table defined in the mainboard directory.
		 */
		targ_dev = NULL;
		new_int_pin = get_pci_irq_pins(irq_dev, &targ_dev);
		if (targ_dev == NULL || new_int_pin < 1)
			continue;

		/* Get the original INT_PIN for record keeping */
		original_int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		parent_bdf = targ_dev->path.pci.devfn
			| targ_dev->bus->secondary << 8;
		device_num = PCI_SLOT(parent_bdf);

		if (ir->pcidev[device_num] == 0) {
			printk(BIOS_WARNING, "PCI Device %d does not have an IRQ entry, "
				"skipping it\n", device_num);
			continue;
		}

		/* Find the PIRQ that is attached to the INT_PIN  */
		pirq = (ir->pcidev[device_num] >> ((new_int_pin - 1) * 4))
					& 0x7;

		/* Get the INT_LINE this device/function will use */
		int_line = ir->pic[pirq];

		if (int_line != PIRQ_PIC_IRQDISABLE) {
			/* Set this IRQ to level triggered */
			i8259_configure_irq_trigger(int_line, IRQ_LEVEL_TRIGGERED);

			/* Set the Interrupt Line register */
			pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
		} else {
			/* Set the Interrupt line register as 'unknown' or 'unused' */
			pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, PIRQ_PIC_UNKNOWN_UNUSED);
		}

		printk(BIOS_SPEW, "\tINT_PIN\t\t: %d (%s)\n", original_int_pin,
			pin_to_str(original_int_pin));

		if (parent_bdf != current_bdf)
			printk(BIOS_SPEW, "\tSwizzled to\t: %d (%s)\n", new_int_pin,
				pin_to_str(new_int_pin));

		printk(BIOS_SPEW, "\tPIRQ\t\t: %c\n\tINT_LINE\t: 0x%X (IRQ %d)\n",
			'A' + pirq, int_line, int_line);
	}
	printk(BIOS_DEBUG, "PCI_CFG IRQ: Finished writing PIRQ assignments\n");
}

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

static void sc_init(struct device *dev)
{
	int i;
	const unsigned long ilb_base = ILB_BASE_ADDRESS;
	const unsigned long pr_base  = ILB_BASE_ADDRESS + 0x08;
	const unsigned long ir_base  = ILB_BASE_ADDRESS + 0x20;

	void *gen_pmcon1 = (void *)(PMC_BASE_ADDRESS + GEN_PMCON1);
	const struct soc_irq_route *ir = &global_soc_irq_route;
	struct soc_intel_braswell_config *config = config_of(dev);

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND,
		PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | PCI_COMMAND_SPECIAL);

	/* Use IRQ9 for SCI Interrupt */
	write32((void *)(ilb_base + ACTL), 0);

	isa_dma_init();

	sc_enable_serial_irqs(dev);

	/* Set up the PIRQ PIC routing based on static config. */
	for (i = 0; i < NUM_PIRQS; i++)
		write8((void *)(pr_base + i*sizeof(ir->pic[i])), ir->pic[i]);

	/* Set up the per device PIRQ routing base on static config. */
	for (i = 0; i < NUM_IR_DEVS; i++)
		write16((void *)(ir_base + i*sizeof(ir->pcidev[i])), ir->pcidev[i]);

	/* Interrupt 9 should be level triggered (SCI) */
	i8259_configure_irq_trigger(9, 1);

	for (i = 0; i < NUM_PIRQS; i++) {
		if (ir->pic[i])
			i8259_configure_irq_trigger(ir->pic[i], 1);
	}

	if (config->disable_slp_x_stretch_sus_fail) {
		printk(BIOS_DEBUG, "Disabling slp_x stretching.\n");
		write32(gen_pmcon1, read32(gen_pmcon1) | DIS_SLP_X_STRCH_SUS_UP);

	} else {
		write32(gen_pmcon1, read32(gen_pmcon1) & ~DIS_SLP_X_STRCH_SUS_UP);
	}

	/* Write IRQ assignments to PCI config space */
	write_pci_config_irqs();

	/* Initialize i8259 pic */
	setup_i8259();

	/* Initialize i8254 timers */
	setup_i8254();

	sc_set_serial_irqs_mode(dev, config->serirq_mode);

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

#define SET_DIS_MASK(name_) \
	case PCI_DEVFN(name_ ## _DEV, name_ ## _FUNC): \
		mask |= name_ ## _DIS

#define SET_DIS_MASK2(name_) \
	case PCI_DEVFN(name_ ## _DEV, name_ ## _FUNC): \
		mask2 |= name_ ## _DIS

	switch (dev->path.pci.devfn) {
	SET_DIS_MASK(SDIO);
		break;
	SET_DIS_MASK(SD);
		break;
	SET_DIS_MASK(SATA);
		break;
	SET_DIS_MASK(XHCI);
		/* Disable super speed PHY when XHCI is not available. */
		mask2 |= USH_SS_PHY_DIS;
		break;
	SET_DIS_MASK(LPE);
		break;
	SET_DIS_MASK(MMC);
		break;
	SET_DIS_MASK(SIO_DMA1);
		break;
	SET_DIS_MASK(I2C1);
		break;
	SET_DIS_MASK(I2C2);
		break;
	SET_DIS_MASK(I2C3);
		break;
	SET_DIS_MASK(I2C4);
		break;
	SET_DIS_MASK(I2C5);
		break;
	SET_DIS_MASK(I2C6);
		break;
	SET_DIS_MASK(I2C7);
		break;
	SET_DIS_MASK(TXE);
		break;
	SET_DIS_MASK(HDA);
		break;
	SET_DIS_MASK(PCIE_PORT1);
		break;
	SET_DIS_MASK(PCIE_PORT2);
		break;
	SET_DIS_MASK(PCIE_PORT3);
		break;
	SET_DIS_MASK(PCIE_PORT4);
		break;
	SET_DIS_MASK(SIO_DMA2);
		break;
	SET_DIS_MASK(PWM1);
		break;
	SET_DIS_MASK(PWM2);
		break;
	SET_DIS_MASK(HSUART1);
		break;
	SET_DIS_MASK(HSUART2);
		break;
	SET_DIS_MASK(SPI);
		break;
	SET_DIS_MASK2(SMBUS);
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
#define DEV_CASE(name_) \
	case PCI_DEVFN(name_ ## _DEV, name_ ## _FUNC)

	switch (dev->path.pci.devfn) {
	DEV_CASE(SDIO) :
	DEV_CASE(SD) :
	DEV_CASE(MMC) :
	DEV_CASE(LPE) :
	DEV_CASE(SIO_DMA1) :
	DEV_CASE(I2C1) :
	DEV_CASE(I2C2) :
	DEV_CASE(I2C3) :
	DEV_CASE(I2C4) :
	DEV_CASE(I2C5) :
	DEV_CASE(I2C6) :
	DEV_CASE(I2C7) :
	DEV_CASE(SIO_DMA2) :
	DEV_CASE(PWM1) :
	DEV_CASE(PWM2) :
	DEV_CASE(HSUART1) :
	DEV_CASE(HSUART2) :
	DEV_CASE(SPI) :
		offset = 0x80;
		break;
	DEV_CASE(SATA) :
	DEV_CASE(XHCI) :
		offset = 0x70;
		break;
	DEV_CASE(HDA) :
	DEV_CASE(SMBUS) :
		offset = 0x50;
		break;
	DEV_CASE(TXE) :
		/* TXE cannot be placed in D3Hot. */
		return 0;
	DEV_CASE(PCIE_PORT1) :
	DEV_CASE(PCIE_PORT2) :
	DEV_CASE(PCIE_PORT3) :
	DEV_CASE(PCIE_PORT4) :
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
	.read_resources			= sc_read_resources,
	.set_resources			= pci_dev_set_resources,
	.write_acpi_tables		= southcluster_write_acpi_tables,
	.init				= sc_init,
	.enable				= southcluster_enable_dev,
	.scan_bus			= scan_static_bus,
	.ops_pci			= &soc_pci_ops,
};

static const struct pci_driver southcluster __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VID_INTEL,
	.device		= LPC_DEVID,
};

static void finalize_chipset(void *unused)
{
	void *bcr = (void *)(SPI_BASE_ADDRESS + BCR);
	void *gcs = (void *)(RCBA_BASE_ADDRESS + GCS);
	void *gen_pmcon2 = (void *)(PMC_BASE_ADDRESS + GEN_PMCON2);
	void *etr = (void *)(PMC_BASE_ADDRESS + ETR);
	uint8_t *spi = (uint8_t *)SPI_BASE_ADDRESS;

	struct vscc_config cfg;

	/* Set the lock enable on the BIOS control register */
	write32(bcr, read32(bcr) | BCR_LE);

	/* Set BIOS lock down bit controlling boot block size and swapping */
	write32(gcs, read32(gcs) | BILD);

	/* Lock sleep stretching policy and set SMI lock */
	write32(gen_pmcon2, read32(gen_pmcon2) | SLPSX_STR_POL_LOCK | SMI_LOCK);

	/*  Set the CF9 lock */
	write32(etr, read32(etr) | CF9LOCK);

	spi_finalize_ops();
	write16(spi + HSFSTS, read16(spi + HSFSTS) | FLOCKDN);

	if (mainboard_get_spi_vscc_config(&cfg) < 0) {
		printk(BIOS_DEBUG, "No SPI VSCC configuration.\n");
	} else {
		write32(spi + UVSCC, cfg.uvscc);
		write32(spi + LVSCC, cfg.lvscc | VCL);
	}
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, finalize_chipset, NULL);
