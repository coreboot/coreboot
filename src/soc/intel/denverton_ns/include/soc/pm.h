/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_PM_H_
#define _DENVERTON_NS_PM_H_

#include <soc/pmc.h>
#include <acpi/acpi.h>

#define GPE_MAX 127

/* P-state configuration */
#define PSS_MAX_ENTRIES		15
#define PSS_RATIO_STEP		1
#define PSS_LATENCY_TRANSITION	10
#define PSS_LATENCY_BUSMASTER	10

struct chipset_power_state {
	uint16_t pm1_sts;
	uint16_t pm1_en;
	uint32_t pm1_cnt;
	uint16_t tco1_sts;
	uint16_t tco2_sts;
	uint32_t gpe0_sts[GPE0_REG_MAX];
	uint32_t gpe0_en[GPE0_REG_MAX];
	uint32_t gen_pmcon_a;
	uint32_t gen_pmcon_b;
	uint32_t gblrst_cause[2];
	uint32_t prev_sleep_state;
} __attribute__((packed));

/* Get base address PMC memory mapped registers. */
uint8_t *pmc_mmio_regs(void);

struct chipset_power_state *fill_power_state(void);

/* Power Management Utility Functions. */
uint32_t clear_smi_status(void);
uint16_t clear_pm1_status(void);
uint32_t clear_tco_status(void);
uint32_t clear_gpe_status(void);
void clear_pmc_status(void);
void enable_smi(uint32_t mask);
void disable_smi(uint32_t mask);
void enable_pm1(uint16_t events);
void enable_pm1_control(uint32_t mask);
void disable_pm1_control(uint32_t mask);
void enable_gpe(uint32_t mask);
void disable_gpe(uint32_t mask);
void disable_all_gpe(void);

#endif /* _DENVERTON_NS_PM_H_ */
