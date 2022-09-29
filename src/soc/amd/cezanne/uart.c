/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/aoac.h>
#include <amdblocks/gpio.h>
#include <amdblocks/uart.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <soc/aoac_defs.h>
#include <soc/gpio.h>
#include <soc/southbridge.h>
#include <soc/uart.h>
#include <types.h>

static const struct {
	uintptr_t base;
	struct soc_amd_gpio mux[2];
} uart_info[] = {
	[0] = { APU_UART0_BASE, {
			PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
			PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	} },
	[1] = { APU_UART1_BASE, {
			PAD_NF(GPIO_140, UART1_TXD, PULL_NONE),
			PAD_NF(GPIO_142, UART1_RXD, PULL_NONE),
	} },
};

uintptr_t get_uart_base(unsigned int idx)
{
	if (idx >= ARRAY_SIZE(uart_info))
		return 0;

	return uart_info[idx].base;
}

void clear_uart_legacy_config(void)
{
	write16p(FCH_LEGACY_UART_DECODE, 0);
}

void set_uart_config(unsigned int idx)
{
	if (idx >= ARRAY_SIZE(uart_info))
		return;

	gpio_configure_pads(uart_info[idx].mux, 2);
}

static const char *uart_acpi_name(const struct device *dev)
{
	switch (dev->path.mmio.addr) {
	case APU_UART0_BASE:
		return "FUR0";
	case APU_UART1_BASE:
		return "FUR1";
	default:
		return NULL;
	}
}

/* Even though this is called enable, it gets called for both enabled and disabled devices. */
static void uart_enable(struct device *dev)
{
	unsigned int dev_id;

	switch (dev->path.mmio.addr) {
	case APU_UART0_BASE:
		dev_id = FCH_AOAC_DEV_UART0;
		break;
	case APU_UART1_BASE:
		dev_id = FCH_AOAC_DEV_UART1;
		break;
	default:
		printk(BIOS_ERR, "%s: Unknown device: %s\n", __func__, dev_path(dev));
		return;
	}

	if (dev->enabled) {
		power_on_aoac_device(dev_id);
		wait_for_aoac_enabled(dev_id);
	} else {
		power_off_aoac_device(dev_id);
	}
}

static void uart_read_resources(struct device *dev)
{
	mmio_resource_kb(dev, 0, dev->path.mmio.addr / KiB, 4);
}

struct device_operations cezanne_uart_mmio_ops = {
	.read_resources = uart_read_resources,
	.set_resources = noop_set_resources,
	.scan_bus = scan_static_bus,
	.enable = uart_enable,
	.acpi_name = uart_acpi_name,
	.acpi_fill_ssdt = uart_inject_ssdt,
};
