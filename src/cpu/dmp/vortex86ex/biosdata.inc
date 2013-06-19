/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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

	.section ".dmp_reserved", "a", @progbits

        .skip 0x3c000 - 0x3bc00, 0xff

.previous

	.section ".dmp_kbd_fw_part2", "a", @progbits

        .skip 0x3d000 - 0x3c000, 0xff

.previous

	.section ".dmp_mtbf_low_cnt", "a", @progbits

        .skip 0x3e000 - 0x3d000, 0xff

.previous

	.section ".dmp_kbd_fw_part1", "a", @progbits

	#include "src/cpu/dmp/vortex86ex/dmp_kbd_fw_part1.inc"

.previous

	.section ".dmp_spi_flash_disk_driver", "a", @progbits

        .skip 0x3f800 - 0x3f000, 0xff

.previous

	.section ".dmp_frontdoor", "a", @progbits

        .skip 0x3fd00 - 0x3f800, 0xff

.previous

	.section ".dmp_isoinfo", "a", @progbits

        .skip 26 * 16, 0xff

.previous

	.section ".dmp_isodata_checksum", "a", @progbits

        .skip 8, 0xff

.previous

	.section ".dmp_mac", "a", @progbits

        .skip 6, 0xff

.previous

	.section ".dmp_mtbf_limit", "a", @progbits

        .skip 3, 0xff

.previous

	.section ".dmp_isodata", "a", @progbits

        .skip 32, 0xff

.previous
