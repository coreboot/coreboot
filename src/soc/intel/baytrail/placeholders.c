
#include <arch/acpi.h>
#include <cpu/cpu.h>
#include <device/pci_rom.h>
#include <baytrail/acpi.h>


void generate_cpu_entries(void) {}

unsigned long acpi_fill_mcfg(unsigned long current) { return current; }

void smm_init(void) {}

/* Rmodules don't like weak symbols. */
u32 map_oprom_vendev(u32 vendev) { return vendev; }
