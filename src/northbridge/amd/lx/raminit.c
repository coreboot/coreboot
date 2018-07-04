/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/lxdef.h>
#include <arch/io.h>
#include <spd.h>
#include <stddef.h>
#include <southbridge/amd/cs5536/cs5536.h>
#include "raminit.h"
#include "northbridge.h"

static const unsigned char NumColAddr[] = {
	0x00, 0x10, 0x11, 0x00, 0x00, 0x00, 0x00, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

static void banner(const char *s)
{
	printk(BIOS_DEBUG, " * %s\n", s);
}

static void __attribute__((noreturn)) hcf(void)
{
	printk(BIOS_EMERG, "DIE\n");
	/* this guarantees we flush the UART fifos (if any) and also
	 * ensures that things, in general, keep going so no debug output
	 * is lost
	 */
	while (1)
		printk(BIOS_EMERG, "%c", 0);
}

static void auto_size_dimm(unsigned int dimm)
{
	uint32_t dimm_setting;
	uint16_t dimm_size;
	uint8_t spd_byte;
	msr_t msr;

	dimm_setting = 0;

	banner("Check present");
	/* Check that we have a dimm */
	if (spd_read_byte(dimm, SPD_MEMORY_TYPE) == 0xFF) {
		return;
	}

	banner("MODBANKS");
	/* Field: Module Banks per DIMM */
	/* EEPROM byte usage: (5) Number of DIMM Banks */
	spd_byte = spd_read_byte(dimm, SPD_NUM_DIMM_BANKS);
	if ((MIN_MOD_BANKS > spd_byte) || (spd_byte > MAX_MOD_BANKS)) {
		printk(BIOS_EMERG, "Number of module banks not compatible\n");
		post_code(ERROR_BANK_SET);
		hcf();
	}
	dimm_setting |= (spd_byte >> 1) << CF07_UPPER_D0_MB_SHIFT;
	banner("FIELDBANKS");

	/* Field: Banks per SDRAM device */
	/* EEPROM byte usage: (17) Number of Banks on SDRAM Device */
	spd_byte = spd_read_byte(dimm, SPD_NUM_BANKS_PER_SDRAM);
	if ((MIN_DEV_BANKS > spd_byte) || (spd_byte > MAX_DEV_BANKS)) {
		printk(BIOS_EMERG, "Number of device banks not compatible\n");
		post_code(ERROR_BANK_SET);
		hcf();
	}
	dimm_setting |= (spd_byte >> 2) << CF07_UPPER_D0_CB_SHIFT;
	banner("SPDNUMROWS");

	/*; Field: DIMM size
	 *; EEPROM byte usage: (3)  Number of Row Addresses
	 *;                                       (4)  Number of Column Addresses
	 *;                                       (5)  Number of DIMM Banks
	 *;                                       (31) Module Bank Density
	 *; Size = Module Density * Module Banks
	 */
	if ((spd_read_byte(dimm, SPD_NUM_ROWS) & 0xF0)
	    || (spd_read_byte(dimm, SPD_NUM_COLUMNS) & 0xF0)) {
		printk(BIOS_EMERG, "Assymetirc DIMM not compatible\n");
		post_code(ERROR_UNSUPPORTED_DIMM);
		hcf();
	}
	banner("SPDBANKDENSITY");

	dimm_size = spd_read_byte(dimm, SPD_BANK_DENSITY);
	banner("DIMMSIZE");
	dimm_size |= (dimm_size << 8);	/* align so 1GB(bit0) is bit 8, this is a little weird to get gcc to not optimize this out */
	dimm_size &= 0x01FC;	/* and off 2GB DIMM size : not supported and the 1GB size we just moved up to bit 8 as well as all the extra on top */

	/*       Module Density * Module Banks */
	dimm_size <<= (dimm_setting >> CF07_UPPER_D0_MB_SHIFT) & 1;	/* shift to multiply by # DIMM banks */
	banner("BEFORT CTZ");
	dimm_size = __builtin_ctz(dimm_size);
	banner("TEST DIMM SIZE > 8");
	if (dimm_size > 8) {	/* 8 is 1GB only support 1GB per DIMM */
		printk(BIOS_EMERG, "Only support up to 1 GB per DIMM\n");
		post_code(ERROR_DENSITY_DIMM);
		hcf();
	}
	dimm_setting |= dimm_size << CF07_UPPER_D0_SZ_SHIFT;
	banner("PAGESIZE");

/*; Field: PAGE size
*; EEPROM byte usage: (4)  Number of Column Addresses
*; PageSize = 2^# Column Addresses * Data width in bytes (should be 8bytes for a normal DIMM)
*
*; But this really works by magic.
*;If ma[12:0] is the memory address pins, and pa[12:0] is the physical column address
*;that MC generates, here is how the MC assigns the pa onto the ma pins:
*
*;ma  12 11 10 09 08 07 06 05 04 03 02 01 00
*;-------------------------------------------
*;pa					09 08 07 06 05 04 03	(7 col addr bits = 1K page size)
*;pa				 10 09 08 07 06 05 04 03	(8 col addr bits = 2K page size)
*;pa			  11 10 09 08 07 06 05 04 03	(9 col addr bits = 4K page size)
*;pa		   12 11 10 09 08 07 06 05 04 03	(10 col addr bits = 8K page size)
*;pa	 13 AP 12 11 10 09 08 07 06 05 04 03	(11 col addr bits = 16K page size)
*;pa  14 13 AP 12 11 10 09 08 07 06 05 04 03	(12 col addr bits = 32K page size)
*; *AP = autoprecharge bit
*
*;Remember that pa[2:0] are zeroed out since it's a 64-bit data bus (8 bytes),
*;so lower 3 address bits are dont_cares.So from the table above,
*;it's easier to see what the old code is doing: if for example,#col_addr_bits = 7(06h),
*;it adds 3 to get 10, then does 2^10 = 1K.  Get it?*/

	spd_byte = NumColAddr[spd_read_byte(dimm, SPD_NUM_COLUMNS) & 0xF];
	banner("MAXCOLADDR");
	if (spd_byte > MAX_COL_ADDR) {
		printk(BIOS_EMERG, "DIMM page size not compatible\n");
		post_code(ERROR_SET_PAGE);
		hcf();
	}
	banner(">12address test");
	spd_byte -= 7;
	if (spd_byte > 5) {	/* if the value is above 6 it means > 12 address lines */
		spd_byte = 7;	/* which means > 32k so set to disabled */
	}
	dimm_setting |= spd_byte << CF07_UPPER_D0_PSZ_SHIFT;	/* 0 = 1k, 1 = 2k, 2 = 4k, etc */

	banner("RDMSR CF07");
	msr = rdmsr(MC_CF07_DATA);
	banner("WRMSR CF07");
	if (dimm == DIMM0) {
		msr.hi &= 0xFFFF0000;
		msr.hi |= dimm_setting;
	} else {
		msr.hi &= 0x0000FFFF;
		msr.hi |= dimm_setting << 16;
	}
	wrmsr(MC_CF07_DATA, msr);
	banner("ALL DONE");
}

static void checkDDRMax(void)
{
	uint8_t spd_byte0, spd_byte1;
	uint16_t speed;

	/* PC133 identifier */
	spd_byte0 = spd_read_byte(DIMM0, SPD_MIN_CYCLE_TIME_AT_CAS_MAX);
	if (spd_byte0 == 0xFF) {
		spd_byte0 = 0;
	}
	spd_byte1 = spd_read_byte(DIMM1, SPD_MIN_CYCLE_TIME_AT_CAS_MAX);
	if (spd_byte1 == 0xFF) {
		spd_byte1 = 0;
	}

	/* I don't think you need this check.
	   if (spd_byte0 >= 0xA0 || spd_byte1 >= 0xA0) {
	   printk(BIOS_EMERG, "DIMM overclocked. Check GeodeLink Speed\n");
	   post_code(POST_PLL_MEM_FAIL);
	   hcf();
	   } */

	/* Use the slowest DIMM */
	if (spd_byte0 < spd_byte1) {
		spd_byte0 = spd_byte1;
	}

	/* Turn SPD ns time into MHz. Check what the asm does to this math. */
	speed = 20000 / (((spd_byte0 >> 4) * 10) + (spd_byte0 & 0x0F));

	/* current speed > max speed? */
	if (GeodeLinkSpeed() > speed) {
		printk(BIOS_EMERG, "DIMM overclocked. Check GeodeLink Speed\n");
		post_code(POST_PLL_MEM_FAIL);
		hcf();
	}
}

const uint16_t REF_RATE[] = { 15, 3, 7, 31, 62, 125 };	/* ns */

static void set_refresh_rate(void)
{
	uint8_t spd_byte0, spd_byte1;
	uint16_t rate0, rate1;
	msr_t msr;

	spd_byte0 = spd_read_byte(DIMM0, SPD_REFRESH);
	spd_byte0 &= 0xF;
	if (spd_byte0 > 5) {
		spd_byte0 = 5;
	}
	rate0 = REF_RATE[spd_byte0];

	spd_byte1 = spd_read_byte(DIMM1, SPD_REFRESH);
	spd_byte1 &= 0xF;
	if (spd_byte1 > 5) {
		spd_byte1 = 5;
	}
	rate1 = REF_RATE[spd_byte1];

	/* Use the faster rate (lowest number) */
	if (rate0 > rate1) {
		rate0 = rate1;
	}

	msr = rdmsr(MC_CF07_DATA);
	msr.lo |= ((rate0 * (GeodeLinkSpeed() / 2)) / 16)
			<< CF07_LOWER_REF_INT_SHIFT;
	wrmsr(MC_CF07_DATA, msr);
}

const uint8_t CASDDR[] = { 5, 5, 2, 6, 3, 7, 4, 0 };	/* 1(1.5), 1.5, 2, 2.5, 3, 3.5, 4, 0 */

static u8 getcasmap(u32 dimm, u16 glspeed)
{
	u16 dimm_speed;
	u8 spd_byte, casmap, casmap_shift = 0;

	/**************************	 DIMM0	**********************************/
	casmap = spd_read_byte(dimm, SPD_ACCEPTABLE_CAS_LATENCIES);
	if (casmap != 0xFF) {
		/* IF -.5 timing is supported, check -.5 timing > GeodeLink */
		spd_byte = spd_read_byte(dimm, SPD_SDRAM_CYCLE_TIME_2ND);
		if (spd_byte != 0) {
			/* Turn SPD ns time into MHz. Check what the asm does to this math. */
			dimm_speed = 20000 / (((spd_byte >> 4) * 10) + (spd_byte & 0x0F));
			if (dimm_speed >= glspeed) {
				casmap_shift = 1; /* -.5 is a shift of 1 */
				/* IF -1 timing is supported, check -1 timing > GeodeLink */
				spd_byte = spd_read_byte(dimm, SPD_SDRAM_CYCLE_TIME_3RD);
				if (spd_byte != 0) {
					/* Turn SPD ns time into MHz. Check what the asm does to this math. */
					dimm_speed = 20000 / (((spd_byte >> 4) * 10) + (spd_byte & 0x0F));
					if (dimm_speed >= glspeed) {
						casmap_shift = 2; /* -1 is a shift of 2 */
					}
				}	/* SPD_SDRAM_CYCLE_TIME_3RD (-1) !=0 */
			} else {
				casmap_shift = 0;
			}
		}	/* SPD_SDRAM_CYCLE_TIME_2ND (-.5) !=0 */
		/* set the casmap based on the shift to limit possible CAS settings */
		spd_byte = 31 - __builtin_clz((uint32_t) casmap);
		/* just want bits in the lower byte since we have to cast to a 32 */
		casmap &= 0xFF << (spd_byte - casmap_shift);
	} else {		/* No DIMM */
		casmap = 0;
	}
	return casmap;
}

static void setCAS(void)
{
/*;*****************************************************************************
;*
;*	setCAS
;*	EEPROM byte usage: (18) SDRAM device attributes - CAS latency
;*	EEPROM byte usage: (23) SDRAM Minimum Clock Cycle Time @ CLX -.5
;*	EEPROM byte usage: (25) SDRAM Minimum Clock Cycle Time @ CLX -1
;*
;*	The CAS setting is based on the information provided in each DIMMs SPD.
;*	 The speed at which a DIMM can run is described relative to the slowest
;*	 CAS the DIMM supports. Each speed for the relative CAS settings is
;*	 checked that it is within the GeodeLink speed. If it isn't within the GeodeLink
;*	 speed, the CAS setting	 is removed from the list of good settings for
;*	 the DIMM. This is done for both DIMMs and the lists are compared to
;*	 find the lowest common CAS latency setting. If there are no CAS settings
;*	 in common we out a ERROR_DIFF_DIMMS (78h) to port 80h and halt.
;*
;*	Entry:
;*	Exit: Set fastest CAS Latency based on GeodeLink speed and SPD information.
;*	Destroys: We really use everything !
;*****************************************************************************/
	uint16_t glspeed;
	uint8_t spd_byte, casmap0, casmap1;
	msr_t msr;

	glspeed = GeodeLinkSpeed();

	casmap0 = getcasmap(DIMM0, glspeed);
	casmap1 = getcasmap(DIMM1, glspeed);

	/*********************	CAS_LAT MAP COMPARE	***************************/
	if (casmap0 == 0) {
		spd_byte = CASDDR[__builtin_ctz(casmap1)];
	} else if (casmap1 == 0) {
		spd_byte = CASDDR[__builtin_ctz(casmap0)];
	} else if ((casmap0 &= casmap1)) {
		spd_byte = CASDDR[__builtin_ctz(casmap0)];
	} else {
		printk(BIOS_EMERG, "DIMM CAS Latencies not compatible\n");
		post_code(ERROR_DIFF_DIMMS);
		hcf();
	}

	msr = rdmsr(MC_CF8F_DATA);
	msr.lo &= ~(7 << CF8F_LOWER_CAS_LAT_SHIFT);
	msr.lo |= spd_byte << CF8F_LOWER_CAS_LAT_SHIFT;
	wrmsr(MC_CF8F_DATA, msr);
}

static void set_latencies(void)
{
	uint32_t memspeed, dimm_setting;
	uint8_t spd_byte0, spd_byte1;
	msr_t msr;

	memspeed = GeodeLinkSpeed() / 2;
	dimm_setting = 0;

	/* MC_CF8F setup */
	/* tRAS */
	spd_byte0 = spd_read_byte(DIMM0, SPD_tRAS);
	if (spd_byte0 == 0xFF) {
		spd_byte0 = 0;
	}
	spd_byte1 = spd_read_byte(DIMM1, SPD_tRAS);
	if (spd_byte1 == 0xFF) {
		spd_byte1 = 0;
	}
	if (spd_byte0 < spd_byte1) {
		spd_byte0 = spd_byte1;
	}

	/* (ns/(1/MHz) = (us*MHz)/1000 = clocks/1000 = clocks) */
	spd_byte1 = (spd_byte0 * memspeed) / 1000;
	if (((spd_byte0 * memspeed) % 1000)) {
		++spd_byte1;
	}
	dimm_setting |= spd_byte1 << CF8F_LOWER_ACT2PRE_SHIFT;

	/* tRP */
	spd_byte0 = spd_read_byte(DIMM0, SPD_tRP);
	if (spd_byte0 == 0xFF) {
		spd_byte0 = 0;
	}
	spd_byte1 = spd_read_byte(DIMM1, SPD_tRP);
	if (spd_byte1 == 0xFF) {
		spd_byte1 = 0;
	}
	if (spd_byte0 < spd_byte1) {
		spd_byte0 = spd_byte1;
	}

	/* (ns/(1/MHz) = (us*MHz)/1000 = clocks/1000 = clocks) */
	spd_byte1 = ((spd_byte0 >> 2) * memspeed) / 1000;
	if ((((spd_byte0 >> 2) * memspeed) % 1000)) {
		++spd_byte1;
	}
	dimm_setting |= spd_byte1 << CF8F_LOWER_PRE2ACT_SHIFT;

	/* tRCD */
	spd_byte0 = spd_read_byte(DIMM0, SPD_tRCD);
	if (spd_byte0 == 0xFF) {
		spd_byte0 = 0;
	}
	spd_byte1 = spd_read_byte(DIMM1, SPD_tRCD);
	if (spd_byte1 == 0xFF) {
		spd_byte1 = 0;
	}
	if (spd_byte0 < spd_byte1) {
		spd_byte0 = spd_byte1;
	}

	/* (ns/(1/MHz) = (us*MHz)/1000 = clocks/1000 = clocks) */
	spd_byte1 = ((spd_byte0 >> 2) * memspeed) / 1000;
	if ((((spd_byte0 >> 2) * memspeed) % 1000)) {
		++spd_byte1;
	}
	dimm_setting |= spd_byte1 << CF8F_LOWER_ACT2CMD_SHIFT;

	/* tRRD */
	spd_byte0 = spd_read_byte(DIMM0, SPD_tRRD);
	if (spd_byte0 == 0xFF) {
		spd_byte0 = 0;
	}
	spd_byte1 = spd_read_byte(DIMM1, SPD_tRRD);
	if (spd_byte1 == 0xFF) {
		spd_byte1 = 0;
	}
	if (spd_byte0 < spd_byte1) {
		spd_byte0 = spd_byte1;
	}

	/* (ns/(1/MHz) = (us*MHz)/1000 = clocks/1000 = clocks) */
	spd_byte1 = ((spd_byte0 >> 2) * memspeed) / 1000;
	if ((((spd_byte0 >> 2) * memspeed) % 1000)) {
		++spd_byte1;
	}
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
	spd_byte0 = spd_read_byte(DIMM0, SPD_tRFC);
	if (spd_byte0 == 0xFF) {
		spd_byte0 = 0;
	}
	spd_byte1 = spd_read_byte(DIMM1, SPD_tRFC);
	if (spd_byte1 == 0xFF) {
		spd_byte1 = 0;
	}
	if (spd_byte0 < spd_byte1) {
		spd_byte0 = spd_byte1;
	}

	if (spd_byte0) {
		/* (ns/(1/MHz) = (us*MHz)/1000 = clocks/1000 = clocks) */
		spd_byte1 = (spd_byte0 * memspeed) / 1000;
		if (((spd_byte0 * memspeed) % 1000)) {
			++spd_byte1;
		}
	} else {		/* Not all SPDs have tRFC setting. Use this formula tRFC = tRC + 1 clk */
		spd_byte1 = ((dimm_setting >> CF8F_LOWER_ACT2ACTREF_SHIFT) & 0x0F) + 1;
	}
	dimm_setting = spd_byte1 << CF1017_LOWER_REF2ACT_SHIFT;	/* note this clears the cf8f dimm setting */
	msr = rdmsr(MC_CF1017_DATA);
	msr.lo &= ~(0x1F << CF1017_LOWER_REF2ACT_SHIFT);
	msr.lo |= dimm_setting;
	wrmsr(MC_CF1017_DATA, msr);

	/* tWTR: Set tWTR to 2 for 400MHz and above GLBUS (200MHz mem) other wise it stay default(1) */
	if (memspeed > 198) {
		msr = rdmsr(MC_CF1017_DATA);
		msr.lo &= ~(0x7 << CF1017_LOWER_WR_TO_RD_SHIFT);
		msr.lo |= 2 << CF1017_LOWER_WR_TO_RD_SHIFT;
		wrmsr(MC_CF1017_DATA, msr);
	}
}

static void set_extended_mode_registers(void)
{
	uint8_t spd_byte0, spd_byte1;
	msr_t msr;
	spd_byte0 = spd_read_byte(DIMM0, SPD_DEVICE_ATTRIBUTES_GENERAL);
	if (spd_byte0 == 0xFF) {
		spd_byte0 = 0;
	}
	spd_byte1 = spd_read_byte(DIMM1, SPD_DEVICE_ATTRIBUTES_GENERAL);
	if (spd_byte1 == 0xFF) {
		spd_byte1 = 0;
	}
	spd_byte1 &= spd_byte0;

	msr = rdmsr(MC_CF07_DATA);
	if (spd_byte1 & 1) {	/* Drive Strength Control */
		msr.lo |= CF07_LOWER_EMR_DRV_SET;
	}
	if (spd_byte1 & 2) {	/* FET Control */
		msr.lo |= CF07_LOWER_EMR_QFC_SET;
	}
	wrmsr(MC_CF07_DATA, msr);
}

#undef TLA_MEMORY_DEBUG
#ifdef TLA_MEMORY_DEBUG
static void EnableMTest(void)
{
	msr_t msr;

	msr = rdmsr(GLCP_DELAY_CONTROLS);
	msr.hi &= ~(7 << 20);	/* clear bits 54:52 */
	if (GeodeLinkSpeed() < 200) {
		msr.hi |= 2 << 20;
	}
	wrmsr(GLCP_DELAY_CONTROLS, msr);

	msr = rdmsr(MC_CFCLK_DBUG);
	msr.hi |=
	    CFCLK_UPPER_MTST_B2B_DIS_SET | CFCLK_UPPER_MTEST_EN_SET |
	    CFCLK_UPPER_MTST_RBEX_EN_SET;
	msr.lo |= CFCLK_LOWER_TRISTATE_DIS_SET;
	wrmsr(MC_CFCLK_DBUG, msr);

	printk(BIOS_INFO, "Enabled MTest for TLA debug\n");
}
#endif

void sdram_set_registers(const struct mem_controller *ctrl)
{
	msr_t msr;
	uint32_t msrnum;

	/* Set Timing Control */
	msrnum = MC_CF1017_DATA;
	msr = rdmsr(msrnum);
	msr.lo &= ~(7 << CF1017_LOWER_RD_TMG_CTL_SHIFT);
	if (GeodeLinkSpeed() < 334) {
		msr.lo |= (3 << CF1017_LOWER_RD_TMG_CTL_SHIFT);
	} else {
		msr.lo |= (4 << CF1017_LOWER_RD_TMG_CTL_SHIFT);
	}
	wrmsr(msrnum, msr);

	/* Set Refresh Staggering */
	msrnum = MC_CF07_DATA;
	msr = rdmsr(msrnum);
	msr.lo &= ~0xF0;
	msr.lo |= 0x40;		/* set refresh to 4SDRAM clocks */
	wrmsr(msrnum, msr);

	/* Memory Interleave: Set HOI here otherwise default is LOI */
	/* msrnum = MC_CF8F_DATA;
	   msr = rdmsr(msrnum);
	   msr.hi |= CF8F_UPPER_HOI_LOI_SET;
	   wrmsr(msrnum, msr); */
}

void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
	uint8_t spd_byte;

	banner("sdram_set_spd_register");
	post_code(POST_MEM_SETUP);	// post_70h

	spd_byte = spd_read_byte(DIMM0, SPD_MODULE_ATTRIBUTES);
	banner("Check DIMM 0");
	/* Check DIMM is not Register and not Buffered DIMMs. */
	if ((spd_byte != 0xFF) && (spd_byte & 3)) {
		printk(BIOS_EMERG, "DIMM0 NOT COMPATIBLE\n");
		post_code(ERROR_UNSUPPORTED_DIMM);
		hcf();
	}
	banner("Check DIMM 1");
	spd_byte = spd_read_byte(DIMM1, SPD_MODULE_ATTRIBUTES);
	if ((spd_byte != 0xFF) && (spd_byte & 3)) {
		printk(BIOS_EMERG, "DIMM1 NOT COMPATIBLE\n");
		post_code(ERROR_UNSUPPORTED_DIMM);
		hcf();
	}

	post_code(POST_MEM_SETUP2);	// post_72h
	banner("Check DDR MAX");

	/* Check that the memory is not overclocked. */
	checkDDRMax();

	/* Size the DIMMS */
	post_code(POST_MEM_SETUP3);	// post_73h
	banner("AUTOSIZE DIMM 0");
	auto_size_dimm(DIMM0);
	post_code(POST_MEM_SETUP4);	// post_74h
	banner("AUTOSIZE DIMM 1");
	auto_size_dimm(DIMM1);

	/* Set CAS latency */
	banner("set cas latency");
	post_code(POST_MEM_SETUP5);	// post_75h
	setCAS();

	/* Set all the other latencies here (tRAS, tRP....) */
	banner("set all latency");
	set_latencies();

	/* Set Extended Mode Registers */
	banner("set emrs");
	set_extended_mode_registers();

	banner("set ref rate");
	/* Set Memory Refresh Rate */
	set_refresh_rate();

}

/* Section 6.1.3, LX processor databooks, BIOS Initialization Sequence
 * Section 4.1.4, GX/CS5535 GeodeROM Porting guide */
void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	uint32_t i, msrnum;
	msr_t msr;

/*********************************************************************
;* Turn on MC/DIMM interface per JEDEC
;* 1) Clock stabilizes > 200us
;* 2) Assert CKE
;* 3) Precharge All to put all banks into an idles state
;* 4) EMRS to enable DLL
;* 6) MRS w/ memory config & reset DLL set
;* 7) Wait 200 clocks (2us)
;* 8) Precharge All and 2 Auto refresh
;* 9) MRS w/ memory config & reset DLL clear
;* 8) DDR SDRAM ready for normal operation
;********************************************************************/
	post_code(POST_MEM_ENABLE);	// post_76h

#ifdef TLA_MEMORY_DEBUG
	/* Only enable MTest for TLA memory debug */
	EnableMTest();
#endif

	/* If both Page Size = "Not Installed" we have a problems and should halt. */
	msr = rdmsr(MC_CF07_DATA);
	if ((msr.hi & ((7 << CF07_UPPER_D1_PSZ_SHIFT) | (7 << CF07_UPPER_D0_PSZ_SHIFT))) ==
			((7 << CF07_UPPER_D1_PSZ_SHIFT) | (7 << CF07_UPPER_D0_PSZ_SHIFT))) {
		printk(BIOS_EMERG, "No memory in the system\n");
		post_code(ERROR_NO_DIMMS);
		hcf();
	}

	/*      Set CKEs */
	msrnum = MC_CFCLK_DBUG;
	msr = rdmsr(msrnum);
	msr.lo &= ~(CFCLK_LOWER_MASK_CKE_SET0 | CFCLK_LOWER_MASK_CKE_SET1);
	wrmsr(msrnum, msr);

	/* Force Precharge All on next command, EMRS */
	msrnum = MC_CFCLK_DBUG;
	msr = rdmsr(msrnum);
	msr.lo |= CFCLK_LOWER_FORCE_PRE_SET;
	wrmsr(msrnum, msr);

	/* EMRS to enable DLL (pre-setup done in setExtendedModeRegisters) */
	msrnum = MC_CF07_DATA;
	msr = rdmsr(msrnum);
	msr.lo |= CF07_LOWER_PROG_DRAM_SET | CF07_LOWER_LOAD_MODE_DDR_SET;
	wrmsr(msrnum, msr);
	msr.lo &= ~(CF07_LOWER_PROG_DRAM_SET | CF07_LOWER_LOAD_MODE_DDR_SET);
	wrmsr(msrnum, msr);

	/* Clear Force Precharge All */
	msrnum = MC_CFCLK_DBUG;
	msr = rdmsr(msrnum);
	msr.lo &= ~CFCLK_LOWER_FORCE_PRE_SET;
	wrmsr(msrnum, msr);

	/* MRS Reset DLL - set */
	msrnum = MC_CF07_DATA;
	msr = rdmsr(msrnum);
	msr.lo |= CF07_LOWER_PROG_DRAM_SET | CF07_LOWER_LOAD_MODE_DLL_RESET;
	wrmsr(msrnum, msr);
	msr.lo &= ~(CF07_LOWER_PROG_DRAM_SET | CF07_LOWER_LOAD_MODE_DLL_RESET);
	wrmsr(msrnum, msr);

	/* 2us delay (200 clocks @ 200MHz). We probably really don't need this but.... better safe. */
	/* Wait 2 PORT61 ticks. between 15us and 30us */
	/* This would be endless if the timer is stuck. */
	while ((inb(0x61)));	/* find the first edge */
	while (!(~inb(0x61)));

	/* Force Precharge All on the next command, auto-refresh */
	msrnum = MC_CFCLK_DBUG;
	msr = rdmsr(msrnum);
	msr.lo |= CFCLK_LOWER_FORCE_PRE_SET;
	wrmsr(msrnum, msr);

	/* Manually AUTO refresh #1 */
	/* If auto refresh was not enabled above we would need to do 8 refreshes to prime the pump before these 2. */
	msrnum = MC_CF07_DATA;
	msr = rdmsr(msrnum);
	msr.lo |= CF07_LOWER_REF_TEST_SET;
	wrmsr(msrnum, msr);
	msr.lo &= ~CF07_LOWER_REF_TEST_SET;
	wrmsr(msrnum, msr);

	/* Clear Force Precharge All */
	msrnum = MC_CFCLK_DBUG;
	msr = rdmsr(msrnum);
	msr.lo &= ~CFCLK_LOWER_FORCE_PRE_SET;
	wrmsr(msrnum, msr);

	/* Manually AUTO refresh */
	/* The MC should insert the right delay between the refreshes */
	msrnum = MC_CF07_DATA;
	msr = rdmsr(msrnum);
	msr.lo |= CF07_LOWER_REF_TEST_SET;
	wrmsr(msrnum, msr);
	msr.lo &= ~CF07_LOWER_REF_TEST_SET;
	wrmsr(msrnum, msr);

	/* MRS Reset DLL - clear */
	msrnum = MC_CF07_DATA;
	msr = rdmsr(msrnum);
	msr.lo |= CF07_LOWER_PROG_DRAM_SET;
	wrmsr(msrnum, msr);
	msr.lo &= ~CF07_LOWER_PROG_DRAM_SET;
	wrmsr(msrnum, msr);

	/* Allow MC to tristate during idle cycles with MTEST OFF */
	msrnum = MC_CFCLK_DBUG;
	msr = rdmsr(msrnum);
	msr.lo &= ~CFCLK_LOWER_TRISTATE_DIS_SET;
	wrmsr(msrnum, msr);

	/* Disable SDCLK DIMM1 slot if no DIMM installed to save power. */
	msr = rdmsr(MC_CF07_DATA);
	if ((msr.hi & (7 << CF07_UPPER_D1_PSZ_SHIFT)) ==
	    (7 << CF07_UPPER_D1_PSZ_SHIFT)) {
		msrnum = GLCP_DELAY_CONTROLS;
		msr = rdmsr(msrnum);
		msr.hi |= (1 << 23);	/* SDCLK bit for 2.0 */
		wrmsr(msrnum, msr);
	}

	/* Set PMode0 Sensitivity Counter */
	msr.lo = 0;		/* pmode 0 = 0 most aggressive */
	msr.hi = 0x200;		/* pmode 1 = 200h */
	wrmsr(MC_CF_PMCTR, msr);

	/* Set PMode1 Up delay enable */
	msrnum = MC_CF1017_DATA;
	msr = rdmsr(msrnum);
	msr.lo |= (209 << 8);	/* bits[15:8] = 209 */
	wrmsr(msrnum, msr);

	banner("DRAM controller init done.\n");
	post_code(POST_MEM_SETUP_GOOD);	//0x7E

	/* make sure there is nothing stale in the cache */
	/* CAR stack is in the cache __asm__ __volatile__("wbinvd\n"); */

	/* The RAM dll needs a write to lock on so generate a few dummy writes */
	/* Note: The descriptor needs to be enabled to point at memory */
	for (i = 0; i < 5; i++) {
		write32(zeroptr + i, i);
	}
	/* SWAPSiF for PBZ 4112 (Errata 34) */
	/* check for failed DLL settings now that we have done a memory write. */
	msrnum = GLCP_DELAY_CONTROLS;
	msr = rdmsr(msrnum);
	if ((msr.lo & 0x7FF) == 0x104) {

		/* If you had it you would need to clear out the fail boot count flag */
		/*       (depending on where it counts from etc). */

		/* The reset we are about to perform clears the PM_SSC register in the */
		/*       5536 so will need to store the S3 resume flag in NVRAM otherwise */
		/*       it would do a normal boot */

		/* Reset the system */
		msrnum = MDD_SOFT_RESET;
		msr = rdmsr(msrnum);
		msr.lo |= 1;
		wrmsr(msrnum, msr);
	}
	printk(BIOS_INFO, "RAM DLL lock\n");

}
