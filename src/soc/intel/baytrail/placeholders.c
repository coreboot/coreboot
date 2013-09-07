
#include <arch/acpi.h>
#include <cpu/cpu.h>
#include <device/pci_rom.h>
#include <baytrail/acpi.h>


void generate_cpu_entries(void) {}

void acpi_create_intel_hpet(acpi_hpet_t * hpet) {}

void acpi_create_serialio_ssdt(acpi_header_t *ssdt) {}

unsigned long acpi_fill_mcfg(unsigned long current) { return current; }

void smm_setup_structures(void *gnvs, void *tcg, void *smi1) {}

void smm_init(void) {}

/* Rmodules don't like weak symbols. */
u32 map_oprom_vendev(u32 vendev) { return vendev; }
