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
	uint8_t elx_mode, elx_el;

	elx->spsr = raw_read_spsr_current();
	elx_mode = get_mode_from_spsr(elx->spsr);
	elx_el = get_el_from_spsr(elx->spsr);

	if (elx_mode == SPSR_USE_H) {
		if (elx_el == get_current_el())
			regs->sp = (uint64_t)&exc_state[1];
		else
			regs->sp = raw_read_sp_elx(elx_el);
	} else {
		regs->sp = raw_read_sp_el0();
	}

	elx->elr = raw_read_elr_current();

	exc_dispatch(exc_state, id);
}

void transition_with_entry(void *entry, void *arg, struct exc_state *exc_state)
{
	/* Argument to entry point goes into X0 */
	exc_state->regs.x[X0_INDEX] = (uint64_t)arg;
	/* Entry point goes into ELR */
	exc_state->elx.elr = (uint64_t)entry;

	transition(exc_state);
}

void transition(struct exc_state *exc_state)
{
	uint64_t sctlr;
	uint32_t current_el = get_current_el();

	struct elx_state *elx = &exc_state->elx;
	struct regs *regs = &exc_state->regs;

	uint8_t elx_el = get_el_from_spsr(elx->spsr);

	/*
	 * Policies enforced:
	 * 1. We support only elx --> (elx - 1) transitions
	 * 2. We support transitions to Aarch64 mode only
	 *
	 * If any of the above conditions holds false, then we need a proper way
	 * to update SCR/HCR before removing the checks below
	 */
	if ((current_el - elx_el) != 1)
		die("ARM64 Error: Do not support transition\n");

	if (elx->spsr & SPSR_ERET_32)
		die("ARM64 Error: Do not support eret to Aarch32\n");

	/* Most parts of coreboot currently don't support EL2 anyway. */
	assert(current_el == EL3);

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
	raw_write_elr_current(elx->elr);
	raw_write_spsr_current(elx->spsr);

	/* SCTLR: Initialize EL with selected properties */
	sctlr = raw_read_sctlr(elx_el);
	sctlr &= SCTLR_MASK;
	raw_write_sctlr(sctlr, elx_el);

	/* SP_ELx: Initialize stack pointer */
	raw_write_sp_elx(elx->sp_elx, elx_el);

	/* Payloads expect to be entered with MMU disabled. Includes an ISB. */
	mmu_disable();

	/* Eret to the entry point */
	trans_switch(regs);
}
