/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_GDT
#define CPU_X86_GDT

/* These symbols are defined in c_start.S. */
extern char gdt[];
extern char per_cpu_segment_descriptors[];
extern uint32_t per_cpu_segment_selector;
extern char gdt_end[];
extern char idtarg[];

/* These symbols are defined in secondary.S. */
extern char _secondary_gdt_addr[];
extern char _secondary_start[];
extern char _secondary_start_end[];

#endif /* CPU_X86_GDT */
