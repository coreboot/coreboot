
#include <arch/acpi.h>
#include <cpu/cpu.h>
#include <device/pci_rom.h>
#include <soc/acpi.h>


void smm_init(void) {}

/* Rmodules don't like weak symbols. */
u32 map_oprom_vendev(u32 vendev) { return vendev; }
