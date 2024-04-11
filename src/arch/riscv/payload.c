/* SPDX-License-Identifier: GPL-2.0-only */

#include <program_loading.h>
#include <stdint.h>
#include <arch/boot.h>
#include <arch/encoding.h>
#include <arch/pmp.h>
#include <arch/smp/atomic.h>
#include <console/console.h>
#include <mcall.h>
#include <vm.h>

/* Run OpenSBI and let OpenSBI hand over control to the payload */
void run_payload_opensbi(struct prog *prog, void *fdt, struct prog *opensbi, int payload_mode)
{
	int hart_id = read_csr(mhartid);
	uintptr_t status = read_csr(mstatus);
	status = INSERT_FIELD(status, MSTATUS_MPIE, 0);

	/*
	 * In case of OpenSBI we always run it in M-Mode.
	 * OpenSBI will switch to payload_mode when done.
	 */

	status = INSERT_FIELD(status, MSTATUS_MPP, PRV_M);
	/* Trap vector base address point to the payload */
	write_csr(mtvec, prog_entry(opensbi));
	/* disable M-Mode interrupt */
	write_csr(mie, 0);
	write_csr(mstatus, status);

	run_opensbi(hart_id, fdt, prog_entry(opensbi), prog_entry(prog), payload_mode);
}

/* Runs the payload without OpenSBI integration */
void run_payload(struct prog *prog, void *fdt, int payload_mode)
{
	void (*doit)(int hart_id, void *fdt) = prog_entry(prog);
	int hart_id = read_csr(mhartid);
	uintptr_t status = read_csr(mstatus);
	extern void *_text, *_estack;
	status = INSERT_FIELD(status, MSTATUS_MPIE, 0);

	switch (payload_mode) {
	case RISCV_PAYLOAD_MODE_S:
		/*
		 * Set up a PMP to protect coreboot, then close the PMPs.
		 * If a mainboard or SoC needs other ranges
		 * set up, they should do so before this point,
		 * as close_pmp puts in a "match all" entry, and
		 * PMPs are processed in linear order.
		 */

		/*
		 * On this code path, coreboot is providing the coreboot SBI, and must
		 * protect the ramstage, from _text to _estack, from S and U
		 * modes. Because the number of PMP registers may be very
		 * small, make this an NAPOT area. The linker scripts
		 * should round _text and _estack to 4K.
		 */
		setup_pmp((u64)(uintptr_t) _text,
				(u64)(uintptr_t) _estack - (u64)(uintptr_t) _text, 0);

		/*
		 * All pmp operations should be finished when close_pmp is called.
		 * Presently, this requirement is not enforced.
		 */
		close_pmp();

		status = INSERT_FIELD(status, MSTATUS_MPP, PRV_S);
		/* Trap vector base address point to the payload */
		write_csr(stvec, doit);
		/* disable S-Mode interrupt */
		write_csr(sie, 0);
		/* disable MMU */
		write_csr(satp, 0);
		/* save stack to mscratch so trap_entry can use that as exception stack */
		write_csr(mscratch, MACHINE_STACK_TOP());
		break;
	case RISCV_PAYLOAD_MODE_M:
		status = INSERT_FIELD(status, MSTATUS_MPP, PRV_M);
		/* Trap vector base address point to the payload */
		write_csr(mtvec, doit);
		/* disable M-Mode interrupt */
		write_csr(mie, 0);
		break;
	default:
		die("wrong privilege level for payload");
		break;
	}
	write_csr(mstatus, status);
	write_csr(mepc, doit);
	asm volatile(
		"mv a0, %0\n\t"
		"mv a1, %1\n\t"
		"mret" ::"r"(hart_id),
		"r"(fdt)
		: "a0", "a1");
}
