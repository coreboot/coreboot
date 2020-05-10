/* SPDX-License-Identifier: GPL-2.0-only */

#include <mcall.h>
#include <device/mmio.h>

#define SPIKE_CLINT_BASE	0x02000000

/* This function is used to initialize HLS()->time/HLS()->timecmp  */
void mtime_init(void)
{
	long hart_id = read_csr(mhartid);
	HLS()->time = (uint64_t *)(SPIKE_CLINT_BASE + 0xbff8);
	HLS()->timecmp = (uint64_t *)(SPIKE_CLINT_BASE + 0x4000 + 8 * hart_id);
}

void set_msip(int hartid, int val)
{
	write32((void *)(SPIKE_CLINT_BASE + 4 * (uintptr_t)hartid), !!val);
}
