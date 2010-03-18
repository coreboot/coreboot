#define ASSEMBLY 1
#define __PRE_RAM__
#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "pc80/mc146818rtc_early.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/intel/e7525/memory_initialized.c"

static unsigned long main(unsigned long bist)
{
	return bist;
}
