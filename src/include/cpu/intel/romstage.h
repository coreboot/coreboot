#ifndef _CPU_INTEL_ROMSTAGE_H
#define _CPU_INTEL_ROMSTAGE_H

#include <arch/cpu.h>

void mainboard_romstage_entry(unsigned long bist);

/* romstage_main is called from the cache-as-ram assembly file. The return
 * value is the stack value to be used for romstage once cache-as-ram is
 * torn down. The following values are pushed onto the stack to setup the
 * MTRRs:
 *   +0: Number of MTRRs
 *   +4: MTRR base 0 31:0
 *   +8: MTRR base 0 63:32
 *  +12: MTRR mask 0 31:0
 *  +16: MTRR mask 0 63:32
 *  +20: MTRR base 1 31:0
 *  +24: MTRR base 1 63:32
 *  +28: MTRR mask 1 31:0
 *  +32: MTRR mask 1 63:32
 *  ...
 */
void *setup_stack_and_mtrrs(void);

void platform_enter_postcar(void);

/* romstage_main is called from the cache-as-ram assembly file to prepare
 * CAR stack guards.*/
asmlinkage void *romstage_main(unsigned long bist);
/* romstage_after_car() is the C function called after cache-as-ram has
 * been torn down. It is responsible for loading the ramstage. */
asmlinkage void romstage_after_car(void);

#endif /* _CPU_INTEL_ROMSTAGE_H */
