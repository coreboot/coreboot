/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_GDT
#define CPU_X86_GDT

#ifndef __ASSEMBLER__
/* These symbols are defined in c_start.S. */
extern char gdt[];
extern char per_cpu_segment_descriptors[];
extern uint32_t per_cpu_segment_selector;
extern char gdt_end[];
extern char idtarg[];
#endif

/* Offset to GDT's segment descriptors: */
#define GDT_CODE_SEG            0x08
#define GDT_DATA_SEG            0x10
#define GDT_CODE_SEG64          0x18
/*
 * This define is placed here to make sure future romstage programmers
 * know about it.
 * It is used only in SMM for STM setup code.
 */
#define GDT_TASK_STATE_SEG      0x20

#define GDT_CODE16_SEG          0x28
#define GDT_DATA16_SEG          0x30
#define GDT_DATA_ACPI_SEG       0x38

#endif /* CPU_X86_GDT */
