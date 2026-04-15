/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>
#include "ldn.h"

// Find the true BAR index for a given LDN, return 0 on error.
static uint8_t ldn_to_bar(uint8_t ldn)
{
	switch (ldn) {
	case MEC152X_LDN_ESPI_IOC:
		return 0x34;
	case MEC152X_LDN_ESPI_MC:
		return 0x38;
	case MEC152X_LDN_MAILBOX:
		return 0x3c;
	case MEC152X_LDN_KBC:
		return 0x40;
	case MEC152X_LDN_ACPI_EC0:
		return 0x44;
	case MEC152X_LDN_ACPI_EC1:
		return 0x48;
	case MEC152X_LDN_ACPI_EC2:
		return 0x4c;
	case MEC152X_LDN_ACPI_EC3:
		return 0x50;
	case MEC152X_LDN_ACPI_PM1:
		return 0x58;
	case MEC152X_LDN_LEGACY_FAST_KB:
		return 0x5c;
	case MEC152X_LDN_UART0:
		return 0x60;
	case MEC152X_LDN_UART1:
		return 0x64;
	case MEC152X_LDN_EMI0:
		return 0x68;
	case MEC152X_LDN_EMI1:
		return 0x6c;
	case MEC152X_LDN_PORT80_0:
		return 0x74;
	case MEC152X_LDN_PORT80_1:
		return 0x78;
	case MEC152X_LDN_RTC:
		return 0x7c;
	case MEC152X_LDN_TEST:
		return 0x84;
	case MEC152X_LDN_UART2:
		return 0x88;
	}
	return 0;
}

// Find the true IRQ register index for a LDN, return 0 on error.
static uint8_t ldn_to_irq(uint8_t ldn, uint8_t index)
{
	if (index == 0x70) {
		switch (ldn) {
		case MEC152X_LDN_MAILBOX:
			return 0xac;
		case MEC152X_LDN_KBC:
			return 0xae;
		case MEC152X_LDN_ACPI_EC0:
			return 0xb0;
		case MEC152X_LDN_ACPI_EC1:
			return 0xb1;
		case MEC152X_LDN_ACPI_EC2:
			return 0xb2;
		case MEC152X_LDN_ACPI_EC3:
			return 0xb3;
		case MEC152X_LDN_UART0:
			return 0xb5;
		case MEC152X_LDN_UART1:
			return 0xb6;
		case MEC152X_LDN_EMI0:
			return 0xb7;
		case MEC152X_LDN_EMI1:
			return 0xb9;
		case MEC152X_LDN_RTC:
			return 0xbd;
		case MEC152X_LDN_UART2:
			return 0xbf;
		// There is currently no way to set the "EC" IRQ register with
		// the index 0xbe. Currently, no known mainboard uses this.
		}
	} else if (index == 0x72) {
		switch (ldn) {
		case MEC152X_LDN_MAILBOX:
			return 0xad;
		case MEC152X_LDN_KBC:
			return 0xaf;
		case MEC152X_LDN_EMI0:
			return 0xb8;
		case MEC152X_LDN_EMI1:
			return 0xba;
		}
	}
	return 0;
}

/*
 * IO BARs don't live in normal LDN configuration space but in the eSPI I/O
 * component. Thus we ignore the index and choose what BAR to set just based
 * on the LDN.
 */
static void set_iobase(struct device *espiioc, struct device *dev,
	uint8_t index, uint16_t iobase)
{
	if (index != 0x60) {
		printk(BIOS_WARNING,
			"%s: invalid base register index (io 0x%02x), ignoring",
			dev_path(dev), index);
		return;
	}

	const uint8_t bar = ldn_to_bar(dev->path.pnp.device);
	if (bar) {
		pnp_set_logical_device(espiioc);
		// writing lsb first, then msb is crucial only for MEC152X_LDN_ESPI_IOC
		pnp_write_config(espiioc, bar + 2, iobase & 0xff); // address lsb
		pnp_write_config(espiioc, bar + 3, (iobase >> 8) & 0xff); // address msb
		pnp_write_config(espiioc, bar + 0, 1); // valid bit
	} else {
		printk(BIOS_WARNING,
			"%s: could not lookup true base register index for io 0x%02x, ignoring",
			dev_path(dev), index);
	}
}

/*
 * IRQs don't live in normal LDN configuration space but in the eSPI I/O
 * component.
 *
 * The following fake offsets are used:
 *   0x70 => First IRQ (lower host config index number)
 *   0x72 => Second IRQ (higher host config index, if present)
 */
static void set_irq(struct device *espiioc, struct device *dev,
	uint8_t index, uint8_t irq)
{
	if (index >= PNP_IDX_MSC0) {
		pnp_set_logical_device(dev);
		pnp_write_config(dev, index, irq);
		return;
	}

	uint8_t true_index = ldn_to_irq(dev->path.pnp.device, index);
	if (true_index) {
		pnp_set_logical_device(espiioc);
		pnp_write_config(espiioc, true_index, irq);
	} else {
		printk(BIOS_WARNING,
			"%s: could not lookup true irq register index for irq 0x%02x, ignoring",
			dev_path(dev), index);
	}
}

/*
 * There are no DMA channels. Since sometimes in coreboot devicetrees, `drq`
 * is used as a "8 bit register write" primitive, we'll retain this
 * behavior and write to the actual LDN, not remapping to eSPI I/O component
 * like we do above.
 */
static void set_drq(struct device *dev, uint8_t index, uint8_t drq)
{
	pnp_set_logical_device(dev);
	pnp_write_config(dev, index, drq);
}

/*
 * This function is a hack/workaround for the nonstandard register layout
 * in this chip; normally, any LDN's register 0x60 sets its base address; but
 * on the MEC152x, an LDN's base address is set via the magic "ESPI I/O
 * Component" LDN at an LDN-specific offset.
 * We hijack the set_resources call that is caused by a `io 0x60 = 0x1337`
 * devicetree line, ignore the 0x60 part and use the parent's LDN to map it to
 * the correct register. (Similar for `irq` and `drq` entries.)
 */
static void set_resources(struct device *dev)
{
	struct device *espiioc = dev_find_slot_pnp(dev->path.pnp.port, MEC152X_LDN_ESPI_IOC);
	if (!espiioc) {
		printk(BIOS_ERR, "MEC152x ESPI I/O component interface not present in device tree!\n");
		return;
	}

	pnp_enter_conf_mode(dev);
	for (struct resource *res = dev->resource_list; res; res = res->next) {
		if (!(res->flags & IORESOURCE_ASSIGNED)) {
			printk(BIOS_NOTICE, "%s %02lx %s size: 0x%010llx not assigned in devicetree\n",
				   dev_path(dev), res->index, resource_type(res),
				   res->size);
			continue;
		}

		if (res->flags & IORESOURCE_IO) {
			set_iobase(espiioc, dev, res->index, res->base);
		} else if (res->flags & IORESOURCE_IRQ) {
			set_irq(espiioc, dev, res->index, res->base);
		} else if (res->flags & IORESOURCE_DRQ) {
			set_drq(dev, res->index, res->base);
		} else {
			printk(BIOS_ERR, "%s %02lx unknown resource type\n",
				   dev_path(dev), res->index);
			continue;
		}

		res->flags |= IORESOURCE_STORED;
		report_resource_stored(dev, res, "");
	}
	pnp_exit_conf_mode(dev);
}

static void init(struct device *dev)
{
	if (dev->enabled && dev->path.pnp.device == MEC152X_LDN_KBC)
		pc_keyboard_init(NO_AUX_DEVICE);
}

static void enable_dev(struct device *dev)
{
	static struct device_operations ops = {
		.read_resources   = pnp_read_resources,
		.set_resources    = set_resources,
		.enable_resources = pnp_enable_resources,
		.enable           = pnp_alt_enable,
		.init             = init,
		.ops_pnp_mode     = &pnp_conf_mode_55_aa,
	};

	/*
	 * source: MEC152x Data Sheet (DS00003427C), table 9-6 and 9-11,
	 * note that masks are inverted.
	 * The datasheet does not tell us whether we may use addresses
	 * from 0-0xfff or from 0-0xffff, so we'll take the safer option.
	 */
	static struct pnp_info pnp_dev_info[] = {
		{ NULL, MEC152X_LDN_ESPI_IOC, PNP_IO0, 0xffe },
		{ NULL, MEC152X_LDN_ESPI_MC, PNP_IO0, 0xfff },
		{ NULL, MEC152X_LDN_MAILBOX, PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0xffe },
		/* The datasheet lists an (inverted) mask of 4 (i.e. 0xffb or
		 * 0b111111111011, but coreboot wants the block of ones continuous.
		 * So we expand the KBC's and ACPI_EC0's mask to 7 (i.e. 0xff8) */
		{ NULL, MEC152X_LDN_KBC, PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0xff8 },
		{ NULL, MEC152X_LDN_ACPI_EC0, PNP_IO0 | PNP_IRQ0, 0xff8 },
		{ NULL, MEC152X_LDN_ACPI_EC1, PNP_IO0 | PNP_IRQ0, 0xff8 },
		{ NULL, MEC152X_LDN_ACPI_EC2, PNP_IO0 | PNP_IRQ0, 0xff8 },
		{ NULL, MEC152X_LDN_ACPI_EC3, PNP_IO0 | PNP_IRQ0, 0xff8 },
		{ NULL, MEC152X_LDN_ACPI_PM1, PNP_IO0, 0xff8 },
		{ NULL, MEC152X_LDN_LEGACY_FAST_KB, PNP_IO0, 0xff8 },
		{ NULL, MEC152X_LDN_UART0, PNP_IO0 | PNP_IRQ0, 0xff8 },
		{ NULL, MEC152X_LDN_UART1, PNP_IO0 | PNP_IRQ0, 0xff8 },
		{ NULL, MEC152X_LDN_EMI0, PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0xff0 },
		{ NULL, MEC152X_LDN_EMI1, PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0xff0 },
		{ NULL, MEC152X_LDN_PORT80_0, PNP_IO0, 0xfff },
		{ NULL, MEC152X_LDN_PORT80_1, PNP_IO0, 0xfff },
		{ NULL, MEC152X_LDN_RTC, PNP_IO0 | PNP_IRQ0, 0xfe0 },
		{ NULL, MEC152X_LDN_TEST, PNP_IO0, 0xfe0 },
		{ NULL, MEC152X_LDN_UART2, PNP_IO0 | PNP_IRQ0, 0xff8 },
	};

	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_microchip_mec152x_ops = {
	.name = "Microchip MEC152x Super I/O / Embedded Controller",
	.enable_dev = enable_dev,
};
