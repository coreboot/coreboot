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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/lib_helpers.h>
#include <arch/transition.h>
#include <console/console.h>

/* Litte-endian, No XN-forced, Instr cache disabled,
 * Stack alignment disabled, Data and unified cache
 * disabled, Alignment check disabled, MMU disabled
 */
#define SCTLR_MASK  (SCTLR_MMU_DISABLE | SCTLR_ACE_DISABLE | \
		     SCTLR_CACHE_DISABLE | SCTLR_SAE_DISABLE | SCTLR_RES1 | \
		     SCTLR_ICE_DISABLE | SCTLR_WXN_DISABLE | SCTLR_LITTLE_END)

void __attribute__((weak)) exc_dispatch(struct exc_state *exc_state, uint64_t id)
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
	uint32_t scr_mask;
	uint64_t hcr_mask;
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
	else {
		scr_mask = SCR_LOWER_AARCH64;
		hcr_mask = HCR_LOWER_AARCH64;
	}

	/* SCR: Write to SCR if current EL is EL3 */
	if (current_el == EL3) {
		uint32_t scr = raw_read_scr_el3();
		scr |= scr_mask;
		raw_write_scr_el3(scr);
	}
	/* HCR: Write to HCR if current EL is EL2 */
	else if (current_el == EL2) {
		uint64_t hcr = raw_read_hcr_el2();
		hcr |= hcr_mask;
		raw_write_hcr_el2(hcr);
	}

	/* ELR/SPSR: Write entry point and processor state of program */
	raw_write_elr_current(elx->elr);
	raw_write_spsr_current(elx->spsr);

	/* SCTLR: Initialize EL with selected properties */
	sctlr = raw_read_sctlr(elx_el);
	sctlr &= SCTLR_MASK;
	raw_write_sctlr(sctlr, elx_el);

	/* SP_ELx: Initialize stack pointer */
	raw_write_sp_elx(elx->sp_elx, elx_el);

	/* Eret to the entry point */
	trans_switch(regs);
}
