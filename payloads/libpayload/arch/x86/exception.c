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

uint8_t exception_stack[0x1000] __attribute__((aligned(8)));
extern void *exception_stack_end;
extern struct exception_handler_state *exception_handler_state_handoff;

struct exception_handler_state
{
	struct exception_state regs;
	u32 error_code;
	u32 vector;
} __attribute__((packed));

struct exception_handler_info
{
	const char *name;
	void (*error_code_printer)(u32 code);
	exception_hook hook;
};

static void print_segment_error_code(u32 code)
{
	printf("%#x - descriptor %#x in the ", code, (code >> 3) & 0x1f);
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

static struct exception_handler_info exceptions[EXC_COUNT] = {
	[EXC_DE] =  { .name = "divide by zero" },
	[EXC_DB] =  { .name = "debug" },
	[EXC_NMI] =  { .name = "non-maskable-interrupt" },
	[EXC_BP] =  { .name = "breakpoint" },
	[EXC_OF] =  { .name = "overflow" },
	[EXC_BR] =  { .name = "bound range" },
	[EXC_UD] =  { .name = "invalid opcode" },
	[EXC_NM] =  { .name = "device not available" },
	[EXC_DF] =  { .name = "double fault",
		      .error_code_printer = &print_raw_error_code },
	[EXC_TS] = { .name = "invalid tss",
		     .error_code_printer = &print_segment_error_code },
	[EXC_NP] = { .name = "segment not present",
		     .error_code_printer = &print_segment_error_code },
	[EXC_SS] = { .name = "stack",
		     .error_code_printer = &print_segment_error_code },
	[EXC_GP] = { .name = "general protection",
		     .error_code_printer = &print_segment_error_code },
	[EXC_PF] = { .name = "page fault",
		     .error_code_printer = &print_page_fault_error_code },
	[EXC_MF] = { .name = "x87 floating point" },
	[EXC_AC] = { .name = "alignment check",
		     .error_code_printer = &print_raw_error_code },
	[EXC_MC] = { .name = "machine check" },
	[EXC_XF] = { .name = "SIMD floating point" },
	[EXC_SX] = { .name = "security",
		     .error_code_printer = &print_raw_error_code },
};

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

static void dump_exception_state(struct exception_handler_state *state,
				 struct exception_handler_info *info)
{
	if (info)
		printf("Exception %d (%s)\n", state->vector, info->name);
	else
		printf("Unrecognized exception %d\n", state->vector);
	if (info->error_code_printer) {
		printf("Error code: ");
		info->error_code_printer(state->error_code);
		printf("\n");
	}
	printf("EIP:    0x%08x\n", state->regs.eip);
	printf("CS:     0x%04x\n", state->regs.cs);
	printf("EFLAGS: 0x%08x\n", state->regs.eflags);
	printf("EAX:    0x%08x\n", state->regs.eax);
	printf("ECX:    0x%08x\n", state->regs.ecx);
	printf("EDX:    0x%08x\n", state->regs.edx);
	printf("EBX:    0x%08x\n", state->regs.ebx);
	printf("ESP:    0x%08x\n", state->regs.esp);
	printf("EBP:    0x%08x\n", state->regs.ebp);
	printf("ESI:    0x%08x\n", state->regs.esi);
	printf("EDI:    0x%08x\n", state->regs.edi);
	printf("DS:     0x%04x\n", state->regs.ds);
	printf("ES:     0x%04x\n", state->regs.es);
	printf("SS:     0x%04x\n", state->regs.ss);
	printf("FS:     0x%04x\n", state->regs.fs);
	printf("GS:     0x%04x\n", state->regs.gs);
}

void exception_dispatch(void)
{
	struct exception_handler_state *state =
		exception_handler_state_handoff;

	struct exception_handler_info *info = NULL;
	if (state->vector < EXC_COUNT)
		info = &exceptions[state->vector];

	if (info && info->hook) {
		info->hook(state->vector, &state->regs);
	} else {
		dump_exception_state(state, info);
		dump_stack(state->regs.esp, 512);
		halt();
	}
}

void exception_init(void)
{
	exception_stack_end = exception_stack + sizeof(exception_stack);
	exception_init_asm();
}

void exception_install_hook(int type, exception_hook hook)
{
	die_if(type >= EXC_COUNT, "Out of bound exception type %d.\n", type);
	exceptions[type].hook = hook;
}
