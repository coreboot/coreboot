/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
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

#ifndef __ARCH_REGISTERS_H
#define __ARCH_REGISTERS_H

#define __PACKED __attribute__((packed))

#define DOWNTO8(A) \
	union { \
		struct { \
			union { \
				struct { \
					uint8_t A##l; \
					uint8_t A##h; \
				} __PACKED; \
				uint16_t A##x; \
			} __PACKED; \
			uint16_t h##A##x; \
		} __PACKED; \
		uint32_t e##A##x; \
	} __PACKED;

#define DOWNTO16(A) \
	union { \
		struct { \
			uint16_t A; \
			uint16_t h##A; \
		} __PACKED; \
		uint32_t e##A; \
	} __PACKED;

struct eregs {
	DOWNTO8(a);
	DOWNTO8(c);
	DOWNTO8(d);
	DOWNTO8(b);
	DOWNTO16(sp);
	DOWNTO16(bp);
	DOWNTO16(si);
	DOWNTO16(di);
	uint32_t vector;
	uint32_t error_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
};

#endif
