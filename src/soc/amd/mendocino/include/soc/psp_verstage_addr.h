/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#ifndef AMD_MENDOCINO_PSP_VERSTAGE_ADDR_H
#define AMD_MENDOCINO_PSP_VERSTAGE_ADDR_H

/*
 * Start of available space is 0x0 and this is where the
 * header for the user app (verstage) must be mapped.
 * Size is 208KB
 */
#define PSP_SRAM_START			0x0
#define PSP_SRAM_SIZE			(208K)
#define VERSTAGE_START			PSP_SRAM_START

/*
 * The top of the stack must be 4k aligned, so set the bottom as 4k aligned
 * and make the size a multiple of 4k
 */

#define PSP_VERSTAGE_STACK_START	0x2a000
#define PSP_VERSTAGE_STACK_SIZE		(40K)

#endif  /* AMD_MENDOCINO_PSP_VERSTAGE_ADDR_H */
