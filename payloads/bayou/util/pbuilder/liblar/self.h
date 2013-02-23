/*
 * This file is part of the bayou project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#ifndef SELF_H_
#define SELF_H_

struct self_segment {
	u32 type;
	u32 offset;
	u64 load_addr;
	u32 len;
	u32 mem_len;
};

#define SELF_TYPE_CODE   0x45444F43
#define SELF_TYPE_DATA   0x41544144
#define SELF_TYPE_BSS    0x20535342
#define SELF_TYPE_PARAMS 0x41524150
#define SELF_TYPE_ENTRY  0x52544E45

int elf_to_self(const char *filename, unsigned char **buffer,
		void (*compress) (char *, int, char *, int *));

int iself(char *filename);

#endif
