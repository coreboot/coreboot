/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_ESPI_H
#define AMD_BLOCK_ESPI_H

#include <types.h>

/* eSPI MMIO base lives at an offset of 0x10000 from the address in SPI BAR. */
#define ESPI_OFFSET_FROM_BAR			0x10000

#define ESPI_DECODE				0x40
/* more bits defined in soc/common/amd/blocks/lpc/espi_def.h */
#define  ESPI_DECODE_IO_0x80_EN			(1 << 2)
#define  ESPI_DECODE_IO_0X60_0X64_EN		(1 << 1)
#define  ESPI_DECODE_IO_0X2E_0X2F_EN		(1 << 0)

/* The extended IO/MMIO decode ranges are only available in SoCs that select
   SOC_AMD_COMMON_BLOCK_ESPI_EXTENDED_DECODE_RANGES */
#if CONFIG(SOC_AMD_COMMON_BLOCK_ESPI_EXTENDED_DECODE_RANGES)
#define ESPI_GENERIC_IO_WIN_COUNT		16
#define ESPI_GENERIC_MMIO_WIN_COUNT		5
#else
#define ESPI_GENERIC_IO_WIN_COUNT		4
#define ESPI_GENERIC_MMIO_WIN_COUNT		4
#endif

#define ESPI_GENERIC_IO_MAX_WIN_SIZE		0x100
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

/*
 * Internally the SoC uses active low signals for the IRQs. This means what when
 * the eSPI controller comes out of reset, it's driving its IRQ lines high.
 * In order to avoid any spurious interrupts the IO-APIC must be configured to
 * trigger on active low signals. The PIC is only capable of triggering on
 * active high signals so the hardware has an inverter that converts the signals
 * before they feed into the PIC.
 *
 * +----------+         Active Low
 * |          |             |            +--------+
 * | IO-APIC  <------+      |      +-----+ LPC    |
 * |          |      |  <---+--->  |     +--------+
 * +----------+      |             |
 *                   |   +-----+   |     +--------+
 *                   +---+ AND <---+-----+ eSPI   |
 *                   |   +-----+   |     +--------+
 * +----------+      |             |
 * |          |   +--v--+          |     +--------+
 * |   PIC    <---+ NOT |          +-----+ PIR    |
 * |          | ^ +-----+                +--------+
 * +----------+ |
 *              | Active High
 *
 * The eSPI controller has an inverter that is applied to incoming Virtual Wire
 * IRQ messages. This allows eSPI peripherals to use active high signaling.
 * If the peripheral uses active low signaling like the SoC does internally, the
 * inverter can be disabled.
 *
 * The polarity register has the following behavior:
 *   0: Invert the incoming VW IRQ before outputting to the AND gates.
 *   1: Do not invert the incoming VW IRQ, but route it directly to the AND
 *      gates.
 */
#define ESPI_VW_IRQ_LEVEL_HIGH(x)		(0 << (x))
#define ESPI_VW_IRQ_LEVEL_LOW(x)		(1 << (x))
#define ESPI_VW_IRQ_EDGE_HIGH(x)		(0 << (x))
#define ESPI_VW_IRQ_EDGE_LOW(x)			(1 << (x))

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

enum espi_alert_pin {
	ESPI_ALERT_PIN_IN_BAND,
	ESPI_ALERT_PIN_PUSH_PULL,
	ESPI_ALERT_PIN_OPEN_DRAIN,
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
	enum espi_alert_pin alert_pin;

	uint32_t crc_check_enable:1;
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
 */
enum cb_err espi_open_io_window(uint16_t base, size_t size);

/*
 * Open MMIO window using the provided base and size.
 */
enum cb_err  espi_open_mmio_window(uint32_t base, size_t size);

/*
 * In cases where eSPI BAR is statically provided by SoC, use that BAR instead of reading
 * SPIBASE. This is required for cases where verstage runs on PSP.
 */
void espi_update_static_bar(uintptr_t bar);

/*
 * Perform eSPI connection setup to the slave. Currently, this supports slave0 only.
 */
enum cb_err  espi_setup(void);

/* Run mainboard configuration needed to set up eSPI */
void mb_set_up_early_espi(void);

/* Setup eSPI with any mainboard specific initialization. */
void configure_espi_with_mb_hook(void);

#endif /* AMD_BLOCK_ESPI_H */
