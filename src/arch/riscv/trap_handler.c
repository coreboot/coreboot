/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Early initialization code for riscv
 */

#include <arch/encoding.h>
#include <arch/exception.h>
#include <console/console.h>
#include <vm.h>
#include <mcall.h>
#include <sbi.h>
#include <types.h>

static const char *const exception_names[] = {
	"Instruction address misaligned",
	"Instruction access fault",
	"Illegal instruction",
	"Breakpoint",
	"Load address misaligned",
	"Load access fault",
	"Store address misaligned",
	"Store access fault",
	"Environment call from U-mode",
	"Environment call from S-mode",
	"Reserved (10)",
	"Environment call from M-mode",
	"Instruction page fault",
	"Load page fault",
	"Reserved (14)",
	"Store page fault",
};

static const char *mstatus_to_previous_mode(uintptr_t ms)
{
	switch (ms & MSTATUS_MPP) {
		case 0x00000000: return "user";
		case 0x00000800: return "supervisor";
		case 0x00001000: return "hypervisor";
		case 0x00001800: return "machine";
	}

	return "unknown";
}

static void print_trap_information(const trapframe *tf)
{
	const char *previous_mode;
	bool mprv = !!(tf->status & MSTATUS_MPRV);
	int hart_id = read_csr(mhartid);

	/* Leave some space around the trap message */
	printk(BIOS_DEBUG, "\n");

	if (tf->cause < ARRAY_SIZE(exception_names))
		printk(BIOS_DEBUG, "Exception:          %s\n",
				exception_names[tf->cause]);
	else
		printk(BIOS_DEBUG, "Trap:               Unknown cause %p\n",
				(void *)tf->cause);

	previous_mode = mstatus_to_previous_mode(read_csr(mstatus));
	printk(BIOS_DEBUG, "Hart ID:            %d\n", hart_id);
	printk(BIOS_DEBUG, "Previous mode:      %s%s\n",
			previous_mode, mprv? " (MPRV)":"");
	printk(BIOS_DEBUG, "Bad instruction pc: %p\n", (void *)tf->epc);
	printk(BIOS_DEBUG, "Bad address:        %p\n", (void *)tf->badvaddr);
	printk(BIOS_DEBUG, "Stored ra:          %p\n", (void *)tf->gpr[1]);
	printk(BIOS_DEBUG, "Stored sp:          %p\n", (void *)tf->gpr[2]);
}

static void interrupt_handler(trapframe *tf)
{
	uint64_t cause = tf->cause & ~0x8000000000000000ULL;

	switch (cause) {
	case IRQ_M_TIMER:
		/*
		 * Set interrupt pending for supervisor mode and disable timer
		 * interrupt in machine mode.
		 * To receive another timer interrupt just set timecmp and
		 * enable machine mode timer interrupt again.
		 */

		clear_csr(mie, MIP_MTIP);
		set_csr(mip, MIP_STIP);

		break;
	case IRQ_M_SOFT:
		if (HLS()->ipi_pending & IPI_SOFT) {
			set_csr(mip, MIP_SSIP);
		} else if (HLS()->ipi_pending & IPI_FENCE_I) {
			asm volatile("fence.i");
		} else if (HLS()->ipi_pending & IPI_SFENCE_VMA) {
			asm volatile("sfence.vma");
		} else if (HLS()->ipi_pending & IPI_SFENCE_VMA_ASID) {
			asm volatile("sfence.vma");
		} else if (HLS()->ipi_pending & IPI_SHUTDOWN) {
			while (HLS()->ipi_pending & IPI_SHUTDOWN)
				asm volatile("wfi");
		}
		break;
	default:
		printk(BIOS_EMERG, "======================================\n");
		printk(BIOS_EMERG, "coreboot: Unknown machine interrupt: 0x%llx\n",
		       cause);
		printk(BIOS_EMERG, "======================================\n");
		print_trap_information(tf);
		break;
	}
}
void trap_handler(trapframe *tf)
{
	write_csr(mscratch, tf);
	if (tf->cause & 0x8000000000000000ULL) {
		interrupt_handler(tf);
		return;
	}

	switch (tf->cause) {
		case CAUSE_MISALIGNED_FETCH:
		case CAUSE_FETCH_ACCESS:
		case CAUSE_ILLEGAL_INSTRUCTION:
		case CAUSE_BREAKPOINT:
		case CAUSE_LOAD_ACCESS:
		case CAUSE_STORE_ACCESS:
		case CAUSE_USER_ECALL:
		case CAUSE_HYPERVISOR_ECALL:
		case CAUSE_MACHINE_ECALL:
			print_trap_information(tf);
			break;
		case CAUSE_SUPERVISOR_ECALL:
			handle_sbi(tf);
			return;
		case CAUSE_MISALIGNED_LOAD:
		case CAUSE_MISALIGNED_STORE:
			print_trap_information(tf);
			handle_misaligned(tf);
			return;
		default:
			printk(BIOS_EMERG, "================================\n");
			printk(BIOS_EMERG, "coreboot: can not handle a trap:\n");
			printk(BIOS_EMERG, "================================\n");
			print_trap_information(tf);
			break;
	}

	die("Can't recover from trap. Halting.\n");
}

/* This function used to redirect trap to s-mode. */
void redirect_trap(void)
{
	write_csr(sbadaddr, read_csr(mbadaddr));
	write_csr(sepc, read_csr(mepc));
	write_csr(scause, read_csr(mcause));
	write_csr(mepc, read_csr(stvec));

	uintptr_t status = read_csr(mstatus);
	uintptr_t mpp = EXTRACT_FIELD(status, MSTATUS_MPP);
	status = INSERT_FIELD(status, MSTATUS_MPP, 1);
	status = INSERT_FIELD(status, MSTATUS_SPP, mpp & 1);
	write_csr(mstatus, status);
}
