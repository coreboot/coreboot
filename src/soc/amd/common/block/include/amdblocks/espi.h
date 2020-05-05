/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __AMDBLOCKS_ESPI_H__
#define __AMDBLOCKS_ESPI_H__

/* eSPI MMIO base lives at an offset of 0x10000 from the address in SPI BAR. */
#define ESPI_OFFSET_FROM_BAR			0x10000

#define ESPI_DECODE				0x40
#define  ESPI_DECODE_MMIO_RANGE_EN(range)	(1 << (((range) & 3) + 12))
#define  ESPI_DECODE_IO_RANGE_EN(range)		(1 << (((range) & 3) + 8))
#define  ESPI_DECODE_IO_0x80_EN			(1 << 2)
#define  ESPI_DECODE_IO_0X60_0X64_EN	        (1 << 1)
#define  ESPI_DECODE_IO_0X2E_0X2F_EN		(1 << 0)

#define ESPI_IO_RANGE_BASE(range)		(0x44 + ((range) & 3) * 2)
#define ESPI_IO_RANGE_SIZE(range)		(0x4c + ((range) & 3))
#define ESPI_MMIO_RANGE_BASE(range)		(0x50 + ((range) & 3) * 4)
#define ESPI_MMIO_RANGE_SIZE(range)		(0x60 + ((range) & 3) * 2)

#define ESPI_GENERIC_IO_WIN_COUNT		4
#define ESPI_GENERIC_IO_MAX_WIN_SIZE		0x100
#define ESPI_GENERIC_MMIO_WIN_COUNT		4
#define ESPI_GENERIC_MMIO_MAX_WIN_SIZE		0x10000

#endif /* __AMDBLOCKS_ESPI_H__ */
