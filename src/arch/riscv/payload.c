/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc
 * Copyright (C) 2018 HardenedLinux
 * Copyright (C) 2018 Jonathan Neuschäfer
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

#include <stdint.h>
#include <arch/boot.h>
#include <arch/encoding.h>
#include <console/console.h>
#include <vm.h>

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
