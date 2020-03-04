/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __ARCH_REGISTERS_H
#define __ARCH_REGISTERS_H

#if !defined(__ASSEMBLER__)
#include <stdint.h>

#define DOWNTO8(A) \
	union { \
		struct { \
			union { \
				struct { \
					uint8_t A##l; \
					uint8_t A##h; \
				} __packed; \
				uint16_t A##x; \
			} __packed; \
			uint16_t h##A##x; \
		} __packed; \
		uint32_t e##A##x; \
	} __packed;

#define DOWNTO16(A) \
	union { \
		struct { \
			uint16_t A; \
			uint16_t h##A; \
		} __packed; \
		uint32_t e##A; \
	} __packed;

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
#endif // !ASSEMBLER

#if CONFIG(COMPILER_LLVM_CLANG)
#define ADDR32(opcode) opcode
#else
#define ADDR32(opcode) addr32 opcode
#endif

#endif
