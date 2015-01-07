/*
 * This file is part of the coreboot project.
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
 * Foundation, Inc.
 */

#ifndef CPU_X86_GDT
#define CPU_X86_GDT

/* These symbols are defined in c_start.S. */
extern char gdt[];
extern char gdt_end[];
extern char idtarg[];

/* These symbols are defined in secondary.S. */
extern char _secondary_gdt_addr[];
extern char _secondary_start[];
extern char _secondary_start_end[];

#endif /* CPU_X86_GDT */
