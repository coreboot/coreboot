/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __MIPS_ARCH_CPU_H
#define __MIPS_ARCH_CPU_H

#define asmlinkage

#ifndef __PRE_RAM__

#include <device/device.h>

struct cpu_driver {
	struct device_operations *ops;
	const struct cpu_device_id *id_table;
};

struct thread;

struct cpu_info {
	struct device *cpu;
	unsigned long index;
};

#endif /* !__PRE_RAM__ */

/***************************************************************************
 * The following section was copied from arch/mips/include/asm/mipsregs.h in
 * the 3.14 kernel tree.
 */

/*
 * Macros to access the system control coprocessor
 */

#define __read_32bit_c0_register(source, sel)				\
({ int __res;								\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mfc0\t%0, " #source "\n\t"			\
			: "=r" (__res));				\
	else								\
		__asm__ __volatile__(					\
			".set\tmips32\n\t"				\
			"mfc0\t%0, " #source ", " #sel "\n\t"		\
			".set\tmips0\n\t"				\
			: "=r" (__res));				\
	__res;								\
})

#define __write_32bit_c0_register(register, sel, value)			\
do {									\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mtc0\t%z0, " #register "\n\t"			\
			: : "Jr" ((unsigned int)(value)));		\
	else								\
		__asm__ __volatile__(					\
			".set\tmips32\n\t"				\
			"mtc0\t%z0, " #register ", " #sel "\n\t"	\
			".set\tmips0"					\
			: : "Jr" ((unsigned int)(value)));		\
} while (0)

/* Shortcuts to access various internal registers, keep adding as needed. */
#define read_c0_index()		__read_32bit_c0_register($0, 0)
#define write_c0_index(val)	__write_32bit_c0_register($0, 0, (val))

#define read_c0_entrylo0()	__read_32bit_c0_register($2, 0)
#define write_c0_entrylo0(val)	__write_32bit_c0_register($2, 0, (val))

#define read_c0_entrylo1()	__read_32bit_c0_register($3, 0)
#define write_c0_entrylo1(val)	__write_32bit_c0_register($3, 0, (val))

#define read_c0_pagemask()	__read_32bit_c0_register($5, 0)
#define write_c0_pagemask(val)	__write_32bit_c0_register($5, 0, (val))

#define read_c0_wired()		__read_32bit_c0_register($6, 0)
#define write_c0_wired(val)	__write_32bit_c0_register($6, 0, (val))

#define read_c0_count()		__read_32bit_c0_register($9, 0)
#define write_c0_count(val)	__write_32bit_c0_register($9, 0, (val))

#define read_c0_entryhi()	__read_32bit_c0_register($10, 0)
#define write_c0_entryhi(val)	__write_32bit_c0_register($10, 0, (val))

#define read_c0_cause()		__read_32bit_c0_register($13, 0)
#define write_c0_cause(val)	__write_32bit_c0_register($13, 0, (val))

#define read_c0_config1()	__read_32bit_c0_register($16, 1)
#define write_c0_config1(val)	__write_32bit_c0_register($16, 1, (val))

#define read_c0_config2()	__read_32bit_c0_register($16, 2)
#define write_c0_config2(val)	__write_32bit_c0_register($16, 2, (val))

#define read_c0_l23taglo()	__read_32bit_c0_register($28, 4)
#define write_c0_l23taglo(val)	__write_32bit_c0_register($28, 4, (val))


#define C0_ENTRYLO_PFN_SHIFT 6

#define C0_ENTRYLO_COHERENCY_MASK	0x00000038
#define C0_ENTRYLO_COHERENCY_SHIFT	3
/* Cacheable, write-back, non-coherent */
#define C0_ENTRYLO_COHERENCY_WB		(0x3 << C0_ENTRYLO_COHERENCY_SHIFT)
/* Uncached, non-coherent */
#define C0_ENTRYLO_COHERENCY_UC		(0x2 << C0_ENTRYLO_COHERENCY_SHIFT)

/* Writeable */
#define C0_ENTRYLO_D			(0x1 << 2)
/* Valid */
#define C0_ENTRYLO_V			(0x1 << 1)
/* Global */
#define C0_ENTRYLO_G			(0x1 << 0)

#define C0_PAGEMASK_SHIFT 13
#define C0_PAGEMASK_MASK 0xffff

#define C0_WIRED_MASK 0x3f

#define C0_CAUSE_DC (1 << 27)

#define C0_CONFIG1_MMUSIZE_SHIFT 25
#define C0_CONFIG1_MMUSIZE_MASK 0x3f

/* Hazard handling */
static inline void __nop(void)
{
	__asm__ __volatile__("nop");
}

static inline void __ssnop(void)
{
	__asm__ __volatile__("sll\t$0, $0, 1");
}

#define mtc0_tlbw_hazard()						\
do {									\
	__nop();							\
	__nop();							\
} while (0)

#define tlbw_use_hazard()						\
do {									\
	__nop();							\
	__nop();							\
	__nop();							\
} while (0)

#define tlb_probe_hazard()						\
do {									\
	__nop();							\
	__nop();							\
	__nop();							\
} while (0)

#define back_to_back_c0_hazard()					\
do {									\
	__ssnop();							\
	__ssnop();							\
	__ssnop();							\
} while (0)
/**************************************************************************/

#endif /* __MIPS_ARCH_CPU_H */
