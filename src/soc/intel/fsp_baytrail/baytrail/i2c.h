/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Siemens AG
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

#ifndef __SOC_INTEL_FSP_BAYTRAIL_I2C_H__
#define __SOC_INTEL_FSP_BAYTRAIL_I2C_H__

#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <stdlib.h>

/* SMBus controller settings in PCI configuration space */
#define I2C_PCI_VENDOR_ID	0x8086
#define I2C0_PCI_DEV_ID		0x0f41
#define I2C1_PCI_DEV_ID		0x0f42
#define I2C2_PCI_DEV_ID		0x0f43
#define I2C3_PCI_DEV_ID		0x0f44
#define I2C4_PCI_DEV_ID		0x0f45
#define I2C5_PCI_DEV_ID		0x0f46
#define I2C6_PCI_DEV_ID		0x0f47

#define I2C0_MEM_BASE		0xd0921000
#define I2C1_MEM_BASE		0xd0923000
#define I2C2_MEM_BASE		0xd0925000
#define I2C3_MEM_BASE		0xd0927000
#define I2C4_MEM_BASE		0xd0929000
#define I2C5_MEM_BASE		0xd092b000
#define I2C6_MEM_BASE		0xd092d000

#define I2C_STANDARD_MODE	0x1
#define I2C_FAST_MODE		0x2

/* Define relevant registers in PCI space */
#define I2C_PCI_COMMAND		0x4
#define I2C_PCI_STATUS		0x6

/* Define memory mapped registers */
#define I2C_CTRL		0x0
#define  I2C_SLAVE_DISABLE	0x40
#define  I2C_RESTART_EN		0x20
#define  I2C_ADR_MODE		0x10
#define  I2C_SPEED_MASK		0x6
#define   I2C_STD_MODE		0x1
#define   I2C_FAST_MODE		0x2
#define  I2C_MASTER_ENABLE	0x1

#define I2C_TARGET_ADR		0x4
#define I2C_TARGET_ADR_MASK	0x3ff

#define I2C_DATA_CMD		0x10
#define  I2C_RESTART		0x400
#define  I2C_STOP		0x200
#define  I2C_RW_CMD		0x100

#define I2C_SS_SCL_HCNT		0x14	/* Counter for high period for 100 kHz SCL */
#define I2C_SS_SCL_LCNT		0x18	/* Counter for low period for 100 kHz SCL */
#define I2C_FS_SCL_HCNT		0x1c	/* Counter for high period for 400 kHz SCL */
#define I2C_FS_SCL_LCNT		0x20	/* Counter for low period for 400 kHz SCL */

#define I2C_INTR_STAT		0x2c	/* Interrupt status register, read only */
#define I2C_INTR_MASK		0x30	/* Interrupt mask register */
#define I2C_RAW_INTR_STAT	0x34	/* Raw interrupt status, read only */
#define  I2C_START_DETECT	0x400
#define  I2C_STOP_DETECT	0x200
#define  I2C_ACTIVITY		0x100
#define  I2C_TX_ABORT		0x40
#define  I2C_RD_REQ		0x20	/* Read request in slave mode */
#define  I2C_TX_EMPTY		0x10
#define  I2C_TX_OVERFLOW	0x8
#define  I2C_RX_FULL		0x4
#define  I2C_RX_OVERFLOW	0x2
#define  I2C_RX_UNDERFLOW	0x1

#define I2C_RX_TL		0x38	/* Rx FIFO threshold level 0..255 */
#define I2C_TX_TL		0x3c	/* Tx FIFO threshold level 0..255 */
#define I2C_CLR_INTR		0x40	/* Clear all events with a read */
#define I2C_CLR_TX_ABRT		0x54	/* Clear TX-Abort event with a read */

/* There are a bunch of interrupt clearing registers now which are not used! */
/* So proceed somewhat later with definition */
#define I2C_ENABLE		0x6c	/* 0: disable I2C controller, 1: enable */
#define I2C_STATUS		0x70
#define  I2C_MST_ACTIVITY	0x20	/* Master FSM activity */
#define  I2C_RFF		0x10	/* Receive FIFO completely full */
#define  I2C_RFNE		0x8	/* Receive FIFO not empty */
#define  I2C_TFE		0x4	/* Transmit FIFO completely empty */
#define  I2C_TFNF		0x2	/* Transmit FIFO not full */
#define  I2C_ACTIVE		0x1	/* 1: I2C currently in operation */

#define I2C_TXFLR		0x74	/* Current transmit FIFO level */
#define I2C_RXFLR		0x78	/* Current receive FIFO level */
#define I2C_SDA_HOLD		0x7c	/* Data hold time after SCL goes low */
#define I2C_ABORT_SOURCE	0x80
#define  I2C_ARB_LOST		0x1000	/* Arbitration lost */
#define  I2C_MASTER_DIS		0x800	/* Master was disabled by user */
#define  I2C_10B_RD_NORSTRT	0x400	/* 10 bit address read and RESTART disabled */
#define  I2C_SBYTE_NORSTRT	0x200	/* START with RESTART disabled */
#define  I2C_START_ACKDET	0x80	/* START byte was acknowledged */
#define  I2C_TX_DATA_NOACK	0x8	/* TX data not acknowledged */
#define  I2C_10B_ADR2_NOACK	0x4	/* Second address byte in 10 bit mode NACK */
#define  I2C_10B_ADR1_NOACK	0x2	/* First address byte in 10 bit NACK */
#define  I2C_7B_ADDR_NACK	0x1	/* 7 bit address byte not acknowledged */

#define I2C_ENABLE_STATUS	0x9c

/* Define some status and error values */
#define I2C_ERR_INVALID_ADR	0x1000000
#define I2C_ERR_TIMEOUT		0x2000000
#define I2C_ERR_ABORT		0x4000000
#define I2C_ERR			0x8000000
#define I2C_SUCCESS		0x0000000


#define I2C_TIMEOUT_US		2000	/* Use 2000 us as time */

/* Prototype section*/
int i2c_init(unsigned bus);
int i2c_read(unsigned bus, unsigned chip, unsigned addr, uint8_t *buf, unsigned len);
int i2c_write(unsigned bus, unsigned chip, unsigned addr, const uint8_t *buf, unsigned len);

#endif	/* __SOC_INTEL_FSP_BAYTRAIL_I2C_H__ */
