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

#include <exception.h>
#include <libpayload.h>
#include <stdint.h>

uint8_t exception_stack[0x1000] __attribute__((aligned(8)));
extern void *exception_stack_end;

struct exception_state
{
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 eip;
	u32 eflags;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 es;
	u32 fs;
	u32 gs;
	u32 error_code;
	u32 vector;
} __attribute__((packed));

struct exception_info
{
	const char *name;
	void (*error_code_printer)(u32 code);
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

static struct exception_info exceptions[] = {
	[0] =  { .name = "divide by zero" },
	[1] =  { .name = "debug" },
	[2] =  { .name = "non-maskable-interrupt" },
	[3] =  { .name = "breakpoint" },
	[4] =  { .name = "overflow" },
	[5] =  { .name = "bound range" },
	[6] =  { .name = "invalid opcode" },
	[7] =  { .name = "device not available" },
	[8] =  { .name = "double fault",
		 .error_code_printer = &print_raw_error_code },
	[10] = { .name = "invalid tss",
		 .error_code_printer = &print_segment_error_code },
	[11] = { .name = "segment not present",
		 .error_code_printer = &print_segment_error_code },
	[12] = { .name = "stack",
		 .error_code_printer = &print_segment_error_code },
	[13] = { .name = "general protection",
		 .error_code_printer = &print_segment_error_code },
	[14] = { .name = "page fault",
		 .error_code_printer = &print_page_fault_error_code },
	[16] = { .name = "x87 floating point" },
	[17] = { .name = "alignment check",
		 .error_code_printer = &print_raw_error_code },
	[18] = { .name = "machine check" },
	[19] = { .name = "SIMD floating point" },
	[30] = { .name = "security",
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

void exception_handler(void);
void exception_handler(void)
{
	struct exception_state *state =
		(void *)((u8 *)exception_stack_end - sizeof(*state));

	struct exception_info *info = NULL;
	if (state->vector < ARRAY_SIZE(exceptions))
		info = &exceptions[state->vector];

	if (info)
		printf("Exception %d (%s)\n", state->vector, info->name);
	else
		printf("Unrecognized exception %d\n", state->vector);
	if (info->error_code_printer) {
		printf("Error code: ");
		info->error_code_printer(state->error_code);
		printf("\n");
	}
	printf("EIP:    0x%08x\n", state->eip);
	printf("CS:     0x%04x\n", state->cs);
	printf("EFLAGS: 0x%08x\n", state->eflags);
	printf("EAX:    0x%08x\n", state->eax);
	printf("ECX:    0x%08x\n", state->ecx);
	printf("EDX:    0x%08x\n", state->edx);
	printf("EBX:    0x%08x\n", state->ebx);
	printf("ESP:    0x%08x\n", state->esp);
	printf("EBP:    0x%08x\n", state->ebp);
	printf("ESI:    0x%08x\n", state->esi);
	printf("EDI:    0x%08x\n", state->edi);
	printf("DS:     0x%04x\n", state->ds);
	printf("ES:     0x%04x\n", state->es);
	printf("SS:     0x%04x\n", state->ss);
	printf("FS:     0x%04x\n", state->fs);
	printf("GS:     0x%04x\n", state->gs);

	dump_stack(state->esp, 512);

	halt();
}

void exception_init_asm(void);
void exception_init(void)
{
	exception_stack_end = exception_stack + sizeof(exception_stack);
	exception_init_asm();
}
