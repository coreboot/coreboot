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

/* Magic value used to locate speedstep configuration in the device tree */
#define SPEEDSTEP_APIC_MAGIC 0xACAC

/* MWAIT coordination I/O base address. This must match
 * the \_PR_.CPU0 PM base address.
 */
#define PMB0_BASE 0x510

/* PMB1: I/O port that triggers SMI once cores are in the same state.
 * See CSM Trigger, at PMG_CST_CONFIG_CONTROL[6:4]
 */
#define PMB1_BASE 0x800


/* Speedstep related MSRs */
#define IA32_PLATFORM_ID  0x017
#define IA32_PERF_STS     0x198
#define IA32_PERF_CTL     0x199
#define MSR_THERM2_CTL    0x19D
#define IA32_MISC_ENABLES 0x1A0
