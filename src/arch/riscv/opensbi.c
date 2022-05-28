/* SPDX-License-Identifier: GPL-2.0-only */

/* OpenSBI wants to make its own definitions for some of our compiler.h macros. */
#undef __packed
#undef __noreturn

#include <sbi/fw_dynamic.h>
#include <arch/boot.h>
/* DO NOT INCLUDE COREBOOT HEADERS HERE */

void run_opensbi(const int hart_id,
		 const void *fdt,
		 const void *opensbi,
		 const void *payload,
		 const int payload_mode)
{
	struct fw_dynamic_info info = {
		.magic = FW_DYNAMIC_INFO_MAGIC_VALUE,
		.version = FW_DYNAMIC_INFO_VERSION_MAX,
		.next_mode = payload_mode,
		.next_addr = (uintptr_t)payload,
	};

	csr_write(mepc, opensbi);
	asm volatile (
			"mv	a0, %0\n\t"
			"mv	a1, %1\n\t"
			"mv	a2, %2\n\t"
			"mret" :
			: "r"(hart_id), "r"(fdt), "r"(&info)
			: "a0", "a1", "a2");
}
