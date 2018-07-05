/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Definitions Two Wire Interface (TWI) (I2C) Allwinner CPUs
 */

#ifndef CPU_ALLWINNER_A10_TWI_H
#define CPU_ALLWINNER_A10_TWI_H

#include <types.h>

/* TWI_CTL values */
#define TWI_CTL_INT_EN		(1 << 7)
#define TWI_CTL_BUS_EN		(1 << 6)
#define TWI_CTL_M_START		(1 << 5)
#define TWI_CTL_M_STOP		(1 << 4)
#define TWI_CTL_INT_FLAG	(1 << 3)
#define TWI_CTL_A_ACK	(1 << 2)

/* TWI_STAT values */
enum twi_status {
	TWI_STAT_BUS_ERROR	= 0x00,		/**< Bus error */
	TWI_STAT_TX_START	= 0x08,		/**< START sent */
	TWI_STAT_TX_RSTART	= 0x10,		/**< Repeated START sent */
	TWI_STAT_TX_AW_ACK	= 0x18,		/**< Sent address+read, ACK */
	TWI_STAT_TX_AW_NAK	= 0x20,		/**< Sent address+read, NAK */
	TWI_STAT_TXD_ACK	= 0x28,		/**< Sent data, got ACK */
	TWI_STAT_TXD_NAK	= 0x30,		/**< Sent data, no ACK */
	TWI_STAT_LOST_ARB	= 0x38,		/**< Lost arbitration */
	TWI_STAT_TX_AR_ACK	= 0x40,		/**< Sent address+write, ACK */
	TWI_STAT_TX_AR_NAK	= 0x48,		/**< Sent address+write, NAK */
	TWI_STAT_RXD_ACK	= 0x50,		/**< Got data, sent ACK */
	TWI_STAT_RXD_NAK	= 0x58,		/**< Got data, no ACK */
	TWI_STAT_IDLE		= 0xf8,		/**< Bus idle*/
};

/* TWI_CLK values */
#define TWI_CLK_M_MASK		(0xf << 3)
#define TWI_CLK_M(m)		(((m - 1) << 3) & TWI_CLK_M_MASK)
#define TWI_CLK_N_MASK		(0x7 << 0)
#define TWI_CLK_N(n)		((n) & TWI_CLK_N_MASK)

struct a1x_twi {
	u32 addr;	/**< 0x00: Slave address */
	u32 xaddr;	/**< 0x04: Extended slave address */
	u32 data;	/**< 0x08: Data byte */
	u32 ctl;	/**< 0x0C: Control register */
	u32 stat;	/**< 0x10: Status register */
	u32 clk;	/**< 0x14: Clock control register */
	u32 reset;	/**< 0x18: Software reset */
	u32 efr;	/**< 0x1C: Enhanced Feature register */
	u32 lcr;	/**< 0x20: Line control register */
};

void a1x_twi_init(u8 bus, u32 speed_hz);

#endif				/* CPU_ALLWINNER_A10_TWI_H */
