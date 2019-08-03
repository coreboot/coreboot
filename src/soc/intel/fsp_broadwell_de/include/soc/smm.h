/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017 Siemens AG
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

#ifndef _BROADWELL_SMM_H_
#define _BROADWELL_SMM_H_

#include <stdint.h>
#include <cpu/x86/msr.h>


struct smm_relocation_params {
	uintptr_t ied_base;
	size_t ied_size;
	msr_t smrr_base;
	msr_t smrr_mask;
	msr_t prmrr_base;
	msr_t prmrr_mask;
	/* The smm_save_state_in_msrs field indicates if SMM save state
	   locations live in MSRs. This indicates to the CPUs how to adjust
	   the SMMBASE and IEDBASE. */
	int smm_save_state_in_msrs;
};


#endif
