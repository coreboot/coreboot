/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

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
