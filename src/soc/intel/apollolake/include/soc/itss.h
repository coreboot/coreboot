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

#ifndef _SOC_APOLLOLAKE_ITSS_H_
#define _SOC_APOLLOLAKE_ITSS_H_

/* Set the interrupt polarity for provided IRQ to the APIC. */
void itss_set_irq_polarity(int irq, int active_low);

#endif /* _SOC_APOLLOLAKE_ITSS_H_ */
