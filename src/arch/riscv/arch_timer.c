/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <arch/encoding.h>
#include <console/console.h>
#include <stddef.h>
#include <timer.h>
#include <mcall.h>

void timer_monotonic_get(struct mono_time *mt)
{
	if (HLS()->time == NULL)
		die("time not set in HLS");
	mono_time_set_usecs(mt, (long)read64((void *)(HLS()->time)));
}
