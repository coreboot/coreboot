/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#ifndef _SERIALIO_H_
#define _SERIALIO_H_

#define SIO_REG_PPR_CLOCK         0x200
#define SIO_REG_PPR_CLOCK_EN      (1 << 0)
#define SIO_REG_PPR_CLOCK_UPDATE  (1 << 31)
#define SIO_REG_PPR_CLOCK_N_DIV   0xc35
#define SIO_REG_PPR_CLOCK_M_DIV   0x30

#define SIO_REG_PPR_RESETS        0x204
#define SIO_REG_PPR_RESETS_FUNC   (1 << 0)
#define SIO_REG_PPR_RESETS_APB    (1 << 1)
#define SIO_REG_PPR_RESETS_IDMA   (1 << 2)

typedef enum {
	PchSerialIoDisabled,
	PchSerialIoAcpi,
	PchSerialIoPci,
	PchSerialIoAcpiHidden,
	PchSerialIoLegacyUart,
	PchSerialIoSkipInit
} PCH_SERIAL_IO_MODE;

typedef enum {
	PchSerialIoIndexI2C0,
	PchSerialIoIndexI2C1,
	PchSerialIoIndexI2C2,
	PchSerialIoIndexI2C3,
	PchSerialIoIndexI2C4,
	PchSerialIoIndexI2C5,
	PchSerialIoIndexSpi0,
	PchSerialIoIndexSpi1,
	PchSerialIoIndexUart0,
	PchSerialIoIndexUart1,
	PchSerialIoIndexUart2,
	PchSerialIoIndexMax
} PCH_SERIAL_IO_CONTROLLER;

#endif

