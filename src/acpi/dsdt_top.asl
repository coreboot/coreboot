/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen_extern.asl>

#if CONFIG(CHROMEOS_NVS)
/* ChromeOS specific */
#include <vendorcode/google/chromeos/acpi/chromeos.asl>
#endif

/* Operating system enumeration. */
Name (OSYS, 0)

/* 0 => PIC mode, 1 => APIC Mode */
Name (PICM, 0)

/* Power state (AC = 1) */
Name (PWRS, 1)

/*
 * The _PIC method is called by the OS to choose between interrupt
 * routing via the i8259 interrupt controller or the APIC.
 *
 * _PIC is called with a parameter of 0 for i8259 configuration and
 * with a parameter of 1 for Local Apic/IOAPIC configuration.
 */

Method (_PIC, 1)
{
	/* Remember the OS' IRQ routing choice. */
	PICM = Arg0
}

#if CONFIG(ECAM_MMCONF_SUPPORT)
Scope(\_SB) {
	/* Base address of PCIe config space */
	Name(PCBA, CONFIG_ECAM_MMCONF_BASE_ADDRESS)

	/* Length of PCIe config space, 1MB each bus */
	Name(PCLN, CONFIG_ECAM_MMCONF_LENGTH)

	/* PCIe Configuration Space */
	OperationRegion(PCFG, SystemMemory, PCBA, PCLN) /* Each bus consumes 1MB */

	/* From the Linux documentation (Documentation/PCI/acpi-info.rst):
	 * [6] PCI Firmware 3.2, sec 4.1.2:
	 *     If the operating system does not natively comprehend reserving the
	 *     MMCFG region, the MMCFG region must be reserved by firmware.  The
	 *     address range reported in the MCFG table or by _CBA method (see Section
	 *     4.1.3) must be reserved by declaring a motherboard resource.  For most
	 *     systems, the motherboard resource would appear at the root of the ACPI
	 *     namespace (under \_SB) in a node with a _HID of EISAID (PNP0C02), and
	 *     the resources in this case should not be claimed in the root PCI bus's
	 *     _CRS.  The resources can optionally be returned in Int15 E820 or
	 *     EFIGetMemoryMap as reserved memory but must always be reported through
	 *     ACPI as a motherboard resource.
	 */
	Device (PERC)	// PCI ECAM Resource Consumption
	{
		Name (_HID, EisaId("PNP0C02"))
		Name (_CRS, ResourceTemplate()
		{
			Memory32Fixed (ReadWrite, CONFIG_ECAM_MMCONF_BASE_ADDRESS,
			CONFIG_ECAM_MMCONF_LENGTH)
		})
	}
}
#endif
