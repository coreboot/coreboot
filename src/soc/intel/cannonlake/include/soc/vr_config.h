/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* VR Settings for each domain */

#ifndef _SOC_VR_CONFIG_H_
#define _SOC_VR_CONFIG_H_

#include <fsp/api.h>

struct vr_config {

	/* The below settings will take effect when this is set to 1
	 * for that domain. */
	uint8_t vr_config_enable;

	/* Power State X current cutoff in 1/4 Amp increments
	 * Range is 0-128A */
	uint16_t psi1threshold;
	uint16_t psi2threshold;
	uint16_t psi3threshold;

	/* Enable power state 3/4 for different domains */
	uint8_t psi3enable;
	uint8_t psi4enable;

	/* Imon slope correction. Specified in 1/100 increment
	 * values. Range is 0-200. 125 = 1.25 */
	uint8_t imon_slope;

	/* Imon offset correction. Units 1/1000, Range 0-63999
	 * For an offset = 12.580, use 12580. 0 = Auto */
	uint8_t imon_offset;

	/* VR Icc Max limit. 0-255A in 1/4 A units. 400 = 100A */
	uint16_t icc_max;

	/* VR Voltage Limit. Range is 0-7999mV */
	uint16_t voltage_limit;

	/* AC and DC Loadline in 1/100 mOhms. Range is 0-6249 */
	uint16_t ac_loadline;
	uint16_t dc_loadline;
};

/* VrConfig Settings for 4 domains
 * 0 = System Agent, 1 = IA Core,
 * 2 = GT unsliced,  3 = GT sliced */
enum vr_domain {
	VR_SYSTEM_AGENT,
	VR_IA_CORE,
	VR_GT_UNSLICED,
	VR_GT_SLICED,
	NUM_VR_DOMAINS
};

void fill_vr_domain_config(void *params,
		int domain, const struct vr_config *cfg);

#endif
