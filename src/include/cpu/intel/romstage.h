#ifndef _CPU_INTEL_ROMSTAGE_H
#define _CPU_INTEL_ROMSTAGE_H

#include <arch/cpu.h>

void mainboard_romstage_entry(unsigned long bist);
void * asmlinkage romstage_main(unsigned long bist);

#endif /* _CPU_INTEL_ROMSTAGE_H */
