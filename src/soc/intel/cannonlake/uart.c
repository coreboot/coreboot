/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation
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

#include <assert.h>
#include <console/uart.h>
#include <device/pci_def.h>
#include <intelblocks/gpio.h>
#include <intelblocks/lpss.h>
#include <intelblocks/pcr.h>
#include <intelblocks/uart.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/iomap.h>

/* Clock divider parameters for 115200 baud rate */
#define CLK_M_VAL	0x30
#define CLK_N_VAL	0xc35

static const struct port {
	struct pad_config pads[2]; /* just TX and RX */
	device_t dev;
} uart_ports[] = {
	{.dev = PCH_DEV_UART0,
	 .pads = { PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1), /* RX */
		   PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1)} /* TX */
	},
	{.dev = PCH_DEV_UART1,
	 .pads = { PAD_CFG_NF(GPP_C12, NONE, DEEP, NF1), /* RX */
		   PAD_CFG_NF(GPP_C13, NONE, DEEP, NF1)} /* TX */
	},
	{.dev = PCH_DEV_UART2,
	 .pads = { PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), /* RX */
		   PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1)} /* TX */
	}
};

void pch_uart_init(void)
{
	uintptr_t base;
	const struct port *p;

	assert(CONFIG_UART_FOR_CONSOLE < ARRAY_SIZE(uart_ports));
	p = &uart_ports[CONFIG_UART_FOR_CONSOLE];
	base = uart_platform_base(CONFIG_UART_FOR_CONSOLE);

	uart_common_init(p->dev, base, CLK_M_VAL, CLK_N_VAL);
	gpio_configure_pads(p->pads, ARRAY_SIZE(p->pads));
}

uintptr_t uart_platform_base(int idx)
{
	/* We can only have one serial console at a time */
	return UART_DEBUG_BASE_ADDRESS;
}
