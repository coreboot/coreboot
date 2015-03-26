/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Felix Held <felix-coreboot@felixheld.de>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/* Both NCT6776D and NCT6776F package variants are supported. */

#ifndef SUPERIO_NUVOTON_NCT6776_H
#define SUPERIO_NUVOTON_NCT6776_H

/* Logical Device Numbers (LDN). */
#define NCT6776_FDC		0x00 /* Floppy */
#define NCT6776_PP		0x01 /* Parallel port */
#define NCT6776_SP1		0x02 /* Com1 */
#define NCT6776_SP2		0x03 /* Com2 & IR */
#define NCT6776_KBC		0x05 /* PS/2 keyboard and mouse */
#define NCT6776_CIR		0x06
#define NCT6776_GPIO6789_V	0x07
#define NCT6776_WDT1_GPIO01A_V	0x08
#define NCT6776_GPIO1234567_V	0x09
#define NCT6776_ACPI		0x0A
#define NCT6776_HWM_FPLED	0x0B /* Hardware monitor & front LED */
#define NCT6776_VID		0x0D
#define NCT6776_CIRWKUP		0x0E /* CIR wakeup */
#define NCT6776_GPIO_PP_OD	0x0F /* GPIO Push-Pull/Open drain select */
#define NCT6776_SVID		0x14
#define NCT6776_DSLP		0x16 /* Deep sleep */
#define NCT6776_GPIOA_LDN	0x17

/* virtual LDN for GPIO and WDT */
#define NCT6776_WDT1		((0 << 8) | NCT6776_WDT1_GPIO01A_V)

#define NCT6776_GPIOBASE	((0 << 8) | NCT6776_WDT1_GPIO01A_V) //?

#define NCT6776_GPIO0		((1 << 8) | NCT6776_WDT1_GPIO01A_V)
#define NCT6776_GPIO1		((1 << 8) | NCT6776_GPIO1234567_V)
#define NCT6776_GPIO2		((2 << 8) | NCT6776_GPIO1234567_V)
#define NCT6776_GPIO3		((3 << 8) | NCT6776_GPIO1234567_V)
#define NCT6776_GPIO4		((4 << 8) | NCT6776_GPIO1234567_V)
#define NCT6776_GPIO5		((5 << 8) | NCT6776_GPIO1234567_V)
#define NCT6776_GPIO6		((6 << 8) | NCT6776_GPIO1234567_V)
#define NCT6776_GPIO7		((7 << 8) | NCT6776_GPIO1234567_V)
#define NCT6776_GPIO8		((0 << 8) | NCT6776_GPIO6789_V)
#define NCT6776_GPIO9		((1 << 8) | NCT6776_GPIO6789_V)
#define NCT6776_GPIOA		((2 << 8) | NCT6776_WDT1_GPIO01A_V)

#endif /* SUPERIO_NUVOTON_NCT6776_H */
