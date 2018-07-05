/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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
#include <console/console.h>
#include <thread.h>

/* The stack frame looks like the following. */
struct pushed_regs {
	u32 r4;
	u32 r5;
	u32 r6;
	u32 r7;
	u32 r8;
	u32 r9;
	u32 r10;
	u32 r11;
	u32 lr;
};

static inline uintptr_t push_stack(uintptr_t cur_stack, uintptr_t value)
{
	uintptr_t *addr;

	cur_stack -= sizeof(value);
	addr = (uintptr_t *)cur_stack;
	*addr = value;
	return cur_stack;
}

void arch_prepare_thread(struct thread *t,
			 void asmlinkage(*thread_entry)(void *), void *arg)
{
	uintptr_t stack = t->stack_current;
	int i;
	uintptr_t poison = 0xdeadbeef;

	/* Push the LR. thread_entry()
	 * is assumed to never return.
	 */
	stack = push_stack(stack, (uintptr_t)thread_entry);
	/* Make room for the registers.
	 * Poison the initial stack. This is good hygiene and finds bugs.
	 * Poisoning the stack with different values helps when you're
	 * hunting for (e.g.) misaligned stacks or other such
	 * weirdness. The -1 is because we already pushed lr.
	 */
	for (i = 0; i < sizeof(struct pushed_regs) / sizeof(u32) - 1; i++)
		stack = push_stack(stack, poison++);

	t->stack_current = stack;
}

/* We could write this as a .S and the first time around that's how we
 * did it. But there's always the question of matching our ARM
 * directives in the .S with how gcc is doing things.  It seems best
 * to follow the pattern of the rest of the ARM port and just use
 * inline assembly and let gcc get all the ELF magic right.
 */
void __attribute__((naked))
switch_to_thread(uintptr_t new_stack, uintptr_t *saved_stack)
{
	/* Defintions for those of us not totally familiar with ARM:
	 * R15 -- PC, R14 -- LR, R13 -- SP
	 * R0-R3 need not be saved, nor R12.
	 * on entry, the only saved state is in LR -- the old PC.
	 * The args are in R0,R1.
	 * R0 is the new stack
	 * R1 is a pointer to the old stack save location
	 * Push R4-R11 and LR
	 * then switch stacks
	 * then pop R0-R12 and LR
	 * then mov PC,LR
	 *
	 * stack layout
	 * +------------+
	 * |    LR      | <-- sp + 0x20
	 * +------------+
	 * |    R11     | <-- sp + 0x1c
	 * +------------+
	 * |    R10     | <-- sp + 0x18
	 * +------------+
	 * |    R9      | <-- sp + 0x14
	 * +------------+
	 * |    R8      | <-- sp + 0x10
	 * +------------+
	 * |    R7      | <-- sp + 0x0c
	 * +------------+
	 * |    R6      | <-- sp + 0x08
	 * +------------+
	 * |    R5      | <-- sp + 0x04
	 * +------------+
	 * |    R4      | <-- sp + 0x00
	 * +------------+
	 */
	asm volatile (
	/* save context. */
	"push {r4-r11,lr}\n\t"
	/* Save the current stack */
	"str sp,[r1]\n\t"
	/* switch to the new stack */
	"mov sp,r0\n\t"
	/* restore the registers */
	"pop {r4-r11,lr}\n\t"
	/* resume other thread. */
	"mov pc,lr\n\t"
	);
}

void *arch_get_thread_stackbase(void)
{
	return (void *)CONFIG_STACK_BOTTOM;
}
