/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors.  All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

extern struct chip_operations cpu_intel_speedstep_ops;

/* Magic value used to locate this chip in the device tree */
#define SPEEDSTEP_APIC_MAGIC 0xACAC

struct cpu_intel_speedstep_config {
	u8 pstate_coord_type;	/* Processor Coordination Type */

	int c1_battery;		/* CPU C-state for ACPI C1 on Battery Power */
	int c2_battery;		/* CPU C-state for ACPI C2 on Battery Power */
	int c3_battery;		/* CPU C-state for ACPI C3 on Battery Power */

	int c1_acpower;		/* CPU C-state for ACPI C1 on AC Power */
	int c2_acpower;		/* CPU C-state for ACPI C2 on AC Power */
	int c3_acpower;		/* CPU C-state for ACPI C3 on AC Power */
};
