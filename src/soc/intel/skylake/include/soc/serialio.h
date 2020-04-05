/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SERIALIO_H_
#define _SERIALIO_H_

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
