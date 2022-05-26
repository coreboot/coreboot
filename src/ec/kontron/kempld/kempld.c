/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>

#include "chip.h"
#include "kempld.h"
#include "kempld_internal.h"

static void kempld_uart_read_resources(struct device *dev)
{
	static const unsigned int io_addr[] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8 };

	const struct ec_kontron_kempld_config *const config = dev->chip_info;
	const unsigned int uart = dev->path.generic.subid;

	if (!config || uart >= KEMPLD_NUM_UARTS)
		return;

	const enum kempld_uart_io io = config->uart[uart].io;
	if (io >= ARRAY_SIZE(io_addr)) {
		printk(BIOS_ERR, "KEMPLD: Bad io value '%d' for UART#%u\n.", io, uart);
		dev->enabled = false;
		return;
	}

	const int irq = config->uart[uart].irq;
	if (irq >= 16) {
		printk(BIOS_ERR, "KEMPLD: Bad irq value '%d' for UART#%u\n.", irq, uart);
		dev->enabled = false;
		return;
	}

	struct resource *res_io = new_resource(dev, 0);
	res_io->base = io_addr[io];
	res_io->size = 8;
	res_io->flags = IORESOURCE_IO | IORESOURCE_FIXED |
			IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	struct resource *res_irq = new_resource(dev, 1);
	res_irq->base = irq;
	res_irq->size = 1;
	res_irq->flags = IORESOURCE_IRQ | IORESOURCE_FIXED |
			 IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	if (kempld_get_mutex(100) < 0)
		return;

	const uint8_t reg = uart ? KEMPLD_UART_1 : KEMPLD_UART_0;
	const uint8_t val = kempld_read8(reg);
	kempld_write8(reg,
			   (val & ~(KEMPLD_UART_IO_MASK | KEMPLD_UART_IRQ_MASK)) |
			   io << KEMPLD_UART_IO_SHIFT |
			   irq << KEMPLD_UART_IRQ_SHIFT);

	kempld_release_mutex();
}

static void kempld_uart_enable_resources(struct device *dev)
{
	if (kempld_get_mutex(100) < 0)
		return;

	const unsigned int uart = dev->path.generic.subid;
	const uint8_t reg = uart ? KEMPLD_UART_1 : KEMPLD_UART_0;
	kempld_write8(reg, kempld_read8(reg) | KEMPLD_UART_ENABLE);

	kempld_release_mutex();
}

static struct device_operations kempld_uart_ops = {
	.read_resources   = kempld_uart_read_resources,
	.enable_resources = kempld_uart_enable_resources,
};

static void kempld_enable_dev(struct device *const dev)
{
	if (dev->path.type == DEVICE_PATH_GENERIC) {
		switch (dev->path.generic.id) {
		case 0:
			if (dev->path.generic.subid < KEMPLD_NUM_UARTS) {
				dev->ops = &kempld_uart_ops;
				break;
			}
			__fallthrough;
		case 1:
			if (dev->path.generic.subid == 0) {
				kempld_i2c_device_init(dev);
				break;
			}
			__fallthrough;
		default:
			printk(BIOS_WARNING, "KEMPLD: Spurious device %s.\n", dev_path(dev));
			break;
		}
	} else if (dev->path.type == DEVICE_PATH_GPIO) {
		if (dev->path.gpio.id == 0) {
			if (kempld_gpio_pads_config(dev) < 0)
				printk(BIOS_ERR, "KEMPLD: GPIO configuration failed!\n");
		} else {
			printk(BIOS_WARNING, "KEMPLD: Spurious GPIO device %s.\n",
			       dev_path(dev));
		}
	}
}

struct chip_operations ec_kontron_kempld_ops = {
	CHIP_NAME("Kontron KEMPLD")
	.enable_dev = kempld_enable_dev,
};
