/*
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

#if CONFIG(LP_ARCH_X86_64)
#define REGISTER_FMT "0x%016zx"
#else
#define REGISTER_FMT "0x%08zx"
#endif

u8 exception_stack[0x400] __aligned(16);

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
	printf("REG_IP:    " REGISTER_FMT "\n", exception_state->regs.reg_ip);
	printf("REG_FLAGS: " REGISTER_FMT "\n", exception_state->regs.reg_flags);
	printf("REG_AX:    " REGISTER_FMT "\n", exception_state->regs.reg_ax);
	printf("REG_BX:    " REGISTER_FMT "\n", exception_state->regs.reg_bx);
	printf("REG_CX:    " REGISTER_FMT "\n", exception_state->regs.reg_cx);
	printf("REG_DX:    " REGISTER_FMT "\n", exception_state->regs.reg_dx);
	printf("REG_SP:    " REGISTER_FMT "\n", exception_state->regs.reg_sp);
	printf("REG_BP:    " REGISTER_FMT "\n", exception_state->regs.reg_bp);
	printf("REG_SI:    " REGISTER_FMT "\n", exception_state->regs.reg_si);
	printf("REG_DI:    " REGISTER_FMT "\n", exception_state->regs.reg_di);
#if CONFIG(LP_ARCH_X86_64)
	printf("REG_R8:    0x%016zx\n", exception_state->regs.reg_r8);
	printf("REG_R9:    0x%016zx\n", exception_state->regs.reg_r9);
	printf("REG_R10:   0x%016zx\n", exception_state->regs.reg_r10);
	printf("REG_R11:   0x%016zx\n", exception_state->regs.reg_r11);
	printf("REG_R12:   0x%016zx\n", exception_state->regs.reg_r12);
	printf("REG_R13:   0x%016zx\n", exception_state->regs.reg_r13);
	printf("REG_R14:   0x%016zx\n", exception_state->regs.reg_r14);
	printf("REG_R15:   0x%016zx\n", exception_state->regs.reg_r15);
#endif
	printf("CS:     0x%04x\n", exception_state->regs.cs);
	printf("DS:     0x%04x\n", exception_state->regs.ds);
	printf("ES:     0x%04x\n", exception_state->regs.es);
	printf("SS:     0x%04x\n", exception_state->regs.ss);
	printf("FS:     0x%04x\n", exception_state->regs.fs);
	printf("GS:     0x%04x\n", exception_state->regs.gs);
}

void exception_dispatch(void)
{
	die_if(exception_state->vector >= ARRAY_SIZE(handlers),
	       "Invalid vector %zu\n", exception_state->vector);

	u8 vec = exception_state->vector;

	if (handlers[vec]) {
		handlers[vec](vec);
		goto success;
	} else if (vec >= EXC_COUNT
		   && CONFIG(LP_IGNORE_UNKNOWN_INTERRUPTS)) {
		goto success;
	} else if (vec >= EXC_COUNT
		   && CONFIG(LP_LOG_UNKNOWN_INTERRUPTS)) {
		printf("Ignoring interrupt vector %u\n", vec);
		goto success;
	}

	die_if(vec >= EXC_COUNT || !names[vec], "Bad exception vector %u\n",
	       vec);

	dump_exception_state();
	dump_stack(exception_state->regs.reg_sp, 512);
	/* We don't call apic_eoi because we don't want to ack the interrupt and
	   allow another interrupt to wake the processor. */
	halt();
	return;

success:
	if (CONFIG(LP_ENABLE_APIC))
		apic_eoi(vec);
}

void exception_init(void)
{
	/* TODO: Add exception init code for x64, currently only supporting 32-bit code */
	if (CONFIG(LP_ARCH_X86_64))
		return;

	exception_stack_end = exception_stack + ARRAY_SIZE(exception_stack);
	exception_init_asm();
}

void set_interrupt_handler(u8 vector, interrupt_handler handler)
{
	handlers[vector] = handler;
}

#if CONFIG(LP_ARCH_X86_64)
static uint64_t eflags(void)
{
	uint64_t eflags;
	asm volatile(
		"pushfq\n\t"
		"popq %0\n\t"
	: "=rm" (eflags));
	return eflags;
}
#else
static uint32_t eflags(void)
{
	uint32_t eflags;
	asm volatile(
		"pushf\n\t"
		"pop %0\n\t"
	: "=rm" (eflags));
	return eflags;
}
#endif

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
