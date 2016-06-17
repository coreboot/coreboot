#ifndef _CPU_INTEL_ROMSTAGE_H
#define _CPU_INTEL_ROMSTAGE_H

#include <arch/cpu.h>

/* std signature of entry-point to romstage.c */
void main(unsigned long bist);

void mainboard_romstage_entry(unsigned long bist);
void * asmlinkage romstage_main(unsigned long bist);

#endif /* _CPU_INTEL_ROMSTAGE_H */
