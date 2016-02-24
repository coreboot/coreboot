/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _SOC_APOLLOLAKE_SPI_H_
#define _SOC_APOLLOLAKE_SPI_H_

/* PCI configuration registers */
#define SPIBAR_BIOS_CONTROL		0xdc

/* Maximum bytes of data that can fit in FDATAn registers */
#define SPIBAR_FDATA_FIFO_SIZE		0x40

/* Bit definitions for BIOS_CONTROL */
#define SPIBAR_BIOS_CONTROL_WPD		(1 << 0)
#define  SPIBAR_BIOS_CONTROL_EISS	(1 << 5)

/* Register offsets from the MMIO region base (PCI_BASE_ADDRESS_0) */
#define SPIBAR_HSFSTS_CTL		0x04
#define SPIBAR_FADDR			0x08
#define SPIBAR_FDATA(n)			(0x10 + ((n) & 0xf) * 4)
#define SPIBAR_PTINX			0xcc
#define SPIBAR_PTDATA			0xd0

/* Bit definitions for HSFSTS_CTL register */
#define  SPIBAR_HSFSTS_FBDC_MASK	(0x3f << 24)
#define  SPIBAR_HSFSTS_FBDC(n)		(((n) << 24) & SPIBAR_HSFSTS_FBDC_MASK)
#define  SPIBAR_HSFSTS_WET		(1 << 21)
#define  SPIBAR_HSFSTS_FCYCLE_MASK	(0xf << 17)
#define  SPIBAR_HSFSTS_FCYCLE(cyc)	(((cyc) << 17) & SPIBAR_HSFSTS_FCYCLE_MASK)
#define  SPIBAR_HSFSTS_FGO		(1 << 16)
#define  SPIBAR_HSFSTS_FLOCKDN		(1 << 15)
#define  SPIBAR_HSFSTS_FDV		(1 << 14)
#define  SPIBAR_HSFSTS_FDOPSS		(1 << 13)
#define  SPIBAR_HSFSTS_SAF_CE		(1 << 8)
#define  SPIBAR_HSFSTS_SAF_ACTIVE	(1 << 7)
#define  SPIBAR_HSFSTS_SAF_LE		(1 << 6)
#define  SPIBAR_HSFSTS_SCIP		(1 << 5)
#define  SPIBAR_HSFSTS_SAF_DLE		(1 << 4)
#define  SPIBAR_HSFSTS_SAF_ERROR	(1 << 3)
#define  SPIBAR_HSFSTS_AEL		(1 << 2)
#define  SPIBAR_HSFSTS_FCERR		(1 << 1)
#define  SPIBAR_HSFSTS_FDONE		(1 << 0)
#define  SPIBAR_HSFSTS_W1C_BITS	(0xff)
/* Supported flash cycle types */
#define  SPIBAR_HSFSTS_CYCLE_READ	SPIBAR_HSFSTS_FCYCLE(0)
#define  SPIBAR_HSFSTS_CYCLE_WRITE	SPIBAR_HSFSTS_FCYCLE(2)
#define  SPIBAR_HSFSTS_CYCLE_4K_ERASE	SPIBAR_HSFSTS_FCYCLE(3)
#define  SPIBAR_HSFSTS_CYCLE_64K_ERASE	SPIBAR_HSFSTS_FCYCLE(4)

/* Bit definitions for PTINX register */
#define  SPIBAR_PTINX_COMP_0		(0 << 14)
#define  SPIBAR_PTINX_COMP_1		(1 << 14)
#define  SPIBAR_PTINX_HORD_SFDP		(0 << 12)
#define  SPIBAR_PTINX_HORD_PARAM	(1 << 12)
#define  SPIBAR_PTINX_HORD_JEDEC	(2 << 12)
#define  SPIBAR_PTINX_IDX_MASK		0xffc

#endif
