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

#include <arch/cache.h>
#include <arch/lib_helpers.h>
#include <arch/mmu.h>
#include <arch/transition.h>
#include <assert.h>

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

	/* Initialize SCR with defaults for running without secure monitor
	   (disable all traps, enable all instructions, run NS at AArch64). */
	raw_write_scr_el3(SCR_FIEN | SCR_API | SCR_APK | SCR_ST | SCR_RW |
			  SCR_HCE | SCR_SMD | SCR_RES1 | SCR_NS);

	/* Initialize CPTR to not trap anything to EL3. */
	raw_write_cptr_el3(CPTR_EL3_TCPAC_DISABLE | CPTR_EL3_TTA_DISABLE |
			   CPTR_EL3_TFP_DISABLE);

	/* ELR/SPSR: Write entry point and processor state of program */
	raw_write_elr_el3(elx->elr);
	raw_write_spsr_el3(elx->spsr);

	/* SCTLR: Initialize EL with everything disabled */
	raw_write_sctlr_el2(SCTLR_RES1);

	/* SP_ELx: Initialize stack pointer */
	raw_write_sp_el2(elx->sp_elx);

	/* Payloads expect to be entered with MMU disabled. Includes an ISB. */
	mmu_disable();

	/* Eret to the entry point */
	trans_switch(regs);
}
