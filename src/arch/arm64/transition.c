/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <arch/cache.h>
#include <arch/lib_helpers.h>
#include <arch/mmu.h>
#include <arch/transition.h>
#include <assert.h>
#include <compiler.h>
#include <console/console.h>

/* Litte-endian, No XN-forced, Instr cache disabled,
 * Stack alignment disabled, Data and unified cache
 * disabled, Alignment check disabled, MMU disabled
 */
#define SCTLR_MASK  (SCTLR_MMU_DISABLE | SCTLR_ACE_DISABLE | \
		     SCTLR_CACHE_DISABLE | SCTLR_SAE_DISABLE | SCTLR_RES1 | \
		     SCTLR_ICE_DISABLE | SCTLR_WXN_DISABLE | SCTLR_LITTLE_END)

void __weak exc_dispatch(struct exc_state *exc_state, uint64_t id)
{
	/* Default weak implementation does nothing. */
}

void exc_entry(struct exc_state *exc_state, uint64_t id)
{
	struct elx_state *elx = &exc_state->elx;
	struct regs *regs = &exc_state->regs;
	uint8_t elx_mode;

	elx->spsr = raw_read_spsr_el3();
	elx_mode = get_mode_from_spsr(elx->spsr);

	if (elx_mode == SPSR_USE_H)
		regs->sp = (uint64_t)&exc_state[1];
	else
		regs->sp = raw_read_sp_el0();

	elx->elr = raw_read_elr_el3();

	exc_dispatch(exc_state, id);
}

void transition_to_el2(void *entry, void *arg, uint64_t spsr)
{
	struct exc_state exc_state;
	struct elx_state *elx = &exc_state.elx;
	struct regs *regs = &exc_state.regs;
	uint32_t sctlr;

	regs->x[X0_INDEX] = (uint64_t)arg;
	elx->elr = (uint64_t)entry;
	elx->spsr = spsr;

	/*
	 * Policies enforced:
	 * 1. We support only transitions to EL2
	 * 2. We support transitions to Aarch64 mode only
	 *
	 * If any of the above conditions holds false, then we need a proper way
	 * to update SCR/HCR before removing the checks below
	 */
	assert(get_el_from_spsr(spsr) == EL2 && !(spsr & SPSR_ERET_32));

	/* Initialize SCR with defaults for running without secure monitor. */
	raw_write_scr_el3(SCR_TWE_DISABLE |	/* don't trap WFE */
			  SCR_TWI_DISABLE |	/* don't trap WFI */
			  SCR_ST_ENABLE |	/* allow secure timer access */
			  SCR_LOWER_AARCH64 |	/* lower level is AArch64 */
			  SCR_SIF_DISABLE |	/* disable secure ins. fetch */
			  SCR_HVC_ENABLE |	/* allow HVC instruction */
			  SCR_SMD_ENABLE |	/* disable SMC instruction */
			  SCR_RES1 |		/* reserved-1 bits */
			  SCR_EA_DISABLE |	/* disable ext. abort trap */
			  SCR_FIQ_DISABLE |	/* disable FIQ trap to EL3 */
			  SCR_IRQ_DISABLE |	/* disable IRQ trap to EL3 */
			  SCR_NS_ENABLE);	/* lower level is non-secure */

	/* Initialize CPTR to not trap anything to EL3. */
	raw_write_cptr_el3(CPTR_EL3_TCPAC_DISABLE | CPTR_EL3_TTA_DISABLE |
			   CPTR_EL3_TFP_DISABLE);

	/* ELR/SPSR: Write entry point and processor state of program */
	raw_write_elr_el3(elx->elr);
	raw_write_spsr_el3(elx->spsr);

	/* SCTLR: Initialize EL with selected properties */
	sctlr = raw_read_sctlr_el2();
	sctlr &= SCTLR_MASK;
	raw_write_sctlr_el2(sctlr);

	/* SP_ELx: Initialize stack pointer */
	raw_write_sp_el2(elx->sp_elx);

	/* Payloads expect to be entered with MMU disabled. Includes an ISB. */
	mmu_disable();

	/* Eret to the entry point */
	trans_switch(regs);
}
