/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *               2012 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef CPU_INTEL_SPEEDSTEP_H
#define CPU_INTEL_SPEEDSTEP_H

#include <stdint.h>

/* Magic value used to locate speedstep configuration in the device tree */
#define SPEEDSTEP_APIC_MAGIC 0xACAC

/* MWAIT coordination I/O base address. This must match
 * the \_PR_.CPU0 PM base address.
 */
#define PMB0_BASE 0x510

/* PMB1: I/O port that triggers SMI once cores are in the same state.
 * See CSM Trigger, at PMG_CST_CONFIG_CONTROL[6:4]
 */
#define PMB1_BASE 0x800


/* Speedstep related MSRs */
#define IA32_PLATFORM_ID  0x017
#define IA32_PERF_STS     0x198
#define IA32_PERF_CTL     0x199
#define MSR_THERM2_CTL    0x19D
#define IA32_MISC_ENABLES 0x1A0
#define MSR_FSB_FREQ		0xcd
#define MSR_FSB_CLOCK_VCC	0xce
#define MSR_PMG_CST_CONFIG_CONTROL	0xe2
#define MSR_PMG_IO_BASE_ADDR	0xe3
#define MSR_PMG_IO_CAPTURE_ADDR	0xe4
#define MSR_EXTENDED_CONFIG	0xee


typedef struct {
	uint8_t dynfsb : 1; /* whether this is SLFM */
	uint8_t nonint : 1; /* add .5 to ratio */
	uint8_t ratio : 6;
	uint8_t vid;
	uint8_t is_turbo;
	uint8_t is_slfm;
	uint32_t power;
} sst_state_t;
#define SPEEDSTEP_RATIO_SHIFT		8
#define SPEEDSTEP_RATIO_DYNFSB_SHIFT	(7 + SPEEDSTEP_RATIO_SHIFT)
#define SPEEDSTEP_RATIO_DYNFSB		(1 << SPEEDSTEP_RATIO_DYNFSB_SHIFT)
#define SPEEDSTEP_RATIO_NONINT_SHIFT	(6 + SPEEDSTEP_RATIO_SHIFT)
#define SPEEDSTEP_RATIO_NONINT		(1 << SPEEDSTEP_RATIO_NONINT_SHIFT)
#define SPEEDSTEP_RATIO_VALUE_MASK	(0x1f << SPEEDSTEP_RATIO_SHIFT)
#define SPEEDSTEP_VID_MASK		0x3f
#define SPEEDSTEP_STATE_FROM_MSR(val, mask) ((sst_state_t){		\
		0, /* dynfsb won't be read. */				\
		((val & mask) & SPEEDSTEP_RATIO_NONINT) ? 1 : 0,	\
		(((val & mask) & SPEEDSTEP_RATIO_VALUE_MASK)		\
					>> SPEEDSTEP_RATIO_SHIFT),	\
		(val & mask) & SPEEDSTEP_VID_MASK,			\
		0, /* not turbo by default */				\
		0, /* not slfm by default */				\
		0  /* power is hardcoded in software. */		\
	})
#define SPEEDSTEP_ENCODE_STATE(state)	(				\
	((uint16_t)(state).dynfsb << SPEEDSTEP_RATIO_DYNFSB_SHIFT) |	\
	((uint16_t)(state).nonint << SPEEDSTEP_RATIO_NONINT_SHIFT) |	\
	((uint16_t)(state).ratio << SPEEDSTEP_RATIO_SHIFT) |		\
	((uint16_t)(state).vid & SPEEDSTEP_VID_MASK))
#define SPEEDSTEP_DOUBLE_RATIO(state)	(				\
	((uint8_t)(state).ratio * 2) + (state).nonint)

typedef struct {
	sst_state_t slfm;
	sst_state_t min;
	sst_state_t max;
	sst_state_t turbo;
} sst_params_t;

/* Looking at core2's spec, the highest normal bus ratio for an eist enabled
   processor is 14, the lowest is always 6. This makes 5 states with the
   minimal step width of 2. With turbo mode and super LFM we have at most 7. */
#define SPEEDSTEP_MAX_NORMAL_STATES	5
#define SPEEDSTEP_MAX_STATES		(SPEEDSTEP_MAX_NORMAL_STATES + 2)
typedef struct {
	/* Table of p-states for EMTTM and ACPI by decreasing performance. */
	sst_state_t states[SPEEDSTEP_MAX_STATES];
	int num_states;
} sst_table_t;

void speedstep_gen_pstates(sst_table_t *);

#define SPEEDSTEP_MAX_POWER_YONAH	31000
#define SPEEDSTEP_MIN_POWER_YONAH	13100
#define SPEEDSTEP_MAX_POWER_MEROM	35000
#define SPEEDSTEP_MIN_POWER_MEROM	25000
#define SPEEDSTEP_SLFM_POWER_MEROM	12000
#define SPEEDSTEP_MAX_POWER_PENRYN	35000
#define SPEEDSTEP_MIN_POWER_PENRYN	15000
#define SPEEDSTEP_SLFM_POWER_PENRYN	12000

#endif /* CPU_INTEL_SPEEDSTEP_H */
