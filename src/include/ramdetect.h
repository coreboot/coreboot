/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */


/*
 * Used in probe_ramsize(). This is a weak function and it's overridden for
 * ARMv8. Return 1 when DRAM exists at the address, otherwise return 0.
 */
int probe_mb(const uintptr_t dram_start, const uintptr_t size);
/*
 * Probe an area if it's read/writable.
 * Primary use case is the detection of DRAM amount on emulators.
 *
 * @param dram_start Physical address of DRAM start
 * @param probe_size Maximum size in MiB to probe for
 * @return The detected DRAM size in MiB
 */
size_t probe_ramsize(const uintptr_t dram_start, const size_t probe_size);
