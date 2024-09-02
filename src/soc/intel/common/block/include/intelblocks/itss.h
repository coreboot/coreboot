/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_ITSS_H
#define SOC_INTEL_COMMON_BLOCK_ITSS_H

#define ITSS_MAX_IRQ	119
#define IRQS_PER_IPC	32
#define NUM_IPC_REGS	((ITSS_MAX_IRQ + IRQS_PER_IPC - 1)/IRQS_PER_IPC)

/* PIRQA Routing Control Register */
#define PCR_ITSS_PIRQA_ROUT	0x3100
/* PIRQB Routing Control Register */
#define PCR_ITSS_PIRQB_ROUT	0x3101
/* PIRQC Routing Control Register */
#define PCR_ITSS_PIRQC_ROUT	0x3102
/* PIRQD Routing Control Register */
#define PCR_ITSS_PIRQD_ROUT	0x3103
/* PIRQE Routing Control Register */
#define PCR_ITSS_PIRQE_ROUT	0x3104
/* PIRQF Routing Control Register */
#define PCR_ITSS_PIRQF_ROUT	0x3105
/* PIRQG Routing Control Register */
#define PCR_ITSS_PIRQG_ROUT	0x3106
/* PIRQH Routing Control Register */
#define PCR_ITSS_PIRQH_ROUT	0x3107
/* ITSS Interrupt polarity control */
#define PCR_ITSS_IPC0_CONF	0x3200
/* ITSS Power reduction control */
#define PCR_ITSS_ITSSPRC	0x3300

#if !defined(__ACPI__)

#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <stdint.h>

/* Set the interrupt polarity for provided IRQ to the APIC. */
void itss_set_irq_polarity(int irq, int active_low);

/* Snapshot and restore IRQ polarity settings for the inclusive range. */
void itss_snapshot_irq_polarities(int start, int end);
void itss_restore_irq_polarities(int start, int end);

void itss_irq_init(const uint8_t pch_interrupt_routing[PIRQ_COUNT]);
void itss_clock_gate_8254(void);

#endif /* !defined(__ACPI__) */

#endif /* SOC_INTEL_COMMON_BLOCK_ITSS_H */
