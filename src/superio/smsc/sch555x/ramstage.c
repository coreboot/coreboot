;;/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <superio/conf_mode.h>
#include "sch555x.h"

static void sch555x_init(struct device *dev)
{
	if (dev->enabled && dev->path.pnp.device == SCH555x_LDN_8042)
		pc_keyboard_init(NO_AUX_DEVICE);
}

static uint8_t sch555x_ldn_to_bar(uint8_t ldn)
{
	switch (ldn) {
	case SCH555x_LDN_LPCI:
		return SCH555x_LPCI_LPCI_BAR;
	case SCH555x_LDN_EMI:
		return SCH555x_LPCI_EMI_BAR;
	case SCH555x_LDN_UART1:
		return SCH555x_LPCI_UART1_BAR;
	case SCH555x_LDN_UART2:
		return SCH555x_LPCI_UART2_BAR;
	case SCH555x_LDN_RUNTIME:
		return SCH555x_LPCI_RUNTIME_BAR;
	case SCH555x_LDN_8042:
		return SCH555x_LPCI_8042_BAR;
	case SCH555x_LDN_FDC:
		return SCH555x_LPCI_FDC_BAR;
	case SCH555x_LDN_PP:
		return SCH555x_LPCI_PP_BAR;
	default:
		return 0;
	}
}

/*
 * IO BARs don't live in normal LDN configuration space but in the LPC interface.
 * Thus we ignore the index and choose what BAR to set just based on the LDN.
 */
static void sch555x_set_iobase(struct device *lpci, struct device *dev,
	uint8_t index, uint16_t iobase)
{
	const uint8_t bar = sch555x_ldn_to_bar(dev->path.pnp.device);
	if (bar) {
		pnp_set_logical_device(lpci);
		pnp_unset_and_set_config(lpci, bar + 1, 0, 1 << 7);
		pnp_write_config(lpci, bar + 2, iobase & 0xff);
		pnp_write_config(lpci, bar + 3, (iobase >> 8) & 0xff);
	}
}

/*
 * IRQs don't live in normal LDN configuration space but in the LPC interface.
 *
 * The following fake offsets are used:
 *   0x70 => First IRQ
 *   0x72 => Second IRQ
 */
static void sch555x_set_irq(struct device *lpci, struct device *dev,
	uint8_t index, uint8_t irq)
{
	if (index >= PNP_IDX_MSC0) {
		pnp_set_logical_device(dev);
		pnp_write_config(dev, index, irq);
		return;
	}

	pnp_set_logical_device(lpci);
	switch (index) {
	case 0x70:
		pnp_write_config(lpci, SCH555x_LPCI_IRQ(irq), dev->path.pnp.device);
		break;
	case 0x72:
		pnp_write_config(lpci, SCH555x_LPCI_IRQ(irq), dev->path.pnp.device | 0x80);
		break;
	}
}

/*
 * DMA channels don't live in normal LDN configuration space but in the LPC interface.
 */
static void sch555x_set_drq(struct device *lpci, struct device *dev,
	uint8_t index, uint8_t drq)
{
	pnp_set_logical_device(lpci);
	pnp_write_config(lpci, SCH555x_LPCI_DMA(drq), dev->path.pnp.device | 0x80);
}

static void sch555x_set_resources(struct device *dev)
{
	struct device *lpci = dev_find_slot_pnp(dev->path.pnp.port, SCH555x_LDN_LPCI);
	if (!lpci) {
		printk(BIOS_ERR, "SCH555x LPC interface not present in device tree!\n");
		return;
	}

	pnp_enter_conf_mode(dev);
	for (struct resource *res = dev->resource_list; res; res = res->next) {
		if (res->flags & IORESOURCE_IO)
			sch555x_set_iobase(lpci, dev, res->index, res->base);
		else if (res->flags & IORESOURCE_DRQ)
			sch555x_set_drq(lpci, dev, res->index, res->base);
		else if (res->flags & IORESOURCE_IRQ)
			sch555x_set_irq(lpci, dev, res->index, res->base);
	}
	pnp_exit_conf_mode(dev);
}

static void sch555x_enable_dev(struct device *dev)
{
	static struct device_operations ops = {
		.read_resources   = pnp_read_resources,
		.set_resources    = sch555x_set_resources,
		.enable_resources = pnp_enable_resources,
		.enable           = pnp_alt_enable,
		.init             = sch555x_init,
		.ops_pnp_mode     = &pnp_conf_mode_55_aa,
	};

	static struct pnp_info pnp_dev_info[] = {
		{ NULL, SCH555x_LDN_EMI, PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0x0ff0 },
		{ NULL, SCH555x_LDN_8042, PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0x0fff },
		{ NULL, SCH555x_LDN_UART1, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8 },
		{ NULL, SCH555x_LDN_UART2, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8 },
		{ NULL, SCH555x_LDN_LPCI, PNP_IO0, 0x0ffe },
		{ NULL, SCH555x_LDN_RUNTIME, PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0x0fc0 },
		{ NULL, SCH555x_LDN_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ff8, },
		{ NULL, SCH555x_LDN_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ff8 },
	};

	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_smsc_sch555x_ops = {
	.name = "SMSC SCH555x Super I/O",
	.enable_dev = sch555x_enable_dev,
};
