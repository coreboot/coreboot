/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/pci_devs.h>
#include <arch/hpet.h>
#include <cpu/amd/mtrr.h>
#include <device/pci.h>
#include <northbridge/amd/agesa/agesa_helper.h>

void amd_initcpuio(void)
{
	/* Enable legacy video routing: D18F1xF4 VGA Enable */
	pci_write_config32(_SOC_DEV(0x18, 1), 0xf4, 1);

	/* The platform BIOS needs to ensure the memory ranges of SB800 legacy
	 * devices (TPM, HPET, BIOS RAM, Watchdog Timer, I/O APIC and ACPI) are
	 * set to non-posted regions. Last address before processor local APIC
	 * at FEE00000, set NP (non-posted) bit.
	 */
	pci_write_config32(_SOC_DEV(0x18, 1), 0x84, 0x00fedf00 | (1 << 7));
	/* lowest NP address is HPET at FED00000 */
	pci_write_config32(_SOC_DEV(0x18, 1), 0x80, (HPET_BASE_ADDRESS >> 8) | 3);

	/* Map the remaining PCI hole as posted MMIO. 0xfecf0000 is the last
	   address before non-posted range */
	pci_write_config32(_SOC_DEV(0x18, 1), 0x8c, 0x00fecf00);
	pci_write_config32(_SOC_DEV(0x18, 1), 0x88, (get_top_of_mem_below_4gb() >> 8) | 3);

	/* Send all IO (0000-FFFF) to southbridge. */
	pci_write_config32(_SOC_DEV(0x18, 1), 0xc4, 0x0000f000);
	pci_write_config32(_SOC_DEV(0x18, 1), 0xc0, 0x00000003);
}
