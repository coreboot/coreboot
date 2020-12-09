/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <console/console.h>
#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <amdblocks/gpio_banks.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/aoac.h>
#include <soc/southbridge.h>
#include <soc/gpio.h>
#include <soc/uart.h>
#include <types.h>

static const struct _uart_info {
	uintptr_t base;
	struct soc_amd_gpio mux[2];
} uart_info[] = {
	[0] = { APU_UART0_BASE, {
			PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),
			PAD_NF(GPIO_136, UART0_RXD, PULL_NONE),
	} },
	[1] = { APU_UART1_BASE, {
			PAD_NF(GPIO_143, UART1_TXD, PULL_NONE),
			PAD_NF(GPIO_141, UART1_RXD, PULL_NONE),
	} },
	[2] = { APU_UART2_BASE, {
			PAD_NF(GPIO_137, UART2_TXD, PULL_NONE),
			PAD_NF(GPIO_135, UART2_RXD, PULL_NONE),
	} },
	[3] = { APU_UART3_BASE, {
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

static bool get_uart_idx(uintptr_t base, unsigned int *idx)
{
	for (unsigned int i = 0; i < ARRAY_SIZE(uart_info); i++) {
		if (base == uart_info[i].base) {
			*idx = i;
			return true;
		}
	}

	return false;
}

void clear_uart_legacy_config(void)
{
	write16((void *)FCH_LEGACY_UART_DECODE, 0);
}

void set_uart_legacy_config(unsigned int uart_idx, unsigned int range_idx)
{
	uint16_t uart_legacy_decode;
	uint8_t uart_map_offset;

	if (uart_idx >= ARRAY_SIZE(uart_info) || range_idx >= ARRAY_SIZE(uart_info))
		return;

	uart_legacy_decode = read16((void *)FCH_LEGACY_UART_DECODE);
	/* Map uart_idx to io range_idx */
	uart_map_offset = range_idx * FCH_LEGACY_UART_MAP_SIZE + FCH_LEGACY_UART_MAP_SHIFT;
	uart_legacy_decode &= ~(FCH_LEGACY_UART_MAP_MASK << uart_map_offset);
	uart_legacy_decode |= uart_idx << uart_map_offset;
	/* Enable io range */
	uart_legacy_decode |= 1 << range_idx;
	write16((void *)FCH_LEGACY_UART_DECODE, uart_legacy_decode);
}

static void enable_uart_legacy_decode(uintptr_t base)
{
	unsigned int idx;
	const uint8_t range_idx[ARRAY_SIZE(uart_info)] = {
		FCH_LEGACY_UART_RANGE_3F8,
		FCH_LEGACY_UART_RANGE_2F8,
		FCH_LEGACY_UART_RANGE_3E8,
		FCH_LEGACY_UART_RANGE_2E8,
	};

	if (get_uart_idx(base, &idx)) {
		set_uart_legacy_config(idx, range_idx[idx]);
	}
}

void set_uart_config(unsigned int idx)
{
	uint32_t uart_ctrl;

	if (idx >= ARRAY_SIZE(uart_info))
		return;

	program_gpios(uart_info[idx].mux, 2);

	if (CONFIG(AMD_SOC_UART_1_8MZ)) {
		uart_ctrl = sm_pci_read32(SMB_UART_CONFIG);
		uart_ctrl |= 1 << (SMB_UART_1_8M_SHIFT + idx);
		sm_pci_write32(SMB_UART_CONFIG, uart_ctrl);
	}

}

static const char *uart_acpi_name(const struct device *dev)
{
	switch (dev->path.mmio.addr) {
	case APU_UART0_BASE:
		return "FUR0";
	case APU_UART1_BASE:
		return "FUR1";
	case APU_UART2_BASE:
		return "FUR2";
	case APU_UART3_BASE:
		return "FUR3";
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
	case APU_UART2_BASE:
		dev_id = FCH_AOAC_DEV_UART2;
		break;
	case APU_UART3_BASE:
		dev_id = FCH_AOAC_DEV_UART3;
		break;
	default:
		printk(BIOS_ERR, "%s: Unknown device: %s\n", __func__, dev_path(dev));
		return;
	}

	if (dev->enabled) {
		power_on_aoac_device(dev_id);
		wait_for_aoac_enabled(dev_id);
		if (CONFIG(AMD_SOC_UART_LEGACY))
			enable_uart_legacy_decode(dev->path.mmio.addr);
	} else {
		power_off_aoac_device(dev_id);
	}
}

/* This gets called for both enabled and disabled devices. */
static void uart_inject_ssdt(const struct device *dev)
{
	acpigen_write_scope(acpi_device_path(dev));

	acpigen_write_STA(acpi_device_status(dev));

	acpigen_pop_len(); /* Scope */
}

struct device_operations picasso_uart_mmio_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.scan_bus = scan_static_bus,
	.acpi_name = uart_acpi_name,
	.enable = uart_enable,
	.acpi_fill_ssdt = uart_inject_ssdt,
};
