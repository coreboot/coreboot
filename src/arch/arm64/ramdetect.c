/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#include <types.h>
#include <device/mmio.h>
#include <ramdetect.h>
#include <arch/exception.h>
#include <arch/transition.h>

static enum {
	ABORT_CHECKER_NOT_TRIGGERED,
	ABORT_CHECKER_TRIGGERED,
} abort_state = ABORT_CHECKER_NOT_TRIGGERED;

static int abort_checker(struct exc_state *state, uint64_t vector_id)
{
	if (raw_read_esr_el3() >> 26 != 0x25)
		return EXC_RET_IGNORED; /* Not a data abort. */

	abort_state = ABORT_CHECKER_TRIGGERED;
	state->elx.elr += sizeof(uint32_t); /* Jump over faulting instruction. */
	raw_write_elr_el3(state->elx.elr);
	return EXC_RET_HANDLED;
}

static struct exception_handler sync_el0 = {.handler = &abort_checker};

int probe_mb(const uintptr_t dram_start, const uintptr_t size)
{
	uintptr_t addr = dram_start + (size * MiB) - sizeof(uint32_t);
	void *ptr = (void *)addr;

	abort_state = ABORT_CHECKER_NOT_TRIGGERED;
	exception_handler_register(EXC_VID_CUR_SP_EL0_SYNC, &sync_el0);
	read32(ptr);
	exception_handler_unregister(EXC_VID_CUR_SP_EL0_SYNC, &sync_el0);
	return abort_state == ABORT_CHECKER_NOT_TRIGGERED;
}
