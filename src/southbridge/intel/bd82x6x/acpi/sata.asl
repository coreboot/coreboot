/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

// Intel SATA Controller 0:1f.2

// Note: Some BIOSes put the S-ATA code into an SSDT to make it easily
// pluggable

Device (SATA)
{
	Name (_ADR, 0x001f0002)
}
