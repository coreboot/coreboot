#ifndef _CPU_AMD_CAR_H
#define _CPU_AMD_CAR_H

#include <arch/cpu.h>

asmlinkage void romstage_main(unsigned long bist);
asmlinkage void ap_romstage_main(void);

#endif
