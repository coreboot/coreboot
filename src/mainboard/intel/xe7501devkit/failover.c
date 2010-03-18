#define ASSEMBLY 1
#define __PRE_RAM__
#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "pc80/mc146818rtc_early.c"
#include "southbridge/intel/i82801cx/cmos_failover.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/intel/e7501/reset_test.c"

static unsigned long main(unsigned long bist)
{
	return bist;
}
