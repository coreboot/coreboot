/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pnp.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/ioapic.h>
#include <pc80/i8254.h>
#include <pc80/i8259.h>
#include <amdblocks/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/espi.h>
#include <amdblocks/lpc.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/southbridge.h>

static void setup_serirq(void)
{
	u8 byte;

	/* Set up SERIRQ, enable continuous mode */
	byte = PM_SERIRQ_NUM_BITS_21;
	if (!CONFIG(SOC_AMD_COMMON_BLOCK_USE_ESPI))
		byte |= PM_SERIRQ_ENABLE;
	if (!CONFIG(SERIRQ_CONTINUOUS_MODE))
		byte |= PM_SERIRQ_MODE;

	pm_write8(PM_SERIRQ_CONF, byte);
}

static void lpc_init(struct device *dev)
{
	u8 byte;

	/* Initialize isa dma */
	isa_dma_init();

	/* Enable DMA transaction on the LPC bus */
	byte = pci_read_config8(dev, LPC_PCI_CONTROL);
	byte |= LEGACY_DMA_EN;
	pci_write_config8(dev, LPC_PCI_CONTROL, byte);

	/* Disable the timeout mechanism on LPC */
	byte = pci_read_config8(dev, LPC_IO_OR_MEM_DECODE_ENABLE);
	byte &= ~LPC_SYNC_TIMEOUT_COUNT_ENABLE;
	pci_write_config8(dev, LPC_IO_OR_MEM_DECODE_ENABLE, byte);

	/* Disable LPC MSI Capability */
	byte = pci_read_config8(dev, LPC_MISC_CONTROL_BITS);
	/* BIT 1 is not defined in public datasheet. */
	byte &= ~(1 << 1);

	pci_write_config8(dev, LPC_MISC_CONTROL_BITS, byte);

	/*
	 * Enable hand-instance of the pulse generator and SPI prefetch from
	 * host (earlier is recommended for boot speed).
	 */
	byte = pci_read_config8(dev, LPC_HOST_CONTROL);
	byte |= PREFETCH_EN_SPI_FROM_HOST | T_START_ENH;
	pci_write_config8(dev, LPC_HOST_CONTROL, byte);

	cmos_check_update_date();

	/*
	 * Initialize the real time clock.
	 * The 0 argument tells cmos_init not to
	 * update CMOS unless it is invalid.
	 * 1 tells cmos_init to always initialize the CMOS.
	 */
	cmos_init(0);

	/* Initialize i8259 pic */
	setup_i8259();

	/* Initialize i8254 timers */
	setup_i8254();

	setup_serirq();
}

static void lpc_read_resources(struct device *dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = FLASH_BASE_ADDR;
	res->size = CONFIG_ROM_SIZE;
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* Add a memory resource for the SPI BAR. */
	fixed_mem_resource(dev, 2, SPI_BASE_ADDRESS / KiB, 1,
			IORESOURCE_SUBTRACTIVE);

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	compact_resources(dev);
}

static void lpc_set_resources(struct device *dev)
{
	struct resource *res;
	u32 spi_enable_bits;

	/* Special case. The SpiRomEnable and other enables should STAY set. */
	res = find_resource(dev, 2);
	spi_enable_bits = pci_read_config32(dev, SPIROM_BASE_ADDRESS_REGISTER);
	spi_enable_bits &= SPI_BASE_ALIGNMENT - 1;
	pci_write_config32(dev, SPIROM_BASE_ADDRESS_REGISTER,
			res->base | spi_enable_bits);

	pci_dev_set_resources(dev);
}

static void configure_child_lpc_windows(struct device *dev, struct device *child)
{
	struct resource *res;
	u32 base, end;
	u32 rsize = 0, set = 0, set_x = 0;
	int wideio_index;
	u32 reg, reg_x;

	reg = pci_read_config32(dev, LPC_IO_PORT_DECODE_ENABLE);
	reg_x = pci_read_config32(dev, LPC_IO_OR_MEM_DECODE_ENABLE);

	/*
	 * Be a bit relaxed, tolerate that LPC region might be bigger than
	 * resource we try to fit, do it like this for all regions < 16 bytes.
	 * If there is a resource > 16 bytes it must be 512 bytes to be able
	 * to allocate the fresh LPC window.
	 *
	 * AGESA and early initialization can set a wide IO port. This code
	 * will verify if required region was previously set and will avoid
	 * setting a new wide IO resource if one is already set.
	 */

	for (res = child->resource_list; res; res = res->next) {
		if (!(res->flags & IORESOURCE_IO))
			continue;
		base = res->base;
		end = resource_end(res);
		printk(BIOS_DEBUG,
			"Southbridge LPC decode:%s, base=0x%08x, end=0x%08x\n",
			dev_path(child), base, end);
		/* find a resource size */
		switch (base) {
		case 0x60:	/*  KB */
		case 0x64:	/*  MS */
			set |= DECODE_ENABLE_KBC_PORT;
			rsize = 1;
			break;
		case 0x3f8:	/*  COM1 */
			set |= DECODE_ENABLE_SERIAL_PORT0;
			rsize = 8;
			break;
		case 0x2f8:	/*  COM2 */
			set |= DECODE_ENABLE_SERIAL_PORT1;
			rsize = 8;
			break;
		case 0x378:	/*  Parallel 1 */
			set |= DECODE_ENABLE_PARALLEL_PORT0;
			/* enable 0x778 for ECP mode */
			set |= DECODE_ENABLE_PARALLEL_PORT1;
			rsize = 8;
			break;
		case 0x3f0:	/*  FD0 */
			set |= DECODE_ENABLE_FDC_PORT0;
			rsize = 8;
			break;
		case 0x220:	/*  0x220 - 0x227 */
			set |= DECODE_ENABLE_SERIAL_PORT2;
			rsize = 8;
			break;
		case 0x228:	/*  0x228 - 0x22f */
			set |= DECODE_ENABLE_SERIAL_PORT3;
			rsize = 8;
			break;
		case 0x238:	/*  0x238 - 0x23f */
			set |= DECODE_ENABLE_SERIAL_PORT4;
			rsize = 8;
			break;
		case 0x300:	/*  0x300 - 0x301 */
			set |= DECODE_ENABLE_MIDI_PORT0;
			rsize = 2;
			break;
		case 0x400:
			set_x |= DECODE_IO_PORT_ENABLE0;
			rsize = 0x40;
			break;
		case 0x480:
			set_x |= DECODE_IO_PORT_ENABLE1;
			rsize = 0x40;
			break;
		case 0x500:
			set_x |= DECODE_IO_PORT_ENABLE2;
			rsize = 0x40;
			break;
		case 0x580:
			set_x |= DECODE_IO_PORT_ENABLE3;
			rsize = 0x40;
			break;
		case 0x4700:
			set_x |= DECODE_IO_PORT_ENABLE5;
			rsize = 0xc;
			break;
		case 0xfd60:
			set_x |= DECODE_IO_PORT_ENABLE6;
			rsize = 16;
			break;
		default:
			rsize = 0;
			wideio_index = lpc_find_wideio_range(base, res->size);
			if (wideio_index != WIDEIO_RANGE_ERROR) {
				rsize = lpc_wideio_size(wideio_index);
				printk(BIOS_DEBUG, "Covered by wideIO");
				printk(BIOS_DEBUG, " %d\n", wideio_index);
			}
		}
		/* check if region found and matches the enable */
		if (res->size <= rsize) {
			reg |= set;
			reg_x |= set_x;
		/* check if we can fit resource in variable range */
		} else {
			wideio_index = lpc_set_wideio_range(base, res->size);
			if (wideio_index != WIDEIO_RANGE_ERROR) {
				/* preserve wide IO related bits. */
				reg_x = pci_read_config32(dev,
					LPC_IO_OR_MEM_DECODE_ENABLE);
				printk(BIOS_DEBUG,
					"Range assigned to wide IO %d\n",
					wideio_index);
			} else {
				printk(BIOS_ERR,
					"cannot fit LPC decode region:");
				printk(BIOS_ERR,
					"%s, base = 0x%08x, end = 0x%08x\n",
					dev_path(child), base, end);
			}
		}
	}

	pci_write_config32(dev, LPC_IO_PORT_DECODE_ENABLE, reg);
	pci_write_config32(dev, LPC_IO_OR_MEM_DECODE_ENABLE, reg_x);
}

static void configure_child_espi_windows(struct device *child)
{
	struct resource *res;

	for (res = child->resource_list; res; res = res->next) {
		if (res->flags & IORESOURCE_IO)
			espi_open_io_window(res->base, res->size);
		else if (res->flags & IORESOURCE_MEM)
			espi_open_mmio_window(res->base, res->size);
	}
}

static void lpc_enable_children_resources(struct device *dev)
{
	struct bus *link;
	struct device *child;

	for (link = dev->link_list; link; link = link->next) {
		for (child = link->children; child; child = child->sibling) {
			if (!child->enabled)
				continue;
			if (child->path.type != DEVICE_PATH_PNP)
				continue;
			if (CONFIG(SOC_AMD_COMMON_BLOCK_USE_ESPI))
				configure_child_espi_windows(child);
			else
				configure_child_lpc_windows(dev, child);
		}
	}
}

static void lpc_enable_resources(struct device *dev)
{
	pci_dev_enable_resources(dev);
	lpc_enable_children_resources(dev);
}

#if CONFIG(HAVE_ACPI_TABLES)
static const char *lpc_acpi_name(const struct device *dev)
{
	return "LPCB";
}
#endif

static struct device_operations lpc_ops = {
	.read_resources = lpc_read_resources,
	.set_resources = lpc_set_resources,
	.enable_resources = lpc_enable_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = lpc_acpi_name,
	.write_acpi_tables = southbridge_write_acpi_tables,
#endif
	.init = lpc_init,
	.scan_bus = scan_static_bus,
	.ops_pci = &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	/* PCI device ID is used on all discrete FCHs and Family 16h Models 00h-3Fh */
	PCI_DID_AMD_SB900_LPC,
	/* PCI device ID is used on all integrated FCHs except Family 16h Models 00h-3Fh */
	PCI_DID_AMD_CZ_LPC,
	0
};
static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &lpc_ops,
	.vendor = PCI_VID_AMD,
	.devices = pci_device_ids,
};
