#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include "arch/romcc_io.h"
#include "pc80/mc146818rtc_early.c"
#include "southbridge/amd/amd8111/amd8111_enable_rom.c"
#include "northbridge/amd/amdk8/early_ht.c"

static void main(void)
{
	if (do_normal_boot()) {
		/* Nothing special needs to be done to find bus 0 */

		/* Allow the HT devices to be found */
		enumerate_ht_chain();

		/* Setup the 8111 */
		amd8111_enable_rom();

		/* Jump to the normal image */
		asm("jmp __normal_image");
	}
}
