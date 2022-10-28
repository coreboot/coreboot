/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_SPI_H
#define AMD_BLOCK_SPI_H

#include <thread.h>
#include <types.h>

#define SPI_CNTRL0			0x00
#define   SPI_BUSY			BIT(31)

enum spi_read_mode {
	SPI_READ_MODE_NORMAL33M = 0,
	/* 1 is reserved. */
	SPI_READ_MODE_DUAL112 = 2,
	SPI_READ_MODE_QUAD114 = 3,
	SPI_READ_MODE_DUAL122 = 4,
	SPI_READ_MODE_QUAD144 = 5,
	SPI_READ_MODE_NORMAL66M = 6,
	SPI_READ_MODE_FAST_READ = 7,
};
/*
 * SPI read mode is split into bits 18, 29, 30 such that [30:29:18] correspond to bits [2:0] for
 * SpiReadMode.
 */
#define   SPI_READ_MODE_MASK		(BIT(30) | BIT(29) | BIT(18))
#define   SPI_READ_MODE_UPPER_BITS(x)	((((x) >> 1) & 0x3) << 29)
#define   SPI_READ_MODE_LOWER_BITS(x)	(((x) & 0x1) << 18)
#define   SPI_READ_MODE(x)		(SPI_READ_MODE_UPPER_BITS(x) | \
					 SPI_READ_MODE_LOWER_BITS(x))
#define   SPI_ACCESS_MAC_ROM_EN		BIT(22)

#define SPI100_ENABLE			0x20
#define   SPI_USE_SPI100		BIT(0)

#define DECODE_SPI_MODE_BITS(x)		((x) & SPI_READ_MODE_MASK)
#define DECODE_SPI_MODE_UPPER_BITS(x)	((DECODE_SPI_MODE_BITS(x) >> 28) & 0x06)
#define DECODE_SPI_MODE_LOWER_BITS(x)	((DECODE_SPI_MODE_BITS(x) >> 18) & 0x01)
#define DECODE_SPI_READ_MODE(x)		(DECODE_SPI_MODE_UPPER_BITS(x) | \
					DECODE_SPI_MODE_LOWER_BITS(x))

/* Use SPI_SPEED_16M-SPI_SPEED_66M below for the southbridge */
#define SPI100_SPEED_CONFIG		0x22
enum spi100_speed {
	SPI_SPEED_66M = 0,
	SPI_SPEED_33M = 1,
	SPI_SPEED_22M = 2,
	SPI_SPEED_16M = 3,
	SPI_SPEED_100M = 4,
	SPI_SPEED_800K = 5,
};

#define   SPI_SPEED_MASK		0xf
#define   SPI_SPEED_MODE(x, shift)	(((x) & SPI_SPEED_MASK) << shift)
#define   SPI_NORM_SPEED(x)		SPI_SPEED_MODE(x, 12)
#define   SPI_FAST_SPEED(x)		SPI_SPEED_MODE(x, 8)
#define   SPI_ALT_SPEED(x)		SPI_SPEED_MODE(x, 4)
#define   SPI_TPM_SPEED(x)		SPI_SPEED_MODE(x, 0)

#define   SPI_SPEED_CFG(n, f, a, t)	(SPI_NORM_SPEED(n) | SPI_FAST_SPEED(f) | \
					 SPI_ALT_SPEED(a) | SPI_TPM_SPEED(t))

#define   DECODE_SPEED_MASK		0x07
#define   DECODE_SPEED_MODE(x, shift)	(((x) >> shift) & DECODE_SPEED_MASK)
#define   DECODE_SPI_NORMAL_SPEED(x)	DECODE_SPEED_MODE(x, 12)
#define   DECODE_SPI_FAST_SPEED(x)	DECODE_SPEED_MODE(x, 8)
#define   DECODE_SPI_ALT_SPEED(x)	DECODE_SPEED_MODE(x, 4)
#define   DECODE_SPI_TPM_SPEED(x)	DECODE_SPEED_MODE(x, 0)

#define SPI100_HOST_PREF_CONFIG		0x2c
#define   SPI_RD4DW_EN_HOST		BIT(15)

#define SPI_FIFO			0x80
#define SPI_FIFO_LAST_BYTE		0xc6 /* 0xc7 for Cezanne */
#define SPI_FIFO_DEPTH			(SPI_FIFO_LAST_BYTE - SPI_FIFO + 1)

struct spi_config {
	/*
	 * Default values if not overridden by mainboard:
	 * Read mode - Normal 33MHz
	 * Normal speed - 66MHz
	 * Fast speed - 66MHz
	 * Alt speed - 66MHz
	 * TPM speed - 66MHz
	 */
	enum spi_read_mode read_mode;
	enum spi100_speed normal_speed;
	enum spi100_speed fast_speed;
	enum spi100_speed altio_speed;
	enum spi100_speed tpm_speed;
};

/*
 * Perform early SPI initialization:
 * 1. Sets SPI ROM base and enables SPI ROM
 * 2. Enables SPI ROM prefetching
 * 3. Disables 4 DWORD burst if !SOC_AMD_COMMON_BLOCK_SPI_4DW_BURST
 * 4. Configures SPI speed and read mode.
 *
 * This function expects SoC to include soc_amd_common_config in chip SoC config and uses
 * settings from mainboard devicetree to configure speed and read mode.
 */
void fch_spi_early_init(void);

/* Set the SPI base address variable */
void spi_set_base(void *base);

/* Show the SPI settings */
void show_spi_speeds_and_modes(void);

/* Get the SPI base address variable's value */
uintptr_t spi_get_bar(void);
uint8_t spi_read8(uint8_t reg);
uint16_t spi_read16(uint8_t reg);
uint32_t spi_read32(uint8_t reg);
void spi_write8(uint8_t reg, uint8_t val);
void spi_write16(uint8_t reg, uint16_t val);
void spi_write32(uint8_t reg, uint32_t val);

void fch_spi_config_modes(void);
void mainboard_spi_cfg_override(uint8_t *fast_speed, uint8_t *read_mode);

/* Ensure you hold the mutex when performing SPI transactions */
extern struct thread_mutex spi_hw_mutex;

#endif /* AMD_BLOCK_SPI_H */
