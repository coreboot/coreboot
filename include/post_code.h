/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef POST_CODE_H
#define POST_CODE_H

#include <types.h>
#include <shared.h>

SHARED(post_code, void, u8 value);

/* This is a collection of existing POST values used by post_code().
 * port80_post() and Geode specific codes are not (yet?) listed here.
 * FIXME: Conflicts remain.
 */
#define POST_START_OF_MAIN		0x01
#define POST_KBD_EMPTY_INPUT_BUFFER	0x00
#define POST_KBD_EMPTY_OUTPUT_BUFFER	0x00
#define POST_STAGE1_STOP_AP		0xf0
#define POST_STAGE1_ENABLE_ROM		0xf2
#define POST_STAGE1_MAIN		0x02
#define POST_STAGE2_BEGIN		0x20
#define POST_STAGE2_PHASE1_START	0x30
#define POST_STAGE2_PHASE1_ENTER	0x31
#define POST_STAGE2_PHASE1_DONE		0x3e
#define POST_STAGE2_PHASE1_EXIT		0x3f
#define POST_STAGE2_PHASE2_PCI_SET_METHOD	0x5f
#define POST_STAGE2_PHASE2_START	0x40
#define POST_STAGE2_PHASE2_ENTER	0x41
#define POST_STAGE2_PHASE2_DONE		0x4e
#define POST_STAGE2_PHASE2_EXIT		0x4f
#define POST_STAGE2_PHASE3_START	0x30
#define POST_STAGE2_PHASE3_MIDDLE	0x41
#define POST_STAGE2_PHASE3_SCAN_ENTER	0x42
#define POST_STAGE2_PHASE3_SCAN_EXIT	0x4e
#define POST_STAGE2_PHASE4_START	0x40
#define POST_STAGE2_PHASE5_START	0x50
#define POST_STAGE2_PHASE6_START	0x60
#define POST_STAGE2_WRITE_TABLES	0x70
#define POST_STAGE2_ARCH_WRITE_TABLES_ENTER	0x9a
#define POST_STAGE2_ARCH_WRITE_TABLES_MIDDLE	0x96
#define POST_STAGE2_PCISCANBUS_ENTER		0x24
#define POST_STAGE2_PCISCANBUS_DONEFORLOOP	0x25
#define POST_STAGE2_PCISCANBUS_EXIT		0x55
#define POST_ELFBOOT_JUMPING_TO_BOOTCODE	0xfe
#define POST_ELFBOOT_LOADER_STARTED		0xf8
#define POST_ELFBOOT_LOADER_IMAGE_FAILED	0xff

#endif /* POST_CODE_H */
