#include <arch/io.h>
#include <device/pci_def.h>
#include "northbridge/amd/amdfam10/early_ht.c"

static void bootblock_northbridge_init(void) {
	/* Nothing special needs to be done to find bus 0 */
	/* Allow the HT devices to be found */
	/* mov bsp to bus 0xff when > 8 nodes */
	set_bsp_node_CHtExtNodeCfgEn();
	enumerate_ht_chain();
}
