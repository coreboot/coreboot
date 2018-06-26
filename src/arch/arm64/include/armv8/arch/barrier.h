/*
 * Based on arch/arm/include/asm/barrier.h
 *
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __ASM_ARM_BARRIER_H
#define __ASM_ARM_BARRIER_H

#ifndef __ASSEMBLER__

#define sevl()		asm volatile("sevl" : : : "memory")
#define sev()		asm volatile("sev" : : : "memory")
#define wfe()		asm volatile("wfe" : : : "memory")
#define wfi()		asm volatile("wfi" : : : "memory")

#define isb()		asm volatile("isb" : : : "memory")
#define dsb()		asm volatile("dsb sy" : : : "memory")
#define dmb()		asm volatile("dmb sy" : : : "memory")

#define mb()		dsb()
#define rmb()		asm volatile("dsb ld" : : : "memory")
#define wmb()		asm volatile("dsb st" : : : "memory")

#if IS_ENABLED(CONFIG_SMP)
#define barrier() __asm__ __volatile__("": : :"memory")
#endif

#define nop()		asm volatile("nop");

#define force_read(x) (*(volatile typeof(x) *)&(x))

#define load_acquire(p) \
({									\
	typeof(*p) ___p1;						\
	switch (sizeof(*p)) {						\
	case 4:								\
		asm volatile ("ldar %w0, %1"				\
			: "=r" (___p1) : "Q" (*p) : "memory");		\
		break;							\
	case 8:								\
		asm volatile ("ldar %0, %1"				\
			: "=r" (___p1) : "Q" (*p) : "memory");		\
		break;							\
	}								\
	___p1;								\
})

#define store_release(p, v)						\
do {									\
	switch (sizeof(*p)) {						\
	case 4:								\
		asm volatile ("stlr %w1, %0"				\
				: "=Q" (*p) : "r" (v) : "memory");	\
		break;							\
	case 8:								\
		asm volatile ("stlr %1, %0"				\
				: "=Q" (*p) : "r" (v) : "memory");	\
		break;							\
	}								\
} while (0)

#define load_acquire_exclusive(p) \
({									\
	typeof(*p) ___p1;						\
	switch (sizeof(*p)) {						\
	case 4:								\
		asm volatile ("ldaxr %w0, %1"				\
			: "=r" (___p1) : "Q" (*p) : "memory");		\
		break;							\
	case 8:								\
		asm volatile ("ldaxr %0, %1"				\
			: "=r" (___p1) : "Q" (*p) : "memory");		\
		break;							\
	}								\
	___p1;								\
})

/* Returns 1 on success. */
#define store_release_exclusive(p, v)					\
({									\
	int ret;							\
	switch (sizeof(*p)) {						\
	case 4:								\
		asm volatile ("stlxr %w0, %w2, %1"			\
				: "=&r" (ret), "=Q" (*p) : "r" (v)	\
				: "memory");				\
		break;							\
	case 8:								\
		asm volatile ("stlxr %w0, %2, %1"			\
				: "=&r" (ret), "=Q" (*p) : "r" (v)	\
				: "memory");				\
		break;							\
	}								\
	!ret;								\
})

#endif	/* __ASSEMBLER__ */

#endif	/* __ASM_ARM_BARRIER_H */
