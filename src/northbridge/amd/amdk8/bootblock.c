#include <arch/io.h>
#include <arch/romcc_io.h>
#include <device/pci_def.h>
#include "northbridge/amd/amdk8/early_ht.c"

static void bootblock_northbridge_init(void) {
	enumerate_ht_chain();
}
