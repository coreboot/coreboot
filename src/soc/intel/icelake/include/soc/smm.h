/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corp.
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

#ifndef _SOC_SMM_H_
#define _SOC_SMM_H_

#include <stdint.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/smm.h>
#include <soc/gpio.h>


struct smm_relocation_params {
	uintptr_t ied_base;
	size_t ied_size;
	msr_t smrr_base;
	msr_t smrr_mask;
	/*
	 * The smm_save_state_in_msrs field indicates if SMM save state
	 * locations live in MSRs. This indicates to the CPUs how to adjust
	 * the SMMBASE and IEDBASE
	 */
	int smm_save_state_in_msrs;
};

#endif
