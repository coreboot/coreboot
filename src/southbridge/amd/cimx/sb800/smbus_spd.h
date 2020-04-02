/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SMBUS_SPD_H_
#define _SMBUS_SPD_H_

#define READ_BIT				0x01

#define SMBUS_INTERRUPT_MASK	0x02
#define HOSTBUSY_MASK			0x01

#define SMBUS_READ_BYTE_COMMAND	0x48
#define SMBUS_READ_COMMAND		0x44

#define MAX_READ_TSC_COUNT		(2000000000 / 10)

#define PMIO_INDEX_REG			0xCD6
#define PMIO_DATA_REG			0xCD7

#define SMBUS_BAR_LOW_BYTE		0x2C
#define SMBUS_BAR_HIGH_BYTE		0x2D

#define SMBUS_STATUS_REG		0x00
#define SMBUS_SLAVE_STATUS_REG	0x01
#define SMBUS_COMMAND_REG		0x02
#define SMBUS_CONTROL_REG		0x03
#define SMBUS_HOST_CMD_REG		0x04
#define SMBUS_DATA0_REG			0x05
#define SMBUS_CLOCK_REG			0x0E

#define STATUS__COMPLETED_SUCCESSFULLY	0x02

#define SMBUS_FREQUENCY_CONST	66000000 / 4

#endif
