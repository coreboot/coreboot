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

enum {
	PchSerialIoDisabled,
	PchSerialIoPci,
	PchSerialIoHidden,
	PchSerialIoLegacyUart,
	PchSerialIoSkipInit
};

enum {
	PchSerialIoIndexI2C0,
	PchSerialIoIndexI2C1,
	PchSerialIoIndexI2C2,
	PchSerialIoIndexI2C3,
	PchSerialIoIndexI2C4,
	PchSerialIoIndexI2C5,
};

enum {
	PchSerialIoIndexGSPI0,
	PchSerialIoIndexGSPI1,
	PchSerialIoIndexGSPI2,
	PchSerialIoIndexGSPI3,
};

enum {
	PchSerialIoIndexUART0,
	PchSerialIoIndexUART1,
	PchSerialIoIndexUART2,
};

#endif
