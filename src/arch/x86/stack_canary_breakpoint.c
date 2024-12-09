/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/breakpoint.h>
#include <arch/stack_canary_breakpoint.h>
#include <arch/symbols.h>
#include <bootstate.h>
#include <console/console.h>
#include <types.h>
#include <symbols.h>

static struct breakpoint_handle stack_canary_bp;

static int handle_stack_canary_written(struct breakpoint_handle handle, struct eregs *regs)
{
#if ENV_X86_64
	printk(BIOS_ERR, "Stack corruption detected at rip: 0x%llx\n", regs->rip);
#else
	printk(BIOS_ERR, "Stack corruption detected at eip: 0x%x\n", regs->eip);
#endif
	return 0;
}

static void create_stack_canary_breakpoint(uintptr_t *addr)
{
	enum breakpoint_result res =
		breakpoint_create_data(&stack_canary_bp, addr, sizeof(uintptr_t), true);

	if (res != BREAKPOINT_RES_OK) {
		printk(BIOS_ERR, "Failed to create stack canary breakpoint\n");
		return;
	}

	breakpoint_set_handler(stack_canary_bp, &handle_stack_canary_written);
	breakpoint_enable(stack_canary_bp, true);
}

void stack_canary_breakpoint_init(void)
{
	uintptr_t *addr;

	if (CONFIG(RESET_VECTOR_IN_RAM)) {
		addr = (uintptr_t *)_earlyram_stack;
	} else if (ENV_CACHE_AS_RAM) {
		addr = (uintptr_t *)_car_stack;
	} else {
		addr = (uintptr_t *)_stack;
	}

	create_stack_canary_breakpoint(addr);
}

void stack_canary_breakpoint_remove(void)
{
	breakpoint_remove(stack_canary_bp);
}

static void stack_canary_breakpoint_remove_hook(void *unused)
{
	stack_canary_breakpoint_remove();
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, stack_canary_breakpoint_remove_hook, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, stack_canary_breakpoint_remove_hook, NULL);
