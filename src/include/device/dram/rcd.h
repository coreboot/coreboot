/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef DEVICE_DRAM_RCD_H
#define DEVICE_DRAM_RCD_H

#include <types.h>
#include <device/i2c_simple.h>

enum rcw_idx {
	VEN_ID_L,
	VEN_ID_H,
	DEV_ID_L,
	DEV_ID_H,
	REV_ID,
	RES_05,
	RES_06,
	RES_07,
	F0RC00_01,
	F0RC02_03,
	F0RC04_05,
	F0RC06_07,
	F0RC08_09,
	F0RC0A_0B,
	F0RC0C_0D,
	F0RC0E_0F,
	F0RC1x,
	F0RC2x,
	F0RC3x,
	F0RC4x,
	F0RC5x,
	F0RC6x,
	F0RC7x,
	F0RC8x,
	F0RC9x,
	F0RCAx,
	F0RCBx,
	F0RCCx,
	F0RCDx,
	F0RCEx,
	F0RCFx,
	RCW_ALL,	/* Total num of bytes */
	RCW_ALL_ALIGNED	/* Total num of bytes after aligning to 4B */
};

_Static_assert(RCW_ALL_ALIGNED % sizeof(uint32_t) == 0,
	       "RCW_ALL_ALIGNED is not aligned");

/* Write an 8-bit register. Returns the number of written bytes. */
int rcd_write_reg(unsigned int bus, uint8_t slave, enum rcw_idx reg,
		  uint8_t data);

/* Write 32 bits of memory (i.e., four 8-bit registers, not 1 32-bit register, which would
 * involve byte swapping). Returns the number of written bytes. */
int rcd_write_32b(unsigned int bus, uint8_t slave, enum rcw_idx reg,
		  uint32_t data);

/* Dump 32 bytes of RCD onto the screen. */
void dump_rcd(unsigned int bus, uint8_t addr);

#endif /* DEVICE_DRAM_RCD_H */
