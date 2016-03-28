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

#define CLKREQ_DISABLED		0xf

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

	/* Configure serial IRQ (SERIRQ) line. */
	enum serirq_mode serirq_mode;

	/* Integrated Sensor Hub */
	uint8_t integrated_sensor_hub_enable;
};

#endif	/* _SOC_APOLLOLAKE_CHIP_H_ */
