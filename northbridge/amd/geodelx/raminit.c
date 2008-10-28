/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <types.h>
#include <lib.h>
#include <console.h>
#include <msr.h>
#include <spd.h>
#include <io.h>
#include <cpu.h>
#include <amd_geodelx.h>
#include <southbridge/amd/cs5536/cs5536.h>

static const u8 num_col_addr[] = {
	0x00, 0x10, 0x11, 0x00, 0x00, 0x00, 0x00, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};

u8 spd_read_byte(u16 device, u8 address);

const static unsigned long msrs[] = {
	MC_CF07_DATA, 
	MC_CF8F_DATA, 
	MC_CF1017_DATA, 
	GLCP_DELAY_CONTROLS, 
	MC_CFCLK_DBUG, 
	MC_CF_PMCTR,
	GLCP_SYS_RSTPLL
};

/* If you ever change any of the strings below, make sure the char array size
 * is the length of the longest string +1
 */
const static char msrnames[][20] = {
	"MC_CF07_DATA", 
	"MC_CF8F_DATA", 
	"MC_CF1017_DATA", 
	"GLCP_DELAY_CONTROLS", 
	"MC_CFCLK_DBUG", 
	"MC_CF_PMCTR",
	"PLL reg"
};

/** 
 * Dump key MSR values for RAM init. You can call this function and then use it to 
 * compare to a factory BIOS setting.
 */
void dumplxmsrs(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(msrs); i++) {
		struct msr msr;
		msr = rdmsr(msrs[i]);
		/* don't change the %p to a %s unless you fix the problem. 
		 * in particular, don't change or submit a patch UNLESS YOU TEST IT
		 */
		printk(BIOS_DEBUG, "%p (%lx): %x.%x\n",  msrnames[i], msrs[i],
			msr.hi, msr.lo);
	}

}

/**
 * Halt and Catch Fire. Print an error, then loop, sending NULLs on
 * serial port, to ensure the message is visible.
 */
void hcf(void)
{
	printk(BIOS_EMERG, "DIE\n");

	/* This guarantees we flush the UART FIFOs (if any) and also ensures
	 * that things, in general, keep going so no debug output is lost.
	 */
	while (1)
		printk(BIOS_EMERG, "\r");
}


/**
 * Auto-detect, using SPD, the DIMM size. It's the usual magic, with
 * all the usual failure points that can happen.
 *
 * @param dimm TODO
 * @param dimm0 The SMBus address of DIMM 0 (mainboard-dependent).
 * @param dimm1 The SMBus address of DIMM 1 (mainboard-dependent).
 */
static void auto_size_dimm(unsigned int dimm, u8 dimm0, u8 dimm1)
{
	u32 dimm_setting;
	u16 dimm_size;
	u8 spd_byte;
	struct msr msr;

	dimm_setting = 0;

	banner(BIOS_DEBUG, "Check present");
	/* Check that we have a DIMM. */
	if (spd_read_byte(dimm, SPD_MEMORY_TYPE) == 0xFF)
		return;

	/* Field: Module Banks per DIMM */
	/* EEPROM byte usage: (5) Number of DIMM Banks */
	banner(BIOS_DEBUG, "MODBANKS");
	spd_byte = spd_read_byte(dimm, SPD_NUM_DIMM_BANKS);
	if ((MIN_MOD_BANKS > spd_byte) || (spd_byte > MAX_MOD_BANKS)) {
		printk(BIOS_EMERG, "Number of module banks not compatible\n");
		post_code(ERROR_BANK_SET);
		hlt();
	}
	dimm_setting |= (spd_byte >> 1) << CF07_UPPER_D0_MB_SHIFT;

	/* Field: Banks per SDRAM device */
	/* EEPROM byte usage: (17) Number of Banks on SDRAM Device */
	banner(BIOS_DEBUG, "FIELDBANKS");
	spd_byte = spd_read_byte(dimm, SPD_NUM_BANKS_PER_SDRAM);
	if ((MIN_DEV_BANKS > spd_byte) || (spd_byte > MAX_DEV_BANKS)) {
		printk(BIOS_EMERG, "Number of device banks not compatible\n");
		post_code(ERROR_BANK_SET);
		hlt();
	}
	dimm_setting |= (spd_byte >> 2) << CF07_UPPER_D0_CB_SHIFT;

	/* Field: DIMM size
	 * EEPROM byte usage: (3)  Number of Row Addresses
	 *                    (4)  Number of Column Addresses
	 *                    (5)  Number of DIMM Banks
	 *                    (31) Module Bank Density
	 * Size = Module Density * Module Banks
	 */
	banner(BIOS_DEBUG, "SPDNUMROWS");

	if ((spd_read_byte(dimm, SPD_NUM_ROWS) & 0xF0)
	    || (spd_read_byte(dimm, SPD_NUM_COLUMNS) & 0xF0)) {
		printk(BIOS_EMERG, "Asymmetric DIMM not compatible\n");
		post_code(ERROR_UNSUPPORTED_DIMM);
		hlt();
	}

	/* Size = Module Density * Module Banks */
	banner(BIOS_DEBUG, "SPDBANKDENSITY");
	dimm_size = spd_read_byte(dimm, SPD_BANK_DENSITY);

       /* Bits 0-1 have been redefined by JEDEC to contain what would have
        * been in non-existing bits 8 (1GB) and 9 (2GB). Move bits 0-1 to
        * bits 8-9 and mask bits 0-1 off.
        */
	dimm_size = ((dimm_size & 0x3) << 8) | (dimm_size & ~0x3);

	/* Module Density * Module Banks */
	/* Shift to multiply by the number of DIMM banks. */
	dimm_size <<= (dimm_setting >> CF07_UPPER_D0_MB_SHIFT) & 1;
	printk(BIOS_DEBUG, "DIMM size is %x\n", dimm_size);
	banner(BIOS_DEBUG, "BEFORT CTZ");
	dimm_size = __builtin_ctz(dimm_size);
	banner(BIOS_DEBUG, "TEST DIMM SIZE>8");
	if (dimm_size > 8) {	/* 8 is 1 GB only support 1 GB per DIMM */
		printk(BIOS_EMERG, "Only support up to 1 GB per DIMM\n");
		post_code(ERROR_DENSITY_DIMM);
		hlt();
	}
	dimm_setting |= dimm_size << CF07_UPPER_D0_SZ_SHIFT;

	/* PageSize = 2 ^ (number of column addresses) * data width in bytes
	 * (should be 8 bytes for a normal DIMM)
	 *
	 * If ma[12:0] is the memory address pins, and pa[12:0] is the
	 * physical column address that the memory controller (MC) generates,
	 * here is how the MC assigns the pa onto the ma pins:
	 * 
	 * ma  12 11 10 09 08 07 06 05 04 03 02 01 00
	 * -------------------------------------------
	 * pa					09 08 07 06 05 04 03	(7 col addr bits = 1K page size)
	 * pa				 10 09 08 07 06 05 04 03	(8 col addr bits = 2K page size)
	 * pa			  11 10 09 08 07 06 05 04 03	(9 col addr bits = 4K page size)
	 * pa		   12 11 10 09 08 07 06 05 04 03	(10 col addr bits = 8K page size)
	 * pa	 13 AP 12 11 10 09 08 07 06 05 04 03	(11 col addr bits = 16K page size)
	 * pa  14 13 AP 12 11 10 09 08 07 06 05 04 03	(12 col addr bits = 32K page size)
	 *
	 * (AP = autoprecharge bit)
	 * 
	 * Remember that pa[2:0] are zeroed out since it's a 64-bit data bus
	 * (8 bytes), so lower 3 address bits are dont_cares. So from the
	 * table above, it's easier to see what the old code is doing: if for
	 * example, #col_addr_bits = 7 (06h), it adds 3 to get 10, then does
	 * 2^10=1K. Get it?
	 */
	banner(BIOS_DEBUG, "PAGESIZE");
	spd_byte = num_col_addr[spd_read_byte(dimm, SPD_NUM_COLUMNS) & 0xF];
	banner(BIOS_DEBUG, "MAXCOLADDR");
	if (spd_byte > MAX_COL_ADDR) {
		printk(BIOS_EMERG, "DIMM page size not compatible\n");
		post_code(ERROR_SET_PAGE);
		hlt();
	}

	spd_byte -= 7;
	/* If the value is above 6 it means >12 address lines... */
	if (spd_byte > 5)
		spd_byte = 7;	/* ...which means >32k so set to disabled. */
	/* 0 = 1k, 1 = 2k, 2 = 4k, etc. */
	dimm_setting |= spd_byte << CF07_UPPER_D0_PSZ_SHIFT;

	banner(BIOS_DEBUG, "RDMSR CF07");
	msr = rdmsr(MC_CF07_DATA);
	banner(BIOS_DEBUG, "WRMSR CF07");
	printk(BIOS_DEBUG, "CF07(%x): %08x.%08x\n", MC_CF07_DATA, msr.hi, msr.lo);
	if (dimm == dimm0) {
		msr.hi &= 0xFFFF0000;
		msr.hi |= dimm_setting;
	} else {
		msr.hi &= 0x0000FFFF;
		msr.hi |= dimm_setting << 16;
	}
	banner(BIOS_DEBUG, "ALL DONE");
	wrmsr(MC_CF07_DATA, msr);
}

/**
 * Try to compute the max. DDR clock rate.
 *
 * The only bad news here is that if you have got a GeodeLink speed that is
 * too fast, you are going to pay for it: the system will hlt!
 *
 * @param dimm0 The SMBus address of DIMM 0 (mainboard-dependent).
 * @param dimm1 The SMBus address of DIMM 1 (mainboard-dependent).
 */
static void check_ddr_max(u8 dimm0, u8 dimm1)
{
	u8 spd_byte0, spd_byte1;
	u16 speed;

	/* PC133 identifier */
	spd_byte0 = spd_read_byte(dimm0, SPD_MIN_CYCLE_TIME_AT_CAS_MAX);
	if (spd_byte0 == 0xFF)
		spd_byte0 = 0;
	spd_byte1 = spd_read_byte(dimm1, SPD_MIN_CYCLE_TIME_AT_CAS_MAX);
	if (spd_byte1 == 0xFF)
		spd_byte1 = 0;

	/* Use the slowest DIMM. */
	if (spd_byte0 < spd_byte1)
		spd_byte0 = spd_byte1;

	/* Turn SPD ns time into MHz. Check what the asm does to this math. */
	speed = 2 * ((10000 / (((spd_byte0 >> 4) * 10) + (spd_byte0 & 0x0F))));

	printk(BIOS_DEBUG, "ddr max speed is %d\n", speed);
	/* Current speed > max speed? */
	if (geode_link_speed() > speed) {
		printk(BIOS_EMERG, "DIMM overclocked. Check GeodeLink speed.\n");
		post_code(POST_PLL_MEM_FAIL);
		hlt();
	}
}

static const u16 REFRESH_RATE[] = { 15, 3, 7, 31, 62, 125 };	/* ns */

/**
 * Compute a refresh rate.
 *
 * You have to read both DIMMs and take the one that requires a faster rate.
 *
 * @param dimm0 The SMBus address of DIMM 0 (mainboard-dependent).
 * @param dimm1 The SMBus address of DIMM 1 (mainboard-dependent).
 */
static void set_refresh_rate(u8 dimm0, u8 dimm1)
{
	u8 spd_byte0, spd_byte1;
	u16 rate0, rate1;
	struct msr msr;

	spd_byte0 = spd_read_byte(dimm0, SPD_REFRESH);
	spd_byte0 &= 0xF;
	if (spd_byte0 > 5)
		spd_byte0 = 5;
	rate0 = REFRESH_RATE[spd_byte0];

	spd_byte1 = spd_read_byte(dimm1, SPD_REFRESH);
	spd_byte1 &= 0xF;
	if (spd_byte1 > 5)
		spd_byte1 = 5;
	rate1 = REFRESH_RATE[spd_byte1];

	/* Use the faster rate (lowest number). */
	if (rate0 > rate1)
		rate0 = rate1;

	msr = rdmsr(MC_CF07_DATA);
	msr.lo |= ((rate0 * (geode_link_speed() / 2)) / 16)
		   << CF07_LOWER_REF_INT_SHIFT;
	printk(BIOS_DEBUG, "Refresh rate set to %x\n", rate0);
	wrmsr(MC_CF07_DATA, msr);
}

/* 1(1.5), 1.5, 2, 2.5, 3, 3.5, 4, 0 */
static const u8 CASDDR[] = { 5, 5, 2, 6, 3, 7, 4, 0 };

/**
 * Compute the CAS rate.
 *
 * The CAS setting is based on the information provided in each DIMMs SPD.
 *
 * The speed at which a DIMM can run is described relative to the slowest
 * CAS the DIMM supports. Each speed for the relative CAS settings is
 * checked that it is within the GeodeLink speed. If it isn't within the
 * GeodeLink speed, the CAS setting is removed from the list of good settings
 * for the DIMM.
 *
 * This is done for both DIMMs and the lists are compared to find the lowest
 * common CAS latency setting. If there are no CAS settings
 * in common we output a ERROR_DIFF_DIMMS (0x78) POST code and halt.
 *
 * Result is that we will set fastest CAS latency based on GeodeLink speed 
 * and SPD information.
 *
 * @param dimm0 The SMBus address of DIMM 0 (mainboard-dependent).
 * @param dimm1 The SMBus address of DIMM 1 (mainboard-dependent).
 */
static void set_cas(u8 dimm0, u8 dimm1)
{
	u16 glspeed, dimm_speed;
	u8 spd_byte = 0xff, casmap0, casmap1;
	struct msr msr;

	glspeed = geode_link_speed();

	/* DIMM 0 */
	casmap0 = spd_read_byte(dimm0, SPD_ACCEPTABLE_CAS_LATENCIES);
	if (casmap0 != 0xFF) {
		/* If -.5 timing is supported, check -.5 timing > GeodeLink. */
		/* EEPROM byte usage: (23) SDRAM Minimum Clock Cycle Time @ CLX -.5 */
		spd_byte = spd_read_byte(dimm0, SPD_SDRAM_CYCLE_TIME_2ND);
		if (spd_byte != 0) {
			/* Turn SPD ns time into MHz. Check what the asm does
			 * to this math.
			 */
			dimm_speed = 2 * (10000 / (((spd_byte >> 4) * 10) +
						(spd_byte & 0x0F)));
			if (dimm_speed >= glspeed) {
				/* If -1 timing is supported, check -1 timing > GeodeLink. */
				/* EEPROM byte usage: (25) SDRAM Minimum Clock Cycle Time @ CLX -1 */
				spd_byte = spd_read_byte(dimm0, SPD_SDRAM_CYCLE_TIME_3RD);
				if (spd_byte != 0) {
					/* Turn SPD ns time into MHz. Check what the asm does to this math. */
					dimm_speed = 2 * (10000 / (((spd_byte >> 4) * 10) + (spd_byte & 0x0F)));
					if (dimm_speed <= glspeed) {
						/* Set we can use -.5 timing but not -1. */
						spd_byte = 31 - __builtin_clz((u32) casmap0);
						/* Just want bits in the lower byte since we have to cast to a 32. */
						casmap0 &= 0xFF << (--spd_byte);
					}
				}	/* MIN_CYCLE_10 != 0 */
			} else {
				/* Timing_05 < GLspeed, can't use -.5 or -1 timing. */
				spd_byte = 31 - __builtin_clz((u32) casmap0);
				/* Just want bits in the lower byte since we have to cast to a 32. */
				casmap0 &= 0xFF << (spd_byte);
			}
		}		/* MIN_CYCLE_05 != 0 */
	} else {		/* No DIMM */
		casmap0 = 0;
	}

	/* DIMM 1 */
	casmap1 = spd_read_byte(dimm1, SPD_ACCEPTABLE_CAS_LATENCIES);
	if (casmap1 != 0xFF) {
		/* If -.5 timing is supported, check -.5 timing > GeodeLink. */
		/* EEPROM byte usage: (23) SDRAM Minimum Clock Cycle Time @ CLX -.5 */
		spd_byte = spd_read_byte(dimm1, SPD_SDRAM_CYCLE_TIME_2ND);
		if (spd_byte != 0) {
			/* Turn SPD ns time into MHz. Check what the asm does to this math. */
			dimm_speed = 2 * (10000 / (((spd_byte >> 4) * 10) + (spd_byte & 0x0F)));
			if (dimm_speed >= glspeed) {
				/* If -1 timing is supported, check -1 timing > GeodeLink. */
				/* EEPROM byte usage: (25) SDRAM Minimum Clock Cycle Time @ CLX -1 */
				spd_byte = spd_read_byte(dimm1, SPD_SDRAM_CYCLE_TIME_3RD);
				if (spd_byte != 0) {
					/* Turn SPD ns time into MHz. Check what the asm does to this math. */
					dimm_speed = 2 * (10000 / (((spd_byte >> 4) * 10) + (spd_byte & 0x0F)));
					if (dimm_speed <= glspeed) {
						/* Set we can use -.5 timing but not -1. */
						spd_byte = 31 - __builtin_clz((u32) casmap1);
						/* Just want bits in the lower byte since we have to cast to a 32. */
						casmap1 &= 0xFF << (--spd_byte);
					}
				}	/* MIN_CYCLE_10 != 0 */
			} else {
				/* Timing_05 < GLspeed, can't use -.5 or -1 timing. */
				spd_byte = 31 - __builtin_clz((u32) casmap1);
				/* Just want bits in the lower byte since we have to cast to a 32. */
				casmap1 &= 0xFF << (spd_byte);
			}
		}		/* MIN_CYCLE_05 != 0 */
	} else {		/* No DIMM */
		casmap1 = 0;
	}

	/* Compare CAS latencies. */
	if (casmap0 == 0) {
		spd_byte = CASDDR[__builtin_ctz((u32) casmap1)];
	} else if (casmap1 == 0) {
		spd_byte = CASDDR[__builtin_ctz((u32) casmap0)];
	} else if ((casmap0 &= casmap1)) {
		spd_byte = CASDDR[__builtin_ctz((u32) casmap0)];
	} else {
		printk(BIOS_EMERG, "DIMM CAS latencies not compatible\n");
		post_code(ERROR_DIFF_DIMMS);
		hlt();
	}

	printk(BIOS_DEBUG, "Set CAS latency to %x\n", spd_byte);
	msr = rdmsr(MC_CF8F_DATA);
	msr.lo &= ~(7 << CF8F_LOWER_CAS_LAT_SHIFT);
	msr.lo |= spd_byte << CF8F_LOWER_CAS_LAT_SHIFT;
	wrmsr(MC_CF8F_DATA, msr);
}

static inline void helper_spd(u8 dimm0, u8 dimm1, u8 addr, u8 *spd0, u8 *spd1)
{
	*spd0 = spd_read_byte(dimm0, addr);
	if (*spd0 == 0xFF)
		*spd0 = 0;
	*spd1 = spd_read_byte(dimm1, addr);
	if (*spd1 == 0xFF)
		*spd1 = 0;
	if (*spd0 < *spd1)
		*spd0 = *spd1;
}

static inline void helper_calc(u8 *spd0, u8 *spd1, u32 memspeed)
{
	/* (ns/(1/MHz) = (us*MHZ)/1000 = clocks/1000 = clocks) */
	*spd1 = ((*spd0 >> 2) * memspeed) / 1000;
	if ((((*spd0 >> 2) * memspeed) % 1000)) {
		++(*spd1);
	}
}

/**
 * Set latencies for DRAM.
 *
 * These are the famed RAS and CAS latencies. Take the one with the tightest
 * requirements, and use that for both.
 *
 * @param dimm0 The SMBus address of DIMM 0 (mainboard-dependent).
 * @param dimm1 The SMBus address of DIMM 1 (mainboard-dependent).
 */
static void set_latencies(u8 dimm0, u8 dimm1)
{
	u32 memspeed, dimm_setting;
	u8 spd_byte0, spd_byte1;
	struct msr msr;

	memspeed = geode_link_speed() / 2;
	dimm_setting = 0;

	/* MC_CF8F setup */
	/* tRAS */
	helper_spd(dimm0, dimm1, SPD_tRAS, &spd_byte0, &spd_byte1);
	/* (ns/(1/MHz) = (us*MHZ)/1000 = clocks/1000 = clocks) */
	/* TODO: This calculation is a bit different. On purpose or bug? */
	spd_byte1 = (spd_byte0 * memspeed) / 1000;
	if (((spd_byte0 * memspeed) % 1000))
		++spd_byte1;
	dimm_setting |= spd_byte1 << CF8F_LOWER_ACT2PRE_SHIFT;

	/* tRP */
	helper_spd(dimm0, dimm1, SPD_tRP, &spd_byte0, &spd_byte1);
	helper_calc(&spd_byte0, &spd_byte1, memspeed);
	dimm_setting |= spd_byte1 << CF8F_LOWER_PRE2ACT_SHIFT;

	/* tRCD */
	helper_spd(dimm0, dimm1, SPD_tRCD, &spd_byte0, &spd_byte1);
	helper_calc(&spd_byte0, &spd_byte1, memspeed);
	dimm_setting |= spd_byte1 << CF8F_LOWER_ACT2CMD_SHIFT;

	/* tRRD */
	helper_spd(dimm0, dimm1, SPD_tRRD, &spd_byte0, &spd_byte1);
	helper_calc(&spd_byte0, &spd_byte1, memspeed);
	dimm_setting |= spd_byte1 << CF8F_LOWER_ACT2ACT_SHIFT;

	/* tRC = tRP + tRAS */
	dimm_setting |= (((dimm_setting >> CF8F_LOWER_ACT2PRE_SHIFT) & 0x0F) +
			((dimm_setting >> CF8F_LOWER_PRE2ACT_SHIFT) & 0x07))
				<< CF8F_LOWER_ACT2ACTREF_SHIFT;

	msr = rdmsr(MC_CF8F_DATA);
	msr.lo &= 0xF00000FF;
	msr.lo |= dimm_setting;
	msr.hi |= CF8F_UPPER_REORDER_DIS_SET;
	wrmsr(MC_CF8F_DATA, msr);

	/* MC_CF1017 setup */
	/* tRFC */
	helper_spd(dimm0, dimm1, SPD_tRFC, &spd_byte0, &spd_byte1);

	if (spd_byte0) {
		/* (ns/(1/MHz) = (us*MHZ)/1000 = clocks/1000 = clocks) */
		spd_byte1 = (spd_byte0 * memspeed) / 1000;
		if (((spd_byte0 * memspeed) % 1000))
			++spd_byte1;
	} else {
		/* Not all SPDs have tRFC setting.
		 * Use this formula: tRFC = tRC + 1 clk.
		 */
		spd_byte1 = ((dimm_setting >> CF8F_LOWER_ACT2ACTREF_SHIFT) & 0x0F) + 1;
	}

	/* Note: This clears the cf8f DIMM setting. */
	dimm_setting = spd_byte1 << CF1017_LOWER_REF2ACT_SHIFT;
	msr = rdmsr(MC_CF1017_DATA);
	msr.lo &= ~(0x1F << CF1017_LOWER_REF2ACT_SHIFT);
	msr.lo |= dimm_setting;
	wrmsr(MC_CF1017_DATA, msr);

	/* tWTR: Set tWTR to 2 for 400 MHz and above GLBUS (200 Mhz mem)
	 * otherwise it stay default (1).
	 */
	if (memspeed > 198) {
		msr = rdmsr(MC_CF1017_DATA);
		msr.lo &= ~(0x7 << CF1017_LOWER_WR_TO_RD_SHIFT);
		msr.lo |= 2 << CF1017_LOWER_WR_TO_RD_SHIFT;
		wrmsr(MC_CF1017_DATA, msr);
	}
}

/**
 * Set the registers for drive, namely drive and fet strength.
 *
 * @param dimm0 The SMBus address of DIMM 0 (mainboard-dependent).
 * @param dimm1 The SMBus address of DIMM 1 (mainboard-dependent).
 */
static void set_extended_mode_registers(u8 dimm0, u8 dimm1)
{
	u8 spd_byte0, spd_byte1;
	struct msr msr;

	spd_byte0 = spd_read_byte(dimm0, SPD_DEVICE_ATTRIBUTES_GENERAL);
	if (spd_byte0 == 0xFF)
		spd_byte0 = 0;
	spd_byte1 = spd_read_byte(dimm1, SPD_DEVICE_ATTRIBUTES_GENERAL);
	if (spd_byte1 == 0xFF)
		spd_byte1 = 0;
	spd_byte1 &= spd_byte0;

	msr = rdmsr(MC_CF07_DATA);
	if (spd_byte1 & 1)
		msr.lo |= CF07_LOWER_EMR_DRV_SET; /* Drive Strength Control */
	if (spd_byte1 & 2)
		msr.lo |= CF07_LOWER_EMR_QFC_SET; /* FET Control */
	wrmsr(MC_CF07_DATA, msr);
}

/**
 * Debug function. Only used when test hardware is connected.
 */
static void EnableMTest(void)
{
	struct msr msr;

	msr = rdmsr(GLCP_DELAY_CONTROLS);
	msr.hi &= ~(7 << 20);	/* Clear bits 54:52. */
	if (geode_link_speed() < 200)
		msr.hi |= 2 << 20;
	wrmsr(GLCP_DELAY_CONTROLS, msr);

	msr = rdmsr(MC_CFCLK_DBUG);
	msr.hi |= CFCLK_UPPER_MTST_B2B_DIS_SET | CFCLK_UPPER_MTEST_EN_SET |
		  CFCLK_UPPER_MTST_RBEX_EN_SET;
	msr.lo |= CFCLK_LOWER_TRISTATE_DIS_SET;
	wrmsr(MC_CFCLK_DBUG, msr);

	printk(BIOS_DEBUG, "Enabled MTest for TLA debug\n");
}

/**
 * Set SDRAM registers that need to be set independent of SPD or even
 * presence or absence of DIMMs in a slot. Parameters are ignored.
 */
void sdram_set_registers(void)
{
	struct msr msr;

	/* Set Timing Control. */
	msr = rdmsr(MC_CF1017_DATA);
	msr.lo &= ~(7 << CF1017_LOWER_RD_TMG_CTL_SHIFT);
	if (geode_link_speed() < 334)
		msr.lo |= (3 << CF1017_LOWER_RD_TMG_CTL_SHIFT);
	else
		msr.lo |= (4 << CF1017_LOWER_RD_TMG_CTL_SHIFT);
	wrmsr(MC_CF1017_DATA, msr);

	/* Set Refresh Staggering. */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo &= ~0xF0;
	msr.lo |= 0x40;		/* Set refresh to 4 SDRAM clocks. */
	wrmsr(MC_CF07_DATA, msr);
}

/**
 * Set SDRAM registers that need to be determined by SPD.
 *
 * @param dimm0 The SMBus address of DIMM 0 (mainboard-dependent).
 * @param dimm1 The SMBus address of DIMM 1 (mainboard-dependent).
 */
void sdram_set_spd_registers(u8 dimm0, u8 dimm1)
{
	u8 spd_byte;

	post_code(POST_MEM_SETUP);

	spd_byte = spd_read_byte(dimm0, SPD_MODULE_ATTRIBUTES);

	/* Check DIMM is not Registered and not Buffered DIMMs. */
	if ((spd_byte != 0xFF) && (spd_byte & 3)) {
		printk(BIOS_EMERG, "DIMM 0 NOT COMPATIBLE!\n");
		post_code(ERROR_UNSUPPORTED_DIMM);
		hlt();
	}
	spd_byte = spd_read_byte(dimm1, SPD_MODULE_ATTRIBUTES);
	if ((spd_byte != 0xFF) && (spd_byte & 3)) {
		printk(BIOS_EMERG, "DIMM 1 NOT COMPATIBLE!\n");
		post_code(ERROR_UNSUPPORTED_DIMM);
		hlt();
	}

	post_code(POST_MEM_SETUP2);

	/* Check that the memory is not overclocked. */
	check_ddr_max(dimm0, dimm1);

	/* Size the DIMMs.
	 * This is gross. It is an artifact of our move to parameters instead of
	 * #defines. FIXME! The fix is trivial but I want to see it work first.
	 */
	post_code(POST_MEM_SETUP3);
	auto_size_dimm(dimm0, dimm0, dimm1);
	post_code(POST_MEM_SETUP4);
	auto_size_dimm(dimm1, dimm0, dimm1);

	/* Set CAS latency. */
	post_code(POST_MEM_SETUP5);
	set_cas(dimm0, dimm1);

	/* Set all the other latencies here (tRAS, tRP, ...). */
	set_latencies(dimm0, dimm1);

	/* Set Extended Mode Registers. */
	set_extended_mode_registers(dimm0, dimm1);

	/* Set Memory Refresh Rate. */
	set_refresh_rate(dimm0, dimm1);
}

/**
 * Enable the DRAMs.
 *
 * Section 6.1.3, LX processor databooks, BIOS Initialization Sequence
 * Section 4.1.4, GX/CS5535 GeodeROM Porting guide
 *
 * Turn on MC/DIMM interface per JEDEC:
 *   1) Clock stabilizes > 200us
 *   2) Assert CKE
 *   3) Precharge All to put all banks into an idle state
 *   4) EMRS to enable DLL
 *   6) MRS w/ memory config & reset DLL set
 *   7) Wait 200 clocks (2us)
 *   8) Precharge All and 2 Auto refresh
 *   9) MRS w/ memory config & reset DLL clear
 *   8) DDR SDRAM ready for normal operation
 *
 * @param dimm0 The SMBus address of DIMM 0 (mainboard-dependent).
 * @param dimm1 The SMBus address of DIMM 1 (mainboard-dependent).
 */
void sdram_enable(u8 dimm0, u8 dimm1)
{
	u32 i;
	struct msr msr;

	post_code(POST_MEM_ENABLE);

	/* Only enable MTest for TLA memory debug. */
	/* EnableMTest(); */

	/* If both Page Size = "Not Installed" we have a problem and
	 * should halt.
	 */
	msr = rdmsr(MC_CF07_DATA);
	if ((msr.hi & ((7 << CF07_UPPER_D1_PSZ_SHIFT) |
			(7 << CF07_UPPER_D0_PSZ_SHIFT))) ==
			((7 << CF07_UPPER_D1_PSZ_SHIFT) |
			(7 << CF07_UPPER_D0_PSZ_SHIFT))) {
		printk(BIOS_EMERG, "No memory in the system\n");
		post_code(ERROR_NO_DIMMS);
		hlt();
	}

	/* Set CKEs. */
	msr = rdmsr(MC_CFCLK_DBUG);
	msr.lo &= ~(CFCLK_LOWER_MASK_CKE_SET0 | CFCLK_LOWER_MASK_CKE_SET1);
	wrmsr(MC_CFCLK_DBUG, msr);

	/* Force Precharge All on next command, EMRS. */
	msr = rdmsr(MC_CFCLK_DBUG);
	msr.lo |= CFCLK_LOWER_FORCE_PRE_SET;
	wrmsr(MC_CFCLK_DBUG, msr);

	/* EMRS to enable DLL (pre-setup done in setExtendedModeRegisters). */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |= CF07_LOWER_PROG_DRAM_SET | CF07_LOWER_LOAD_MODE_DDR_SET;
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~(CF07_LOWER_PROG_DRAM_SET | CF07_LOWER_LOAD_MODE_DDR_SET);
	wrmsr(MC_CF07_DATA, msr);

	/* Clear Force Precharge All. */
	msr = rdmsr(MC_CFCLK_DBUG);
	msr.lo &= ~CFCLK_LOWER_FORCE_PRE_SET;
	wrmsr(MC_CFCLK_DBUG, msr);

	/* MRS Reset DLL - set. */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |= CF07_LOWER_PROG_DRAM_SET | CF07_LOWER_LOAD_MODE_DLL_RESET;
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~(CF07_LOWER_PROG_DRAM_SET | CF07_LOWER_LOAD_MODE_DLL_RESET);
	wrmsr(MC_CF07_DATA, msr);

	/* 2us delay (200 clocks @ 200Mhz). We probably really don't need
	 * this but... better safe.
	 *
	 * Wait two 'port 61 ticks' (between 15us and 30us).
	 * This would be endless if the timer is stuck.
	 */
	while ((inb(0x61)));	/* Find the first edge. */
	while (!(~inb(0x61)));

	/* Force Precharge All on the next command, auto-refresh. */
	msr = rdmsr(MC_CFCLK_DBUG);
	msr.lo |= CFCLK_LOWER_FORCE_PRE_SET;
	wrmsr(MC_CFCLK_DBUG, msr);

	/* Manually AUTO refresh #1. If auto refresh was not enabled above we
	 * would need to do 8 refreshes to prime the pump before these 2.
	 */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |= CF07_LOWER_REF_TEST_SET;
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~CF07_LOWER_REF_TEST_SET;
	wrmsr(MC_CF07_DATA, msr);

	/* Clear Force Precharge All. */
	msr = rdmsr(MC_CFCLK_DBUG);
	msr.lo &= ~CFCLK_LOWER_FORCE_PRE_SET;
	wrmsr(MC_CFCLK_DBUG, msr);

	/* Manually AUTO refresh.
	 * The MC should insert the right delay between the refreshes.
	 */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |= CF07_LOWER_REF_TEST_SET;
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~CF07_LOWER_REF_TEST_SET;
	wrmsr(MC_CF07_DATA, msr);

	/* MRS Reset DLL - clear. */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |= CF07_LOWER_PROG_DRAM_SET;
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~CF07_LOWER_PROG_DRAM_SET;
	wrmsr(MC_CF07_DATA, msr);

	/* Allow MC to tristate during idle cycles with MTEST OFF. */
	msr = rdmsr(MC_CFCLK_DBUG);
	msr.lo &= ~CFCLK_LOWER_TRISTATE_DIS_SET;
	wrmsr(MC_CFCLK_DBUG, msr);

	/* Disable SDCLK DIMM 1 slot if no DIMM installed (to save power). */
	msr = rdmsr(MC_CF07_DATA);
	if ((msr.hi & (7 << CF07_UPPER_D1_PSZ_SHIFT)) ==
	    (7 << CF07_UPPER_D1_PSZ_SHIFT)) {
		msr = rdmsr(GLCP_DELAY_CONTROLS);
		msr.hi |= (1 << 23);	/* SDCLK bit for 2.0 */
		wrmsr(GLCP_DELAY_CONTROLS, msr);
	}

	/* Set PMode0 Sensitivity Counter. */
	msr.lo = 0;		/* pmode 0=0 most aggressive */
	msr.hi = 0x200;		/* pmode 1=200h */
	wrmsr(MC_CF_PMCTR, msr);

	/* Set PMode1 Up delay enable. */
	msr = rdmsr(MC_CF1017_DATA);
	msr.lo |= (209 << 8);	/* bits[15:8] = 209 */
	wrmsr(MC_CF1017_DATA, msr);

	printk(BIOS_DEBUG, "DRAM controller init done.\n");
	post_code(POST_MEM_SETUP_GOOD);

	/* Make sure there is nothing stale in the cache. */
	/* CAR stack is in the cache __asm__ __volatile__("wbinvd\n"); */

	/* The RAM dll needs a write to lock on so generate a few dummy
	 * writes. Note: The descriptor needs to be enabled to point at memory.
	 */
	volatile unsigned long *ptr;
	for (i = 0; i < 5; i++) {
		ptr = (void *)i;
		*ptr = (unsigned long)i;
	}

	/* SWAPSiF for PBZ 4112 (Errata 34).
	 * Check for failed DLL settings now that we have done a memory write.
	 */
	msr = rdmsr(GLCP_DELAY_CONTROLS);
	if ((msr.lo & 0x7FF) == 0x104) {
		/* If you had it you would need to clear out the fail boot
		 * count flag (depending on where it counts from etc).
		 */

		/* The operation we are about to perform clears the PM_SSC
		 * register in the CS5536 so will need to store the S3
		 * resume flag in NVRAM otherwise it would do a normal boot.
		 */

		/* Reset the system. */
		msr = rdmsr(MDD_SOFT_RESET);
		msr.lo |= 1;
		wrmsr(MDD_SOFT_RESET, msr);
	}

	printk(BIOS_DEBUG, "RAM DLL lock\n");
}
