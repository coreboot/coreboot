/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Nils Jacobs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <cpu/amd/gx2def.h>
#include <spd.h>

static const unsigned char NumColAddr[] = {
	0x00, 0x10, 0x11, 0x00, 0x00, 0x00, 0x00, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};

static void hcf(void)
{
	printk(BIOS_EMERG, "DIE\n");
	/* this guarantees we flush the UART fifos (if any) and also
	 * ensures that things, in general, keep going so no debug output
	 * is lost
	 */
	while (1)
		printk(BIOS_EMERG, (0));
}

static void auto_size_dimm(unsigned int dimm)
{
	uint32_t dimm_setting;
	uint16_t dimm_size;
	uint8_t spd_byte;
	msr_t msr;

	dimm_setting = 0;

	printk(BIOS_DEBUG, "Check present\n");
	/* Check that we have a dimm */
	if (spd_read_byte(dimm, SPD_MEMORY_TYPE) == 0xFF) {
		return;
	}

	printk(BIOS_DEBUG, "MODBANKS\n");
	/* Field: Module Banks per DIMM */
	/* EEPROM byte usage: (5) Number of DIMM Banks */
	spd_byte = spd_read_byte(dimm, SPD_NUM_DIMM_BANKS);
	if ((MIN_MOD_BANKS > spd_byte) || (spd_byte > MAX_MOD_BANKS)) {
		printk(BIOS_EMERG, "Number of module banks not compatible\n");
		post_code(ERROR_BANK_SET);
		hcf();
	}
	dimm_setting |= (spd_byte >> 1) << CF07_UPPER_D0_MB_SHIFT;

	printk(BIOS_DEBUG, "FIELDBANKS\n");
	/* Field: Banks per SDRAM device */
	/* EEPROM byte usage: (17) Number of Banks on SDRAM Device */
	spd_byte = spd_read_byte(dimm, SPD_NUM_BANKS_PER_SDRAM);
	if ((MIN_DEV_BANKS > spd_byte) || (spd_byte > MAX_DEV_BANKS)) {
		printk(BIOS_EMERG, "Number of device banks not compatible\n");
		post_code(ERROR_BANK_SET);
		hcf();
	}
	dimm_setting |= (spd_byte >> 2) << CF07_UPPER_D0_CB_SHIFT;

	printk(BIOS_DEBUG, "SPDNUMROWS\n");
	/* Field: DIMM size
	 * EEPROM byte usage:
	 *   (3)  Number of Row Addresses
	 *   (4)  Number of Column Addresses
	 *   (5)  Number of DIMM Banks
	 *   (31) Module Bank Density
	 * Size = Module Density * Module Banks
	 */
	if ((spd_read_byte(dimm, SPD_NUM_ROWS) & 0xF0)
	    || (spd_read_byte(dimm, SPD_NUM_COLUMNS) & 0xF0)) {
		printk(BIOS_EMERG, "Asymmetric DIMM not compatible\n");
		post_code(ERROR_UNSUPPORTED_DIMM);
		hcf();
	}

	printk(BIOS_DEBUG, "SPDBANKDENSITY\n");
	dimm_size = spd_read_byte(dimm, SPD_BANK_DENSITY);
	printk(BIOS_DEBUG, "DIMMSIZE\n");
	dimm_size |= (dimm_size << 8);	/* align so 1GB(bit0) is bit 8, this is a little weird to get gcc to not optimize this out */
	dimm_size &= 0x01FC;	/* and off 2GB DIMM size : not supported and the 1GB size we just moved up to bit 8 as well as all the extra on top */

	/* Module Density * Module Banks */
	dimm_size <<= (dimm_setting >> CF07_UPPER_D0_MB_SHIFT) & 1;	/* shift to multiply by # DIMM banks */
	printk(BIOS_DEBUG, "BEFORT CTZ\n");
	dimm_size = __builtin_ctz(dimm_size);
	printk(BIOS_DEBUG, "TEST DIMM SIZE>7\n");
	if (dimm_size > 7) {	/* 7 is 512MB only support 512MB per DIMM */
		printk(BIOS_EMERG, "Only support up to 512MB per DIMM\n");
		post_code(ERROR_DENSITY_DIMM);
		hcf();
	}
	dimm_setting |= dimm_size << CF07_UPPER_D0_SZ_SHIFT;
	printk(BIOS_DEBUG, "PAGESIZE\n");

/*
 * Field: PAGE size
 * EEPROM byte usage: (4)  Number of Column Addresses
 * PageSize = 2^# Column Addresses * Data width in bytes
 *                                   (should be 8bytes for a normal DIMM)
 *
 * But this really works by magic.
 * If ma[11:0] is the memory address pins, and pa[13:0] is the physical column
 * address that MC generates, here is how the MC assigns the pa onto the
 * ma pins:
 *
 * ma  11 10 09 08 07 06 05 04 03 02 01 00
 * ---------------------------------------
 * pa                 09 08 07 06 05 04 03  (7 col addr bits = 1K page size)
 * pa              10 09 08 07 06 05 04 03  (8 col addr bits = 2K page size)
 * pa           11 10 09 08 07 06 05 04 03  (9 col addr bits = 4K page size)
 * pa        12 11 10 09 08 07 06 05 04 03  (10 col addr bits = 8K page size)
 * pa  13 AP 12 11 10 09 08 07 06 05 04 03  (11 col addr bits = 16K page size)
 *
 * (AP = autoprecharge bit)
 *
 * Remember that pa[2:0] are zeroed out since it's a 64-bit data bus (8 bytes),
 * so lower 3 address bits are dont_cares. So from the table above,
 * it's easier to see what the old code is doing: if for example,
 * #col_addr_bits=7(06h), it adds 3 to get 10, then does 2^10=1K.
 */

	spd_byte = NumColAddr[spd_read_byte(dimm, SPD_NUM_COLUMNS) & 0xF];
	printk(BIOS_DEBUG, "MAXCOLADDR\n");
	if (spd_byte > MAX_COL_ADDR) {
		printk(BIOS_EMERG, "DIMM page size not compatible\n");
		post_code(ERROR_SET_PAGE);
		hcf();
	}
	printk(BIOS_DEBUG, ">11address test\n");
	spd_byte -= 7;
	if (spd_byte > 4) {	/* if the value is above 4 it means >11 col address lines */
		spd_byte = 7;	/* which means >16k so set to disabled */
	}
	dimm_setting |= spd_byte << CF07_UPPER_D0_PSZ_SHIFT;	/* 0=1k,1=2k,2=4k,etc */

	printk(BIOS_DEBUG, "RDMSR CF07\n");
	msr = rdmsr(MC_CF07_DATA);
	printk(BIOS_DEBUG, "WRMSR CF07\n");
	if (dimm == DIMM0) {
		msr.hi &= 0xFFFF0000;
		msr.hi |= dimm_setting;
	} else {
		msr.hi &= 0x0000FFFF;
		msr.hi |= dimm_setting << 16;
	}
	wrmsr(MC_CF07_DATA, msr);
	printk(BIOS_DEBUG, "ALL DONE\n");
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

	/* Use the slowest DIMM */
	if (spd_byte0 < spd_byte1) {
		spd_byte0 = spd_byte1;
	}

	/* Turn SPD ns time into MHZ. Check what the asm does to this math. */
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

const uint8_t CASDDR[] = { 5, 5, 2, 6, 0 };	/* 1(1.5), 1.5, 2, 2.5, 0 */

static u8 getcasmap(u32 dimm, u16 glspeed)
{
	u16 dimm_speed;
	u8 spd_byte, casmap, casmap_shift=0;

	/**************************	 DIMM0	**********************************/
	casmap = spd_read_byte(dimm, SPD_ACCEPTABLE_CAS_LATENCIES);
	if (casmap != 0xFF) {
		/* IF -.5 timing is supported, check -.5 timing > GeodeLink */
		spd_byte = spd_read_byte(dimm, SPD_SDRAM_CYCLE_TIME_2ND);
		if (spd_byte != 0) {
			/* Turn SPD ns time into MHZ. Check what the asm does to this math. */
			dimm_speed = 20000 / (((spd_byte >> 4) * 10) + (spd_byte & 0x0F));
			if (dimm_speed >= glspeed) {
				casmap_shift = 1; /* -.5 is a shift of 1 */
				/* IF -1 timing is supported, check -1 timing > GeodeLink */
				spd_byte = spd_read_byte(dimm, SPD_SDRAM_CYCLE_TIME_3RD);
				if (spd_byte != 0) {
					/* Turn SPD ns time into MHZ. Check what the asm does to this math. */
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
/*
 *	setCAS
 *	EEPROM byte usage: (18) SDRAM device attributes - CAS latency
 *	EEPROM byte usage: (23) SDRAM Minimum Clock Cycle Time @ CLX -.5
 *	EEPROM byte usage: (25) SDRAM Minimum Clock Cycle Time @ CLX -1
 *
 *	The CAS setting is based on the information provided in each DIMMs SPD.
 *	 The speed at which a DIMM can run is described relative to the slowest
 *	 CAS the DIMM supports. Each speed for the relative CAS settings is
 *	 checked that it is within the GeodeLink speed. If it isn't within the GeodeLink
 *	 speed, the CAS setting	 is removed from the list of good settings for
 *	 the DIMM. This is done for both DIMMs and the lists are compared to
 *	 find the lowest common CAS latency setting. If there are no CAS settings
 *	 in common we out a ERROR_DIFF_DIMMS (78h) to port 80h and halt.
 *
 *	Entry:
 *	Exit: Set fastest CAS Latency based on GeodeLink speed and SPD information.
 *	Destroys: We really use everything !
 */
	uint16_t glspeed;
	uint8_t spd_byte, casmap0, casmap1;
	msr_t msr;

	glspeed = GeodeLinkSpeed();

	casmap0 = getcasmap(DIMM0, glspeed);
	casmap1 = getcasmap(DIMM1, glspeed);

	/* CAS_LAT MAP COMPARE */
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
	/* (ns/(1/MHz) = (us*MHZ)/1000 = clocks/1000 = clocks) */
	spd_byte1 = (spd_byte0 * memspeed) / 1000;
	if (((spd_byte0 * memspeed) % 1000)) {
		++spd_byte1;
	}
	if (spd_byte1 > 6) {
		--spd_byte1;
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
	/* (ns/(1/MHz) = (us*MHZ)/1000 = clocks/1000 = clocks) */
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
	/* (ns/(1/MHz) = (us*MHZ)/1000 = clocks/1000 = clocks) */
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
	/* (ns/(1/MHz) = (us*MHZ)/1000 = clocks/1000 = clocks) */
	spd_byte1 = ((spd_byte0 >> 2) * memspeed) / 1000;
	if ((((spd_byte0 >> 2) * memspeed) % 1000)) {
		++spd_byte1;
	}
	dimm_setting |= spd_byte1 << CF8F_LOWER_ACT2ACT_SHIFT;

	/* tRC = tRP + tRAS */
	dimm_setting |= (((dimm_setting >> CF8F_LOWER_ACT2PRE_SHIFT) & 0x0F) +
	     		((dimm_setting >> CF8F_LOWER_PRE2ACT_SHIFT) & 0x07))
	    			<< CF8F_LOWER_REF2ACT_SHIFT;

	msr = rdmsr(MC_CF8F_DATA);
	msr.lo &= 0xF00000FF;
	msr.lo |= dimm_setting;
	msr.hi |= CF8F_UPPER_REORDER_DIS_SET;
	wrmsr(MC_CF8F_DATA, msr);
	printk(BIOS_DEBUG, "MSR MC_CF8F_DATA (%08x) value is %08x:%08x\n",
	MC_CF8F_DATA, msr.hi, msr.lo);
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

static void sdram_set_registers(const struct mem_controller *ctrl)
{
	msr_t msr;
	uint32_t msrnum;

	/* Set Refresh Staggering */
	msrnum = MC_CF07_DATA;
	msr = rdmsr(msrnum);
	msr.lo &= ~0xC0;
	msr.lo |= 0x0;		/* set refresh to 4SDRAM clocks */
	wrmsr(msrnum, msr);
}

static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
	uint8_t spd_byte;

	printk(BIOS_DEBUG, "sdram_set_spd_register\n");
	post_code(POST_MEM_SETUP);	/* post_70h */

	spd_byte = spd_read_byte(DIMM0, SPD_MODULE_ATTRIBUTES);
	printk(BIOS_DEBUG, "Check DIMM 0\n");
	/* Check DIMM is not Register and not Buffered DIMMs. */
	if ((spd_byte != 0xFF) && (spd_byte & 3)) {
		printk(BIOS_EMERG, "DIMM0 NOT COMPATIBLE\n");
		post_code(ERROR_UNSUPPORTED_DIMM);
		hcf();
	}
	printk(BIOS_DEBUG, "Check DIMM 1\n");
	spd_byte = spd_read_byte(DIMM1, SPD_MODULE_ATTRIBUTES);
	if ((spd_byte != 0xFF) && (spd_byte & 3)) {
		printk(BIOS_EMERG, "DIMM1 NOT COMPATIBLE\n");
		post_code(ERROR_UNSUPPORTED_DIMM);
		hcf();
	}

	post_code(POST_MEM_SETUP2);	/* post_72h */
	printk(BIOS_DEBUG, "Check DDR MAX\n");

	/* Check that the memory is not overclocked. */
	checkDDRMax();

	/* Size the DIMMS */
	post_code(POST_MEM_SETUP3);	/* post_73h */
	printk(BIOS_DEBUG, "AUTOSIZE DIMM 0\n");
	auto_size_dimm(DIMM0);
	post_code(POST_MEM_SETUP4);	/* post_74h */
	printk(BIOS_DEBUG, "AUTOSIZE DIMM 1\n");
	auto_size_dimm(DIMM1);

	/* Set CAS latency */
	printk(BIOS_DEBUG, "set cas latency\n");
	post_code(POST_MEM_SETUP5);	/* post_75h */
	setCAS();

	/* Set all the other latencies here (tRAS, tRP....) */
	printk(BIOS_DEBUG, "set all latency\n");
	set_latencies();

	/* Set Extended Mode Registers */
	printk(BIOS_DEBUG, "set emrs\n");
	set_extended_mode_registers();

	printk(BIOS_DEBUG, "set ref rate\n");
	/* Set Memory Refresh Rate */
	set_refresh_rate();
}

/* Section 6.1.3, LX processor databooks, BIOS Initialization Sequence
 * Section 4.1.4, GX/CS5535 GeodeROM Porting guide */
static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;
	msr_t msr;

	/* 2. clock gating for PMode */
	msr = rdmsr(MC_GLD_MSR_PM);
	msr.lo &= ~0x04;
	msr.lo |=  0x01;
	wrmsr(MC_GLD_MSR_PM, msr);
	/* undocmented bits in GX, in LX there are
	 * 8 bits in PM1_UP_DLY */
	msr = rdmsr(MC_CF1017_DATA);
	msr.lo = 0x0101;
	wrmsr(MC_CF1017_DATA, msr);
	printk(BIOS_DEBUG, "sdram_enable step 2\n");

	/* 3. release CKE mask to enable CKE */
	msr = rdmsr(MC_CFCLK_DBUG);
	msr.lo &= ~(0x03 << 8);
	wrmsr(MC_CFCLK_DBUG, msr);
	printk(BIOS_DEBUG, "sdram_enable step 3\n");

	/* 4. set and clear REF_TST 16 times, more shouldn't hurt
	 * why this is before EMRS and MRS ? */
	for (i = 0; i < 19; i++) {
		msr = rdmsr(MC_CF07_DATA);
		msr.lo |=  (0x01 << 3);
		wrmsr(MC_CF07_DATA, msr);
		msr.lo &= ~(0x01 << 3);
		wrmsr(MC_CF07_DATA, msr);
	}
	printk(BIOS_DEBUG, "sdram_enable step 4\n");

	/* 6. enable DLL, load Extended Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |=  ((0x01 << 28) | 0x01);
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~((0x01 << 28) | 0x01);
	wrmsr(MC_CF07_DATA, msr);
	printk(BIOS_DEBUG, "sdram_enable step 6\n");

	/* 7. Reset DLL, Bit 27 is undocumented in GX datasheet,
	 * it is documented in LX datasheet */
	/* load Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |=  ((0x01 << 27) | 0x01);
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~((0x01 << 27) | 0x01);
	wrmsr(MC_CF07_DATA, msr);
	printk(BIOS_DEBUG, "sdram_enable step 7\n");

	/* 8. load Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |=  0x01;
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~0x01;
	wrmsr(MC_CF07_DATA, msr);
	printk(BIOS_DEBUG, "sdram_enable step 8\n");

	/* wait 200 SDCLKs */
	for (i = 0; i < 200; i++)
		outb(0xaa, 0x80);

	/* load RDSYNC */
	msr = rdmsr(MC_CF_RDSYNC);
	msr.hi = 0x000ff310;
	/* the above setting is supposed to be good for "slow" ram. We have found that for
	 * some dram, at some clock rates, e.g. hynix at 366/244, this will actually
	 * cause errors. The fix is to just set it to 0x310. Tested on 3 boards
	 * with 3 different type of dram -- Hynix, PSC, infineon.
	 * I am leaving this comment here so that at some future time nobody is tempted
	 * to mess with this setting -- RGM, 9/2006
	 */
	msr.hi = 0x00000310;
	msr.lo = 0x00000000;
	wrmsr(MC_CF_RDSYNC, msr);

	/* set delay control */
	msr = rdmsr(GLCP_DELAY_CONTROLS);
	msr.hi = 0x830d415a;
	msr.lo = 0x8ea0ad6a;
	wrmsr(GLCP_DELAY_CONTROLS, msr);

	/* The RAM dll needs a write to lock on so generate a few dummy writes */
	/* Note: The descriptor needs to be enabled to point at memory */
	volatile unsigned long *ptr;
	for (i = 0; i < 5; i++) {
		ptr = (void *)i;
		*ptr = (unsigned long)i;
	}

	print_info("RAM DLL lock\n");

}
