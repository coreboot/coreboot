/* SPDX-License-Identifier: BSD-3-Clause */

#include <arch/cpu.h>
#include <commonlib/helpers.h>

/* Return the CPU struct which is at the high memory address of the stack.
 */
struct cpu_info *cpu_info(void)
{
#error "This is BROKEN! ARM stacks are currently not guaranteed to be " \
	"STACK_SIZE-aligned in any way. If you ever plan to revive this " \
	"feature, make sure you add the proper assertions " \
	"(and maybe consider revising the whole thing to work closer to what " \
	"arm64 is doing now)."
	uintptr_t addr = ALIGN_UP((uintptr_t)__builtin_frame_address(0),
		CONFIG_STACK_SIZE);
	addr -= sizeof(struct cpu_info);
	return (void *)addr;
}
