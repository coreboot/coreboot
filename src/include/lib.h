/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Myles Watson <mylesgw@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

/* This file is for "nuisance prototypes" that have no other home. */

#ifndef __LIB_H__
#define __LIB_H__
#include <stdint.h>
#ifndef __ROMCC__ /* romcc doesn't support prototypes. */

#ifndef __PRE_RAM__ /* Conflicts with romcc_io.h */
/* Defined in src/lib/clog2.c */
unsigned long log2(unsigned long x);
#endif

/* Defined in src/lib/lzma.c */
unsigned long ulzma(unsigned char *src, unsigned char *dst);

/* Defined in src/arch/x86/boot/gdt.c */
void move_gdt(void);

/* Defined in src/lib/ramtest.c */
void ram_check(unsigned long start, unsigned long stop);
int ram_check_nodie(unsigned long start, unsigned long stop);
void quick_ram_check(void);

/* Defined in src/lib/stack.c */
int checkstack(void *top_of_stack, int core);

/* currently defined by a ldscript */
extern unsigned char _estack[];

/* Defined in romstage.c */
#if CONFIG_CPU_AMD_GEODE_LX
void cache_as_ram_main(void);
#else
void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx);
#endif

/* definition is architecture-dependent but at minimum, for most architectures,
 * defined in src/lib/romstream.c. There are so few systems that don't have
 * memory mapped ROM that we yanked this years ago. Now, thanks to
 * some ARM systems, it's back.
 * A stream is a struct which contains a base (of arbitrary type, so u64)
 * and a size (u32; will we ever have 4GB flash?)
 */
struct stream {
	u64 base;
	u32 size;
};

struct stream *stream_start(struct stream *stream, u64 base, u32 size);
int stream_read(struct stream *stream, void *where, u32 size, u32 off);
void stream_fini(struct stream *stream);

#endif /* __ROMCC__ */
#endif /* __LIB_H__ */
