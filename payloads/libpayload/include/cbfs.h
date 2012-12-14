#include <arch/types.h>

/* FIXME: workaround for coreboot/libpayload Kconfig differences */
#ifndef CONFIG_ARCH_ARMV7
#define CONFIG_ARCH_ARMV7 0
#endif
#ifndef CONFIG_ARCH_POWERPC
#define CONFIG_ARCH_POWERPC 0
#endif
#ifndef CONFIG_ARCH_X86
#define CONFIG_ARCH_X86 0
#endif

#include "cbfs_core.h"
void setup_cbfs_from_ram(void* start, uint32_t size);
void setup_cbfs_from_flash(void);
