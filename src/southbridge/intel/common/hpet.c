/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/common/rcba.h>
#include <stdint.h>

#include "hpet.h"

#define HPTC		0x3404

#define HPET_BASE 0xfed00000
#define HPET32(x) (*((volatile u32 *)(HPET_BASE + (x))))

void enable_hpet(void)
{
	u32 reg32;
	reg32 = RCBA32(HPTC);
	reg32 &= ~0x03;
	reg32 |= (1 << 7);
	RCBA32(HPTC) = reg32;
	/* Read back for posted write to take effect */
	RCBA32(HPTC);
	HPET32(0x10) = HPET32(0x10) | 1;
}

void hpet_udelay(u32 delay)
{
	u32 start, finish, now;

	delay *= 15; /* now in usec */

	start = HPET32(0xf0);
	finish = start + delay;
	while (1) {
		now = HPET32(0xf0);
		if (finish > start) {
			if (now >= finish)
				break;
		} else {
			if ((now < start) && (now >= finish))
				break;
		}
	}
}
