/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbmem.h>
#include <symbols.h>
#include <ramdetect.h>
#include <commonlib/device_tree.h>
#include <mcall.h>

uintptr_t cbmem_top_chipset(void)
{
	uint64_t top;

	top = fdt_get_memory_top((void *)HLS()->fdt);
	ASSERT_MSG(top, "Failed reading memory range from FDT");

#if __riscv_xlen == 32
	/* rv32 cannot address memory above 4 GiB; QEMU virt RAM ends there. */
	return (uintptr_t)MIN(top, (uint64_t)4 * GiB - 1);
#else
	/* Report the full FDT memory top so payloads see all installed RAM
	   (e.g. > 4 GiB); the 32-bit MMIO windows sit below QEMU_VIRT_DRAM. */
	return (uintptr_t)top;
#endif
}
