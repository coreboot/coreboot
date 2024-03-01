/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/exception.h>
#include <types.h>
#include <console/console.h>
#include <device/mmio.h>
#include <ramdetect.h>
#include <arch/smp/spinlock.h>
#include <vm.h>

static enum {
	ABORT_CHECKER_NOT_TRIGGERED,
	ABORT_CHECKER_TRIGGERED,
} abort_state = ABORT_CHECKER_NOT_TRIGGERED;

extern void (*trap_handler)(trapframe *tf);

static int get_instruction_len(uintptr_t addr)
{
	uint16_t ins = read16p(addr);

	/*
	 * 16-bit or 32-bit instructions supported
	 */
	if ((ins & 0x3) != 3) {
		return 2;
	} else if ((ins & 0x1f) != 0x1f) {
		return 4;
	}

	die("Not a 16bit or 32bit instruction 0x%x\n", ins);
}

static void ramcheck_trap_handler(trapframe *tf)
{
	abort_state = ABORT_CHECKER_TRIGGERED;

	/*
	 * skip read instruction.
	 */
	int insn_size = get_instruction_len(tf->epc);

	write_csr(mepc, read_csr(mepc) + insn_size);
}

int probe_mb(const uintptr_t dram_start, const uintptr_t size)
{
	uintptr_t addr = dram_start + (size * MiB) - sizeof(uint32_t);
	void *ptr = (void *)addr;

	abort_state = ABORT_CHECKER_NOT_TRIGGERED;
	trap_handler = ramcheck_trap_handler;
	barrier();
	read32(ptr);
	trap_handler = default_trap_handler;
	barrier();
	printk(BIOS_DEBUG, "%lx is %s DRAM\n", dram_start + size * MiB,
	       abort_state == ABORT_CHECKER_NOT_TRIGGERED ? "" : "not");

	return abort_state == ABORT_CHECKER_NOT_TRIGGERED;
}
