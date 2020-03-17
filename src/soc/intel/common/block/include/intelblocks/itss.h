/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_BLOCK_ITSS_H
#define SOC_INTEL_COMMON_BLOCK_ITSS_H

/* Max PXRC registers in ITSS*/
#define MAX_PXRC_CONFIG	(PCR_ITSS_PIRQH_ROUT - PCR_ITSS_PIRQA_ROUT + 1)

/* PIRQA Routing Control Register*/
#define PCR_ITSS_PIRQA_ROUT	0x3100
/* PIRQB Routing Control Register*/
#define PCR_ITSS_PIRQB_ROUT	0x3101
/* PIRQC Routing Control Register*/
#define PCR_ITSS_PIRQC_ROUT	0x3102
/* PIRQD Routing Control Register*/
#define PCR_ITSS_PIRQD_ROUT	0x3103
/* PIRQE Routing Control Register*/
#define PCR_ITSS_PIRQE_ROUT	0x3104
/* PIRQF Routing Control Register*/
#define PCR_ITSS_PIRQF_ROUT	0x3105
/* PIRQG Routing Control Register*/
#define PCR_ITSS_PIRQG_ROUT	0x3106
/* PIRQH Routing Control Register*/
#define PCR_ITSS_PIRQH_ROUT	0x3107
/* ITSS Interrupt polarity control*/
#define PCR_ITSS_IPC0_CONF	0x3200
/* ITSS Power reduction control */
#define PCR_ITSS_ITSSPRC	0x3300

#if !defined(__ACPI__)

#include <stdint.h>

/* Set the interrupt polarity for provided IRQ to the APIC. */
void itss_set_irq_polarity(int irq, int active_low);

/* Snapshot and restore IRQ polarity settings for the inclusive range. */
void itss_snapshot_irq_polarities(int start, int end);
void itss_restore_irq_polarities(int start, int end);

void itss_irq_init(uint8_t pch_interrupt_routing[MAX_PXRC_CONFIG]);
void itss_clock_gate_8254(void);

#endif /* !defined(__ACPI__) */

#endif /* SOC_INTEL_COMMON_BLOCK_ITSS_H */
