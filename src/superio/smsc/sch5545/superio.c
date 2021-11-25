/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <console/console.h>
#include <pc80/keyboard.h>
#include <stdlib.h>

#include "sch5545.h"

int sch5545_get_gpio(uint8_t sio_port, uint8_t gpio)
{
	struct device *dev;
	uint16_t runtime_reg_base;
	uint8_t gpio_bank, gpio_num;

	gpio_bank = gpio / 10;
	gpio_num = gpio % 10;
	/*
	 * GPIOs are divided into banks of 8 GPIOs (kind of). Each group starts
	 * at decimal base, i.e. 8 GPIOs from GPIO000, 8 GPIOs from GPIO010,
	 * etc., up to GPIO071 and GPIO072 which are an exception (only two
	 * gpios in the bank 7).
	 */
	if (gpio_num > 7)
		return -1;
	else if (gpio_bank == 7 && gpio_num > 1)
		return -1;
	else if (gpio_bank > 7)
		return -1;

	dev = dev_find_slot_pnp(sio_port, SCH5545_LDN_LPC);

	if (!dev) {
		printk(BIOS_ERR, "%s: ERROR: LPC interface LDN not present."
		       "Check the devicetree!\n", __func__);
		return -1;
	}

	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);

	runtime_reg_base = pnp_read_config(dev, SCH5545_BAR_RUNTIME_REG + 2);
	runtime_reg_base |= pnp_read_config(dev, SCH5545_BAR_RUNTIME_REG + 3) << 8;

	pnp_exit_conf_mode(dev);

	if (runtime_reg_base == 0)
		return -1;

	outb(gpio_bank * 8 + gpio_num, runtime_reg_base + SCH5545_RR_GPIO_SEL);

	return inb(runtime_reg_base + SCH5545_RR_GPIO_READ) & 1;
}

static void sch5545_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case SCH5545_LDN_KBC:
		pnp_enter_conf_mode(dev);
		pnp_set_logical_device(dev);
		/* Disable PS/2 clock and data isolation */
		pnp_unset_and_set_config(dev, 0xf0,
					 SCH5545_KBD_ISOLATION | SCH5545_MOUSE_ISOLATION, 0);
		pnp_exit_conf_mode(dev);
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	case SCH5545_LDN_LPC:
		pnp_enter_conf_mode(dev);
		pnp_set_logical_device(dev);
		/* Enable SERIRQ */
		pnp_write_config(dev, 0x24, pnp_read_config(dev, 0x24) | 0x04);
		pnp_exit_conf_mode(dev);
		break;
	}
}

static void sch5545_set_iobase(struct device *dev, u8 index, u16 iobase)
{
	u8 val;
	struct device *lpc_if;
	u8 iobase_reg = 0;

	lpc_if = dev_find_slot_pnp(dev->path.pnp.port, SCH5545_LDN_LPC);

	if (!lpc_if) {
		printk(BIOS_ERR, "ERROR: %s LPC interface LDN not present."
		       "Check the devicetree!\n", dev_path(dev));
		return;
	}

	switch (dev->path.pnp.device) {
	case SCH5545_LDN_EMI:
		iobase_reg = SCH5545_BAR_EM_IF;
		break;
	case SCH5545_LDN_KBC:
		iobase_reg = SCH5545_BAR_KBC;
		break;
	case SCH5545_LDN_UART1:
		iobase_reg = SCH5545_BAR_UART1;
		break;
	case SCH5545_LDN_UART2:
		iobase_reg = SCH5545_BAR_UART2;
		break;
	case SCH5545_LDN_RR:
		iobase_reg = SCH5545_BAR_RUNTIME_REG;
		break;
	case SCH5545_LDN_FDC:
		iobase_reg = SCH5545_BAR_FLOPPY;
		break;
	case SCH5545_LDN_LPC:
		iobase_reg = SCH5545_BAR_LPC_IF;
		break;
	case SCH5545_LDN_PP:
		iobase_reg = SCH5545_BAR_PARPORT;
		break;
	default:
		return;
	}

	pnp_set_logical_device(lpc_if);

	/* Flip the bytes in IO base, LSB goes first */
	pnp_write_config(lpc_if, iobase_reg + 2, iobase & 0xff);
	pnp_write_config(lpc_if, iobase_reg + 3, (iobase >> 8) & 0xff);

	/* Set valid bit */
	val = pnp_read_config(lpc_if, iobase_reg + 1);
	val |= 0x80;
	pnp_write_config(lpc_if, iobase_reg + 1, val);

	pnp_set_logical_device(dev);
}

static void sch5545_set_irq(struct device *dev, u8 index, u8 irq)
{
	u8 select_bit = 0;
	struct device *lpc_if;

	/* In case it is not the IRQ, write misc register directly */
	if (index >= PNP_IDX_MSC0) {
		pnp_write_config(dev, index, irq);
		return;
	}

	lpc_if = dev_find_slot_pnp(dev->path.pnp.port, SCH5545_LDN_LPC);

	if (!lpc_if) {
		printk(BIOS_ERR, "ERROR: %s LPC interface LDN not present."
		       "Check the devicetree!\n", dev_path(dev));
		return;
	}

	pnp_set_logical_device(lpc_if);

	/*
	 * Some LDNs can generate IRQs from two sources, i.e.
	 * - EMI may generate interrupts for Mailbox and INT source register
	 * - KBC may generate separate IRQ for mouse and keyboard,
	 * - RR LDN may generate IRQ for PME and SMI etc.
	 * SELECT bit allows to distinguish IRQ source for single LDN.
	 * Use the standard IRQs for devices.
	 */
	switch (dev->path.pnp.device) {
	case SCH5545_LDN_EMI:
	case SCH5545_LDN_KBC:
	case SCH5545_LDN_RR:
		if (index == 0x72)
			select_bit = 0x80;
		break;
	default:
		break;
	}

	/*
	 * IRQs are set in a little different manner. Each IRQ number has its
	 * own register which is programmed with LDN number which should use
	 * the IRQ. Ignore the index offset and choose register based on IRQ
	 * number counting from IRQ base.
	 */
	pnp_write_config(lpc_if, SCH5545_IRQ_BASE + irq, dev->path.pnp.device | select_bit);
	pnp_set_logical_device(dev);
}

static void sch5545_set_drq(struct device *dev, u8 index, u8 drq)
{
	struct device *lpc_if;

	if (drq == 4) {
		printk(BIOS_ERR, "ERROR: %s %02x: Trying to set reserved DMA channel 4!\n",
		       dev_path(dev), index);
		printk(BIOS_ERR, "This configuration is untested. Trying to continue.\n");
	}

	/* DMA channel is programmed via LPC LDN */
	lpc_if = dev_find_slot_pnp(dev->path.pnp.port, SCH5545_LDN_LPC);

	if (!lpc_if) {
		printk(BIOS_ERR, "ERROR: %s LPC interface LDN not present."
		       "Check the devicetree!\n", dev_path(dev));
		return;
	}

	pnp_set_logical_device(lpc_if);

	/*
	 * There are 8 configurable DMA channels. DRQs are set in a little
	 * different manner. Each DMA channel has its own 16-bit register which
	 * is programmed with LDN number (in higher byte) which should use the
	 * IRQ. Ignore the index offset and choose register based on IRQ number
	 * counting from IRQ base. Set valid bit (bit 7) additionally.
	 */
	pnp_write_config(dev, SCH5545_DRQ_BASE + (drq * 2) + 1, dev->path.pnp.device | 0x80);

	pnp_set_logical_device(dev);
}

static void sch5545_set_resource(struct device *dev, struct resource *resource)
{
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
		/*
		 * The PNP_MSC super IO registers have the IRQ flag set. If no
		 * value is assigned in the devicetree, the corresponding
		 * PNP_MSC register doesn't get written, which should be printed
		 * as warning and not as error.
		 */
		if (resource->flags & IORESOURCE_IRQ &&
		    (resource->index != PNP_IDX_IRQ0) &&
		    (resource->index != PNP_IDX_IRQ1))
			printk(BIOS_WARNING, "WARNING: %s %02lx %s size: "
			       "0x%010llx not assigned\n", dev_path(dev),
			       resource->index, resource_type(resource),
			       resource->size);
		else
			printk(BIOS_ERR, "ERROR: %s %02lx %s size: 0x%010llx "
			       "not assigned\n", dev_path(dev), resource->index,
			       resource_type(resource), resource->size);
		return;
	}

	/* Now store the resource. */
	if (resource->flags & IORESOURCE_IO) {
		sch5545_set_iobase(dev, resource->index, resource->base);
	} else if (resource->flags & IORESOURCE_DRQ) {
		sch5545_set_drq(dev, resource->index, resource->base);
	} else if (resource->flags & IORESOURCE_IRQ) {
		sch5545_set_irq(dev, resource->index, resource->base);
	} else {
		printk(BIOS_ERR, "ERROR: %s %02lx unknown resource type\n",
		       dev_path(dev), resource->index);
		return;
	}
	resource->flags |= IORESOURCE_STORED;

	report_resource_stored(dev, resource, "");
}

static void sch5545_set_resources(struct device *dev)
{
	struct resource *res;

	pnp_enter_conf_mode(dev);

	/* Select the logical device (LDN). */
	pnp_set_logical_device(dev);

	for (res = dev->resource_list; res; res = res->next)
		sch5545_set_resource(dev, res);

	pnp_exit_conf_mode(dev);
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = sch5545_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = sch5545_init,
	.ops_pnp_mode     = &pnp_conf_mode_55_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, SCH5545_LDN_EMI, PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0x0ff0 },
	{ NULL, SCH5545_LDN_KBC, PNP_IO0 | PNP_IRQ0 | PNP_IRQ1 | PNP_MSC0 | PNP_MSC1, 0x0fff },
	{ NULL, SCH5545_LDN_UART1, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8 },
	{ NULL, SCH5545_LDN_UART2, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8 },
	{ NULL, SCH5545_LDN_RR, PNP_IO0 | PNP_IRQ0 | PNP_IRQ1 | PNP_MSC0, 0x0fc0 },
	{ NULL, SCH5545_LDN_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_MSC0 | PNP_MSC1 |
				 PNP_MSC2 | PNP_MSC3 | PNP_MSC4 | PNP_MSC5, 0x0ff8, },
	{ NULL, SCH5545_LDN_LPC, PNP_IO0, 0x0ffe },
	{ NULL, SCH5545_LDN_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_MSC0 | PNP_MSC1, 0x0ff8 },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_smsc_sch5545_ops = {
	CHIP_NAME("SMSC SCH5545 Super I/O")
	.enable_dev = enable_dev,
};
