/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_REGISTERS_H
#define __ARCH_REGISTERS_H

#if !defined(__ASSEMBLER__)
#include <stdint.h>

#define LONG_DOWNTO8(A) \
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

#define LONG_DOWNTO16(A) \
	union { \
		struct { \
			uint16_t A; \
			uint16_t h##A; \
		} __packed; \
		uint32_t e##A; \
	} __packed;

#define QUAD_DOWNTO8(A) \
	union { \
		LONG_DOWNTO8(A) \
		uint64_t r##A##x; \
	} __packed

#define QUAD_DOWNTO16(A) \
	union {\
		LONG_DOWNTO16(A) \
		uint64_t r##A; \
	} __packed

#ifdef __ARCH_x86_64__
struct eregs {
	QUAD_DOWNTO8(a);
	QUAD_DOWNTO8(c);
	QUAD_DOWNTO8(d);
	QUAD_DOWNTO8(b);
	QUAD_DOWNTO16(bp);
	QUAD_DOWNTO16(si);
	QUAD_DOWNTO16(di);
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	uint64_t vector;
	uint64_t error_code;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	QUAD_DOWNTO16(sp);
	uint64_t ss;
};
#else
struct eregs {
	LONG_DOWNTO8(a);
	LONG_DOWNTO8(c);
	LONG_DOWNTO8(d);
	LONG_DOWNTO8(b);
	LONG_DOWNTO16(sp);
	LONG_DOWNTO16(bp);
	LONG_DOWNTO16(si);
	LONG_DOWNTO16(di);
	uint32_t vector;
	uint32_t error_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
};
#endif
#endif // !ASSEMBLER

#if CONFIG(COMPILER_LLVM_CLANG)
#define ADDR32(opcode) opcode
#else
#define ADDR32(opcode) addr32 opcode
#endif

#endif
