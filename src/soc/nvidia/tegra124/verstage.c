#include "verstage.h"
#include <vendorcode/google/chromeos/chromeos.h>

/**
 * Stage entry point
 */
void vboot_main(void)
{
	/* Stub to force arm_init_caches to the top, before any stack/memory
	 * accesses */
	asm volatile ("bl arm_init_caches"
		      ::: "r0","r1","r2","r3","r4","r5","ip");

	select_firmware();
}
