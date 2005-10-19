#include <stdint.h>
#include <arch/io.h>
#include "cpc925.h"

void cpc925_init(void);
void sdram_init(void);

void
memory_init(void)
{
	cpc925_init();
	sdram_init();
	cpc925_pci_init();
}

void
cpc925_init(void)
{
}

void
sdram_init()
{
}
