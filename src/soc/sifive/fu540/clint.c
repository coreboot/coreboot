/* SPDX-License-Identifier: GPL-2.0-only */

#include <mcall.h>
#include <stdint.h>
#include <device/mmio.h>
#include <soc/addressmap.h>

void mtime_init(void)
{
	long hart_id = read_csr(mhartid);
	HLS()->time = (uint64_t *)(FU540_CLINT + 0xbff8);
	HLS()->timecmp = (uint64_t *)(FU540_CLINT + 0x4000 + 8 * hart_id);
}

void set_msip(int hartid, int val)
{
	write32((void *)(FU540_CLINT + 4 * (uintptr_t)hartid), !!val);
}
