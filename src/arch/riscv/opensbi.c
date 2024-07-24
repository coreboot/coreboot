/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/boot.h>
#include <arch/encoding.h>
#include <stdint.h>
#include <stddef.h>

#define FW_DYNAMIC_INFO_VERSION_2    2
#define FW_DYNAMIC_INFO_MAGIC_VALUE  0x4942534f // "OSBI"

/*
 * structure passed to OpenSBI as 3rd argument
 * NOTE: This structure may need to be updated when the OpenSBI submodule is updated.
 */
static struct __packed fw_dynamic_info {
	unsigned long magic;     // magic value "OSBI"
	unsigned long version;   // version number (2)
	unsigned long next_addr; // Next booting stage address (payload address)
	unsigned long next_mode; // Next booting stage mode (usually supervisor mode)
	unsigned long options;   // options for OpenSBI library
	unsigned long boot_hart; // usually CONFIG_RISCV_WORKING_HARTID
} info;

void run_opensbi(const int hart_id,
		 const void *fdt,
		 const void *opensbi,
		 const void *payload,
		 const int payload_mode)
{
	info.magic = FW_DYNAMIC_INFO_MAGIC_VALUE,
	info.version = FW_DYNAMIC_INFO_VERSION_2,
	info.next_mode = payload_mode,
	info.next_addr = (uintptr_t)payload,
	info.options = 0,
	info.boot_hart = CONFIG_OPENSBI_FW_DYNAMIC_BOOT_HART,

	write_csr(mepc, opensbi); // set program counter to OpenSBI (jumped to with mret)
	asm volatile (
		"mv  a0, %0\n\t"
		"mv  a1, %1\n\t"
		"mv  a2, %2\n\t"
		"mret"
		:
		: "r"(hart_id), "r"(fdt), "r"(&info)
		: "a0", "a1", "a2"
	);
}
