/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <stdint.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <timer.h>

static struct monotonic_counter {
	int initialized;
	struct mono_time time;
	uint32_t last_value;
} mono_counter;

static inline uint32_t read_counter_msr(void)
{
	/* D0F0xE4_x0130_80F0 BIOS Timer
	 *
	 * This field increments once every microsecond when the timer is
	 * enabled. The counter rolls over and continues counting when it
	 * reaches FFFF_FFFFh. A write to this register causes the counter
	 * to reset and begin counting from the value written. */
	pci_write_config32(PCI_DEV(0, 0, 0), 0xe4, 0x013080F0);

	return pci_read_config32(PCI_DEV(0, 0, 0), 0xe4);
}

void timer_monotonic_get(struct mono_time *mt)
{
	uint32_t current_tick;
	uint32_t usecs_elapsed;

	if (!mono_counter.initialized) {
		mono_counter.last_value = read_counter_msr();
		mono_counter.initialized = 1;
	}

	current_tick = read_counter_msr();
	usecs_elapsed = current_tick - mono_counter.last_value;

	/* Update current time and tick values only if a full tick occurred. */
	if (usecs_elapsed) {
		mono_time_add_usecs(&mono_counter.time, usecs_elapsed);
		mono_counter.last_value = current_tick;
	}

	/* Save result. */
	*mt = mono_counter.time;
}
