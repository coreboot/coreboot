/*
 * This file is part of the coreboot project.
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

#ifndef __X86_SMI_DEPRECATED_H__
#define __X86_SMI_DEPRECATED_H__

#include <cpu/amd/amd64_save_state.h>
#include <cpu/intel/em64t_save_state.h>
#include <cpu/intel/em64t100_save_state.h>
#include <cpu/intel/em64t101_save_state.h>
#include <cpu/x86/legacy_save_state.h>

typedef enum {
	AMD64,
	EM64T,
	EM64T101,
	LEGACY
} save_state_type_t;

typedef struct {
	save_state_type_t type;
	union {
	amd64_smm_state_save_area_t *amd64_state_save;
	em64t_smm_state_save_area_t *em64t_state_save;
	em64t101_smm_state_save_area_t *em64t101_state_save;
	legacy_smm_state_save_area_t *legacy_state_save;
	};
} smm_state_save_area_t;

#if CONFIG(PARALLEL_MP) || !CONFIG(HAVE_SMI_HANDLER)
/* Empty stubs for platforms without SMI handlers. */
static inline void smm_init(void) { }
static inline void smm_init_completion(void) { }
#else
void smm_init(void);
void smm_init_completion(void);
#endif

/* Entry from smmhandler.S. */
void smi_handler(u32 smm_revision);

#endif
