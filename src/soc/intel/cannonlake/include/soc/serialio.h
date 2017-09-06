/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2017 Intel Corporation.
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

typedef enum {
	PchSerialIoDisabled,
	PchSerialIoPci,
	PchSerialIoAcpi,
	PchSerialIoHidden,
} PCH_SERIAL_IO_MODE;

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
