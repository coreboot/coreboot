
#include <arch/acpi.h>
#include <cpu/cpu.h>
#include <device/pci_rom.h>
#include <soc/acpi.h>


void acpi_create_serialio_ssdt(acpi_header_t *ssdt) {}

#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
void smm_init(void) {}
#endif

/* Rmodules don't like weak symbols. */
u32 map_oprom_vendev(u32 vendev) { return vendev; }
