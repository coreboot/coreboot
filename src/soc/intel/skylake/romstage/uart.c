/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <device/pci_def.h>
#include <stdint.h>
#include <soc/pci_devs.h>
#include <soc/pcr.h>
#include <soc/romstage.h>
#include <soc/serialio.h>
#include <gpio.h>

static void uartgpioinit(u8 hwflowcontrol);

void pch_uart_init(void)
{
	device_t dev = PCH_DEV_UART2;
	u32 tmp;
	u8 *base = (u8 *)CONFIG_TTYS0_BASE;

	/* Set configured UART2 base address */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, (u32)base);

	/* Enable memory access and bus master */
	tmp = pci_read_config32(dev, PCI_COMMAND);
	tmp |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	pci_write_config32(dev, PCI_COMMAND, tmp);

	/* Take UART2 out of reset */
	tmp = read32(base + SIO_REG_PPR_RESETS);
	tmp |= SIO_REG_PPR_RESETS_FUNC | SIO_REG_PPR_RESETS_APB |
		SIO_REG_PPR_RESETS_IDMA;
	write32(base + SIO_REG_PPR_RESETS, tmp);

	/* Set M and N divisor inputs and enable clock */
	tmp = read32(base + SIO_REG_PPR_CLOCK);
	tmp |= SIO_REG_PPR_CLOCK_EN | SIO_REG_PPR_CLOCK_UPDATE |
		(SIO_REG_PPR_CLOCK_N_DIV << 16) |
		(SIO_REG_PPR_CLOCK_M_DIV << 1);
	write32(base + SIO_REG_PPR_CLOCK, tmp);

	/* Put UART2 in byte access mode for 16550 compatibility */
	if (!IS_ENABLED(CONFIG_CONSOLE_SERIAL8250MEM_32))
		pcr_andthenor32(PID_SERIALIO,
		R_PCH_PCR_SERIAL_IO_GPPRVRW7, 0, SIO_PCH_LEGACY_UART2);

	/* Configure GPIO for UART2 in native mode*/
	uartgpioinit(FALSE);
}

/*
 * GPIO modes
 */
typedef struct {
	GPIO_PAD	pad;
	GPIO_PAD_MODE	mode;
} GPIO_PAD_NATIVE_FUNCTION;

/*
 * GPP_C Community for UART2 GPIO
 * pin order RXD/TXD/RTSB/CTSB
 */
static const GPIO_PAD_NATIVE_FUNCTION uartgpio[] = {
	{GPIO_LP_GPP_C20, GpioPadModeNative1},
	{GPIO_LP_GPP_C21, GpioPadModeNative1},
	{GPIO_LP_GPP_C22, GpioPadModeNative1},
	{GPIO_LP_GPP_C23, GpioPadModeNative1}
};

/*
 * GPIO config registers
 */
static const GPIO_GROUP_INFO gpio_group_info = {
	PID_GPIOCOM1,
	R_PCH_PCR_GPIO_GPP_C_PADCFG_OFFSET,
	V_PCH_GPIO_GPP_C_PAD_MAX,
	R_PCH_PCR_GPIO_GPP_C_SMI_STS,
	R_PCH_PCR_GPIO_GPP_C_SMI_EN
};

static void uartgpioinit(u8 hwflowcontrol)
{
	u32 index, pinsused, dw0reg, dw0regmask, padcfgreg, padnumber;

	if (hwflowcontrol)
		pinsused = PCH_SERIAL_IO_PINS_PER_UART_CONTROLLER;
	else
		pinsused = PCH_SERIAL_IO_PINS_PER_UART_CONTROLLER_NO_FLOW_CTRL;

	for (index = 0; index < pinsused; index++) {
		padnumber = GPIO_GET_PAD_NUMBER(uartgpio[index].pad);
		padcfgreg = 0x8 * padnumber + gpio_group_info.padcfgoffset;
		dw0regmask = (uartgpio[index].mode & GPIO_CONF_PAD_MODE_MASK)
						>> GPIO_CONF_PAD_MODE_BIT_POS;
		dw0regmask = (GpioHardwareDefault == dw0regmask) ?
						0x0 : B_PCH_GPIO_PAD_MODE;
		dw0reg = (((uartgpio[index].mode & GPIO_CONF_PAD_MODE_MASK) >>
		(GPIO_CONF_PAD_MODE_BIT_POS + 1)) << N_PCH_GPIO_PAD_MODE);

		pcr_andthenor32(gpio_group_info.community, padcfgreg,
					~(u32)dw0regmask, (u32)dw0reg);
	}
}
