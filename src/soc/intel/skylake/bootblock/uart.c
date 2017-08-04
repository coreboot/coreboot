/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
 * Copyright (C) 2016 Intel Corporation
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
#include <device/pci_def.h>
#include <gpio.h>
#include <intelblocks/lpss.h>
#include <intelblocks/pcr.h>
#include <intelblocks/uart.h>
#include <soc/bootblock.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>

/* Serial IO UART controller legacy mode */
#define PCR_SERIAL_IO_GPPRVRW7		0x618
#define PCR_SIO_PCH_LEGACY_UART0	(1 << 0)
#define PCR_SIO_PCH_LEGACY_UART1	(1 << 1)
#define PCR_SIO_PCH_LEGACY_UART2	(1 << 2)

/* UART2 pad configuration. Support RXD and TXD for now. */
static const struct pad_config uart2_pads[] = {
/* UART2_RXD */		PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
/* UART2_TXD */		PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
};

void pch_uart_init(void)
{
	uintptr_t base = uart_platform_base(CONFIG_UART_FOR_CONSOLE);

	uart_common_init(PCH_DEV_UART2, base);

	/* Put UART2 in byte access mode for 16550 compatibility */
	if (!IS_ENABLED(CONFIG_DRIVERS_UART_8250MEM_32))
		pcr_write32(PID_SERIALIO, PCR_SERIAL_IO_GPPRVRW7,
			PCR_SIO_PCH_LEGACY_UART2);

	gpio_configure_pads(uart2_pads, ARRAY_SIZE(uart2_pads));
}
