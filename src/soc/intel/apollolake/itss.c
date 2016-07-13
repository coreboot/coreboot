/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <soc/iosf.h>
#include <soc/itss.h>

#define IOSF_ITSS_PORT_ID	0xd0
#define ITSS_MAX_IRQ		119
#define IPC0			0x3200
#define IRQS_PER_IPC		32

void itss_set_irq_polarity(int irq, int active_low)
{
	uint32_t mask;
	uint32_t val;
	uint16_t reg;
	const uint16_t port = IOSF_ITSS_PORT_ID;

	if (irq < 0 || irq > ITSS_MAX_IRQ)
		return;

	reg = IPC0 + sizeof(uint32_t) * (irq / IRQS_PER_IPC);
	mask = 1 << (irq % IRQS_PER_IPC);

	val = iosf_read(port, reg);
	val &= ~mask;
	/* Setting the bit makes the IRQ active low. */
	val |= active_low ? mask : 0;
	iosf_write(port, reg, val);
}
