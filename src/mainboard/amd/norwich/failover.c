#define ASSEMBLY 1
#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include "arch/romcc_io.h"
#include "pc80/mc146818rtc_early.c"

static unsigned long main(unsigned long bist)
{
	return bist;
}
