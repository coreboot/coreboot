/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 *
 * Copyright 2013  Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

	.section ".id", "a", %progbits

	.globl __id_start
__id_start:
ver:
	.asciz COREBOOT_VERSION
vendor:
	.asciz CONFIG_MAINBOARD_VENDOR
part:
	.asciz CONFIG_MAINBOARD_PART_NUMBER
.long __id_end - ver  /* Reverse offset to the vendor id */
.long __id_end - vendor  /* Reverse offset to the vendor id */
.long __id_end - part    /* Reverse offset to the part number */
.long CONFIG_ROM_SIZE    /* Size of this romimage */
	.globl __id_end

__id_end:
.previous
