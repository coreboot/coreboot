/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Patrick Rudolph <siro@das-labor.org>
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

#ifndef SUPERIO_NUVOTON_NPCD378_H
#define SUPERIO_NUVOTON_NPCD378_H

/* HWM at LDN8 */
#define NPCD837_HWM_WRITE_LOCK_CTRL 0x4
#define NPCD837_HWM_WRITE_LOCK_BIT 0x1

#define NPCD378_HWM_PSU_FAN_MIN 0x17
#define NPCD378_HWM_PSU_FAN_MAX 0xf8
#define NPCD378_HWM_PSU_FAN_PWM_CTRL 0x198

/*
 * Read HWM register at specific page and offset.
 *
 * @param iobase IOBASE address of LDN8
 * @param reg MSB is page, LSB sets the offset in selected page
 *
 * @return Byte read from HWM
 */
uint8_t npcd378_hwm_read(const uint16_t iobase, const uint16_t reg);

/*
 * Write to HWM register at specific page and offset.
 *
 * @param iobase IOBASE address of LDN8
 * @param reg MSB is page, LSB sets the offset in selected page
 * @param val The value to write to HWM register
 */
void npcd378_hwm_write(const uint16_t iobase, const uint16_t reg,
		       const uint8_t val);

/*
 * Notify SuperIO a host-to-device transfer is ongoing.
 * Has to be called before any register in HWM is written to.
 *
 * @param iobase IOBASE address of LDN8
 */
void npcd378_hwm_write_start(const uint16_t iobase);

/*
 * Notify SuperIO a host-to-device transfer has finished.
 * Has to be called after any register in HWM was written to.
 *
 * @param iobase IOBASE address of LDN8
 */
void npcd378_hwm_write_finished(const uint16_t iobase);

/* Logical Device Numbers (LDN). */
/* Default Nuvoton hardware: */
#define NPCD378_FDC		0x00 /* Floppy */
#define NPCD378_PP		0x01 /* Parallel port */
#define NPCD378_SP1		0x02 /* Com1 */
#define NPCD378_SP2		0x03 /* Com2 & IR */
#define NPCD378_PWR		0x04 /* LED and PWR button control */
#define NPCD378_AUX		0x05 /* PS/2 mouse */
#define NPCD378_KBC		0x06 /* PS/2 keyboard */
/* The following is guessed based on observation and might be wrong: */
#define NPCD378_WDT1		0x07
#define NPCD378_HWM		0x08 /* HWM: 16 pages with each 255 byte */
#define NPCD378_GPIO_PP_OD	0x0F /* GPIO Push-Pull/Open drain select */
#define NPCD378_I2C		0x15 /* Provides 35Khz at IOBASE + 0x9 */
#define NPCD378_SUSPEND		0x1c /* Suspend control ? */
#define NPCD378_GPIOA		0x1e

#endif /* SUPERIO_NUVOTON_NPCD378_H */
