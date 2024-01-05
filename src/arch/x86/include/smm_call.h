/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <cpu/x86/smm.h>

/*
 * Call the APMC SMI handler that resides in SMM. First, the command and sub-command are stored
 * in eax, and the argument pointer is stored in ebx, then the command byte is written to the
 * APMC IO port to trigger the SMI. The APMC SMI handler then reads the command from the APMC
 * IO port and the contents of eax and ebx from the SMM state save area.
 *
 * static inline because the resulting assembly is often smaller than
 * the call sequence due to constant folding.
 */
static inline u32 call_smm(u8 cmd, u8 subcmd, void *arg)
{
	const uint16_t apmc_port = pm_acpi_smi_cmd_port();
	u32 res = 0;
	__asm__ __volatile__ (
		"outb %%al, %%dx"
		: "=a" (res)
		: "a" ((subcmd << 8) | cmd),
		  "b" (arg),
		  "d" (apmc_port)
		: "memory");
	return res;
}
