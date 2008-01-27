/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef LIB_H
#define LIB_H

/**
 * Return the size of a given array, no matter of which data type
 * the individual array elements are.
 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

int log2(unsigned int n);

void udelay(unsigned int usecs);
void mdelay(unsigned int msecs);
void delay(unsigned int secs);

void beep_short(void);
void beep_long(void);

/* smbus functions */
int smbus_read_byte(unsigned device, unsigned address);

/* dram functions */
void ram_failure(const char *why);
void ram_initialize(int controllers, void *ctrl);

#endif /* LIB_H */
