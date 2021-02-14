/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen_extern.asl>

#if CONFIG(CHROMEOS)
/* Chrome OS specific */
#include <vendorcode/google/chromeos/acpi/gnvs.asl>
#include <vendorcode/google/chromeos/acpi/chromeos.asl>
#endif

/* Operating system enumeration. */
Name (OSYS, 0)

/* Zero => PIC mode, One => APIC Mode */
Name (PICM, Zero)

/* Power state (AC = 1) */
Name (PWRS, One)

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

#if CONFIG(MMCONF_SUPPORT)
Scope(\_SB) {
	/* Base address of PCIe config space */
	Name(PCBA, CONFIG_MMCONF_BASE_ADDRESS)

	/* Length of PCIe config space, 1MB each bus */
	Name(PCLN, CONFIG_MMCONF_LENGTH)

	/* PCIe Configuration Space */
	OperationRegion(PCFG, SystemMemory, PCBA, PCLN) /* Each bus consumes 1MB */
}
#endif
