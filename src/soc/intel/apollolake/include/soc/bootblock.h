#ifndef _SOC_APOLLOLAKE_BOOTBLOCK_H_
#define _SOC_APOLLOLAKE_BOOTBLOCK_H_
#include <arch/cpu.h>

void asmlinkage bootblock_c_entry(void);
/* provide prototype for lib/bootblock.c main as workaround */
void main(void);
#endif
