/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#define THINKPAD_EC_GPE 17
#define BRIGHTNESS_UP \_SB.PCI0.GFX0.LCD0.INCB
#define BRIGHTNESS_DOWN \_SB.PCI0.GFX0.LCD0.DECB
#define ACPI_VIDEO_DEVICE \_SB.PCI0.GFX0
#define DISPLAY_DEVICE_2_IS_LCD_SCREEN 1

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x03,		// DSDT revision: ACPI v3.0
	"COREv4",	// OEM id
	"COREBOOT",	// OEM table id
	0x20090419	// OEM revision
)
{
	// Some generic macros
	#include "acpi/platform.asl"

	// global NVS and variables
	#include <southbridge/intel/i82801ix/acpi/globalnvs.asl>

	// General Purpose Events
	#include "acpi/gpe.asl"

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/gm45/acpi/gm45.asl>
			#include <southbridge/intel/i82801ix/acpi/ich9.asl>
		}
	}

	/* Chipset specific sleep states */
	#include <southbridge/intel/i82801ix/acpi/sleepstates.asl>

	/* Dock support code */
	#include "acpi/dock.asl"
}
