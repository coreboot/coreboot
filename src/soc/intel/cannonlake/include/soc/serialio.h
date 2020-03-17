/*
 * This file is part of the coreboot project.
 *
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

#if CONFIG(SOC_INTEL_COMETLAKE)
typedef enum {
	PchSerialIoNotInitialized,
	PchSerialIoDisabled,
	PchSerialIoPci,
	PchSerialIoHidden,
	PchSerialIoLegacyUart,
	PchSerialIoSkipInit,
	PchSerialIoMax,
} PCH_SERIAL_IO_MODE;
#else
typedef enum {
	PchSerialIoNotInitialized,
	PchSerialIoDisabled,
	PchSerialIoPci,
	PchSerialIoAcpi,
	PchSerialIoHidden,
	PchSerialIoMax,
} PCH_SERIAL_IO_MODE;
#endif

typedef enum {
	PchSerialIoIndexI2C0,
	PchSerialIoIndexI2C1,
	PchSerialIoIndexI2C2,
	PchSerialIoIndexI2C3,
	PchSerialIoIndexI2C4,
	PchSerialIoIndexI2C5,
	PchSerialIoIndexSPI0,
	PchSerialIoIndexSPI1,
	PchSerialIoIndexSPI2,
	PchSerialIoIndexUART0,
	PchSerialIoIndexUART1,
	PchSerialIoIndexUART2,
	PchSerialIoIndexMAX
} PCH_SERIAL_IO_CONTROLLER;

#endif
