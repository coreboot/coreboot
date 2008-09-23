/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
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

#include <string.h>
#include <stdint.h>
#include <arch/smp/mpspec.h>
#include <../../../southbridge/via/vt8237r/vt8237r.h>
#include <../../../southbridge/via/k8t890/k8t890.h>

unsigned long write_smp_table(unsigned long addr)
{
	/* place holder for a table, need to have it defined because
	of MP_ used by ACPI */

	return addr;
}
