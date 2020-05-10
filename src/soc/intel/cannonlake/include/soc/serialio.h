/* SPDX-License-Identifier: GPL-2.0-only */

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
