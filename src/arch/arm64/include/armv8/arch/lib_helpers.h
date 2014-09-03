/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc
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
 *
 * lib_helpers.h: All library function prototypes and macros are defined in this
 * file.
 */

#define EL0               0
#define EL1               1
#define EL2               2
#define EL3               3

#define CURRENT_EL_MASK   0x3
#define CURRENT_EL_SHIFT  2

#define DAIF_DBG_BIT      (1<<3)
#define DAIF_ABT_BIT      (1<<2)
#define DAIF_IRQ_BIT      (1<<1)
#define DAIF_FIQ_BIT      (1<<0)

#define SWITCH_CASE_READ(func,var,type)	 do {	\
	type var = -1;				\
	uint8_t current_el = get_current_el();	\
	switch(current_el) {			\
        case EL1:				\
		var = func##_el1();		\
			break;			\
        case EL2:				\
		var = func##_el2();		\
			break;			\
        case EL3:                               \
		var = func##_el3();		\
			break;			\
	}					\
	return var;				\
	} while(0)

#define SWITCH_CASE_WRITE(func,var)	do {   \
	uint8_t current_el = get_current_el(); \
	switch(current_el) {		       \
	case EL1:			       \
		func##_el1(var);	       \
			break;		       \
        case EL2:			       \
		func##_el2(var);	       \
			break;		       \
        case EL3:			       \
		func##_el3(var);	       \
			break;		       \
	}				       \
	} while(0)

#define SWITCH_CASE_TLBI(func) do {		\
	uint8_t current_el = get_current_el();	\
	switch(current_el) {			\
        case EL1:        			\
		func##_el1();			\
			break;			\
        case EL2:        			\
		func##_el2();			\
			break;			\
        case EL3:        			\
		func##_el3();			\
			break;			\
	}					\
	} while(0)

/* PSTATE and special purpose register access functions */
uint32_t raw_read_current_el(void);
uint32_t get_current_el(void);
uint32_t raw_read_daif(void);
void raw_write_daif(uint32_t daif);
void enable_debug_exceptions(void);
void enable_serror_exceptions(void);
void enable_irq(void);
void enable_fiq(void);
void disable_debug_exceptions(void);
void disable_serror_exceptions(void);
void disable_irq(void);
void disable_fiq(void);
uint64_t raw_read_dlr_el0(void);
void raw_write_dlr_el0(uint64_t dlr_el0);
uint64_t raw_read_dspsr_el0(void);
void raw_write_dspsr_el0(uint64_t dspsr_el0);
uint64_t raw_read_elr_el1(void);
void raw_write_elr_el1(uint64_t elr_el1);
uint64_t raw_read_elr_el2(void);
void raw_write_elr_el2(uint64_t elr_el2);
uint64_t raw_read_elr_el3(void);
void raw_write_elr_el3(uint64_t elr_el3);
uint64_t raw_read_elr_current(void);
void raw_write_elr_current(uint64_t elr);
uint32_t raw_read_fpcr(void);
void raw_write_fpcr(uint32_t fpcr);
uint32_t raw_read_fpsr(void);
void raw_write_fpsr(uint32_t fpsr);
uint32_t raw_read_nzcv(void);
void raw_write_nzcv(uint32_t nzcv);
uint64_t raw_read_sp_el0(void);
void raw_write_sp_el0(uint64_t sp_el0);
uint64_t raw_read_sp_el1(void);
void raw_write_sp_el1(uint64_t sp_el1);
uint64_t raw_read_sp_el2(void);
void raw_write_sp_el2(uint64_t sp_el2);
uint32_t raw_read_spsel(void);
void raw_write_spsel(uint32_t spsel);
uint64_t raw_read_sp_el3(void);
void raw_write_sp_el3(uint64_t sp_el3);
uint32_t raw_read_spsr_abt(void);
void raw_write_spsr_abt(uint32_t spsr_abt);
uint32_t raw_read_spsr_el1(void);
void raw_write_spsr_el1(uint32_t spsr_el1);
uint32_t raw_read_spsr_el2(void);
void raw_write_spsr_el2(uint32_t spsr_el2);
uint32_t raw_read_spsr_el3(void);
void raw_write_spsr_el3(uint32_t spsr_el3);
uint32_t raw_read_spsr_current(void);
void raw_write_spsr_current(uint32_t spsr);
uint32_t raw_read_spsr_fiq(void);
void raw_write_spsr_fiq(uint32_t spsr_fiq);
uint32_t raw_read_spsr_irq(void);
void raw_write_spsr_irq(uint32_t spsr_irq);
uint32_t raw_read_spsr_und(void);
void raw_write_spsr_und(uint32_t spsr_und);

/* System control register access */
uint32_t raw_read_actlr_el1(void);
void raw_write_actlr_el1(uint32_t actlr_el1);
uint32_t raw_read_actlr_el2(void);
void raw_write_actlr_el2(uint32_t actlr_el2);
uint32_t raw_read_actlr_el3(void);
void raw_write_actlr_el3(uint32_t actlr_el3);
uint32_t raw_read_actlr_current(void);
void raw_write_actlr_current(uint32_t actlr);
uint32_t raw_read_afsr0_el1(void);
void raw_write_afsr0_el1(uint32_t afsr0_el1);
uint32_t raw_read_afsr0_el2(void);
void raw_write_afsr0_el2(uint32_t afsr0_el2);
uint32_t raw_read_afsr0_el3(void);
void raw_write_afsr0_el3(uint32_t afsr0_el3);
uint32_t raw_read_afsr0_current(void);
void raw_write_afsr0_current(uint32_t afsr0);
uint32_t raw_read_afsr1_el1(void);
void raw_write_afsr1_el1(uint32_t afsr1_el1);
uint32_t raw_read_afsr1_el2(void);
void raw_write_afsr1_el2(uint32_t afsr1_el2);
uint32_t raw_read_afsr1_el3(void);
void raw_write_afsr1_el3(uint32_t afsr1_el3);
uint32_t raw_read_afsr1_current(void);
void raw_write_afsr1_current(uint32_t afsr1);
uint32_t raw_read_aidr_el1(void);
uint64_t raw_read_amair_el1(void);
void raw_write_amair_el1(uint64_t amair_el1);
uint64_t raw_read_amair_el2(void);
void raw_write_amair_el2(uint64_t amair_el2);
uint64_t raw_read_amair_el3(void);
void raw_write_amair_el3(uint64_t amair_el3);
uint64_t raw_read_amair_current(void);
void raw_write_amair_current(uint64_t amair);
uint32_t raw_read_ccsidr_el1(void);
uint32_t raw_read_clidr_el1(void);
uint32_t raw_read_cpacr_el1(void);
void raw_write_cpacr_el1(uint32_t cpacr_el1);
uint32_t raw_read_cptr_el2(void);
void raw_write_cptr_el2(uint32_t cptr_el2);
uint32_t raw_read_cptr_el3(void);
void raw_write_cptr_el3(uint32_t cptr_el3);
uint32_t raw_read_cptr_current(void);
void raw_write_cptr_current(uint32_t cptr);
uint32_t raw_read_csselr_el1(void);
void raw_write_csselr_el1(uint32_t csselr_el1);
uint32_t raw_read_ctr_el0(void);
uint32_t raw_read_esr_el1(void);
void raw_write_esr_el1(uint32_t esr_el1);
uint32_t raw_read_esr_el2(void);
void raw_write_esr_el2(uint32_t esr_el2);
uint32_t raw_read_esr_el3(void);
void raw_write_esr_el3(uint32_t esr_el3);
uint32_t raw_read_esr_current(void);
void raw_write_esr_current(uint32_t esr);
uint64_t raw_read_far_el1(void);
void raw_write_far_el1(uint64_t far_el1);
uint64_t raw_read_far_el2(void);
void raw_write_far_el2(uint64_t far_el2);
uint64_t raw_read_far_el3(void);
void raw_write_far_el3(uint64_t far_el3);
uint64_t raw_read_far_current(void);
void raw_write_far_current(uint64_t far);
uint64_t raw_read_hcr_el2(void);
void raw_write_hcr_el2(uint64_t hcr_el2);
uint64_t raw_read_aa64pfr0_el1(void);
uint64_t raw_read_mair_el1(void);
void raw_write_mair_el1(uint64_t mair_el1);
uint64_t raw_read_mair_el2(void);
void raw_write_mair_el2(uint64_t mair_el2);
uint64_t raw_read_mair_el3(void);
void raw_write_mair_el3(uint64_t mair_el3);
uint64_t raw_read_mair_current(void);
void raw_write_mair_current(uint64_t mair);
uint32_t raw_read_midr_el1(void);
uint64_t raw_read_mpidr_el1(void);
uint32_t raw_read_rmr_el1(void);
void raw_write_rmr_el1(uint32_t rmr_el1);
uint32_t raw_read_rmr_el2(void);
void raw_write_rmr_el2(uint32_t rmr_el2);
uint32_t raw_read_rmr_el3(void);
void raw_write_rmr_el3(uint32_t rmr_el3);
uint32_t raw_read_rmr_current(void);
void raw_write_rmr_current(uint32_t rmr);
uint64_t raw_read_rvbar_el1(void);
void raw_write_rvbar_el1(uint64_t rvbar_el1);
uint64_t raw_read_rvbar_el2(void);
void raw_write_rvbar_el2(uint64_t rvbar_el2);
uint64_t raw_read_rvbar_el3(void);
void raw_write_rvbar_el3(uint64_t rvbar_el3);
uint64_t raw_read_rvbar_current(void);
void raw_write_rvbar_current(uint64_t rvbar);
uint32_t raw_read_scr_el3(void);
void raw_write_scr_el3(uint32_t scr_el3);
uint32_t raw_read_sctlr_el1(void);
void raw_write_sctlr_el1(uint32_t sctlr_el1);
uint32_t raw_read_sctlr_el2(void);
void raw_write_sctlr_el2(uint32_t sctlr_el2);
uint32_t raw_read_sctlr_el3(void);
void raw_write_sctlr_el3(uint32_t sctlr_el3);
uint32_t raw_read_sctlr_current(void);
void raw_write_sctlr_current(uint32_t sctlr);
uint64_t raw_read_tcr_el1(void);
void raw_write_tcr_el1(uint64_t tcr_el1);
uint32_t raw_read_tcr_el2(void);
void raw_write_tcr_el2(uint32_t tcr_el2);
uint32_t raw_read_tcr_el3(void);
void raw_write_tcr_el3(uint32_t tcr_el3);
uint64_t raw_read_ttbr0_el1(void);
void raw_write_ttbr0_el1(uint64_t ttbr0_el1);
uint64_t raw_read_ttbr0_el2(void);
void raw_write_ttbr0_el2(uint64_t ttbr0_el2);
uint64_t raw_read_ttbr0_el3(void);
void raw_write_ttbr0_el3(uint64_t ttbr0_el3);
uint64_t raw_read_ttbr0_current(void);
void raw_write_ttbr0_current(uint64_t ttbr0);
uint64_t raw_read_ttbr1_el1(void);
void raw_write_ttbr1_el1(uint64_t ttbr1_el1);
uint64_t raw_read_vbar_el1(void);
void raw_write_vbar_el1(uint64_t vbar_el1);
uint64_t raw_read_vbar_el2(void);
void raw_write_vbar_el2(uint64_t vbar_el2);
uint64_t raw_read_vbar_el3(void);
void raw_write_vbar_el3(uint64_t vbar_el3);
uint64_t raw_read_vbar_current(void);
void raw_write_vbar_current(uint64_t vbar);

/* Cache maintenance system instructions */
void dccisw(uint64_t cisw);
void dccivac(uint64_t civac);
void dccsw(uint64_t csw);
void dccvac(uint64_t cvac);
void dccvau(uint64_t cvau);
void dcisw(uint64_t isw);
void dcivac(uint64_t ivac);
void dczva(uint64_t zva);
void iciallu(void);
void icialluis(void);
void icivau(uint64_t ivau);

/* TLB maintenance instructions */
void tlbiall_el1(void);
void tlbiall_el2(void);
void tlbiall_el3(void);
void tlbiall_current(void);
void tlbiallis_el1(void);
void tlbiallis_el2(void);
void tlbiallis_el3(void);
void tlbiallis_current(void);
void tlbivaa_el1(uint64_t va);

/* Memory barrier */
void dmb(void);
void dsb(void);
void isb(void);
