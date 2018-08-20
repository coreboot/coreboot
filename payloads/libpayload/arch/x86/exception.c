/*
 * This file is part of the libpayload project.
 *
 * Copyright 2013 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <arch/exception.h>
#include <exception.h>
#include <libpayload.h>
#include <stdint.h>
#include <arch/apic.h>

#define IF_FLAG				(1 << 9)
/*
 * Local and I/O APICs support 240 vectors (in the range of 16 to 255) as valid
 * interrupts. The Intel 64 and IA-32 architectures reserve vectors 16
 * through 31 for predefined interrupts, exceptions, and Intel-reserved
 * encodings.
*/
#define FIRST_USER_DEFINED_VECTOR	32

u32 exception_stack[0x400] __attribute__((aligned(8)));

static exception_hook hook;

static interrupt_handler handlers[256];

static const char *names[EXC_COUNT] = {
	[EXC_DE]  = "Divide by Zero",
	[EXC_DB]  = "Debug",
	[EXC_NMI] = "Non-Maskable-Interrupt",
	[EXC_BP]  = "Breakpoint",
	[EXC_OF]  = "Overflow",
	[EXC_BR]  = "Bound Range",
	[EXC_UD]  = "Invalid Opcode",
	[EXC_NM]  = "Device Not Available",
	[EXC_DF]  = "Double Fault",
	[EXC_TS]  = "Invalid TSS",
	[EXC_NP]  = "Segment Not Present",
	[EXC_SS]  = "Stack Fault",
	[EXC_GP]  = "General Protection Fault",
	[EXC_PF]  = "Page Fault",
	[EXC_MF]  = "x87 Floating Point",
	[EXC_AC]  = "Alignment Check",
	[EXC_MC]  = "Machine Check",
	[EXC_XF]  = "SIMD Floating Point",
	[EXC_SX]  = "Security",
};

static void print_segment_error_code(u32 code)
{
	printf("%#x - descriptor %#x in the ", code, (code >> 3) & 0x1FFF);
	if (code & (0x1 << 1)) {
		printf("IDT");
	} else {
		if (code & 0x04)
			printf("LDT");
		else
			printf("GDT");
	}
	if (code & (0x1 << 0))
		printf(", external to the CPU");
	else
		printf(", internal to the CPU");
}

static void print_page_fault_error_code(u32 code)
{
	printf("%#x -", code);
	if (code & (0x1 << 0))
		printf(" page protection");
	else
		printf(" page not present");
	if (code & (0x1 << 1))
		printf(", write");
	else
		printf(", read");
	if (code & (0x1 << 2))
		printf(", user");
	else
		printf(", supervisor");
	if (code & (0x1 << 3))
		printf(", reserved bits set");
	if (code & (0x1 << 4))
		printf(", instruction fetch");
}

static void print_raw_error_code(u32 code)
{
	printf("%#x", code);
}

static void dump_stack(uintptr_t addr, size_t bytes)
{
	int i, j;
	const int line = 8;
	uint32_t *ptr = (uint32_t *)(addr & ~(line * sizeof(*ptr) - 1));

	printf("Dumping stack:\n");
	for (i = bytes / sizeof(*ptr); i >= 0; i -= line) {
		printf("%p: ", ptr + i);
		for (j = i; j < i + line; j++)
			printf("%08x ", *(ptr + j));
		printf("\n");
	}
}

static void dump_exception_state(void)
{
	printf("%s Exception\n", names[exception_state->vector]);

	printf("Error code: ");
	switch (exception_state->vector) {
	case EXC_PF:
		print_page_fault_error_code(exception_state->error_code);
		break;
	case EXC_TS:
	case EXC_NP:
	case EXC_SS:
	case EXC_GP:
		print_segment_error_code(exception_state->error_code);
		break;
	case EXC_DF:
	case EXC_AC:
	case EXC_SX:
		print_raw_error_code(exception_state->error_code);
		break;
	default:
		printf("n/a");
		break;
	}
	printf("\n");
	printf("EIP:    0x%08x\n", exception_state->regs.eip);
	printf("CS:     0x%04x\n", exception_state->regs.cs);
	printf("EFLAGS: 0x%08x\n", exception_state->regs.eflags);
	printf("EAX:    0x%08x\n", exception_state->regs.eax);
	printf("ECX:    0x%08x\n", exception_state->regs.ecx);
	printf("EDX:    0x%08x\n", exception_state->regs.edx);
	printf("EBX:    0x%08x\n", exception_state->regs.ebx);
	printf("ESP:    0x%08x\n", exception_state->regs.esp);
	printf("EBP:    0x%08x\n", exception_state->regs.ebp);
	printf("ESI:    0x%08x\n", exception_state->regs.esi);
	printf("EDI:    0x%08x\n", exception_state->regs.edi);
	printf("DS:     0x%04x\n", exception_state->regs.ds);
	printf("ES:     0x%04x\n", exception_state->regs.es);
	printf("SS:     0x%04x\n", exception_state->regs.ss);
	printf("FS:     0x%04x\n", exception_state->regs.fs);
	printf("GS:     0x%04x\n", exception_state->regs.gs);
}

void exception_dispatch(void)
{
	u32 vec = exception_state->vector;

	die_if(vec >= ARRAY_SIZE(handlers), "Invalid vector %u\n", vec);

	if (handlers[vec]) {
		handlers[vec](vec);
		if (IS_ENABLED(CONFIG_LP_ENABLE_APIC)
				&& vec >= FIRST_USER_DEFINED_VECTOR)
			apic_eoi();
		return;
	}

	die_if(vec >= EXC_COUNT || !names[vec], "Bad exception vector %u\n",
	       vec);

	if (hook && hook(vec))
		return;

	dump_exception_state();
	dump_stack(exception_state->regs.esp, 512);
	halt();
}

void exception_init(void)
{
	exception_stack_end = exception_stack + ARRAY_SIZE(exception_stack);
	exception_init_asm();
}

void exception_install_hook(exception_hook h)
{
	die_if(hook, "Implement support for a list of hooks if you need it.");
	hook = h;
}

void set_interrupt_handler(u8 vector, interrupt_handler handler)
{
	handlers[vector] = handler;
}

static uint32_t eflags(void)
{
	uint32_t eflags;
	asm volatile(
		"pushf\n\t"
		"pop %0\n\t"
	: "=rm" (eflags));
	return eflags;
}

void enable_interrupts(void)
{
	asm volatile (
		"sti\n"
		: : : "cc"
	);
}
void disable_interrupts(void)
{
	asm volatile (
		"cli\n"
		: : : "cc"
	);
}

int interrupts_enabled(void)
{
	return !!(eflags() & IF_FLAG);
}
