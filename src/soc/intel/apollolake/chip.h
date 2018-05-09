/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * Copyright (C) 2017 Siemens AG
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

#include <commonlib/helpers.h>
#include <intelblocks/chip.h>
#include <intelblocks/gspi.h>
#include <soc/gpe.h>
#include <soc/gpio.h>
#include <intelblocks/lpc_lib.h>
#include <device/i2c_simple.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/pm.h>
#include <soc/usb.h>

#define MAX_PCIE_PORTS			6
#define CLKREQ_DISABLED		0xf

enum pnp_settings {
	PNP_PERF,
	PNP_POWER,
	PNP_PERF_POWER,
};

struct soc_intel_apollolake_config {

	/* Common structure containing soc config data required by common code*/
	struct soc_intel_common_config common_soc_config;

	/*
	 * Mapping from PCIe root port to CLKREQ input on the SOC. The SOC has
	 * four CLKREQ inputs, but six root ports. Root ports without an
	 * associated CLKREQ signal must be marked with "CLKREQ_DISABLED"
	 */
	uint8_t pcie_rp_clkreq_pin[MAX_PCIE_PORTS];

	/* Enable/disable hot-plug for root ports (0 = disable, 1 = enable). */
	uint8_t pcie_rp_hotplug_enable[MAX_PCIE_PORTS];

	/* De-emphasis enable configuration for each PCIe root port */
	uint8_t pcie_rp_deemphasis_enable[MAX_PCIE_PORTS];

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
	 * [6:0] SDR12/Compatibility mode Number of dealy elements.
	 *       Each = 125pSec.
	 */
	uint32_t emmc_tx_data_cntl2;

	/* [30:24] SDR50 mode Number of dealy elements.Each = 125pSec.
	 * [22:16] DDR50 mode Number of dealy elements.Each = 125pSec.
	 * [14:8] SDR25/HS50 mode Number of dealy elements.Each = 125pSec.
	 * [6:0] SDR12/Compatibility mode Number of dealy elements.
	 *       Each = 125pSec.
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

	/* Specifies on which IRQ the SCI will internally appear. */
	uint8_t sci_irq;

	/* Configure serial IRQ (SERIRQ) line. */
	enum serirq_mode serirq_mode;

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

	/* USB2 eye diagram settings per port */
	struct usb2_eye_per_port usb2eye[APOLLOLAKE_USB2_PORT_MAX];

	/* GPIO SD card detect pin */
	unsigned int sdcard_cd_gpio;

	/* PRMRR size setting with three options
	 *  0x02000000 - 32MiB
	 *  0x04000000 - 64MiB
	 *  0x08000000 - 128MiB */
	uint32_t PrmrrSize;

	/* Enable SGX feature.
	 * Enabling SGX feature is 2 step process,
	 * (1) set sgx_enable = 1
	 * (2) set PrmrrSize to supported size */
	uint8_t sgx_enable;

	/* Select PNP Settings.
	 * (0) Performance,
	 * (1) Power
	 * (2) Power & Performance */
	enum pnp_settings pnp_settings;

	/*
	 * Option for mainboard to skip coreboot MP initialization
	 * 0 = Make use of coreboot MP Init
	 * 1 = Make use of FSP MP Init
	 */
	uint8_t use_fsp_mp_init;
};

typedef struct soc_intel_apollolake_config config_t;

#endif	/* _SOC_APOLLOLAKE_CHIP_H_ */
