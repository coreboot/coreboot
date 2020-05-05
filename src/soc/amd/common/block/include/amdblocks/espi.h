/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __AMDBLOCKS_ESPI_H__
#define __AMDBLOCKS_ESPI_H__

#include <stdint.h>
#include <stddef.h>

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

struct espi_config {
	/* Bitmap for standard IO decodes. Use ESPI_DECODE_IO_* above. */
	uint32_t std_io_decode_bitmap;

	struct {
		uint16_t base;
		size_t size;
	} generic_io_range[ESPI_GENERIC_IO_WIN_COUNT];
};

/*
 * Open I/O window using the provided base and size.
 * Return value: 0 = success, -1 = error.
 */
int espi_open_io_window(uint16_t base, size_t size);

/*
 * Open MMIO window using the provided base and size.
 * Return value: 0 = success, -1 = error.
 */
int espi_open_mmio_window(uint32_t base, size_t size);

/*
 * Configure generic and standard I/O decode windows using the espi_config structure settings
 * provided by mainboard in device tree.
 */
void espi_configure_decodes(void);

#endif /* __AMDBLOCKS_ESPI_H__ */
