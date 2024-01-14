/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __SOC_SIFIVE_HIFIVE_U_SPI_INTERNAL_H__
#define __SOC_SIFIVE_HIFIVE_U_SPI_INTERNAL_H__

#include <stdint.h>

#define _ASSERT_SIZEOF(type, size) _Static_assert( \
		sizeof(type) == (size), \
		#type " must be " #size " bytes wide")

// Chip Select Mode Register (csmode)

#define FU740_SPI_CSMODE_AUTO 0
#define FU740_SPI_CSMODE_HOLD 2
#define FU740_SPI_CSMODE_OFF  3

union fu740_spi_reg_sckmode {
	struct {
		uint32_t pha : 1;
		uint32_t pol : 1;
		uint32_t reserved : 30;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_sckmode, 4);

union fu740_spi_reg_csmode {
	struct {
		uint32_t mode : 2;
		uint32_t reserved : 30;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_csmode, 4);

union fu740_spi_reg_delay0 {
	struct {
		uint32_t cssck : 8;
		uint32_t reserved0 : 8;
		uint32_t sckcs : 8;
		uint32_t reserved1 : 8;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_delay0, 4);

union fu740_spi_reg_delay1 {
	struct {
		uint32_t intercs : 8;
		uint32_t reserved0 : 8;
		uint32_t interxfr : 8;
		uint32_t reserved1 : 8;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_delay1, 4);

union fu740_spi_reg_fmt {
	struct {
		uint32_t proto : 2;
		uint32_t endian : 1;
		uint32_t dir : 1;
		uint32_t reserved0 : 12;
		uint32_t len : 4;
		uint32_t reserved1 : 12;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_fmt, 4);

union fu740_spi_reg_txdata {
	struct {
		uint32_t data : 8;
		uint32_t reserved : 23;
		uint32_t full : 1;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_txdata, 4);

union fu740_spi_reg_rxdata {
	struct {
		uint32_t data : 8;
		uint32_t reserved : 23;
		uint32_t empty : 1;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_rxdata, 4);

union fu740_spi_reg_txmark {
	struct {
		uint32_t txmark : 3;
		uint32_t reserved : 29;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_txmark, 4);

union fu740_spi_reg_rxmark {
	struct {
		uint32_t rxmark : 3;
		uint32_t reserved : 29;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_rxmark, 4);

union fu740_spi_reg_fctrl {
	struct {
		uint32_t en : 1;
		uint32_t reserved : 31;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_fctrl, 4);

union fu740_spi_reg_ffmt {
	struct {
		uint32_t cmd_en : 1;
		uint32_t addr_len : 3;
		uint32_t pad_cnt : 4;
		uint32_t cmd_proto : 2;
		uint32_t addr_proto : 2;
		uint32_t data_proto : 2;
		uint32_t reserved : 2;
		uint32_t cmd_code : 8;
		uint32_t pad_code : 8;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_ffmt, 4);

union fu740_spi_reg_ie {
	struct {
		uint32_t txwm : 1;
		uint32_t rxwm : 1;
		uint32_t reserved : 30;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_ie, 4);

union fu740_spi_reg_ip {
	struct {
		uint32_t txwm : 1;
		uint32_t rxwm : 1;
		uint32_t reserved : 30;
	};
	uint32_t raw_bits;
};
_ASSERT_SIZEOF(union fu740_spi_reg_ip, 4);

#undef _ASSERT_SIZEOF

/**
 * SPI control register memory map.
 *
 * All functions take a pointer to a SPI device's control registers.
 */
struct fu740_spi_ctrl {
	uint32_t sckdiv;
	union fu740_spi_reg_sckmode sckmode;
	uint32_t reserved08;
	uint32_t reserved0c;

	uint32_t csid;
	uint32_t csdef;
	union fu740_spi_reg_csmode csmode;
	uint32_t reserved1c;

	uint32_t reserved20;
	uint32_t reserved24;
	union fu740_spi_reg_delay0 delay0;
	union fu740_spi_reg_delay1 delay1;

	uint32_t reserved30;
	uint32_t reserved34;
	uint32_t reserved38;
	uint32_t reserved3c;

	union fu740_spi_reg_fmt fmt;
	uint32_t reserved44;
	union fu740_spi_reg_txdata txdata;
	union fu740_spi_reg_rxdata rxdata;

	union fu740_spi_reg_txmark txmark;
	union fu740_spi_reg_rxmark rxmark;
	uint32_t reserved58;
	uint32_t reserved5c;

	union fu740_spi_reg_fctrl fctrl;
	union fu740_spi_reg_ffmt ffmt;
	uint32_t reserved68;
	uint32_t reserved6c;

	union fu740_spi_reg_ie ie;
	union fu740_spi_reg_ip ip;
};

/**
 * Get smallest clock divisor that divides input_khz to a quotient less than or
 * equal to max_target_khz;
 */
static inline unsigned int
fu740_spi_min_clk_divisor(unsigned int input_khz, unsigned int max_target_khz)
{
	// f_sck = f_in / (2 * (div + 1)) => div = (f_in / (2*f_sck)) - 1
	//
	// The nearest integer solution for div requires rounding up as to not
	// exceed max_target_khz.
	//
	// div = ceil(f_in / (2*f_sck)) - 1
	//     = floor((f_in - 1 + 2*f_sck) / (2*f_sck)) - 1
	//
	// This should not overflow as long as (f_in - 1 + 2*f_sck) does not
	// exceed 2^32 - 1, which is unlikely since we represent frequencies
	// in kHz.
	unsigned int quotient =
		(input_khz + 2 * max_target_khz - 1) / (2 * max_target_khz);
	// Avoid underflow
	if (quotient == 0)
		return 0;
	return quotient - 1;
}

#endif /* __SOC_SIFIVE_HIFIVE_U_SPI_INTERNAL_H__ */
