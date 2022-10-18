/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <commonlib/helpers.h>
#include <device/device.h>
#include <device/mmio.h>
#include <amdblocks/gpio.h>
#include <amdblocks/aoac.h>
#include <amdblocks/uart.h>
#include <soc/aoac_defs.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/southbridge.h>
#include <soc/uart.h>
#include <types.h>

static const struct soc_uart_ctrlr_info uart_info[] = {
	[0] =	{ APU_UART0_BASE, FCH_AOAC_DEV_UART0, "FUR0", {
			PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),
			PAD_NF(GPIO_136, UART0_RXD, PULL_NONE),
		} },
	[1] =	{ APU_UART1_BASE, FCH_AOAC_DEV_UART1, "FUR1", {
			PAD_NF(GPIO_143, UART1_TXD, PULL_NONE),
			PAD_NF(GPIO_141, UART1_RXD, PULL_NONE),
		} },
	[2] =	{ APU_UART2_BASE, FCH_AOAC_DEV_UART2, "FUR2", {
			PAD_NF(GPIO_137, UART2_TXD, PULL_NONE),
			PAD_NF(GPIO_135, UART2_RXD, PULL_NONE),
		} },
	[3] =	{ APU_UART3_BASE, FCH_AOAC_DEV_UART3, "FUR3", {
			PAD_NF(GPIO_140, UART3_TXD, PULL_NONE),
			PAD_NF(GPIO_142, UART3_RXD, PULL_NONE),
		} },
};

uintptr_t get_uart_base(unsigned int idx)
{
	if (idx >= ARRAY_SIZE(uart_info))
		return 0;

	return uart_info[idx].base;
}

static enum cb_err get_uart_idx(uintptr_t base, unsigned int *idx)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(uart_info); i++) {
		if (base == uart_info[i].base) {
			*idx = i;
			return CB_SUCCESS;
		}
	}
	return CB_ERR;
}

static enum cb_err get_uart_aoac_device(uintptr_t base, unsigned int *aoac_dev)
{
	unsigned int idx;
	if (get_uart_idx(base, &idx) == CB_ERR)
		return CB_ERR;

	*aoac_dev = uart_info[idx].aoac_device;
	return CB_SUCCESS;
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
	unsigned int idx;
	if (get_uart_idx(dev->path.mmio.addr, &idx) == CB_SUCCESS)
		return uart_info[idx].acpi_name;
	else
		return NULL;
}

/* Even though this is called enable, it gets called for both enabled and disabled devices. */
static void uart_enable(struct device *dev)
{
	unsigned int dev_id;

	if (get_uart_aoac_device(dev->path.mmio.addr, &dev_id) == CB_ERR) {
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

struct device_operations picasso_uart_mmio_ops = {
	.read_resources = uart_read_resources,
	.set_resources = noop_set_resources,
	.scan_bus = scan_static_bus,
	.acpi_name = uart_acpi_name,
	.enable = uart_enable,
	.acpi_fill_ssdt = uart_inject_ssdt,
};
