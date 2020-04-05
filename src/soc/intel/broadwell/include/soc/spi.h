/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _BROADWELL_SPI_H_
#define _BROADWELL_SPI_H_

/*
 * SPI Opcode Menu setup for SPIBAR lockdown
 * should support most common flash chips.
 */

#define SPIBAR_OFFSET		0x3800
#define SPIBAR8(x)		RCBA8(x + SPIBAR_OFFSET)
#define SPIBAR32(x)		RCBA32(x + SPIBAR_OFFSET)

/* Registers within the SPIBAR */
#define SPIBAR_SSFC		0x91
#define SPIBAR_FDOC		0xb0
#define SPIBAR_FDOD		0xb4

#define SPIBAR_HSFS                 0x04   /* SPI hardware sequence status */
#define  SPIBAR_HSFS_FLOCKDN        (1 << 15)/* Flash Configuration Lock-Down */

#endif
