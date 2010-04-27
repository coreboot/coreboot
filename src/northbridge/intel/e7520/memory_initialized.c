#include "e7520.h"
#define NB_DEV PCI_DEV(0, 0, 0)

static inline int memory_initialized(void)
{
	uint32_t drc;
        drc = pci_read_config32(NB_DEV, DRC);
        //print_debug("memory_initialized: DRC: ");
        //print_debug_hex32(drc);
        //print_debug("\n");

    	return (drc & (1<<29));
}
