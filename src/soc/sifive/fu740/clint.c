/* SPDX-License-Identifier: GPL-2.0-only */

#include <mcall.h>
#include <stdint.h>
#include <device/mmio.h>
#include <soc/addressmap.h>

#define CLINT_MTIME 0xBFF8
#define CLINT_MTIMECMP 0x4000

void mtime_init(void)
{
	long hart_id = read_csr(mhartid);
	HLS()->time = (uint64_t *)(FU740_CLINT + CLINT_MTIME);
	HLS()->timecmp = (uint64_t *)(FU740_CLINT + CLINT_MTIMECMP + 8 * hart_id);
}

void set_msip(int hartid, int val)
{
	write32((void *)(FU740_CLINT + 4 * (uintptr_t)hartid), !!val);
}
