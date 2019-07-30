/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * Early initialization code for POWER8.
 */

.section ".text._start", "ax", %progbits
.globl _start
.org 0x100, 0xff
_start:
	b _start
	.section ".id", "a", %progbits

	.section ".id", "a", @progbits

	.globl __id_start
__id_start:
ver:
	.asciz "4" //COREBOOT_VERSION
vendor:
	.asciz "qemu" //CONFIG_MAINBOARD_VENDOR
part:
	.asciz "1" //CONFIG_MAINBOARD_PART_NUMBER
	/* Reverse offset to the vendor id */
.long __id_end + CONFIG_ID_SECTION_OFFSET - ver
	/* Reverse offset to the vendor id */
.long __id_end + CONFIG_ID_SECTION_OFFSET - vendor
	/* Reverse offset to the part number */
.long __id_end + CONFIG_ID_SECTION_OFFSET - part
	/* of this romimage */
.long CONFIG_ROM_SIZE
	.globl __id_end

__id_end:
.previous
