/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DEVICE_OPROM_REALMODE_X86_H__
#define __DEVICE_OPROM_REALMODE_X86_H__

#include <arch/cpu.h>
#include <stdint.h>

#define REALMODE_BASE ((void *)0x600)

struct realmode_idt {
	u16 offset, cs;
};

void x86_exception(struct eregs *info);

/* From x86_asm.S */
extern unsigned char __idt_handler;
extern unsigned int __idt_handler_size;
extern unsigned char __realmode_code;
extern unsigned int __realmode_code_size;

extern unsigned int (*realmode_call)(u32 addr, u32 eax, u32 ebx, u32 ecx,
		u32 edx, u32 esi, u32 edi) asmlinkage;

extern unsigned int (*realmode_interrupt)(u32 intno, u32 eax, u32 ebx, u32 ecx,
		u32 edx, u32 esi, u32 edi) asmlinkage;

#define FAKE_MEMORY_SIZE (1024*1024) // only 1MB
#define INITIAL_EBDA_SEGMENT 0xF600
#define INITIAL_EBDA_SIZE 0x400

int int10_handler(void);
int int12_handler(void);
int int16_handler(void);
int int1a_handler(void);

#endif /* __DEVICE_OPROM_REALMODE_X86_H__ */
