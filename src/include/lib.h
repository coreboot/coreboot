/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009, Myles Watson <mylesgw@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

/* This file is for "nuisance prototypes" that have no other home. */

/* Defined in src/lib/clog2.c */
unsigned long log2(unsigned long x);

/* Defined in src/lib/lzma.c */
unsigned long ulzma(unsigned char *src, unsigned char *dst);

/* Defined in src/arch/i386/boot/gdt.c */
void move_gdt(void);

