#ifndef _CPU_INTEL_ROMSTAGE_H
#define _CPU_INTEL_ROMSTAGE_H

#include <arch/cpu.h>

void mainboard_romstage_entry(unsigned long bist);

void platform_enter_postcar(void);

/* romstage_main is called from the cache-as-ram assembly file to prepare
 * CAR stack guards.*/
asmlinkage void *romstage_main(unsigned long bist);

#endif /* _CPU_INTEL_ROMSTAGE_H */
