/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* Intel SOC PCIe support */

Device (RP01)
{
	Name (_ADR, 0x001c0000)

	Method (_PRT)
	{
		If (PICM) {
			Return (Package() {
				#undef PIC_MODE
				#include <soc/intel/baytrail/acpi/irq_helper.h>
				PCI_DEV_PIRQ_ROUTE(0x0, A, B, C, D)
			})
		} Else {
			Return (Package() {
				#define PIC_MODE
				#include <soc/intel/baytrail/acpi/irq_helper.h>
				PCI_DEV_PIRQ_ROUTE(0x0, A, B, C, D)
			})
		}
	}
}

Device (RP02)
{
	Name (_ADR, 0x001c0001)

	Method (_PRT)
	{
		If (PICM) {
			Return (Package() {
				#undef PIC_MODE
				#include <soc/intel/baytrail/acpi/irq_helper.h>
				PCI_DEV_PIRQ_ROUTE(0x0, B, C, D, A)
			})
		} Else {
			Return (Package() {
				#define PIC_MODE
				#include <soc/intel/baytrail/acpi/irq_helper.h>
				PCI_DEV_PIRQ_ROUTE(0x0, B, C, D, A)
			})
		}
	}
}

Device (RP03)
{
	Name (_ADR, 0x001c0002)

	Method (_PRT)
	{
		If (PICM) {
			Return (Package() {
				#undef PIC_MODE
				#include <soc/intel/baytrail/acpi/irq_helper.h>
				PCI_DEV_PIRQ_ROUTE(0x0, C, D, A, B)
			})
		} Else {
			Return (Package() {
				#define PIC_MODE
				#include <soc/intel/baytrail/acpi/irq_helper.h>
				PCI_DEV_PIRQ_ROUTE(0x0, C, D, A, B)
			})
		}
	}
}

Device (RP04)
{
	Name (_ADR, 0x001c0003)

	Method (_PRT)
	{
		If (PICM) {
			Return (Package() {
				#undef PIC_MODE
				#include <soc/intel/baytrail/acpi/irq_helper.h>
				PCI_DEV_PIRQ_ROUTE(0x0, D, A, B, C)
			})
		} Else {
			Return (Package() {
				#define PIC_MODE
				#include <soc/intel/baytrail/acpi/irq_helper.h>
				PCI_DEV_PIRQ_ROUTE(0x0, D, A, B, C)
			})
		}
	}
}
