/* SPDX-License-Identifier: GPL-2.0-only */
#include <arch/breakpoint.h>
#include <arch/null_breakpoint.h>
#include <bootstate.h>
#include <console/console.h>
#include <stdint.h>

static struct breakpoint_handle null_deref_bp;
static struct breakpoint_handle null_fetch_bp;

static int handle_fetch_breakpoint(struct breakpoint_handle handle, struct eregs *regs)
{
	printk(BIOS_ERR, "Instruction fetch from address zero\n");
	return CONFIG(DEBUG_NULL_DEREF_HALT);
}

static int handle_deref_breakpoint(struct breakpoint_handle handle, struct eregs *regs)
{
#if ENV_X86_64
	printk(BIOS_ERR, "Null dereference at rip: 0x%llx\n", regs->rip);
#else
	printk(BIOS_ERR, "Null dereference at eip: 0x%x\n", regs->eip);
#endif
	return CONFIG(DEBUG_NULL_DEREF_HALT);
}

static void create_deref_breakpoint(void)
{
	enum breakpoint_result res =
		breakpoint_create_data(&null_deref_bp, NULL, sizeof(uintptr_t), false);

	if (res != BREAKPOINT_RES_OK) {
		printk(BIOS_ERR, "Failed to create NULL dereference breakpoint\n");
		return;
	}

	breakpoint_set_handler(null_deref_bp, &handle_deref_breakpoint);
	breakpoint_enable(null_deref_bp, true);
}

static void create_instruction_breakpoint(void)
{
	enum breakpoint_result res = breakpoint_create_instruction(&null_fetch_bp, NULL);

	if (res != BREAKPOINT_RES_OK) {
		printk(BIOS_ERR, "Failed to create address zero instruction fetch breakpoint\n");
		return;
	}

	breakpoint_set_handler(null_fetch_bp, &handle_fetch_breakpoint);
	breakpoint_enable(null_fetch_bp, true);
}

void null_breakpoint_init(void)
{
	create_deref_breakpoint();
	create_instruction_breakpoint();
}

static void null_breakpoint_disable(void *unused)
{
	breakpoint_remove(null_fetch_bp);
	breakpoint_remove(null_deref_bp);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, null_breakpoint_disable, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, null_breakpoint_disable, NULL);
