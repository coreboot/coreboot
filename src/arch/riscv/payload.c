/* SPDX-License-Identifier: GPL-2.0-only */

#include <program_loading.h>
#include <stdint.h>
#include <arch/boot.h>
#include <arch/encoding.h>
#include <arch/smp/atomic.h>
#include <console/console.h>
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
	status = INSERT_FIELD(status, MSTATUS_MPIE, 0);

	switch (payload_mode) {
	case RISCV_PAYLOAD_MODE_U:
		status = INSERT_FIELD(status, MSTATUS_MPP, PRV_U);
		/* Trap vector base address point to the payload */
		write_csr(utvec, doit);
		/* disable U-Mode interrupt */
		write_csr(uie, 0);
		break;
	case RISCV_PAYLOAD_MODE_S:
		status = INSERT_FIELD(status, MSTATUS_MPP, PRV_S);
		/* Trap vector base address point to the payload */
		write_csr(stvec, doit);
		/* disable S-Mode interrupt */
		write_csr(sie, 0);
		/* disable MMU */
		write_csr(satp, 0);
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
