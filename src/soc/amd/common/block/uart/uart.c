/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <amdblocks/aoac.h>
#include <amdblocks/gpio.h>
#include <amdblocks/uart.h>
#include <device/device.h>

uintptr_t get_uart_base(unsigned int idx)
{
	size_t num_ctrlrs;
	const struct soc_uart_ctrlr_info *ctrlr = soc_get_uart_ctrlr_info(&num_ctrlrs);

	if (idx >= num_ctrlrs)
		return 0;

	return ctrlr[idx].base;
}

static enum cb_err get_uart_idx(uintptr_t base, const struct soc_uart_ctrlr_info *ctrlr,
				size_t num_ctrlrs, unsigned int *idx)
{
	unsigned int i;
	for (i = 0; i < num_ctrlrs; i++) {
		if (base == ctrlr[i].base) {
			*idx = i;
			return CB_SUCCESS;
		}
	}
	return CB_ERR;
}

static enum cb_err get_uart_aoac_device(uintptr_t base, unsigned int *aoac_dev)
{
	unsigned int idx;
	size_t num_ctrlrs;
	const struct soc_uart_ctrlr_info *ctrlr = soc_get_uart_ctrlr_info(&num_ctrlrs);

	if (get_uart_idx(base, ctrlr, num_ctrlrs, &idx) == CB_ERR)
		return CB_ERR;

	*aoac_dev = ctrlr[idx].aoac_device;
	return CB_SUCCESS;
}

void set_uart_config(unsigned int idx)
{
	size_t num_ctrlrs;
	const struct soc_uart_ctrlr_info *ctrlr = soc_get_uart_ctrlr_info(&num_ctrlrs);

	if (idx >= num_ctrlrs)
		return;

	gpio_configure_pads(ctrlr[idx].mux, 2);
}

#if CONFIG(HAVE_ACPI_TABLES)
static const char *uart_acpi_name(const struct device *dev)
{
	unsigned int idx;
	size_t num_ctrlrs;
	const struct soc_uart_ctrlr_info *ctrlr = soc_get_uart_ctrlr_info(&num_ctrlrs);

	if (get_uart_idx(dev->path.mmio.addr, ctrlr, num_ctrlrs, &idx) == CB_SUCCESS)
		return ctrlr[idx].acpi_name;
	else
		return NULL;
}

/* This gets called for both enabled and disabled devices. */
static void uart_inject_ssdt(const struct device *dev)
{
	acpigen_write_scope(acpi_device_path(dev));

	acpigen_write_STA(acpi_device_status(dev));

	acpigen_pop_len(); /* Scope */
}
#endif

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

struct device_operations amd_uart_mmio_ops = {
	.read_resources = uart_read_resources,
	.set_resources = noop_set_resources,
	.scan_bus = scan_static_bus,
	.enable = uart_enable,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = uart_acpi_name,
	.acpi_fill_ssdt = uart_inject_ssdt,
#endif
};
