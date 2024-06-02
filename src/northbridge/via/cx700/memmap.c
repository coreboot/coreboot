/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <commonlib/bsd/helpers.h>
#include <stddef.h>
#include <stdint.h>
#include <device/pci_ops.h>
#include <static_devices.h>
#include <arch/romstage.h>
#include <cbmem.h>

static uintptr_t top_of_low_ram(void)
{
	return (uintptr_t)pci_read_config8(_sdev_dram_ctrl, 0x88) << 24;
}

static size_t get_stolen_framebuffer_size(void)
{
	const size_t size_selection = pci_read_config8(_sdev_dram_ctrl, 0xa1) >> 4 & 7;

	return size_selection ? 1 << (size_selection + 22) : 0;
}

static size_t get_tseg_size(void)
{
	return pci_read_config8(_sdev_dram_ctrl, 0x86) & 0x04 ? 1*MiB : 0;
}

uintptr_t cbmem_top_chipset(void)
{
	/* Keep it below 2GiB to leave space for PCI MMIO.
	   If more than 2GiB are installed, CBMEM won't be
	   at the top of the available memory. */
	return MIN(2ul*GiB, top_of_low_ram() - get_stolen_framebuffer_size() - get_tseg_size());
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	/* TSEG top is at least 8MiB aligned, so
	   we cache 8MiB including TSEG if enabled. */
	const uintptr_t top_down = ALIGN_DOWN(cbmem_top_chipset() - 7*MiB, 8*MiB);
	postcar_frame_add_mtrr(pcf, top_down, 8*MiB, MTRR_TYPE_WRBACK);
}
