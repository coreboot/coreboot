/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_ESPI_H
#define AMD_BLOCK_ESPI_H

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

#define ESPI_SLAVE0_CONFIG			0x68
#define  ESPI_CRC_CHECKING_EN			(1 << 31)
#define  ESPI_ALERT_MODE			(1 << 30)

#define  ESPI_IO_MODE_SHIFT			28
#define  ESPI_IO_MODE_MASK			(0x3 << ESPI_IO_MODE_SHIFT)
#define  ESPI_IO_MODE_VALUE(x)			((x) << ESPI_IO_MODE_SHIFT)

#define  ESPI_OP_FREQ_SHIFT			25
#define  ESPI_OP_FREQ_MASK			(0x7 << ESPI_OP_FREQ_SHIFT)
#define  ESPI_OP_FREQ_VALUE(x)			((x) << ESPI_OP_FREQ_SHIFT)

#define  ESPI_PERIPH_CH_EN			(1 << 3)
#define  ESPI_VW_CH_EN				(1 << 2)
#define  ESPI_OOB_CH_EN				(1 << 1)
#define  ESPI_FLASH_CH_EN			(1 << 0)

/* Virtual wire interrupt polarity. eSPI interrupts are active level high signals. The
   polarity register inverts the incoming signal if the associated bit with the irq is
   0. */
#define ESPI_VW_IRQ_LEVEL_HIGH(x)		(1 << (x))
#define ESPI_VW_IRQ_LEVEL_LOW(x)		(0 << (x))
#define ESPI_VW_IRQ_EDGE_HIGH(x)		(1 << (x))
#define ESPI_VW_IRQ_EDGE_LOW(x)			(0 << (x))

enum espi_io_mode {
	ESPI_IO_MODE_SINGLE = ESPI_IO_MODE_VALUE(0),
	ESPI_IO_MODE_DUAL = ESPI_IO_MODE_VALUE(1),
	ESPI_IO_MODE_QUAD = ESPI_IO_MODE_VALUE(2),
};

enum espi_op_freq {
	ESPI_OP_FREQ_16_MHZ = ESPI_OP_FREQ_VALUE(0),
	ESPI_OP_FREQ_33_MHZ = ESPI_OP_FREQ_VALUE(1),
	ESPI_OP_FREQ_66_MHZ = ESPI_OP_FREQ_VALUE(2),
};

struct espi_config {
	/* Bitmap for standard IO decodes. Use ESPI_DECODE_IO_* above. */
	uint32_t std_io_decode_bitmap;

	struct {
		uint16_t base;
		size_t size;
	} generic_io_range[ESPI_GENERIC_IO_WIN_COUNT];

	/* Slave configuration parameters */
	enum espi_io_mode io_mode;
	enum espi_op_freq op_freq_mhz;

	uint32_t crc_check_enable:1;
	uint32_t dedicated_alert_pin:1;
	uint32_t periph_ch_en:1;
	uint32_t vw_ch_en:1;
	uint32_t oob_ch_en:1;
	uint32_t flash_ch_en:1;
	uint32_t subtractive_decode:1;

	/* Use ESPI_VW_IRQ_* above */
	uint32_t vw_irq_polarity;
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

/*
 * Clear all configured eSPI memory and I/O decode ranges.  This is useful for changing
 * the decodes, or if something else has previously setup decode windows that conflict
 * with the windows that coreboot needs.
 */
void espi_clear_decodes(void);

/*
 * In cases where eSPI BAR is statically provided by SoC, use that BAR instead of reading
 * SPIBASE. This is required for cases where verstage runs on PSP.
 */
void espi_update_static_bar(uintptr_t bar);

/*
 * Perform eSPI connection setup to the slave. Currently, this supports slave0 only.
 * Returns 0 on success and -1 on error.
 */
int espi_setup(void);

#endif /* AMD_BLOCK_ESPI_H */
