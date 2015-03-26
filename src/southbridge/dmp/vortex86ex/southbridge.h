/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef SOUTHBRIDGE_H
#define SOUTHBRIDGE_H

#define	SB			PCI_DEV(0, 7, 0)
#define	SB_REG_LPCCR		0x41
#define	SB_REG_FRCSCR		0x42
#define	SB_REG_PIRQ_ROUTE	0x58
#define	SB_REG_UART_CFG_IO_BASE	0x60
#define	SB_REG_GPIO_CFG_IO_BASE	0x62
#define	SB_REG_CS_BASE0		0x90
#define	SB_REG_CS_BASE_MASK0	0x94
#define	SB_REG_CS_BASE1		0x98
#define	SB_REG_CS_BASE_MASK1	0x9c
#define	SB_REG_IPPCR		0xb0
#define	SB_REG_EXT_PIRQ_ROUTE	0xb4
#define	SB_REG_OCDCR		0xbc
#define	SB_REG_IPFCR		0xc0
#define	SB_REG_FRWPR		0xc4
#define	SB_REG_STRAP		0xce
#define	SB_REG_II2CCR		0xd4

#define	SB1			PCI_DEV(0, 7, 1)
#define	SB1_REG_EXT_PIRQ_ROUTE2	0xb4

#define	SYSTEM_CTL_PORT         0x92

#endif				/* SOUTHBRIDGE_H */
