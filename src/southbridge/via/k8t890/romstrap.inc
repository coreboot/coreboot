/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer)
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

/* This file constructs the ROM strap table for K8T890 and K8M890 */

.section ".romstrap", "a", @progbits

.globl __romstrap_start
.globl __romstrap_end

__romstrap_start:

/*
 * Below are some Dev0 Func2 HT control registers values,
 * depending on strap pin, one of below lines is used.
 */
#if CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8M800 || CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8T800 || CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8T800_OLD

tblpointer:
.long 0x50220000, 0X619707C2
.long 0x50220000, 0X619707C2
.long 0x50220000, 0X619707C2
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0

#elif CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8M890

tblpointer:
.long 0x504400FF, 0x61970FC2	//;200M
.long 0x504400FF, 0x61970FC2	//;400M
.long 0x504400FF, 0x61970FC2	//;600M
.long 0x504400FF, 0x61970FC2	//;800M
.long 0x504400FF, 0x61970FC2	//;1000M
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0


#elif CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8T890

tblpointer:
.long 0x504400AA, 0x61970FC2	//;200M
.long 0x504400AA, 0x61970FC2	//;400M
.long 0x504400AA, 0x61970FC2	//;600M
.long 0x504400AA, 0x61970FC2	//;800M
.long 0x504400AA, 0x61970FC2	//;1000M
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0
.long 0x0

#endif
/*
 * The pointer to above table should be at 0xffffd,
 * the table itself MUST be aligned to 128B it seems!
 */
.long tblpointer - 0xFFF00000

__romstrap_end:

.previous
