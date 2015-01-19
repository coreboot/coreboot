/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
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
 */

#ifndef __MIPS_ARCH_CPU_H__
#define __MIPS_ARCH_CPU_H__

/*
 * Reading at this address allows to identify the platform the code is running
 * on
 */

/*
 * This register holds the FPGA image version
 * If we're not working on the FPGA this will be 0
 */
#define PRIMARY_FPGA_VERSION		0xB8149060
#define IMG_PLATFORM_ID()		read32(PRIMARY_FPGA_VERSION)
#define IMG_PLATFORM_ID_FPGA		0xD1400003 /* Last FPGA image */
#define IMG_PLATFORM_ID_SILICON		0

#define CP0_COUNT	9
#define CP0_COMPARE	11
#define CP0_STATUS	12
#define CP0_CAUSE	13
#define CP0_WATCHLO	18
#define CP0_WATCHHI	19

/* coprocessor 0 enable */
#define ST0_CU0		(1 << 28)
#define C0_CAUSE_DC	(1 << 27)

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
#define read_c0_count()		__read_32bit_c0_register($9, 0)
#define write_c0_count(val)	__write_32bit_c0_register($9, 0, (val))

#define read_c0_cause()		__read_32bit_c0_register($13, 0)
#define write_c0_cause(val)	__write_32bit_c0_register($13, 0, (val))
/***************************************************************************/

#endif /* __MIPS_ARCH_CPU_H__ */
