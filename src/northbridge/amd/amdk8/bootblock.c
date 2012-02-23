#include <arch/io.h>
#include <arch/romcc_io.h>
#include <device/pci_def.h>
#include "northbridge/amd/amdk8/early_ht.c"

static void init_northbridge_amd_amdk8(void) {
	enumerate_ht_chain();
}
