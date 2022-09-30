/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <arch/ioapic.h>
#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <bootstate.h>

#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/iomap.h>
#include <soc/pcr.h>
#include <soc/p2sb.h>
#include <soc/acpi.h>

#include "chip.h"

/* PCH I/O APIC redirection entries */
#define PCH_REDIR_ETR 120

/**
 * Set miscellaneous static southbridge features.
 *
 * @param dev PCI device with I/O APIC control registers
 */
static void pch_enable_ioapic(struct device *dev)
{
	/* affirm full set of redirection table entries ("write once") */
	ioapic_set_max_vectors(VIO_APIC_VADDR, PCH_REDIR_ETR);

	setup_ioapic((void *)IO_APIC_ADDR, IO_APIC0);
}

/* interrupt router lookup for internal devices */
struct dnv_ir_lut {
	/* (dev << 3) | fn */
	u8 devfn;
	u8 ir;
};

#define DEVFN(dev, fn) ((dev << 3) | (fn))

static const struct dnv_ir_lut dnv_ir_lut[] = {
	{.devfn = DEVFN(0x05, 0), .ir = 3},  /* RCEC */
	{.devfn = DEVFN(0x06, 0), .ir = 4},  /* Virtual RP to QAT */
	{.devfn = DEVFN(0x09, 0), .ir = 7},  /* PCIe RP0 */
	{.devfn = DEVFN(0x0a, 0), .ir = 7},  /* PCIe RP1 */
	{.devfn = DEVFN(0x0b, 0), .ir = 7},  /* PCIe RP2 */
	{.devfn = DEVFN(0x0c, 0), .ir = 7},  /* PCIe RP3 */
	{.devfn = DEVFN(0x0e, 0), .ir = 8},  /* PCIe RP4 */
	{.devfn = DEVFN(0x0f, 0), .ir = 8},  /* PCIe RP5 */
	{.devfn = DEVFN(0x10, 0), .ir = 8},  /* PCIe RP6 */
	{.devfn = DEVFN(0x11, 0), .ir = 8},  /* PCIe RP7 */
	{.devfn = DEVFN(0x12, 0), .ir = 10}, /* SMBus - Host */
	{.devfn = DEVFN(0x13, 0), .ir = 6},  /* AHCI0 */
	{.devfn = DEVFN(0x14, 0), .ir = 11}, /* AHCI1 */
	{.devfn = DEVFN(0x15, 0), .ir = 9},  /* USB */
	{.devfn = DEVFN(0x16, 0), .ir = 1},  /* Virtual RP to LAN0 */
	{.devfn = DEVFN(0x17, 0), .ir = 2},  /* Virtual RP to LAN1 */
	{.devfn = DEVFN(0x18, 0), .ir = 5},  /* ME HECI1 */
	{.devfn = DEVFN(0x18, 1), .ir = 5},  /* ME HECI1 */
	{.devfn = DEVFN(0x18, 2), .ir = 5},  /* ME PTIO-IDER */
	{.devfn = DEVFN(0x18, 3), .ir = 5},  /* ME PTIO-KT */
	{.devfn = DEVFN(0x18, 4), .ir = 5},  /* ME HECI3 */
	{.devfn = DEVFN(0x1a, 0), .ir = 10}, /* HSUART0 */
	{.devfn = DEVFN(0x1a, 1), .ir = 10}, /* HSUART1 */
	{.devfn = DEVFN(0x1a, 2), .ir = 10}, /* HSUART2 */
	{.devfn = DEVFN(0x1b, 0), .ir = 12}, /* IE HECI1 */
	{.devfn = DEVFN(0x1b, 1), .ir = 12}, /* IE HECI1 */
	{.devfn = DEVFN(0x1b, 2), .ir = 12}, /* IE PTIO-IDER */
	{.devfn = DEVFN(0x1b, 3), .ir = 12}, /* IE PTIO-KT */
	{.devfn = DEVFN(0x1b, 4), .ir = 12}, /* IE HECI3 */
	{.devfn = DEVFN(0x1c, 0), .ir = 12}, /* SDHCI */
	{.devfn = DEVFN(0x1f, 0), .ir = 0},  /* LPC */
	{.devfn = DEVFN(0x1f, 1), .ir = 0},  /* PS2B */
	{.devfn = DEVFN(0x1f, 4), .ir = 0},  /* SMBus - Legacy */
	{.devfn = DEVFN(0x1f, 7), .ir = 0},  /* Trace Hub */
};

/*
 * Only 6 of the 8 root ports have swizzling, return '1' if this bdf is one of
 * them, '0' otherwise
 */
static int is_dnv_swizzled_rp(uint16_t bdf)
{
	switch (bdf) {
	case DEVFN(10, 0):
	case DEVFN(11, 0):
	case DEVFN(12, 0):
	case DEVFN(15, 0):
	case DEVFN(16, 0):
	case DEVFN(17, 0):
		return 1;
	}

	return 0;
}

/*
 * Figure out which upstream interrupt pin a downstream device gets swizzled to
 *
 * config - pointer to chip_info containing routing info
 * devfn - device/function of root port to check swizzling for
 * pin - interrupt pin 1-4 = A-D
 *
 * Return new pin mapping, 0 if invalid pin
 */
static int dnv_get_swizzled_pin(config_t *config, u8 devfn, u8 pin)
{
	if (pin < 1 || pin > 4)
		return 0;

	devfn >>= 3;
	if (devfn < 13)
		devfn -= 9;
	else
		devfn -= 14;

	return ((pin - 1 + devfn) % 4) + 1;
}

/*
 * Figure out which upstream interrupt pin a downstream device gets swizzled to
 *
 * config - pointer to chip_info containing routing info
 * devfn - device/function of root port to check swizzling for
 * pin - interrupt pin 1-4 = A-D
 *
 * Return new pin mapping, 0 if invalid pin
 */
static int dnv_get_ir(config_t *config, u8 devfn, u8 pin)
{
	int i = 0;
	int line = 0xff;
	u16 ir = 0xffff;

	/* The only valid pin values are 1-4 for A-D */
	if (pin < 1 || pin > 4) {
		printk(BIOS_WARNING, "%s: pin %d is invalid\n", __func__, pin);
		goto dnv_get_ir_done;
	}

	for (i = 0; i < ARRAY_SIZE(dnv_ir_lut); i++) {
		if (dnv_ir_lut[i].devfn == devfn)
			break;
	}

	if (i == ARRAY_SIZE(dnv_ir_lut)) {
		printk(BIOS_WARNING, "%s: no entry\n", __func__);
		goto dnv_get_ir_done;
	}

	switch (dnv_ir_lut[i].ir) {
	case 0:
		ir = config->ir00_routing;
		break;
	case 1:
		ir = config->ir01_routing;
		break;
	case 2:
		ir = config->ir02_routing;
		break;
	case 3:
		ir = config->ir03_routing;
		break;
	case 4:
		ir = config->ir04_routing;
		break;
	case 5:
		ir = config->ir05_routing;
		break;
	case 6:
		ir = config->ir06_routing;
		break;
	case 7:
		ir = config->ir07_routing;
		break;
	case 8:
		ir = config->ir08_routing;
		break;
	case 9:
		ir = config->ir09_routing;
		break;
	case 10:
		ir = config->ir10_routing;
		break;
	case 11:
		ir = config->ir11_routing;
		break;
	case 12:
		ir = config->ir12_routing;
		break;
	default:
		printk(BIOS_ERR, "%s: invalid ir %d for entry %d\n", __func__, dnv_ir_lut[i].ir,
		       i);
		goto dnv_get_ir_done;
	}

	ir >>= (pin - 1) * 4;
	ir &= 0xf;
	switch (ir) {
	case 0:
		line = config->pirqa_routing;
		break;
	case 1:
		line = config->pirqb_routing;
		break;
	case 2:
		line = config->pirqc_routing;
		break;
	case 3:
		line = config->pirqd_routing;
		break;
	case 4:
		line = config->pirqe_routing;
		break;
	case 5:
		line = config->pirqf_routing;
		break;
	case 6:
		line = config->pirqg_routing;
		break;
	case 7:
		line = config->pirqh_routing;
		break;
	default:
		printk(BIOS_ERR, "%s: invalid ir pirq %d for entry %d\n", __func__, ir, i);
		break;
	}

dnv_get_ir_done:
	return line;
}

/*
 * PCI devices have the INT_LINE (0x3C) and INT_PIN (0x3D) registers which
 * report interrupt routing information to operating systems and drivers.  The
 * INT_PIN register is generally read only and reports which interrupt pin
 * A - D it uses.  The INT_LINE register is configurable and reports which IRQ
 * (generally the PIC IRQs 1 - 15) it will use.  This needs to take interrupt
 * pin swizzling on devices that are downstream on a PCI bridge into account.
 */
static u8 dnv_get_int_line(struct device *irq_dev)
{
	config_t *config;
	struct device *targ_dev = NULL;
	uint16_t parent_bdf = 0;
	int8_t original_int_pin = 0, new_int_pin = 0, swiz_int_pin = 0;
	uint8_t int_line = 0xff;

	if (!is_enabled_pci(irq_dev)) {
		printk(BIOS_ERR, "%s for non pci device?\n", __func__);
		goto dnv_get_int_line_done;
	}

	/*
	 * Get the INT_PIN swizzled up to the root port if necessary
	 * using the existing coreboot pci_device code
	 */
	original_int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);
	new_int_pin = get_pci_irq_pins(irq_dev, &targ_dev);
	if (targ_dev == NULL || new_int_pin < 1)
		goto dnv_get_int_line_done;

	printk(BIOS_DEBUG, "%s: irq_dev %s, targ_dev %s:\n", __func__, dev_path(irq_dev),
	       dev_path(targ_dev));
	printk(BIOS_DEBUG, "%s: std swizzle %s from %c to %c\n", __func__, dev_path(targ_dev),
	       '@' + original_int_pin, '@' + new_int_pin);

	/* Swizzle this device if needed */
	config = targ_dev->chip_info;
	parent_bdf = targ_dev->path.pci.devfn | targ_dev->bus->secondary << 8;
	if (is_dnv_swizzled_rp(parent_bdf) && irq_dev != targ_dev) {
		swiz_int_pin = dnv_get_swizzled_pin(config, parent_bdf, new_int_pin);
		printk(BIOS_DEBUG, "%s: dnv swizzle %s from %c to %c\n", __func__,
		       dev_path(targ_dev), '@' + new_int_pin, '@' + swiz_int_pin);
	} else {
		swiz_int_pin = new_int_pin;
	}

	/* Look up the routing for the pin */
	int_line = dnv_get_ir(config, parent_bdf, swiz_int_pin);

dnv_get_int_line_done:
	printk(BIOS_DEBUG, "\tINT_LINE\t\t: %d\n", int_line);
	return int_line;
}

/* PIRQ[n]_ROUT[3:0] - PIRQ Routing Control
 * 0x00 - 0000 = Reserved
 * 0x01 - 0001 = Reserved
 * 0x02 - 0010 = Reserved
 * 0x03 - 0011 = IRQ3
 * 0x04 - 0100 = IRQ4
 * 0x05 - 0101 = IRQ5
 * 0x06 - 0110 = IRQ6
 * 0x07 - 0111 = IRQ7
 * 0x08 - 1000 = Reserved
 * 0x09 - 1001 = IRQ9
 * 0x0A - 1010 = IRQ10
 * 0x0B - 1011 = IRQ11
 * 0x0C - 1100 = IRQ12
 * 0x0D - 1101 = Reserved
 * 0x0E - 1110 = IRQ14
 * 0x0F - 1111 = IRQ15
 * PIRQ[n]_ROUT[7] - PIRQ Routing Control
 * 0x80 - The PIRQ is not routed.
 */

static void pch_pirq_init(struct device *dev)
{
	struct device *irq_dev;
	/* Get the chip configuration */
	config_t *config = config_of(dev);

	/* Initialize PIRQ Routings */
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQA_ROUT),
	       config->pirqa_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQB_ROUT),
	       config->pirqb_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQC_ROUT),
	       config->pirqc_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQD_ROUT),
	       config->pirqd_routing);

	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQE_ROUT),
	       config->pirqe_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQF_ROUT),
	       config->pirqf_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQG_ROUT),
	       config->pirqg_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQH_ROUT),
	       config->pirqh_routing);

	/* Initialize device's Interrupt Routings */
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR00),
		config->ir00_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR01),
		config->ir01_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR02),
		config->ir02_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR03),
		config->ir03_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR04),
		config->ir04_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR05),
		config->ir05_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR06),
		config->ir06_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR07),
		config->ir07_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR08),
		config->ir08_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR09),
		config->ir09_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR10),
		config->ir10_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR11),
		config->ir11_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR12),
		config->ir12_routing);

	/* Initialize device's Interrupt Polarity Control */
	write32((void *)PCH_PCR_ADDRESS(PID_ITSS, PCH_PCR_ITSS_IPC0),
		config->ipc0);
	write32((void *)PCH_PCR_ADDRESS(PID_ITSS, PCH_PCR_ITSS_IPC1),
		config->ipc1);
	write32((void *)PCH_PCR_ADDRESS(PID_ITSS, PCH_PCR_ITSS_IPC2),
		config->ipc2);
	write32((void *)PCH_PCR_ADDRESS(PID_ITSS, PCH_PCR_ITSS_IPC3),
		config->ipc3);

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		int devfn = irq_dev->path.pci.devfn;
		u8 int_pin = 0, int_line = 0;

		if (!is_enabled_pci(irq_dev))
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		int_line = dnv_get_int_line(irq_dev);
		printk(BIOS_DEBUG, "%s: %02x:%02x.%d pin %d int line %d\n", __func__,
		       irq_dev->bus->secondary, devfn >> 3, devfn & 0x7, int_pin, int_line);

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
	}
}

static void pci_p2sb_read_resources(struct device *dev)
{
	struct resource *res;

	/* Add MMIO resource
	 * Use 0xda as an unused index for PCR BAR.
	 */
	res = new_resource(dev, 0xda);
	res->base = DEFAULT_PCR_BASE;
	res->size = 16 * 1024 * 1024; /* 16MB PCR config space */
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |
		     IORESOURCE_ASSIGNED;

	/* Add MMIO resource
	 * Use 0xdb as an unused index for IOAPIC.
	 */
	res = new_resource(dev, 0xdb); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void pch_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit for one frame. */
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
#if !CONFIG(SERIRQ_CONTINUOUS_MODE)
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (0 << 6) | ((21 - 17) << 2) | (0 << 0));
#endif
}

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "pch: %s\n", __func__);

	/* Get the base address */

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_SPECIAL | PCI_COMMAND_MASTER |
				   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	/* Serial IRQ initialization. */
	pch_enable_serial_irqs(dev);

	/* IO APIC initialization. */
	pch_enable_ioapic(dev);

	/* Setup the PIRQ. */
	pch_pirq_init(dev);
}

static void pch_lpc_add_mmio_resources(struct device *dev) { /* TODO */ }

static void pch_lpc_add_io_resources(struct device *dev)
{
	struct resource *res;
	u8 io_index = 0;

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
	res->base = 0xff000000;
	res->size = 0x01000000; /* 16 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void lpc_read_resources(struct device *dev)
{
	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add non-standard MMIO resources. */
	pch_lpc_add_mmio_resources(dev);

	/* Add IO resources. */
	pch_lpc_add_io_resources(dev);

	/* Add MMIO resource for IOAPIC. */
	pci_p2sb_read_resources(dev);
}

static void pch_decode_init(struct device *dev) { /* TODO */ }

static void lpc_enable_resources(struct device *dev)
{
	pch_decode_init(dev);
	pci_dev_enable_resources(dev);
}

/* Set bit in Function Disable register to hide this device */
static void pch_hide_devfn(uint32_t devfn) { /* TODO */ }

void southcluster_enable_dev(struct device *dev)
{
	u16 reg16;

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(dev));

		/* Ensure memory, io, and bus master are all disabled */
		reg16 = pci_read_config16(dev, PCI_COMMAND);
		reg16 &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY |
			   PCI_COMMAND_IO);
		pci_write_config16(dev, PCI_COMMAND, reg16);

		/* Hide this device if possible */
		pch_hide_devfn(dev->path.pci.devfn);
	} else {
		/* Enable SERR */
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);
	}
}

static struct device_operations device_ops = {
	.read_resources = lpc_read_resources,
	.set_resources = pci_dev_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = southcluster_write_acpi_tables,
#endif
	.enable_resources = lpc_enable_resources,
	.init = lpc_init,
	.enable = southcluster_enable_dev,
	.scan_bus = scan_static_bus,
	.ops_pci = &soc_pci_ops,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &device_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_DNV_LPC,
};

static void finalize_chipset(void *unused)
{
	apm_control(APM_CNT_FINALIZE);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, finalize_chipset, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, finalize_chipset, NULL);
