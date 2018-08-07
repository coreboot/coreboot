/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SUPERIO_ITE_IT8772F_H
#define SUPERIO_ITE_IT8772F_H

/* Supported thermal mode on TMPINx */
enum thermal_mode {
	THERMAL_MODE_DISABLED	= 0,
	THERMAL_DIODE,
	THERMAL_RESISTOR,
};

#define IT8772F_FDC  0x00 /* Floppy disk controller */
#define IT8772F_SP1  0x01 /* Com1 */
#define IT8772F_EC   0x04 /* Environment controller */
#define IT8772F_KBCK 0x05 /* Keyboard */
#define IT8772F_KBCM 0x06 /* Mouse */
#define IT8772F_GPIO 0x07 /* GPIO */
#define IT8772F_IR   0x0a /* Consumer IR */

/* Environmental Controller interface */
#define IT8772F_CONFIGURATION			0x00
#define IT8772F_INTERFACE_SELECT		0x0a
#define  IT8772F_INTERFACE_PSEUDO_EOC		(1 << 7)
#define  IT8772F_INTERFACE_SMB_ENABLE		(1 << 6)
#define  IT8772F_INTERFACE_SEL_DISABLE		(0 << 4)
#define  IT8772F_INTERFACE_SEL_SST_SLAVE	(1 << 4)
#define  IT8772F_INTERFACE_SEL_PECI		(2 << 4)
#define  IT8772F_INTERFACE_SEL_SST_HOST		(3 << 4)
#define  IT8772F_INTERFACE_CLOCK_32MHZ		(0 << 3)
#define  IT8772F_INTERFACE_CLOCK_24MHZ		(1 << 3)
#define  IT8772F_INTERFACE_SPEED_TOLERANCE	(1 << 2)
#define  IT8772F_INTERFACE_PECI_AWFCS		(1 << 0)
#define IT8772F_FAN_TAC2_READ			0x0e
#define IT8772F_FAN_TAC3_READ			0x0f
#define IT8772F_FAN_MAIN_CTL			0x13
#define  IT8772F_FAN_MAIN_CTL_TAC_SMART(x)	(1 << ((x)-1))
#define  IT8772F_FAN_MAIN_CTL_TAC_EN(x)		(1 << ((x)+3))
#define IT8772F_FAN_CTL_MODE			0x14
#define  IT8772F_FAN_CTL_ON(x)			(1 << ((x)-1))
#define  IT8772F_FAN_PWM_MIN_DUTY		(1 << 3)
#define  IT8772F_FAN_PWM_CLOCK_48MHZ		(0 << 4)
#define  IT8772F_FAN_PWM_CLOCK_24MHZ		(1 << 4)
#define  IT8772F_FAN_PWM_CLOCK_8MHZ		(3 << 4)
#define  IT8772F_FAN_PWM_CLOCK_6MHZ		(4 << 4)
#define  IT8772F_FAN_PWM_CLOCK_3MHZ		(5 << 4)
#define  IT8772F_FAN_PWM_CLOCK_1_5MHZ		(6 << 4)
#define  IT8772F_FAN_PWM_CLOCK_51KHZ		(7 << 4)
#define  IT8772F_FAN_CTL_POLARITY_LOW		(0 << 7)
#define  IT8772F_FAN_CTL_POLARITY_HIGH		(1 << 7)
#define IT8772F_FAN_CTL2_PWM_MODE		0x16
#define IT8772F_FAN_CTL3_PWM_MODE		0x17
#define  IT8772F_FAN_CTL_PWM_MODE_SOFTWARE	(0 << 7)
#define  IT8772F_FAN_CTL_PWM_MODE_AUTOMATIC	(1 << 7)
#define IT8772F_ADC_TEMP_CHANNEL_ENABLE		0x51
#define IT8772F_FAN_CTL2_PWM_START		0x6b
#define IT8772F_FAN_CTL2_AUTO_MODE		0x6c
#define IT8772F_FAN_CTL3_PWM_START		0x73
#define IT8772F_FAN_CTL3_AUTO_MODE		0x74
#define  IT8772F_FAN_CTL_AUTO_SMOOTHING_DIS	(0 << 7)
#define  IT8772F_FAN_CTL_AUTO_SMOOTHING_EN	(1 << 7)
#define IT8772F_EXTEMP_STATUS			0x88
#define  IT8772F_EXTEMP_STATUS_HOST_BUSY	(1 << 0)
#define IT8772F_EXTEMP_ADDRESS			0x89
#define IT8772F_EXTEMP_WRITE_LENGTH		0x8a
#define IT8772F_EXTEMP_READ_LENGTH		0x8b
#define IT8772F_EXTEMP_COMMAND			0x8c
#define IT8772F_EXTEMP_WRITE_DATA_2		0x8d
#define IT8772F_EXTEMP_CONTROL			0x8e
#define  IT8772F_EXTEMP_CONTROL_AUTO_32HZ	(0 << 6)
#define  IT8772F_EXTEMP_CONTROL_AUTO_16HZ	(1 << 6)
#define  IT8772F_EXTEMP_CONTROL_AUTO_8HZ	(2 << 6)
#define  IT8772F_EXTEMP_CONTROL_AUTO_4HZ	(3 << 6)
#define  IT8772F_EXTEMP_CONTROL_AUTO_START	(1 << 5)
#define  IT8772F_EXTEMP_CONTROL_AUTO_ABORT	(1 << 4)
#define  IT8772F_EXTEMP_CONTROL_AUTO_TWO_DOMAIN	(1 << 3)
#define  IT8772F_EXTEMP_CONTROL_CONTENTION	(1 << 2)
#define  IT8772F_EXTEMP_CONTROL_SST_IDLE_HIGH	(1 << 1)
#define  IT8772F_EXTEMP_CONTROL_START		(1 << 0)

/* Standard PECI GetTemp */
#define PECI_CLIENT_ADDRESS			0x30
#define PECI_GETTEMP_COMMAND			0x01
#define PECI_GETTEMP_WRITE_LENGTH		0x01
#define PECI_GETTEMP_READ_LENGTH		0x02

/* GPIO interface */
#define IT8772F_GPIO_LED_BLINK1_PINMAP		0xf8

#define SIO_GPIO_BLINK_GPIO10 0x08
#define SIO_GPIO_BLINK_GPIO22 0x12
#define SIO_GPIO_BLINK_GPIO45 0x25

#define IT8772F_GPIO_LED_BLINK1_CONTROL		0xf9

#define IT8772F_GPIO_BLINK_FREQUENCY_4_HZ   (0<<1)
#define IT8772F_GPIO_BLINK_FREQUENCY_1_HZ   (1<<1)
#define IT8772F_GPIO_BLINK_FREQUENCY_1_4_HZ (2<<1)
#define IT8772F_GPIO_BLINK_FREQUENCY_1_8_HZ (3<<1)

#define GPIO_REG_SELECT(x)   (0x25 + (x))

/* GPIO Polarity Select: 1: Inverting, 0: Non-inverting */
#define GPIO_REG_POLARITY(x) (0xb0 + (x))

/* GPIO Inernal Pull-up: 1: Enable, 0: Disable */
#define GPIO_REG_PULLUP(x)   (0xb8 + (x))

/* GPIO Function Select: 1: Simple I/O, 0: Alternate function */
#define GPIO_REG_ENABLE(x)   (0xc0 + (x))

/* GPIO Mode: 0: input mode, 1: output mode */
#define GPIO_REG_OUTPUT(x)   (0xc8 + (x))

#include <arch/io.h>
#include <stdint.h>

u8 it8772f_sio_read(pnp_devfn_t dev, u8 reg);
void it8772f_sio_write(pnp_devfn_t dev, u8 reg, u8 value);
void it8772f_ac_resume_southbridge(pnp_devfn_t dev);
void it8772f_gpio_setup(pnp_devfn_t dev, int set, u8 select, u8 polarity,
			u8 pullup, u8 output, u8 enable);

/* FIXME: should be static so will be removed later.. */
/* Global configuration registers. */
#define IT8772F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8772F_CONFIG_REG_LDN       0x07 /* Logical Device Number. */

void it8772f_enter_conf(pnp_devfn_t dev);
void it8772f_exit_conf(pnp_devfn_t dev);
void it8772f_gpio_led(pnp_devfn_t dev, int set, u8 select, u8 polarity, u8 pullup,
			u8 output, u8 enable, u8 led_pin_map, u8 led_freq);


#endif /* SUPERIO_ITE_IT8772F_H */
