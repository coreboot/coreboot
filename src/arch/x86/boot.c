/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <mode_switch.h>
#include <program_loading.h>
#include <symbols.h>
#include <assert.h>

int payload_arch_usable_ram_quirk(uint64_t start, uint64_t size)
{
	if (start < 1 * MiB && (start + size) <= 1 * MiB) {
		printk(BIOS_DEBUG,
			"Payload being loaded at below 1MiB without region being marked as RAM usable.\n");
		return 1;
	}

	return 0;
}

void arch_prog_run(struct prog *prog)
{
#if ENV_RAMSTAGE && ENV_X86_64
	const uint32_t arg = pointer_to_uint32_safe(prog_entry_arg(prog));
	const uint32_t entry = pointer_to_uint32_safe(prog_entry(prog));

	/* On x86 coreboot payloads expect to be called in protected mode */
	protected_mode_call_1arg((void *)(uintptr_t)entry, arg);
#else
#if ENV_X86_64
	void (*doit)(void *arg);
#else
	/* Ensure the argument is pushed on the stack. */
	asmlinkage void (*doit)(void *arg);
#endif
	doit = prog_entry(prog);
	doit(prog_entry_arg(prog));
#endif
}
