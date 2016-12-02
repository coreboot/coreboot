/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_CHIP_H_
#define _SOC_APOLLOLAKE_CHIP_H_

#include <soc/gpe.h>
#include <soc/gpio_defs.h>
#include <soc/gpio.h>
#include <soc/intel/common/lpss_i2c.h>
#include <device/i2c.h>
#include <soc/pm.h>

#define CLKREQ_DISABLED		0xf
#define APOLLOLAKE_I2C_DEV_MAX	8

/* Serial IRQ control. SERIRQ_QUIET is the default (0). */
enum serirq_mode {
	SERIRQ_QUIET,
	SERIRQ_CONTINUOUS,
	SERIRQ_OFF,
};

struct soc_intel_apollolake_config {
	/*
	 * Mapping from PCIe root port to CLKREQ input on the SOC. The SOC has
	 * four CLKREQ inputs, but six root ports. Root ports without an
	 * associated CLKREQ signal must be marked with "CLKREQ_DISABLED"
	 */
	uint8_t pcie_rp0_clkreq_pin;
	uint8_t pcie_rp1_clkreq_pin;
	uint8_t pcie_rp2_clkreq_pin;
	uint8_t pcie_rp3_clkreq_pin;
	uint8_t pcie_rp4_clkreq_pin;
	uint8_t pcie_rp5_clkreq_pin;

	/* [14:8] DDR mode Number of dealy elements.Each = 125pSec.
	 * [6:0] SDR mode Number of dealy elements.Each = 125pSec.
	 */
	uint32_t emmc_tx_cmd_cntl;

	/* [14:8] HS400 mode Number of dealy elements.Each = 125pSec.
	 * [6:0] SDR104/HS200 mode Number of dealy elements.Each = 125pSec.
	 */
	uint32_t emmc_tx_data_cntl1;

	/* [30:24] SDR50 mode Number of dealy elements.Each = 125pSec.
	 * [22:16] DDR50 mode Number of dealy elements.Each = 125pSec.
	 * [14:8] SDR25/HS50 mode Number of dealy elements.Each = 125pSec.
	 * [6:0] SDR12/Compatibility mode Number of dealy elements.Each = 125pSec.
	 */
	uint32_t emmc_tx_data_cntl2;

	/* [30:24] SDR50 mode Number of dealy elements.Each = 125pSec.
	 * [22:16] DDR50 mode Number of dealy elements.Each = 125pSec.
	 * [14:8] SDR25/HS50 mode Number of dealy elements.Each = 125pSec.
	 * [6:0] SDR12/Compatibility mode Number of dealy elements.Each = 125pSec.
	 */
	uint32_t emmc_rx_cmd_data_cntl1;

	/* [14:8] HS400 mode 1 Number of dealy elements.Each = 125pSec.
	 * [6:0] HS400 mode 2 Number of dealy elements.Each = 125pSec.
	 */
	uint32_t emmc_rx_strobe_cntl;

	/* [13:8] Auto Tuning mode Number of dealy elements.Each = 125pSec.
	 * [6:0] SDR104/HS200 Number of dealy elements.Each = 125pSec.
	 */
	uint32_t emmc_rx_cmd_data_cntl2;

	/* Configure serial IRQ (SERIRQ) line. */
	enum serirq_mode serirq_mode;

	/* I2C bus configuration */
	struct lpss_i2c_bus_config i2c[APOLLOLAKE_I2C_DEV_MAX];

	uint8_t gpe0_dw1; /* GPE0_63_32 STS/EN */
	uint8_t gpe0_dw2; /* GPE0_95_64 STS/EN */
	uint8_t gpe0_dw3; /* GPE0_127_96 STS/EN */

	/* Configure LPSS S0ix Enable */
	uint8_t lpss_s0ix_enable;

	/* Enable DPTF support */
	int dptf_enable;

	/* PL1 override value in mW for APL */
	uint16_t tdp_pl1_override_mw;
	/* PL2 override value in mW for APL */
	uint16_t tdp_pl2_override_mw;

	/* Configure Audio clk gate and power gate
	 * IOSF-SB port ID 92 offset 0x530 [5] and [3]
	 */
	uint8_t hdaudio_clk_gate_enable;
	uint8_t hdaudio_pwr_gate_enable;
	uint8_t hdaudio_bios_config_lockdown;

	/* SLP S3 minimum assertion width. */
	int slp_s3_assertion_width_usecs;

	/* GPIO pin for PERST_0 */
	uint16_t prt0_gpio;
};

#endif	/* _SOC_APOLLOLAKE_CHIP_H_ */
