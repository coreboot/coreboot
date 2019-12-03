/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/uart.h>
#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <amdblocks/gpio_banks.h>
#include <amdblocks/acpimmio.h>
#include <soc/southbridge.h>
#include <soc/gpio.h>

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

uintptr_t uart_platform_base(int idx)
{
	if (idx < 0 || idx > ARRAY_SIZE(uart_info))
		return 0;

	return uart_info[idx].base;
}

void set_uart_config(int idx)
{
	uint32_t uart_ctrl;
	uint16_t uart_leg;

	if (idx < 0 || idx > ARRAY_SIZE(uart_info))
		return;

	program_gpios(uart_info[idx].mux, 2);

	if (CONFIG(PICASSO_UART_1_8MZ)) {
		uart_ctrl = sm_pci_read32(SMB_UART_CONFIG);
		uart_ctrl |= 1 << (SMB_UART_1_8M_SHIFT + idx);
		sm_pci_write32(SMB_UART_CONFIG, uart_ctrl);
	}

	if (CONFIG(PICASSO_UART_LEGACY) && idx != 3) {
		/* Force 3F8 if idx=0, 2F8 if idx=1, 3E8 if idx=2 */

		/* TODO: make clearer once PPR is updated */
		uart_leg = (idx << 8) | (idx << 10) | (idx << 12) | (idx << 14);
		if (idx == 0)
			uart_leg |= 1 << FCH_LEGACY_3F8_SH;
		else if (idx == 1)
			uart_leg |= 1 << FCH_LEGACY_2F8_SH;
		else if (idx == 2)
			uart_leg |= 1 << FCH_LEGACY_3E8_SH;

		write16((void *)FCH_UART_LEGACY_DECODE, uart_leg);
	}
}

unsigned int uart_platform_refclk(void)
{
	return CONFIG(PICASSO_UART_48MZ) ? 48000000 : 115200 * 16;
}
