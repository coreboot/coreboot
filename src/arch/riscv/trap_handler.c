/*
 * Early initialization code for riscv
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <arch/exception.h>
#include <spike_util.h>
#include <string.h>

#define HART_ID 0
#define CONSOLE_PUT 1
#define SEND_DEVICE_REQUEST 2
#define RECEIVE_DEVICE_RESPONSE 3
#define SEND_IPI 4
#define CLEAR_IPI 5
#define SHUTDOWN 6
#define SET_TIMER 7
#define QUERY_MEMORY 8

int loopBreak2 = 1;

void handle_supervisor_call(trapframe *tf) {
	uintptr_t call = tf->gpr[17];
	uintptr_t arg0 = tf->gpr[10];
	uintptr_t arg1 = tf->gpr[11];
	uintptr_t returnValue;
	switch(call) {
		case HART_ID:
			printk(BIOS_DEBUG, "Getting hart id...\n");
			returnValue = mcall_hart_id();
			break;
		case CONSOLE_PUT:
			returnValue = mcall_console_putchar(arg0);
			break;
		case SEND_DEVICE_REQUEST:
			printk(BIOS_DEBUG, "Sending device request...\n");
			returnValue = mcall_dev_req((sbi_device_message*) arg0);
			break;
		case RECEIVE_DEVICE_RESPONSE:
			printk(BIOS_DEBUG, "Getting device response...\n");
			returnValue = mcall_dev_resp();
			break;
		case SEND_IPI:
			printk(BIOS_DEBUG, "Sending IPI...\n");
			returnValue = mcall_send_ipi(arg0);
			break;
		case CLEAR_IPI:
			printk(BIOS_DEBUG, "Clearing IPI...\n");
			returnValue = mcall_clear_ipi();
			break;
		case SHUTDOWN:
			printk(BIOS_DEBUG, "Shutting down...\n");
			returnValue = mcall_shutdown();
			break;
		case SET_TIMER:
			printk(BIOS_DEBUG, "Setting timer...\n");
			returnValue = mcall_set_timer(arg0);
			break;
		case QUERY_MEMORY:
			printk(BIOS_DEBUG, "Querying memory, CPU #%lld...\n", arg0);
			returnValue = mcall_query_memory(arg0, (memory_block_info*) arg1);
			break;
		default:
			printk(BIOS_DEBUG, "ERROR! Unrecognized system call\n");
			returnValue = 0;
			break; // note: system call we do not know how to handle
	}
	tf->gpr[10] = returnValue;
	write_csr(mepc, read_csr(mepc) + 4);
	asm volatile("j supervisor_call_return");
}

void trap_handler(trapframe *tf) {
	write_csr(mscratch, tf);
	int cause = 0;
	void* epc = 0;
	void* badAddr = 0;

	// extract cause
	asm("csrr t0, mcause");
	asm("move %0, t0" : "=r"(cause));

	// extract faulting Instruction pc
	epc = (void*) tf->epc;

	// extract bad address
	asm("csrr t0, mbadaddr");
	asm("move %0, t0" : "=r"(badAddr));

	switch(cause) {
		case 0:
			printk(BIOS_DEBUG, "Trap: Instruction address misaligned\n");
			break;
		case 1:
			printk(BIOS_DEBUG, "Trap: Instruction access fault\n");
			printk(BIOS_DEBUG, "Bad instruction pc: %p\n", epc);
			printk(BIOS_DEBUG, "Address: %p\n", badAddr);
			break;
		case 2:
			printk(BIOS_DEBUG, "Trap: Illegal instruction\n");
			printk(BIOS_DEBUG, "Bad instruction pc: %p\n", epc);
			printk(BIOS_DEBUG, "Address: %p\n", badAddr);
			break;
		case 3:
			printk(BIOS_DEBUG, "Trap: Breakpoint\n");
			break;
		case 4:
			printk(BIOS_DEBUG, "Trap: Load address misaligned\n");
			//handleMisalignedLoad(tf);
			break;
		case 5:
			printk(BIOS_DEBUG, "Trap: Load access fault\n");
			break;
		case 6:
			printk(BIOS_DEBUG, "Trap: Store address misaligned\n");
			printk(BIOS_DEBUG, "Bad instruction pc: %p\n", epc);
			printk(BIOS_DEBUG, "Store Address: %p\n", badAddr);
			handle_misaligned_store(tf);
			break;
		case 7:
			printk(BIOS_DEBUG, "Trap: Store access fault\n");
			printk(BIOS_DEBUG, "Bad instruction pc: %p\n", epc);
			printk(BIOS_DEBUG, "Store Address: %p\n", badAddr);
			break;
		case 8:
			printk(BIOS_DEBUG, "Trap: Environment call from U-mode\n");
			break;
		case 9:
			// Don't print so we make console putchar calls look the way they should
			// printk(BIOS_DEBUG, "Trap: Environment call from S-mode\n");
			handle_supervisor_call(tf);
			break;
		case 10:
			printk(BIOS_DEBUG, "Trap: Environment call from H-mode\n");
			break;
		case 11:
			printk(BIOS_DEBUG, "Trap: Environment call from M-mode\n");
			break;
		default:
			printk(BIOS_DEBUG, "Trap: Unknown cause\n");
			break;
	}
	printk(BIOS_DEBUG, "Stored ra: %p\n", (void*) tf->gpr[1]);
	printk(BIOS_DEBUG, "Stored sp: %p\n", (void*) tf->gpr[2]);
	printk(BIOS_DEBUG, "looping...\n");
	while(1);
}

void handleMisalignedLoad(trapframe *tf) {
	printk(BIOS_DEBUG, "Trapframe ptr: %p\n", tf);
	printk(BIOS_DEBUG, "Stored sp: %p\n", (void*) tf->gpr[2]);
	insn_t faultingInstruction = 0;
	uintptr_t faultingInstructionAddr = tf->epc;
	asm("move t0, %0" : /* No outputs */ : "r"(faultingInstructionAddr));
	asm("lw t0, 0(t0)");
	asm("move %0, t0" : "=r"(faultingInstruction));
	printk(BIOS_DEBUG, "Faulting instruction: 0x%x\n", faultingInstruction);
	insn_t widthMask = 0x7000;
	insn_t memWidth = (faultingInstruction & widthMask) >> 12;
	insn_t destMask = 0xF80;
	insn_t destRegister = (faultingInstruction & destMask) >> 7;
	printk(BIOS_DEBUG, "Width: 0x%x\n", memWidth);
	if (memWidth == 3) {
		// load double, handle the issue
		void* badAddress = (void*) tf->badvaddr;
		memcpy(&(tf->gpr[destRegister]), badAddress, 8);
	} else {
		// panic, this should not have happened
		printk(BIOS_DEBUG, "Code should not reach this path, misaligned on a non-64 bit store/load\n");
		while(1);
	}

	// return to where we came from
	write_csr(mepc, read_csr(mepc) + 4);
	asm volatile("j machine_call_return");
}

void handle_misaligned_store(trapframe *tf) {
	printk(BIOS_DEBUG, "Trapframe ptr: %p\n", tf);
	printk(BIOS_DEBUG, "Stored sp: %p\n", (void*) tf->gpr[2]);
	insn_t faultingInstruction = 0;
	uintptr_t faultingInstructionAddr = tf->epc;
	asm("move t0, %0" : /* No outputs */ : "r"(faultingInstructionAddr));
	asm("lw t0, 0(t0)");
	asm("move %0, t0" : "=r"(faultingInstruction));
	printk(BIOS_DEBUG, "Faulting instruction: 0x%x\n", faultingInstruction);
	insn_t widthMask = 0x7000;
	insn_t memWidth = (faultingInstruction & widthMask) >> 12;
	insn_t srcMask = 0x1F00000;
	insn_t srcRegister = (faultingInstruction & srcMask) >> 20;
	printk(BIOS_DEBUG, "Width: 0x%x\n", memWidth);
	if (memWidth == 3) {
		// store double, handle the issue
		void* badAddress = (void*) tf->badvaddr;
		long valueToStore = tf->gpr[srcRegister];
		memcpy(badAddress, &valueToStore, 8);
	} else {
		// panic, this should not have happened
		printk(BIOS_DEBUG, "Code should not reach this path, misaligned on a non-64 bit store/load\n");
		while(1);
	}

	// return to where we came from
	write_csr(mepc, read_csr(mepc) + 4);
	asm volatile("j machine_call_return");
}
