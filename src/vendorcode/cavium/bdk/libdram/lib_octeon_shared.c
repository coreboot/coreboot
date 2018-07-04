/***********************license start***********************************
* Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
* reserved.
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/

/* $Revision: 102369 $ */

#include <bdk.h>
#include "libbdk-arch/bdk-csrs-l2c.h"
#include "dram-internal.h"

#include "dram-env.h"
#include <libbdk-hal/bdk-rng.h>
#include <lame_string.h>

/* Define DDR_DEBUG to debug the DDR interface.  This also enables the
** output necessary for review by Cavium Inc., Inc. */
/* #define DDR_DEBUG */


static int global_ddr_clock_initialized = 0;
static int global_ddr_memory_preserved  = 0;

#if 1
uint64_t max_p1 = 0UL;
#endif

/*
 * SDRAM Physical Address (figure 6-2 from the HRM)
 *                                                                   7 6    3 2   0
 * +---------+----+----------------------+---------------+--------+---+------+-----+
 * |  Dimm   |Rank|         Row          |      Col      |  Bank  | C |  Col | Bus |
 * +---------+----+----------------------+---------------+--------+---+------+-----+
 *     |    ^   |            |          ^        |           |      |
 *   0 or 1 |   |       12-18 bits      |      6-8 bits      |    1 or 2 bits
 *    bit   | 0 or 1 bit           LMC_CONFIG[ROW_LSB]+X     |    (X=1 or 2, resp)
 *          |                                                |
 *   LMC_CONFIG[PBANK_LSB]+X                               3 or 4 bits
 *
 *    Bus     = Selects the byte on the 72-bit DDR3 bus
 *    Col     = Column Address for the memory part (10-12 bits)
 *    C       = Selects the LMC that services the reference
 *              (2 bits for 4 LMC mode, 1 bit for 2 LMC mode; X=width)
 *    Bank    = Bank Address for the memory part (DDR3=3 bits, DDR4=3 or 4 bits)
 *    Row     = Row Address for the memory part (12-18 bits)
 *    Rank    = Optional Rank Address for dual-rank DIMMs
 *              (present when LMC_CONFIG[RANK_ENA] is set)
 *    Dimm    = Optional DIMM address (preseent with more than 1 DIMM)
 */


/**
 * Divide and round results to the nearest integer.
 *
 * @param dividend
 * @param divisor
 *
 * @return
 */
uint64_t divide_nint(uint64_t dividend, uint64_t divisor)
{
    uint64_t quotent, remainder;
    quotent   = dividend / divisor;
    remainder = dividend % divisor;
    return quotent + ((remainder * 2) >= divisor);
}

/* Sometimes the pass/fail results for all possible delay settings
 * determined by the read-leveling sequence is too forgiving.  This
 * usually occurs for DCLK speeds below 300 MHz. As a result the
 * passing range is exaggerated. This function accepts the bitmask
 * results from the sequence and truncates the passing range to a
 * reasonable range and recomputes the proper deskew setting.
 */

/* Default ODT config must disable ODT */
/* Must be const (read only) so that the structure is in flash */
const dimm_odt_config_t disable_odt_config[] = {
	/* DDR4 needs an additional field in the struct (odt_mask2) */
	/* DIMMS   ODT_ENA ODT_MASK   ODT_MASK1      ODT_MASK2     QS_DIC RODT_CTL */
	/* =====   ======= ========   =========      =========     ====== ======== */
	/*   1 */ {   0,    0x0000,   {.u = 0x0000}, {.u = 0x0000},    0,   0x0000  },
	/*   2 */ {   0,    0x0000,   {.u = 0x0000}, {.u = 0x0000},    0,   0x0000  },
	/*   3 */ {   0,    0x0000,   {.u = 0x0000}, {.u = 0x0000},    0,   0x0000  },
	/*   4 */ {   0,    0x0000,   {.u = 0x0000}, {.u = 0x0000},    0,   0x0000  },
};
/* Memory controller setup function */
static int init_octeon_dram_interface(bdk_node_t node,
				      const ddr_configuration_t *ddr_configuration,
				      uint32_t ddr_hertz,
				      uint32_t cpu_hertz,
				      uint32_t ddr_ref_hertz,
				      int board_type,
				      int board_rev_maj,
				      int board_rev_min,
				      int ddr_interface_num,
				      uint32_t ddr_interface_mask)
{
    uint32_t mem_size_mbytes = 0;
    int lmc_restart_retries = 0;

    const char *s;
    if ((s = lookup_env_parameter("ddr_timing_hertz")) != NULL)
	ddr_hertz = strtoul(s, NULL, 0);

 restart_lmc_init:

    /* Poke the watchdog timer so it doesn't expire during DRAM init */
    bdk_watchdog_poke();

    mem_size_mbytes = init_octeon3_ddr3_interface(node,
						  ddr_configuration,
						  ddr_hertz,
						  cpu_hertz,
						  ddr_ref_hertz,
						  board_type,
						  board_rev_maj,
						  board_rev_min,
						  ddr_interface_num,
						  ddr_interface_mask);
#define DEFAULT_RESTART_RETRIES 3
    if (mem_size_mbytes == 0) { // means restart is possible
        if (lmc_restart_retries < DEFAULT_RESTART_RETRIES) {
            lmc_restart_retries++;
            ddr_print("N%d.LMC%d Configuration problem: attempting LMC reset and init restart %d\n",
                        node, ddr_interface_num, lmc_restart_retries);
            // re-assert RESET first, as that is the assumption of the init code
            if (!ddr_memory_preserved(node))
                cn88xx_lmc_ddr3_reset(node, ddr_interface_num, LMC_DDR3_RESET_ASSERT);
            goto restart_lmc_init;
        } else {
            error_print("INFO: N%d.LMC%d Configuration: fatal problem remains after %d LMC init retries - Resetting node...\n",
                        node, ddr_interface_num, lmc_restart_retries);
            bdk_wait_usec(500000);
            bdk_reset_chip(node);
        }
    }

    printf("N%d.LMC%d Configuration Completed: %d MB\n",
           node, ddr_interface_num, mem_size_mbytes);
    return mem_size_mbytes;
}

#define DO_LIKE_RANDOM_XOR 1

#if !DO_LIKE_RANDOM_XOR
/*
 * Suggested testing patterns.
 *
 *  0xFFFF_FFFF_FFFF_FFFF
 *  0xAAAA_AAAA_AAAA_AAAA
 *  0xFFFF_FFFF_FFFF_FFFF
 *  0xAAAA_AAAA_AAAA_AAAA
 *  0x5555_5555_5555_5555
 *  0xAAAA_AAAA_AAAA_AAAA
 *  0xFFFF_FFFF_FFFF_FFFF
 *  0xAAAA_AAAA_AAAA_AAAA
 *  0xFFFF_FFFF_FFFF_FFFF
 *  0x5555_5555_5555_5555
 *  0xFFFF_FFFF_FFFF_FFFF
 *  0x5555_5555_5555_5555
 *  0xAAAA_AAAA_AAAA_AAAA
 *  0x5555_5555_5555_5555
 *  0xFFFF_FFFF_FFFF_FFFF
 *  0x5555_5555_5555_5555
 *
 *  or possibly
 *
 *  0xFDFD_FDFD_FDFD_FDFD
 *  0x8787_8787_8787_8787
 *  0xFEFE_FEFE_FEFE_FEFE
 *  0xC3C3_C3C3_C3C3_C3C3
 *  0x7F7F_7F7F_7F7F_7F7F
 *  0xE1E1_E1E1_E1E1_E1E1
 *  0xBFBF_BFBF_BFBF_BFBF
 *  0xF0F0_F0F0_F0F0_F0F0
 *  0xDFDF_DFDF_DFDF_DFDF
 *  0x7878_7878_7878_7878
 *  0xEFEF_EFEF_EFEF_EFEF
 *  0x3C3C_3C3C_3C3C_3C3C
 *  0xF7F7_F7F7_F7F7_F7F7
 *  0x1E1E_1E1E_1E1E_1E1E
 *  0xFBFB_FBFB_FBFB_FBFB
 *  0x0F0F_0F0F_0F0F_0F0F
 */

static const uint64_t test_pattern[] = {
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
};
#endif  /* !DO_LIKE_RANDOM_XOR */

int test_dram_byte(bdk_node_t node, int lmc, uint64_t p, uint64_t bitmask, uint64_t *xor_data)
{
    uint64_t p1, p2, d1, d2;
    uint64_t v, v1;
    uint64_t p2offset = 0x10000000;
    uint64_t datamask;
    uint64_t xor;
    int i, j, k;
    int errors = 0;
    int index;
#if DO_LIKE_RANDOM_XOR
    uint64_t pattern1 = bdk_rng_get_random64();
    uint64_t this_pattern;
#endif
    uint64_t bad_bits[2] = {0,0};

    // When doing in parallel, the caller must provide full 8-byte bitmask.
    // Byte lanes may be clear in the mask to indicate no testing on that lane.
    datamask = bitmask;

    // final address must include LMC and node
    p |= (lmc<<7); /* Map address into proper interface */
    p = bdk_numa_get_address(node, p); /* Map to node */

    // Not on THUNDER:	p |= 1ull<<63;

    /* Add offset to both test regions to not clobber boot stuff
     * when running from L2.
     */
    p += 0x10000000; // FIXME? was: 0x4000000; // make sure base is out of the way of boot

    /* The loop ranges and increments walk through a range of addresses avoiding bits that alias
     * to different memory interfaces (LMCs) on the CN88XX; ie we want to limit activity to a
     * single memory channel.
     */

    /* Store something into each location first */
    // NOTE: the ordering of loops is purposeful: fill full cachelines and flush
    for (k = 0; k < (1 << 20); k += (1 << 14)) {
	for (j = 0; j < (1 << 12); j += (1 << 9)) {
	    for (i = 0; i < (1 << 7); i += 8) {
		index = i + j + k;
		p1 = p + index;
		p2 = p1 + p2offset;

#if DO_LIKE_RANDOM_XOR
		v = pattern1 * p1;
		v1 = v; // write the same thing to both areas
#else
		v = 0ULL;
		v1 = v;
#endif
		__bdk_dram_write64(p1, v);
		__bdk_dram_write64(p2, v1);

		/* Write back and invalidate the cache lines
		 *
		 * For OCX we cannot limit the number of L2 ways
		 * so instead we just write back and invalidate
		 * the L2 cache lines.  This is not possible
		 * when booting remotely, however so this is
		 * only enabled for U-Boot right now.
		 * Potentially the BDK can also take advantage
		 * of this.
		 */
		BDK_CACHE_WBI_L2(p1);
		BDK_CACHE_WBI_L2(p2);
	    }
	}
    }

    BDK_DCACHE_INVALIDATE;

#if DO_LIKE_RANDOM_XOR
    this_pattern = bdk_rng_get_random64();
#endif

    // modify the contents of each location in some way
    // NOTE: the ordering of loops is purposeful: modify full cachelines and flush
    for (k = 0; k < (1 << 20); k += (1 << 14)) {
	for (j = 0; j < (1 << 12); j += (1 << 9)) {
	    for (i = 0; i < (1 << 7); i += 8) {
		index = i + j + k;
		p1 = p + index;
		p2 = p1 + p2offset;
#if DO_LIKE_RANDOM_XOR
		v  = __bdk_dram_read64(p1) ^ this_pattern;
		v1 = __bdk_dram_read64(p2) ^ this_pattern;
#else
		v = test_pattern[index%(sizeof(test_pattern)/sizeof(uint64_t))];
		v &= datamask;
		v1 = ~v;
#endif

		debug_print("[0x%016llX]: 0x%016llX, [0x%016llX]: 0x%016llX\n",
		            p1, v, p2, v1);

		__bdk_dram_write64(p1, v);
		__bdk_dram_write64(p2, v1);

		/* Write back and invalidate the cache lines
		 *
		 * For OCX we cannot limit the number of L2 ways
		 * so instead we just write back and invalidate
		 * the L2 cache lines.  This is not possible
		 * when booting remotely, however so this is
		 * only enabled for U-Boot right now.
		 * Potentially the BDK can also take advantage
		 * of this.
		 */
		BDK_CACHE_WBI_L2(p1);
		BDK_CACHE_WBI_L2(p2);
	    }
	}
    }

    BDK_DCACHE_INVALIDATE;

    // test the contents of each location by predicting what should be there
    // NOTE: the ordering of loops is purposeful: test full cachelines to detect
    //       an error occuring in any slot thereof
    for (k = 0; k < (1 << 20); k += (1 << 14)) {
	for (j = 0; j < (1 << 12); j += (1 << 9)) {
	    for (i = 0; i < (1 << 7); i += 8) {
		index = i + j + k;
		p1 = p + index;
		p2 = p1 + p2offset;
#if DO_LIKE_RANDOM_XOR
		v = (p1 * pattern1) ^ this_pattern; // FIXME: this should predict what we find...???
		d1 = __bdk_dram_read64(p1);
		d2 = __bdk_dram_read64(p2);
#else
		v = test_pattern[index%(sizeof(test_pattern)/sizeof(uint64_t))];
		d1 = __bdk_dram_read64(p1);
		d2 = ~__bdk_dram_read64(p2);
#endif
		debug_print("[0x%016llX]: 0x%016llX, [0x%016llX]: 0x%016llX\n",
                            p1, d1, p2, d2);

		xor = ((d1 ^ v) | (d2 ^ v)) & datamask; // union of error bits only in active byte lanes

                if (!xor)
                    continue;

                // accumulate bad bits
                bad_bits[0] |= xor;
                //bad_bits[1] |= ~mpr_data1 & 0xffUL; // cannot do ECC here

		int bybit = 1;
		uint64_t bymsk = 0xffULL; // start in byte lane 0
		while (xor != 0) {
		    debug_print("ERROR: [0x%016llX] [0x%016llX]  expected 0x%016llX xor %016llX\n",
				p1, p2, v, xor);
		    if (xor & bymsk) { // error(s) in this lane
			errors |= bybit; // set the byte error bit
			xor &= ~bymsk; // clear byte lane in error bits
			datamask &= ~bymsk; // clear the byte lane in the mask
			if (datamask == 0) { // nothing left to do
			    goto done_now; // completely done when errors found in all byte lanes in datamask
			}
		    }
		    bymsk <<= 8; // move mask into next byte lane
		    bybit <<= 1; // move bit into next byte position
		}
	    }
	}
    }

 done_now:
    if (xor_data != NULL) { // send the bad bits back...
        xor_data[0] = bad_bits[0];
        xor_data[1] = bad_bits[1]; // let it be zeroed
    }
    return errors;
}

// NOTE: "mode" argument:
//         DBTRAIN_TEST: for testing using GP patterns, includes ECC
//         DBTRAIN_DBI:  for DBI deskew training behavior (uses GP patterns)
//         DBTRAIN_LFSR: for testing using LFSR patterns, includes ECC
// NOTE: trust the caller to specify the correct/supported mode
//
int test_dram_byte_hw(bdk_node_t node, int ddr_interface_num,
                      uint64_t p, int mode, uint64_t *xor_data)
{
    uint64_t p1;
    uint64_t k;
    int errors = 0;

    uint64_t mpr_data0, mpr_data1;
    uint64_t bad_bits[2] = {0,0};

    int node_address, lmc, dimm;
    int prank, lrank;
    int bank, row, col;
    int save_or_dis;
    int byte;
    int ba_loop, ba_bits;

    bdk_lmcx_rlevel_ctl_t rlevel_ctl;
    bdk_lmcx_dbtrain_ctl_t dbtrain_ctl;

    int bank_errs;

    // FIXME: K iterations set to 4 for now.
    // FIXME: decrement to increase interations.
    // FIXME: must be no less than 22 to stay above an LMC hash field. 
    int kshift = 26;
    const char *s;

    // allow override default setting for kshift
    if ((s = getenv("ddr_tune_set_kshift")) != NULL) {
        int temp = strtoul(s, NULL, 0);
        if ((temp < 22) || (temp > 27)) {
            ddr_print("N%d.LMC%d: ILLEGAL override of kshift to %d, using default %d\n",
                      node, ddr_interface_num, temp, kshift);
        } else {
            VB_PRT(VBL_DEV2, "N%d.LMC%d: overriding kshift (%d) to %d\n",
                   node, ddr_interface_num, kshift, temp);
            kshift = temp;
        }
    }

    /*
      1) Make sure that RLEVEL_CTL[OR_DIS] = 0.
    */
    rlevel_ctl.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_CTL(ddr_interface_num));
    save_or_dis = rlevel_ctl.s.or_dis;
    rlevel_ctl.s.or_dis = 0;    /* or_dis must be disabled for this sequence */
    DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_CTL(ddr_interface_num), rlevel_ctl.u);

    /*
      NOTE: this step done in the calling routine(s)
      3) Setup GENERAL_PURPOSE[0-2] registers with the data pattern of choice.
        a. GENERAL_PURPOSE0[DATA<63:0>] – sets the initial lower (rising edge) 64 bits of data.
        b. GENERAL_PURPOSE1[DATA<63:0>] – sets the initial upper (falling edge) 64 bits of data.
        c. GENERAL_PURPOSE2[DATA<15:0>] – sets the initial lower (rising edge <7:0>) and upper
           (falling edge <15:8>) ECC data.
     */

    // final address must include LMC and node
    p |= (ddr_interface_num << 7); /* Map address into proper interface */
    p = bdk_numa_get_address(node, p); /* Map to node */

    /*
     * Add base offset to both test regions to not clobber u-boot stuff
     * when running from L2 for NAND boot.
     */
    p += 0x10000000; // offset to 256MB

    errors = 0;

    bdk_dram_address_extract_info(p, &node_address, &lmc, &dimm, &prank, &lrank, &bank, &row, &col);
    VB_PRT(VBL_DEV2, "test_dram_byte_hw: START at A:0x%012llx, N%d L%d D%d R%d/%d B%1x Row:%05x Col:%05x\n",
           p, node_address, lmc, dimm, prank, lrank, bank, row, col);

    // only check once per call, and ignore if no match...
    if ((int)node != node_address) {
        error_print("ERROR: Node address mismatch; ignoring...\n");
        return 0;
    }
    if (lmc != ddr_interface_num) {
        error_print("ERROR: LMC address mismatch\n");
        return 0;
    }

    /*
      7) Set PHY_CTL[PHY_RESET] = 1 (LMC automatically clears this as it’s a one-shot operation).
         This is to get into the habit of resetting PHY’s SILO to the original 0 location.
    */
    BDK_CSR_MODIFY(phy_ctl, node, BDK_LMCX_PHY_CTL(ddr_interface_num),
		   phy_ctl.s.phy_reset = 1);

    /* Walk through a range of addresses avoiding bits that alias
     * interfaces on the CN88XX.
     */

    // FIXME: want to try to keep the K increment from affecting the LMC via hash,
    // FIXME: so keep it above bit 21
    // NOTE:  we also want to keep k less than the base offset of bit 28 (256MB)

    for (k = 0; k < (1UL << 28); k += (1UL << kshift)) {

	// FIXME: the sequence will interate over 1/2 cacheline
	// FIXME: for each unit specified in "read_cmd_count",
	// FIXME: so, we setup each sequence to do the max cachelines it can

	p1 = p + k;

	bdk_dram_address_extract_info(p1, &node_address, &lmc, &dimm, &prank, &lrank, &bank, &row, &col);
        VB_PRT(VBL_DEV3, "test_dram_byte_hw: NEXT interation at A:0x%012llx, N%d L%d D%d R%d/%d B%1x Row:%05x Col:%05x\n",
               p1, node_address, lmc, dimm, prank, lrank, bank, row, col);

        /*
          2) Setup the fields of the CSR DBTRAIN_CTL as follows:
            a. COL, ROW, BA, BG, PRANK points to the starting point of the address.
               You can just set them to all 0.
            b. RW_TRAIN – set this to 1.
            c. TCCD_L – set this to 0.
            d. READ_CMD_COUNT – instruct the sequence to the how many writes/reads.
               It is 5 bits field, so set to 31 of maximum # of r/w.
        */
        dbtrain_ctl.u = BDK_CSR_READ(node, BDK_LMCX_DBTRAIN_CTL(ddr_interface_num));
        dbtrain_ctl.s.column_a       = col;
        dbtrain_ctl.s.row_a          = row;
        dbtrain_ctl.s.bg             = (bank >> 2) & 3;
        dbtrain_ctl.s.prank          = (dimm * 2) + prank; // FIXME? 
        dbtrain_ctl.s.lrank          = lrank; // FIXME? 
        dbtrain_ctl.s.activate       = (mode == DBTRAIN_DBI);
        dbtrain_ctl.s.write_ena      = 1;
        dbtrain_ctl.s.read_cmd_count = 31; // max count pass 1.x
        if (! CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) // added 81xx and 83xx
            dbtrain_ctl.s.cmd_count_ext = 3; // max count pass 2.x
        else
            dbtrain_ctl.s.cmd_count_ext = 0; // max count pass 1.x
        dbtrain_ctl.s.rw_train       = 1;
        dbtrain_ctl.s.tccd_sel       = (mode == DBTRAIN_DBI);

        // LFSR should only be on when chip supports it...
        dbtrain_ctl.s.lfsr_pattern_sel = (mode == DBTRAIN_LFSR) ? 1 : 0;

        bank_errs = 0;

	// for each address, iterate over the 4 "banks" in the BA
	for (ba_loop = 0, ba_bits = bank & 3;
	     ba_loop < 4;
	     ba_loop++, ba_bits = (ba_bits + 1) & 3)
	{
            dbtrain_ctl.s.ba = ba_bits;
            DRAM_CSR_WRITE(node, BDK_LMCX_DBTRAIN_CTL(ddr_interface_num), dbtrain_ctl.u);

            VB_PRT(VBL_DEV3, "test_dram_byte_hw: DBTRAIN: Pr:%d Lr:%d Bg:%d Ba:%d Row:%05x Col:%05x\n",
                   dbtrain_ctl.s.prank, dbtrain_ctl.s.lrank,
                   dbtrain_ctl.s.bg, dbtrain_ctl.s.ba, row, col);
	    /*
	      4) Kick off the sequence (SEQ_CTL[SEQ_SEL] = 14, SEQ_CTL[INIT_START] = 1).
	      5) Poll on SEQ_CTL[SEQ_COMPLETE] for completion.
	    */
	    perform_octeon3_ddr3_sequence(node, prank, ddr_interface_num, 14);

	    /*
	      6) Read MPR_DATA0 and MPR_DATA1 for results:
	        a. MPR_DATA0[MPR_DATA<63:0>] – comparison results for DQ63:DQ0.
	           (1 means MATCH, 0 means FAIL).
                b. MPR_DATA1[MPR_DATA<7:0>] – comparison results for ECC bit7:0.
	    */
	    mpr_data0 = BDK_CSR_READ(node, BDK_LMCX_MPR_DATA0(ddr_interface_num));
	    mpr_data1 = BDK_CSR_READ(node, BDK_LMCX_MPR_DATA1(ddr_interface_num));

	    /*
	      7) Set PHY_CTL[PHY_RESET] = 1 (LMC automatically clears this as it’s a one-shot operation).
	      This is to get into the habit of resetting PHY’s SILO to the original 0 location.
	    */
	    BDK_CSR_MODIFY(phy_ctl, node, BDK_LMCX_PHY_CTL(ddr_interface_num),
			   phy_ctl.s.phy_reset = 1);

            if (mode == DBTRAIN_DBI)
                continue; // bypass any error checking or updating when DBI mode

            // data bytes
            if (~mpr_data0) {
                for (byte = 0; byte < 8; byte++) {
                    if ((~mpr_data0 >> (8 * byte)) & 0xffUL)
                        bank_errs |= (1 << byte);
                }
                // accumulate bad bits
                bad_bits[0] |= ~mpr_data0;
            }

            // include ECC byte errors
            if (~mpr_data1 & 0xffUL) {
                bank_errs |= (1 << 8);
                bad_bits[1] |= ~mpr_data1 & 0xffUL;
            }

	} /* for (int ba_loop = 0; ba_loop < 4; ba_loop++) */

        errors |= bank_errs;

    } /* end for (k=...) */

    rlevel_ctl.s.or_dis = save_or_dis;
    DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_CTL(ddr_interface_num), rlevel_ctl.u);

    if ((mode != DBTRAIN_DBI) && (xor_data != NULL)) { // send the bad bits back...
        xor_data[0] = bad_bits[0];
        xor_data[1] = bad_bits[1];
    }

    return errors;
}

static void set_ddr_memory_preserved(bdk_node_t node)
{
    global_ddr_memory_preserved |= 0x1 << node;

}
int ddr_memory_preserved(bdk_node_t node)
{
    return (global_ddr_memory_preserved & (0x1 << node)) != 0;
}

void perform_ddr_init_sequence(bdk_node_t node, int rank_mask,
				       int ddr_interface_num)
{
    const char *s;
    int ddr_init_loops = 1;
    int rankx;

    if ((s = lookup_env_parameter("ddr%d_init_loops", ddr_interface_num)) != NULL)
	ddr_init_loops = strtoul(s, NULL, 0);

    while (ddr_init_loops--) {
	for (rankx = 0; rankx < 8; rankx++) {
	    if (!(rank_mask & (1 << rankx)))
		continue;

            perform_octeon3_ddr3_sequence(node, (1 << rankx),
                                          ddr_interface_num, 0); /* power-up/init */

	    bdk_wait_usec(1000);   /* Wait a while. */

	    if ((s = lookup_env_parameter("ddr_sequence1")) != NULL) {
		int sequence1;
		sequence1 = strtoul(s, NULL, 0);
		perform_octeon3_ddr3_sequence(node, (1 << rankx),
					      ddr_interface_num, sequence1);
	    }

	    if ((s = lookup_env_parameter("ddr_sequence2")) != NULL) {
		int sequence2;
		sequence2 = strtoul(s, NULL, 0);
		perform_octeon3_ddr3_sequence(node, (1 << rankx),
					      ddr_interface_num, sequence2);
	    }
	}
    }
}

static void set_ddr_clock_initialized(bdk_node_t node, int ddr_interface, int inited_flag)
{
	int bit = node * 8 + ddr_interface;
	if (inited_flag)
		global_ddr_clock_initialized |= (0x1 << bit);
	else
		global_ddr_clock_initialized &= ~(0x1 << bit);
}
static int ddr_clock_initialized(bdk_node_t node, int ddr_interface)
{
	int bit = node * 8 + ddr_interface;
	return (!!(global_ddr_clock_initialized & (0x1 << bit)));
}


static void cn78xx_lmc_dreset_init (bdk_node_t node, int ddr_interface_num)
{
        /*
	 * This is the embodiment of the 6.9.4 LMC DRESET Initialization section below.
	 *
         * The remainder of this section describes the sequence for LMCn.
         *
         * 1. If not done already, write LMC(0..3)_DLL_CTL2 to its reset value
         *    (except without changing the LMC(0..3)_DLL_CTL2[INTF_EN] value from
         *    that set in the prior Step 3), including LMC(0..3)_DLL_CTL2[DRESET] = 1.
         *
         * 2. Without changing any other LMC(0..3)_DLL_CTL2 fields, write
         *    LMC(0..3)_DLL_CTL2[DLL_BRINGUP] = 1.
         */

        DRAM_CSR_MODIFY(c, node, BDK_LMCX_DLL_CTL2(ddr_interface_num),
			c.s.dll_bringup = 1);

        /*
         * 3. Read LMC(0..3)_DLL_CTL2 and wait for the result.
         */

        BDK_CSR_READ(node, BDK_LMCX_DLL_CTL2(ddr_interface_num));

        /*
         * 4. Wait for a minimum of 10 LMC CK cycles.
         */

        bdk_wait_usec(1);

        /*
         * 5. Without changing any other fields in LMC(0..3)_DLL_CTL2, write
         *    LMC(0..3)_DLL_CTL2[QUAD_DLL_ENA] = 1.
         *    LMC(0..3)_DLL_CTL2[QUAD_DLL_ENA] must not change after this point
         *    without restarting the LMCn DRESET initialization sequence.
         */

        DRAM_CSR_MODIFY(c, node, BDK_LMCX_DLL_CTL2(ddr_interface_num),
			c.s.quad_dll_ena = 1);

        /*
         * 6. Read LMC(0..3)_DLL_CTL2 and wait for the result.
         */

        BDK_CSR_READ(node, BDK_LMCX_DLL_CTL2(ddr_interface_num));

        /*
         * 7. Wait a minimum of 10 us.
         */

        bdk_wait_usec(10);

        /*
         * 8. Without changing any other fields in LMC(0..3)_DLL_CTL2, write
         *    LMC(0..3)_DLL_CTL2[DLL_BRINGUP] = 0.
         *    LMC(0..3)_DLL_CTL2[DLL_BRINGUP] must not change after this point
         *    without restarting the LMCn DRESET initialization sequence.
         */

        DRAM_CSR_MODIFY(c, node, BDK_LMCX_DLL_CTL2(ddr_interface_num),
			c.s.dll_bringup = 0);

        /*
         * 9. Read LMC(0..3)_DLL_CTL2 and wait for the result.
         */

        BDK_CSR_READ(node, BDK_LMCX_DLL_CTL2(ddr_interface_num));

        /*
         * 10. Without changing any other fields in LMC(0..3)_DLL_CTL2, write
         *    LMC(0..3)_DLL_CTL2[DRESET] = 0.
         *    LMC(0..3)_DLL_CTL2[DRESET] must not change after this point without
         *    restarting the LMCn DRESET initialization sequence.
         *
         *    After completing LMCn DRESET initialization, all LMC CSRs may be
         *    accessed.  Prior to completing LMC DRESET initialization, only
         *    LMC(0..3)_DDR_PLL_CTL, LMC(0..3)_DLL_CTL2, LMC(0..3)_RESET_CTL, and
         *    LMC(0..3)_COMP_CTL2 LMC CSRs can be accessed.
         */

        DRAM_CSR_MODIFY(c, node, BDK_LMCX_DLL_CTL2(ddr_interface_num),
			c.s.dreset = 0);

        /*
	 * NEW STEP - necessary for O73, O78 P2.0, O75, and T88 P2.0
	 * McBuggin: #24821
	 *
         * 11. Wait for a minimum of 10 LMC CK cycles.
         */

        bdk_wait_usec(1);
}

/*static*/ void cn88xx_lmc_ddr3_reset(bdk_node_t node, int ddr_interface_num, int reset)
{
    /*
     * 4. Deassert DDRn_RESET_L pin by writing LMC(0..3)_RESET_CTL[DDR3RST] = 1
     *    without modifying any other LMC(0..3)_RESET_CTL fields.
     * 5. Read LMC(0..3)_RESET_CTL and wait for the result.
     * 6. Wait a minimum of 500us. This guarantees the necessary T = 500us
     *    delay between DDRn_RESET_L deassertion and DDRn_DIMM*_CKE* assertion.
     */
    ddr_print("LMC%d %s DDR_RESET_L\n", ddr_interface_num,
	      (reset == LMC_DDR3_RESET_DEASSERT) ? "De-asserting" : "Asserting");
    DRAM_CSR_MODIFY(c, node, BDK_LMCX_RESET_CTL(ddr_interface_num),
		    c.cn8.ddr3rst = reset);
    BDK_CSR_READ(node, BDK_LMCX_RESET_CTL(ddr_interface_num));
    bdk_wait_usec(500);
}

int initialize_ddr_clock(bdk_node_t node,
			 const ddr_configuration_t *ddr_configuration,
                         uint32_t cpu_hertz,
                         uint32_t ddr_hertz,
                         uint32_t ddr_ref_hertz,
                         int ddr_interface_num,
                         uint32_t ddr_interface_mask
                         )
{
    const char *s;

    if (ddr_clock_initialized(node, ddr_interface_num))
        return 0;

    if (!ddr_clock_initialized(node, 0)) { /* Do this once */
        int i;
        bdk_lmcx_reset_ctl_t reset_ctl;
        /* Check to see if memory is to be preserved and set global flag */
        for (i=3; i>=0; --i) {
            if ((ddr_interface_mask & (1 << i)) == 0)
                continue;
            reset_ctl.u = BDK_CSR_READ(node, BDK_LMCX_RESET_CTL(i));
            if (reset_ctl.s.ddr3psv == 1) {
                ddr_print("LMC%d Preserving memory\n", i);
                set_ddr_memory_preserved(node);

                /* Re-initialize flags */
                reset_ctl.cn8.ddr3pwarm = 0;
                reset_ctl.cn8.ddr3psoft = 0;
                reset_ctl.s.ddr3psv   = 0;
                DRAM_CSR_WRITE(node, BDK_LMCX_RESET_CTL(i), reset_ctl.u);
            }
        }
    }

    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX)) {

        bdk_lmcx_ddr_pll_ctl_t ddr_pll_ctl;
        const dimm_config_t *dimm_config_table = ddr_configuration->dimm_config_table;

        /* ddr_type only indicates DDR4 or DDR3 */
        int ddr_type = get_ddr_type(node, &dimm_config_table[0]);

        /*
         * 6.9 LMC Initialization Sequence
         *
         * There are 14 parts to the LMC initialization procedure:
         *
         * 1. LMC interface enable initialization
         *
         * 2. DDR PLL initialization
         *
         * 3. LMC CK initialization
         *
         * 4. LMC DRESET initialization
         *
         * 5. LMC CK local initialization
         *
         * 6. LMC RESET initialization
         *
         * 7. Early LMC initialization
         *
         * 8. LMC offset training
         *
         * 9. LMC internal Vref training
         *
         * 10. LMC deskew training
         *
         * 11. LMC write leveling
         *
         * 12. LMC read leveling
         *
         * 13. DRAM Vref Training for DDR4
	 *
         * 14. Final LMC initialization	 
         *
         * CN88XX supports two modes:
         *
         * ­ two-LMC mode: both LMCs 2/3 must not be enabled
         *   (LMC2/3_DLL_CTL2[DRESET] must be set to 1 and LMC2/3_DLL_CTL2[INTF_EN]
         *   must be set to 0) and both LMCs 0/1 must be enabled).
         *
         * ­ four-LMC mode: all four LMCs 0..3 must be enabled.
         *
         * Steps 4 and 6..14 should each be performed for each enabled LMC (either
         * twice or four times). Steps 1..3 and 5 are more global in nature and
         * each must be executed exactly once (not once per LMC) each time the
         *  DDR PLL changes or is first brought up. Steps 1..3 and 5 need not be
         * performed if the DDR PLL is stable.
         *
         * Generally, the steps are performed in order. The exception is that the
         * CK local initialization (step 5) must be performed after some DRESET
         * initializations (step 4) and before other DRESET initializations when
         * the DDR PLL is brought up or changed. (The CK local initialization
         * uses information from some LMCs to bring up the other local CKs.) The
         * following text describes these ordering requirements in more detail.
         *
         * Following any chip reset, the DDR PLL must be brought up, and all 14
         * steps should be executed. Subsequently, it is possible to execute only
         * steps 4 and 6..14, or to execute only steps 8..14.
         *
         * The remainder of this section covers these initialization steps in
         * sequence.
         */

        if (ddr_interface_num == 0) { /* Do this once */
            bdk_lmcx_dll_ctl2_t	dll_ctl2;
            int loop_interface_num;

            /*
             * 6.9.1 LMC Interface-Enable Initialization
             *
             * LMC interface-enable initialization (Step 1) must be performed only
             * once, not once per LMC in four-LMC mode. This step is not required
	     * in two-LMC mode.
	     *
             * Perform the following three substeps for the LMC interface-enable
             * initialization:
             *
             * 1. Without changing any other LMC2_DLL_CTL2 fields (LMC(0..3)_DLL_CTL2
             *    should be at their reset values after Step 1), write
             *    LMC2_DLL_CTL2[INTF_EN] = 1 if four-LMC mode is desired.
             *
             * 2. Without changing any other LMC3_DLL_CTL2 fields, write
             *    LMC3_DLL_CTL2[INTF_EN] = 1 if four-LMC mode is desired.
             *
             * 3. Read LMC2_DLL_CTL2 and wait for the result.
             *
             * The LMC2_DLL_CTL2[INTF_EN] and LMC3_DLL_CTL2[INTF_EN] values should
             * not be changed by software from this point.
             *
             */

	    /* Put all LMCs into DRESET here; these are the reset values... */
            for (loop_interface_num = 0; loop_interface_num < 4; ++loop_interface_num) {
                if ((ddr_interface_mask & (1 << loop_interface_num)) == 0)
                    continue;

                dll_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL2(loop_interface_num));

                dll_ctl2.s.byp_setting          = 0;
                dll_ctl2.s.byp_sel              = 0;
                dll_ctl2.s.quad_dll_ena         = 0;
                dll_ctl2.s.dreset               = 1;
                dll_ctl2.s.dll_bringup          = 0;
                dll_ctl2.s.intf_en              = 0;

                DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL2(loop_interface_num), dll_ctl2.u);
            }

	    /* Now set INTF_EN for *ONLY* LMC2/3 if they are to be active on 88XX. */
	    /* Do *NOT* touch LMC0/1 INTF_EN=0 setting on 88XX. */
            /* But we do have to set LMC1 INTF_EN=1 on 83XX if we want it active... */
            /* Note that 81xx has only LMC0 so the mask should reflect that. */
            for (loop_interface_num = (CAVIUM_IS_MODEL(CAVIUM_CN83XX)) ? 1 : 2;
                 loop_interface_num < 4; ++loop_interface_num) {
                if ((ddr_interface_mask & (1 << loop_interface_num)) == 0)
                    continue;

                DRAM_CSR_MODIFY(c, node, BDK_LMCX_DLL_CTL2(loop_interface_num),
				c.s.intf_en = 1);
                BDK_CSR_READ(node, BDK_LMCX_DLL_CTL2(loop_interface_num));
            }

            /*
             * 6.9.2 DDR PLL Initialization
             *
             * DDR PLL initialization (Step 2) must be performed for each chip reset
             * and whenever the DDR clock speed changes. This step needs to be
             * performed only once, not once per LMC.
             *
             * Perform the following eight substeps to initialize the DDR PLL:
             *
             * 1. If not done already, write all fields in LMC(0..1)_DDR_PLL_CTL and
             *    LMC(0..1)_DLL_CTL2 to their reset values, including:
             *
             * .. LMC0_DDR_PLL_CTL[DDR_DIV_RESET] = 1
             * .. LMC0_DLL_CTL2[DRESET] = 1
             *
             * This substep is not necessary after a chip reset.
             *
             */

            ddr_pll_ctl.u = BDK_CSR_READ(node, BDK_LMCX_DDR_PLL_CTL(0));

            ddr_pll_ctl.cn83xx.reset_n           = 0;
            ddr_pll_ctl.cn83xx.ddr_div_reset     = 1;
            ddr_pll_ctl.cn83xx.phy_dcok          = 0;
            ddr_pll_ctl.cn83xx.dclk_invert       = 0;

            // allow override of LMC0 desired setting for DCLK_INVERT
            if ((s = lookup_env_parameter("ddr0_set_dclk_invert")) != NULL) {
                ddr_pll_ctl.cn83xx.dclk_invert = !!strtoul(s, NULL, 0);
                ddr_print("LMC0: override DDR_PLL_CTL[dclk_invert] to %d\n",
                          ddr_pll_ctl.cn83xx.dclk_invert);
            }
            
            // always write LMC0 CSR, it must be active
            DRAM_CSR_WRITE(node, BDK_LMCX_DDR_PLL_CTL(0), ddr_pll_ctl.u);
            ddr_print("%-45s : 0x%016llx\n", "LMC0: DDR_PLL_CTL", ddr_pll_ctl.u);

            // only when LMC1 is active
            // NOTE: 81xx has only 1 LMC, and 83xx can operate in 1-LMC mode
            if (ddr_interface_mask & 0x2) {

                ddr_pll_ctl.cn83xx.dclk_invert       ^= 1; /* DEFAULT: Toggle dclk_invert from LMC0 */

                // allow override of LMC1 desired setting for DCLK_INVERT
                if ((s = lookup_env_parameter("ddr1_set_dclk_invert")) != NULL) {
                    ddr_pll_ctl.cn83xx.dclk_invert = !!strtoul(s, NULL, 0);
                    ddr_print("LMC1: override DDR_PLL_CTL[dclk_invert] to %d\n",
                              ddr_pll_ctl.cn83xx.dclk_invert);
                }

                // always write LMC1 CSR when it is active
                DRAM_CSR_WRITE(node, BDK_LMCX_DDR_PLL_CTL(1), ddr_pll_ctl.u);
                ddr_print("%-45s : 0x%016llx\n", "LMC1: DDR_PLL_CTL", ddr_pll_ctl.u);
            }

            /*
             * 2. If the current DRAM contents are not preserved (see
             *    LMC(0..3)_RESET_ CTL[DDR3PSV]), this is also an appropriate time to
             *    assert the RESET# pin of the DDR3/DDR4 DRAM parts. If desired, write
             *    LMC0_RESET_ CTL[DDR3RST] = 0 without modifying any other
             *    LMC0_RESET_CTL fields to assert the DDR_RESET_L pin. No action is
             *    required here to assert DDR_RESET_L following a chip reset. Refer to
             *    Section 6.9.6. Do this for all enabled LMCs.
             */

            for (loop_interface_num = 0;
                 ( !ddr_memory_preserved(node)) && loop_interface_num < 4;
                 ++loop_interface_num)
	    {

                if ((ddr_interface_mask & (1 << loop_interface_num)) == 0)
                    continue;

		cn88xx_lmc_ddr3_reset(node, loop_interface_num, LMC_DDR3_RESET_ASSERT);
            }

            /*
             * 3. Without changing any other LMC0_DDR_PLL_CTL values, write LMC0_DDR_
             *    PLL_CTL[CLKF] with a value that gives a desired DDR PLL speed. The
             *    LMC0_DDR_PLL_CTL[CLKF] value should be selected in conjunction with
             *    the post-scalar divider values for LMC (LMC0_DDR_PLL_CTL[DDR_PS_EN])
             *    so that the desired LMC CK speeds are is produced (all enabled LMCs
             *    must run the same speed). Section 5.14 describes
             *    LMC0_DDR_PLL_CTL[CLKF] and LMC0_DDR_PLL_CTL[DDR_PS_EN] programmings
             *    that produce the desired LMC CK speed. Section 6.9.3 describes LMC CK
             *    initialization, which can be done separately from the DDR PLL
             *    initialization described in this section.
             *
             * The LMC0_DDR_PLL_CTL[CLKF] value must not change after this point
             * without restarting this SDRAM PLL initialization sequence.
             */

            {
                /* CLKF = (DCLK * (CLKR+1) * EN(1, 2, 3, 4, 5, 6, 7, 8, 10, 12))/DREF - 1 */
                int en_idx, save_en_idx, best_en_idx=0;
                uint64_t clkf, clkr, max_clkf = 127;
                uint64_t best_clkf=0, best_clkr=0;
                uint64_t best_pll_MHz = 0;
                uint64_t pll_MHz;
                uint64_t min_pll_MHz = 800;
                uint64_t max_pll_MHz = 5000;
                uint64_t error;
                uint64_t best_error;
                uint64_t best_calculated_ddr_hertz = 0;
                uint64_t calculated_ddr_hertz = 0;
		uint64_t orig_ddr_hertz = ddr_hertz;
                static const int _en[] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 12};
                int override_pll_settings;
                int new_bwadj;

                error = best_error = ddr_hertz;  /* Init to max error */

                ddr_print("DDR Reference Hertz = %d\n", ddr_ref_hertz);

                while (best_error == ddr_hertz) {

		    for (clkr = 0; clkr < 4; ++clkr) {
			for (en_idx=sizeof(_en)/sizeof(int)-1; en_idx>=0; --en_idx) {
			    save_en_idx = en_idx;
			    clkf = ((ddr_hertz) * (clkr+1) * (_en[save_en_idx]));
			    clkf = divide_nint(clkf, ddr_ref_hertz) - 1;
			    pll_MHz = ddr_ref_hertz * (clkf+1) / (clkr+1) / 1000000;
			    calculated_ddr_hertz = ddr_ref_hertz * (clkf + 1) / ((clkr + 1) * (_en[save_en_idx]));
			    error = ddr_hertz - calculated_ddr_hertz;

			    if ((pll_MHz < min_pll_MHz) || (pll_MHz > max_pll_MHz)) continue;
			    if (clkf > max_clkf) continue; /* PLL requires clkf to be limited */
			    if (_abs(error) > _abs(best_error)) continue;

			    VB_PRT(VBL_TME, "clkr: %2llu, en[%d]: %2d, clkf: %4llu, pll_MHz: %4llu, ddr_hertz: %8llu, error: %8lld\n",
                                    clkr, save_en_idx, _en[save_en_idx], clkf, pll_MHz, calculated_ddr_hertz, error);

			    /* Favor the highest PLL frequency. */
			    if ((_abs(error) < _abs(best_error)) || (pll_MHz > best_pll_MHz)) {
				best_pll_MHz = pll_MHz;
				best_calculated_ddr_hertz = calculated_ddr_hertz;
				best_error = error;
				best_clkr = clkr;
				best_clkf = clkf;
				best_en_idx = save_en_idx;
			    }
			}
		    }

		    override_pll_settings = 0;

		    if ((s = lookup_env_parameter("ddr_pll_clkr")) != NULL) {
			best_clkr = strtoul(s, NULL, 0);
			override_pll_settings = 1;
		    }
		    if ((s = lookup_env_parameter("ddr_pll_clkf")) != NULL) {
			best_clkf = strtoul(s, NULL, 0);
			override_pll_settings = 1;
		    }
		    if ((s = lookup_env_parameter("ddr_pll_en_idx")) != NULL) {
			best_en_idx = strtoul(s, NULL, 0);
			override_pll_settings = 1;
		    }

		    if (override_pll_settings) {
			best_pll_MHz = ddr_ref_hertz * (best_clkf+1) / (best_clkr+1) / 1000000;
			best_calculated_ddr_hertz = ddr_ref_hertz * (best_clkf + 1) / ((best_clkr + 1) * (_en[best_en_idx]));
			best_error = ddr_hertz - best_calculated_ddr_hertz;
		    }

		    ddr_print("clkr: %2llu, en[%d]: %2d, clkf: %4llu, pll_MHz: %4llu, ddr_hertz: %8llu, error: %8lld <==\n",
			      best_clkr, best_en_idx, _en[best_en_idx], best_clkf, best_pll_MHz,
			      best_calculated_ddr_hertz, best_error);

		    /* Try lowering the frequency if we can't get a working configuration */
		    if (best_error == ddr_hertz) {
			if (ddr_hertz < orig_ddr_hertz - 10000000)
			    break;
			ddr_hertz -= 1000000;
			best_error = ddr_hertz;
		    }

		} /* while (best_error == ddr_hertz) */


                if (best_error == ddr_hertz) {
                    error_print("ERROR: Can not compute a legal DDR clock speed configuration.\n");
                    return(-1);
                }

                new_bwadj = (best_clkf + 1) / 10;
                VB_PRT(VBL_TME, "bwadj: %2d\n", new_bwadj);

                if ((s = lookup_env_parameter("ddr_pll_bwadj")) != NULL) {
                    new_bwadj = strtoul(s, NULL, 0);
                    VB_PRT(VBL_TME, "bwadj: %2d\n", new_bwadj);
                }

                for (loop_interface_num = 0; loop_interface_num<2; ++loop_interface_num) {
                    if ((ddr_interface_mask & (1 << loop_interface_num)) == 0)
                        continue;

                    // make sure we preserve any settings already there
                    ddr_pll_ctl.u = BDK_CSR_READ(node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num));
                    ddr_print("LMC%d: DDR_PLL_CTL                             : 0x%016llx\n",
                              loop_interface_num, ddr_pll_ctl.u);

                    ddr_pll_ctl.cn83xx.ddr_ps_en = best_en_idx;
                    ddr_pll_ctl.cn83xx.clkf = best_clkf;
                    ddr_pll_ctl.cn83xx.clkr = best_clkr;
                    ddr_pll_ctl.cn83xx.reset_n = 0;
                    ddr_pll_ctl.cn83xx.bwadj = new_bwadj;

                    DRAM_CSR_WRITE(node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num), ddr_pll_ctl.u);
                    ddr_print("LMC%d: DDR_PLL_CTL                             : 0x%016llx\n",
                              loop_interface_num, ddr_pll_ctl.u);
                }
            }


            for (loop_interface_num = 0; loop_interface_num<4; ++loop_interface_num) {
                if ((ddr_interface_mask & (1 << loop_interface_num)) == 0)
                    continue;

		/*
		 * 4. Read LMC0_DDR_PLL_CTL and wait for the result.
		 */

		BDK_CSR_READ(node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num));

		/*
		 * 5. Wait a minimum of 3 us.
		 */

		bdk_wait_usec(3);          /* Wait 3 us */

		/*
		 * 6. Write LMC0_DDR_PLL_CTL[RESET_N] = 1 without changing any other
		 *    LMC0_DDR_PLL_CTL values.
		 */

		DRAM_CSR_MODIFY(c, node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num),
				c.cn83xx.reset_n = 1);

		/*
		 * 7. Read LMC0_DDR_PLL_CTL and wait for the result.
		 */

		BDK_CSR_READ(node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num));

		/*
		 * 8. Wait a minimum of 25 us.
		 */

		bdk_wait_usec(25);          /* Wait 25 us */

            } /* for (loop_interface_num = 0; loop_interface_num<4; ++loop_interface_num) */

            for (loop_interface_num = 0; loop_interface_num<4; ++loop_interface_num) {
                if ((ddr_interface_mask & (1 << loop_interface_num)) == 0)
                    continue;
		/*
		 * 6.9.3 LMC CK Initialization
		 *
		 * DDR PLL initialization must be completed prior to starting LMC CK
		 * initialization.
		 *
		 * Perform the following substeps to initialize the LMC CK. Perform
		 * substeps 1..3 for both LMC0 and LMC1.
		 *
		 * 1. Without changing any other LMC(0..3)_DDR_PLL_CTL values, write
		 *    LMC(0..3)_DDR_PLL_CTL[DDR_DIV_RESET] = 1 and
		 *    LMC(0..3)_DDR_PLL_CTL[DDR_PS_EN] with the appropriate value to get the
		 *    desired LMC CK speed. Section 5.14 discusses CLKF and DDR_PS_EN
		 *    programmings.  The LMC(0..3)_DDR_PLL_CTL[DDR_PS_EN] must not change
		 *    after this point without restarting this LMC CK initialization
		 *  sequence.
		 */

		DRAM_CSR_MODIFY(c, node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num),
				c.cn83xx.ddr_div_reset = 1);

		/*
		 * 2. Without changing any other fields in LMC(0..3)_DDR_PLL_CTL, write
		 *    LMC(0..3)_DDR_PLL_CTL[DDR4_MODE] = 0.
		 */

		DRAM_CSR_MODIFY(c, node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num),
				c.cn83xx.ddr4_mode = (ddr_type == DDR4_DRAM) ? 1 : 0);

		/*
		 * 3. Read LMC(0..3)_DDR_PLL_CTL and wait for the result.
		 */

		BDK_CSR_READ(node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num));

		/*
		 * 4. Wait a minimum of 1 us.
		 */

		bdk_wait_usec(1);          /* Wait 1 us */

		/*
		 * 5. Without changing any other fields in LMC(0..3)_DDR_PLL_CTL, write
		 *    LMC(0..3)_DDR_PLL_CTL[PHY_DCOK] = 1.
		 */

		DRAM_CSR_MODIFY(c, node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num),
				c.cn83xx.phy_dcok = 1);

		/*
		 * 6. Read LMC(0..3)_DDR_PLL_CTL and wait for the result.
		 */

		BDK_CSR_READ(node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num));

		/*
		 * 7. Wait a minimum of 20 us.
		 */

		bdk_wait_usec(20);          /* Wait 20 us */

		/*
		 * 8. Without changing any other LMC(0..3)_COMP_CTL2 values, write
		 *    LMC(0..3)_COMP_CTL2[CK_CTL,CONTROL_CTL,CMD_CTL] to the desired
		 *    DDR*_CK_*_P control and command signals drive strength.
		 */

		{
		    bdk_lmcx_comp_ctl2_t comp_ctl2;
		    const ddr3_custom_config_t *custom_lmc_config = &ddr_configuration->custom_lmc_config;

		    comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(loop_interface_num));

		    comp_ctl2.s.dqx_ctl  = 4; /* Default 4=34.3 ohm */
		    comp_ctl2.s.ck_ctl   =
			(custom_lmc_config->ck_ctl  == 0) ? 4 : custom_lmc_config->ck_ctl;  /* Default 4=34.3 ohm */
		    comp_ctl2.s.cmd_ctl  =
			(custom_lmc_config->cmd_ctl == 0) ? 4 : custom_lmc_config->cmd_ctl; /* Default 4=34.3 ohm */

		    comp_ctl2.s.rodt_ctl           = 0x4; /* 60 ohm */

		    // These need to be done here, not later in Step 6.9.7.
		    // NOTE: these are/will be specific to a chip; for now, set to 0
		    // should we provide overrides for these?
		    comp_ctl2.s.ntune_offset    = 0;
		    comp_ctl2.s.ptune_offset    = 0;

		    // now do any overrides...
		    if ((s = lookup_env_parameter("ddr_ck_ctl")) != NULL) {
			comp_ctl2.s.ck_ctl  = strtoul(s, NULL, 0);
		    }

		    if ((s = lookup_env_parameter("ddr_cmd_ctl")) != NULL) {
			comp_ctl2.s.cmd_ctl  = strtoul(s, NULL, 0);
		    }

		    if ((s = lookup_env_parameter("ddr_dqx_ctl")) != NULL) {
			comp_ctl2.s.dqx_ctl  = strtoul(s, NULL, 0);
		    }

		    DRAM_CSR_WRITE(node, BDK_LMCX_COMP_CTL2(loop_interface_num), comp_ctl2.u);
		}

		/*
		 * 9. Read LMC(0..3)_DDR_PLL_CTL and wait for the result.
		 */

		BDK_CSR_READ(node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num));

		/*
		 * 10. Wait a minimum of 200 ns.
		 */

		bdk_wait_usec(1);          /* Wait 1 us */

		/*
		 * 11. Without changing any other LMC(0..3)_DDR_PLL_CTL values, write
		 *     LMC(0..3)_DDR_PLL_CTL[DDR_DIV_RESET] = 0.
		 */

		DRAM_CSR_MODIFY(c, node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num),
				c.cn83xx.ddr_div_reset = 0);

		/*
		 * 12. Read LMC(0..3)_DDR_PLL_CTL and wait for the result.
		 */

		BDK_CSR_READ(node, BDK_LMCX_DDR_PLL_CTL(loop_interface_num));

		/*
		 * 13. Wait a minimum of 200 ns.
		 */
		bdk_wait_usec(1);          /* Wait 1 us */

	    } /* for (loop_interface_num = 0; loop_interface_num<4; ++loop_interface_num) */

        }  /* if (ddr_interface_num == 0) */ /* Do this once */

        if (ddr_interface_num == 0) { /* Do this once */
            bdk_lmcx_dll_ctl3_t ddr_dll_ctl3;

            /*
             * 6.9.4 LMC DRESET Initialization
             *
             * All of the DDR PLL, LMC global CK, and LMC interface enable
             * initializations must be completed prior to starting this LMC DRESET
             * initialization (Step 4).
             *
             * This LMC DRESET step is done for all enabled LMCs.
             *
             * There are special constraints on the ordering of DRESET initialization
             * (Steps 4) and CK local initialization (Step 5) whenever CK local
             * initialization must be executed.  CK local initialization must be
             * executed whenever the DDR PLL is being brought up (for each chip reset
             * and whenever the DDR clock speed changes).
             *
             * When Step 5 must be executed in the two-LMC mode case:
             * ­ LMC0 DRESET initialization must occur before Step 5.
             * ­ LMC1 DRESET initialization must occur after Step 5.
             *
             * When Step 5 must be executed in the four-LMC mode case:
             * ­ LMC2 and LMC3 DRESET initialization must occur before Step 5.
             * ­ LMC0 and LMC1 DRESET initialization must occur after Step 5.
             */

            if ((ddr_interface_mask == 0x1) || (ddr_interface_mask == 0x3)) {
                /* ONE-LMC MODE FOR 81XX AND 83XX BEFORE STEP 5 */
                /* TWO-LMC MODE BEFORE STEP 5 */
                cn78xx_lmc_dreset_init(node, 0);

            } else if (ddr_interface_mask == 0xf) {
                /* FOUR-LMC MODE BEFORE STEP 5 */
                cn78xx_lmc_dreset_init(node, 2);
                cn78xx_lmc_dreset_init(node, 3);
            }

            /*
             * 6.9.5 LMC CK Local Initialization
             *
             * All of DDR PLL, LMC global CK, and LMC interface-enable
             * initializations must be completed prior to starting this LMC CK local
             * initialization (Step 5).
             *
             * LMC CK Local initialization must be performed for each chip reset and
             * whenever the DDR clock speed changes. This step needs to be performed
             * only once, not once per LMC.
             *
             * There are special constraints on the ordering of DRESET initialization
             * (Steps 4) and CK local initialization (Step 5) whenever CK local
             * initialization must be executed.  CK local initialization must be
             * executed whenever the DDR PLL is being brought up (for each chip reset
             * and whenever the DDR clock speed changes).
             *
             * When Step 5 must be executed in the two-LMC mode case:
             * ­ LMC0 DRESET initialization must occur before Step 5.
             * ­ LMC1 DRESET initialization must occur after Step 5.
             *
             * When Step 5 must be executed in the four-LMC mode case:
             * ­ LMC2 and LMC3 DRESET initialization must occur before Step 5.
             * ­ LMC0 and LMC1 DRESET initialization must occur after Step 5.
             *
             * LMC CK local initialization is different depending on whether two-LMC
             * or four-LMC modes are desired.
             */

            if (ddr_interface_mask == 0x3) {
		/*
		 * 6.9.5.1 LMC CK Local Initialization for Two-LMC Mode
		 *
		 * 1. Write LMC0_DLL_CTL3 to its reset value. (Note that
		 *    LMC0_DLL_CTL3[DLL_90_BYTE_SEL] = 0x2 .. 0x8 should also work.)
		 */

		ddr_dll_ctl3.u = 0;
		ddr_dll_ctl3.s.dclk90_recal_dis = 1;
		ddr_dll_ctl3.s.dll90_byte_sel = 1;
		DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(0),	ddr_dll_ctl3.u);

		/*
		 * 2. Read LMC0_DLL_CTL3 and wait for the result.
		 */

		BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(0));

		/*
		 * 3. Without changing any other fields in LMC0_DLL_CTL3, write
		 *    LMC0_DLL_CTL3[DCLK90_FWD] = 1.  Writing LMC0_DLL_CTL3[DCLK90_FWD] = 1
		 *    causes clock-delay information to be forwarded from LMC0 to LMC1.
		 */

		ddr_dll_ctl3.s.dclk90_fwd = 1;
		DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(0),	ddr_dll_ctl3.u);

		/*
		 * 4. Read LMC0_DLL_CTL3 and wait for the result.
		 */

		BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(0));
            } /* if (ddr_interface_mask == 0x3) */

            if (ddr_interface_mask == 0xf) {
		/*
		 * 6.9.5.2 LMC CK Local Initialization for Four-LMC Mode
		 *
		 * 1. Write LMC2_DLL_CTL3 to its reset value except
		 *    LMC2_DLL_CTL3[DLL90_BYTE_SEL] = 0x7.
		 */

		ddr_dll_ctl3.u = 0;
		ddr_dll_ctl3.s.dclk90_recal_dis = 1;
		ddr_dll_ctl3.s.dll90_byte_sel = 7;
		DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(2),	ddr_dll_ctl3.u);

		/*
		 * 2. Write LMC3_DLL_CTL3 to its reset value except
		 *    LMC3_DLL_CTL3[DLL90_BYTE_SEL] = 0x0.
		 */

		ddr_dll_ctl3.u = 0;
		ddr_dll_ctl3.s.dclk90_recal_dis = 1;
		ddr_dll_ctl3.s.dll90_byte_sel = 0; /* HRM wants 0, not 2 */
		DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(3),	ddr_dll_ctl3.u); /* HRM wants LMC3 */

		/*
		 * 3. Read LMC3_DLL_CTL3 and wait for the result.
		 */

		BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(3));

		/*
		 * 4. Without changing any other fields in LMC2_DLL_CTL3, write
		 *    LMC2_DLL_CTL3[DCLK90_FWD] = 1 and LMC2_DLL_CTL3[DCLK90_RECAL_DIS] = 1.
		 *    Writing LMC2_DLL_CTL3[DCLK90_FWD] = 1 causes LMC 2 to forward
		 *    clock-delay information to LMC0. Setting
		 *    LMC2_DLL_CTL3[DCLK90_RECAL_DIS] to 1 prevents LMC2 from periodically
		 *    recalibrating this delay information.
		 */

		DRAM_CSR_MODIFY(c, node, BDK_LMCX_DLL_CTL3(2),
				c.s.dclk90_fwd = 1;
				c.s.dclk90_recal_dis = 1);

		/*
		 * 5. Without changing any other fields in LMC3_DLL_CTL3, write
		 *    LMC3_DLL_CTL3[DCLK90_FWD] = 1 and LMC3_DLL_CTL3[DCLK90_RECAL_DIS] = 1.
		 *    Writing LMC3_DLL_CTL3[DCLK90_FWD] = 1 causes LMC3 to forward
		 *    clock-delay information to LMC1. Setting
		 *    LMC3_DLL_CTL3[DCLK90_RECAL_DIS] to 1 prevents LMC3 from periodically
		 *    recalibrating this delay information.
		 */

		DRAM_CSR_MODIFY(c, node, BDK_LMCX_DLL_CTL3(3),
				c.s.dclk90_fwd = 1;
				c.s.dclk90_recal_dis = 1);

		/*
		 * 6. Read LMC3_DLL_CTL3 and wait for the result.
		 */

		BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(3));
            } /* if (ddr_interface_mask == 0xf) */


            /* ONE-LMC MODE AFTER STEP 5 - NOTHING */

            /* TWO-LMC MODE AFTER STEP 5 */
            if (ddr_interface_mask == 0x3) {
                cn78xx_lmc_dreset_init(node, 1);
            }

            /* FOUR-LMC MODE AFTER STEP 5 */
            if (ddr_interface_mask == 0xf) {
                cn78xx_lmc_dreset_init(node, 0);
                cn78xx_lmc_dreset_init(node, 1);

                /* Enable periodic recalibration of DDR90 delay line in. */
		DRAM_CSR_MODIFY(c, node, BDK_LMCX_DLL_CTL3(0),
				c.s.dclk90_recal_dis = 0);
		DRAM_CSR_MODIFY(c, node, BDK_LMCX_DLL_CTL3(1),
				c.s.dclk90_recal_dis = 0);
            }


            /* Enable fine tune mode for all LMCs */
            for (int lmc = 0; lmc<4; ++lmc) {
                if ((ddr_interface_mask & (1 << lmc)) == 0)
                    continue;
		DRAM_CSR_MODIFY(c, node, BDK_LMCX_DLL_CTL3(lmc),
				c.s.fine_tune_mode = 1);
            }

            /* Enable the trim circuit on the appropriate channels to
               adjust the DDR clock duty cycle for chips that support
               it. */
            if (! CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) { // added 81xx and 83xx
                bdk_lmcx_phy_ctl_t lmc_phy_ctl;
                int loop_interface_num;

                for (loop_interface_num = 0; loop_interface_num<4; ++loop_interface_num) {
                    if ((ddr_interface_mask & (1 << loop_interface_num)) == 0)
                        continue;

                    lmc_phy_ctl.u = BDK_CSR_READ(node, BDK_LMCX_PHY_CTL(loop_interface_num));
                    lmc_phy_ctl.cn83xx.lv_mode = (~loop_interface_num) & 1; /* Odd LMCs = 0, Even LMCs = 1 */

                    ddr_print("LMC%d: PHY_CTL                                 : 0x%016llx\n",
                              loop_interface_num, lmc_phy_ctl.u);
                    DRAM_CSR_WRITE(node, BDK_LMCX_PHY_CTL(loop_interface_num), lmc_phy_ctl.u);
                }
            }

        } /* Do this once */

    } /* if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX)) */

    set_ddr_clock_initialized(node, ddr_interface_num, 1);
    return(0);
}
void
perform_lmc_reset(bdk_node_t node, int ddr_interface_num)
{
    /*
     * 6.9.6 LMC RESET Initialization
     *
     * The purpose of this step is to assert/deassert the RESET# pin at the
     * DDR3/DDR4 parts.
     *
     * This LMC RESET step is done for all enabled LMCs.
     *
     * It may be appropriate to skip this step if the DDR3/DDR4 DRAM parts
     * are in self refresh and are currently preserving their
     * contents. (Software can determine this via
     * LMC(0..3)_RESET_CTL[DDR3PSV] in some circumstances.) The remainder of
     * this section assumes that the DRAM contents need not be preserved.
     *
     * The remainder of this section assumes that the CN78XX DDRn_RESET_L pin
     * is attached to the RESET# pin of the attached DDR3/DDR4 parts, as will
     * be appropriate in many systems.
     *
     * (In other systems, such as ones that can preserve DDR3/DDR4 part
     * contents while CN78XX is powered down, it will not be appropriate to
     * directly attach the CN78XX DDRn_RESET_L pin to DRESET# of the
     * DDR3/DDR4 parts, and this section may not apply.)
     *
     * The remainder of this section describes the sequence for LMCn.
     *
     * Perform the following six substeps for LMC reset initialization:
     *
     * 1. If not done already, assert DDRn_RESET_L pin by writing
     *    LMC(0..3)_RESET_ CTL[DDR3RST] = 0 without modifying any other
     *    LMC(0..3)_RESET_CTL fields.
     */

    if ( !ddr_memory_preserved(node)) {
        /*
         * 2. Read LMC(0..3)_RESET_CTL and wait for the result.
         */

        BDK_CSR_READ(node, BDK_LMCX_RESET_CTL(ddr_interface_num));

        /*
         * 3. Wait until RESET# assertion-time requirement from JEDEC DDR3/DDR4
         *    specification is satisfied (200 us during a power-on ramp, 100ns when
         *    power is already stable).
         */

        bdk_wait_usec(200);

        /*
         * 4. Deassert DDRn_RESET_L pin by writing LMC(0..3)_RESET_CTL[DDR3RST] = 1
         *    without modifying any other LMC(0..3)_RESET_CTL fields.
         * 5. Read LMC(0..3)_RESET_CTL and wait for the result.
         * 6. Wait a minimum of 500us. This guarantees the necessary T = 500us
         *    delay between DDRn_RESET_L deassertion and DDRn_DIMM*_CKE* assertion.
         */
        cn88xx_lmc_ddr3_reset(node, ddr_interface_num, LMC_DDR3_RESET_DEASSERT);

        /* Toggle Reset Again */
        /* That is, assert, then de-assert, one more time */
        cn88xx_lmc_ddr3_reset(node, ddr_interface_num, LMC_DDR3_RESET_ASSERT);
        cn88xx_lmc_ddr3_reset(node, ddr_interface_num, LMC_DDR3_RESET_DEASSERT);

    } /* if ( !ddr_memory_preserved(node)) */
}

///////////////////////////////////////////////////////////
// start of DBI switchover

/* first pattern example:
   GENERAL_PURPOSE0.DATA == 64'h00ff00ff00ff00ff;
   GENERAL_PURPOSE1.DATA == 64'h00ff00ff00ff00ff;
   GENERAL_PURPOSE0.DATA == 16'h0000;
*/
const uint64_t dbi_pattern[3] = { 0x00ff00ff00ff00ffULL, 0x00ff00ff00ff00ffULL, 0x0000ULL };

// Perform switchover to DBI
static void dbi_switchover_interface(int node, int lmc)
{
    bdk_lmcx_modereg_params0_t modereg_params0;
    bdk_lmcx_modereg_params3_t modereg_params3;
    bdk_lmcx_phy_ctl_t phy_ctl;
    bdk_lmcx_config_t lmcx_config;
    bdk_lmcx_ddr_pll_ctl_t ddr_pll_ctl;
    int rank_mask, rankx, active_ranks;
    uint64_t phys_addr, rank_offset;
    int num_lmcs, errors;
    int dbi_settings[9], byte, unlocked, retries;
    int ecc_ena;
    int rank_max = 1; // FIXME: make this 4 to try all the ranks

    ddr_pll_ctl.u = BDK_CSR_READ(node, BDK_LMCX_DDR_PLL_CTL(0));

    lmcx_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(lmc));
    rank_mask = lmcx_config.s.init_status;
    ecc_ena = lmcx_config.s.ecc_ena;

    // FIXME: must filter out any non-supported configs
    //        ie, no DDR3, no x4 devices, no 81XX
    if ((ddr_pll_ctl.cn83xx.ddr4_mode == 0)  ||
        (lmcx_config.s.mode_x4dev == 1) ||
        CAVIUM_IS_MODEL(CAVIUM_CN81XX)      )
    {
        ddr_print("N%d.LMC%d: DBI switchover: inappropriate device; EXITING...\n",
                  node, lmc);
        return;
    }

    // this should be correct for 1 or 2 ranks, 1 or 2 DIMMs
    num_lmcs = __bdk_dram_get_num_lmc(node);
    rank_offset = 1ull << (28 + lmcx_config.s.pbank_lsb - lmcx_config.s.rank_ena + (num_lmcs/2));

    ddr_print("N%d.LMC%d: DBI switchover: rank mask 0x%x, rank size 0x%016llx.\n",
	      node, lmc, rank_mask, (unsigned long long)rank_offset);

    /* 1. conduct the current init sequence as usual all the way
         after software write leveling.
     */

    read_DAC_DBI_settings(node, lmc, /*DBI*/0, dbi_settings);

    display_DAC_DBI_settings(node, lmc, /* DBI */0, ecc_ena, dbi_settings, " INIT");
 
   /* 2. set DBI related CSRs as below and issue MR write. 
         MODEREG_PARAMS3.WR_DBI=1
         MODEREG_PARAMS3.RD_DBI=1
         PHY_CTL.DBI_MODE_ENA=1
    */
    modereg_params0.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS0(lmc));

    modereg_params3.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS3(lmc));
    modereg_params3.s.wr_dbi = 1;
    modereg_params3.s.rd_dbi = 1;
    DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS3(lmc), modereg_params3.u);

    phy_ctl.u = BDK_CSR_READ(node, BDK_LMCX_PHY_CTL(lmc));
    phy_ctl.s.dbi_mode_ena = 1;
    DRAM_CSR_WRITE(node, BDK_LMCX_PHY_CTL(lmc), phy_ctl.u);

    /*
        there are two options for data to send.  Lets start with (1) and could move to (2) in the future:
        
        1) DBTRAIN_CTL[LFSR_PATTERN_SEL] = 0 (or for older chips where this does not exist)
           set data directly in these reigsters.  this will yield a clk/2 pattern:
           GENERAL_PURPOSE0.DATA == 64'h00ff00ff00ff00ff;
           GENERAL_PURPOSE1.DATA == 64'h00ff00ff00ff00ff;
           GENERAL_PURPOSE0.DATA == 16'h0000;
        2) DBTRAIN_CTL[LFSR_PATTERN_SEL] = 1
           here data comes from the LFSR generating a PRBS pattern
           CHAR_CTL.EN = 0
           CHAR_CTL.SEL = 0; // for PRBS
           CHAR_CTL.DR = 1;
           CHAR_CTL.PRBS = setup for whatever type of PRBS to send
           CHAR_CTL.SKEW_ON = 1;
    */
    DRAM_CSR_WRITE(node, BDK_LMCX_GENERAL_PURPOSE0(lmc), dbi_pattern[0]);
    DRAM_CSR_WRITE(node, BDK_LMCX_GENERAL_PURPOSE1(lmc), dbi_pattern[1]);
    DRAM_CSR_WRITE(node, BDK_LMCX_GENERAL_PURPOSE2(lmc), dbi_pattern[2]);

    /* 
      3. adjust cas_latency (only necessary if RD_DBI is set).
         here is my code for doing this:
 
         if (csr_model.MODEREG_PARAMS3.RD_DBI.value == 1) begin
           case (csr_model.MODEREG_PARAMS0.CL.value)
             0,1,2,3,4: csr_model.MODEREG_PARAMS0.CL.value += 2; // CL 9-13 -> 11-15
             5: begin
                // CL=14, CWL=10,12 gets +2, CLW=11,14 gets +3
                if((csr_model.MODEREG_PARAMS0.CWL.value==1 || csr_model.MODEREG_PARAMS0.CWL.value==3))
                  csr_model.MODEREG_PARAMS0.CL.value = 7; // 14->16
                else
                  csr_model.MODEREG_PARAMS0.CL.value = 13; // 14->17
                end
             6: csr_model.MODEREG_PARAMS0.CL.value = 8; // 15->18
             7: csr_model.MODEREG_PARAMS0.CL.value = 14; // 16->19
             8: csr_model.MODEREG_PARAMS0.CL.value = 15; // 18->21
             default:
             `cn_fatal(("Error mem_cfg (%s) CL (%d) with RD_DBI=1, I am not sure what to do.", 
                        mem_cfg, csr_model.MODEREG_PARAMS3.RD_DBI.value))
           endcase
        end
    */
    if (modereg_params3.s.rd_dbi == 1) {
        int old_cl, new_cl, old_cwl;

        old_cl  = modereg_params0.s.cl;
        old_cwl = modereg_params0.s.cwl;

        switch (old_cl) {
        case 0: case 1: case 2: case 3: case 4: new_cl = old_cl + 2; break; // 9-13->11-15
        // CL=14, CWL=10,12 gets +2, CLW=11,14 gets +3
        case 5: new_cl = ((old_cwl == 1) || (old_cwl == 3)) ? 7 : 13; break;
        case 6: new_cl =  8; break; // 15->18
        case 7: new_cl = 14; break; // 16->19
        case 8: new_cl = 15; break; // 18->21
        default:
            error_print("ERROR: Bad CL value (%d) for DBI switchover.\n", old_cl);
            // FIXME: need to error exit here...
            old_cl = -1;
            new_cl = -1;
            break;
        }
        ddr_print("N%d.LMC%d: DBI switchover: CL ADJ: old_cl 0x%x, old_cwl 0x%x, new_cl 0x%x.\n",
                  node, lmc, old_cl, old_cwl, new_cl);
        modereg_params0.s.cl = new_cl;
        DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS0(lmc), modereg_params0.u);
    }

    /*
      4. issue MRW to MR0 (CL) and MR5 (DBI), using LMC sequence SEQ_CTL[SEQ_SEL] = MRW.
     */
    // Use the default values, from the CSRs fields
    // also, do B-sides for RDIMMs...

    for (rankx = 0; rankx < 4; rankx++) {
        if (!(rank_mask & (1 << rankx)))
            continue;

        // for RDIMMs, B-side writes should get done automatically when the A-side is written
        ddr4_mrw(node, lmc, rankx, -1/* use_default*/,   0/*MRreg*/, 0 /*A-side*/); /* MR0 */
        ddr4_mrw(node, lmc, rankx, -1/* use_default*/,   5/*MRreg*/, 0 /*A-side*/); /* MR5 */

    } /* for (rankx = 0; rankx < 4; rankx++) */

    /*
      5. conduct DBI bit deskew training via the General Purpose R/W sequence (dbtrain).
         may need to run this over and over to get a lock (I need up to 5 in simulation):
         SEQ_CTL[SEQ_SEL] = RW_TRAINING (15)
         DBTRAIN_CTL.CMD_COUNT_EXT = all 1's
         DBTRAIN_CTL.READ_CMD_COUNT = all 1's
         DBTRAIN_CTL.TCCD_SEL = set according to MODEREG_PARAMS3[TCCD_L]
         DBTRAIN_CTL.RW_TRAIN = 1
         DBTRAIN_CTL.READ_DQ_COUNT = dont care
         DBTRAIN_CTL.WRITE_ENA = 1;
         DBTRAIN_CTL.ACTIVATE = 1;
         DBTRAIN_CTL LRANK, PRANK, ROW_A, BG, BA, COLUMN_A = set to a valid address
     */

    // NOW - do the training
    ddr_print("N%d.LMC%d: DBI switchover: TRAINING begins...\n",
                  node, lmc);

    active_ranks = 0;
    for (rankx = 0; rankx < rank_max; rankx++) {
        if (!(rank_mask & (1 << rankx)))
            continue;

        phys_addr = rank_offset * active_ranks;
        // FIXME: now done by test_dram_byte_hw()
        //phys_addr |= (lmc << 7);
        //phys_addr = bdk_numa_get_address(node, phys_addr); // map to node

        active_ranks++;

        retries = 0;

#if 0
        phy_ctl.u = BDK_CSR_READ(node, BDK_LMCX_PHY_CTL(lmc));
        phy_ctl.s.phy_reset = 1; // FIXME: this may reset too much?
        DRAM_CSR_WRITE(node, BDK_LMCX_PHY_CTL(lmc), phy_ctl.u);
#endif

restart_training:

        // NOTE: return is a bitmask of the erroring bytelanes - we only print it
        errors = test_dram_byte_hw(node, lmc, phys_addr, DBTRAIN_DBI, NULL);

        ddr_print("N%d.LMC%d: DBI switchover: TEST: rank %d, phys_addr 0x%llx, errors 0x%x.\n",
                  node, lmc, rankx, phys_addr, errors);

        // NEXT - check for locking
        unlocked = 0;
        read_DAC_DBI_settings(node, lmc, /*DBI*/0, dbi_settings);

        for (byte = 0; byte < (8+ecc_ena); byte++) {
            unlocked += (dbi_settings[byte] & 1) ^ 1;
        }

        // FIXME: print out the DBI settings array after each rank?
        if (rank_max > 1) // only when doing more than 1 rank
            display_DAC_DBI_settings(node, lmc, /* DBI */0, ecc_ena, dbi_settings, " RANK");

        if (unlocked > 0) {
            ddr_print("N%d.LMC%d: DBI switchover: LOCK: %d still unlocked.\n",
                  node, lmc, unlocked);

            retries++;
            if (retries < 10) {
                goto restart_training;
            } else {
                ddr_print("N%d.LMC%d: DBI switchover: LOCK: %d retries exhausted.\n",
                          node, lmc, retries);
            }
        }
    } /* for (rankx = 0; rankx < rank_max; rankx++) */

    // print out the final DBI settings array
    display_DAC_DBI_settings(node, lmc, /* DBI */0, ecc_ena, dbi_settings, "FINAL");
}
// end of DBI switchover
///////////////////////////////////////////////////////////

static uint32_t measure_octeon_ddr_clock(bdk_node_t node,
				  const ddr_configuration_t *ddr_configuration,
				  uint32_t cpu_hertz,
				  uint32_t ddr_hertz,
				  uint32_t ddr_ref_hertz,
				  int ddr_interface_num,
				  uint32_t ddr_interface_mask)
{
	uint64_t core_clocks;
	uint64_t ddr_clocks;
	uint64_t calc_ddr_hertz;

	if (ddr_configuration) {
	    if (initialize_ddr_clock(node,
				     ddr_configuration,
				     cpu_hertz,
				     ddr_hertz,
				     ddr_ref_hertz,
				     ddr_interface_num,
				     ddr_interface_mask) != 0)
		return 0;
	}

	/* Dynamically determine the DDR clock speed */
        core_clocks = bdk_clock_get_count(BDK_CLOCK_TIME);
        ddr_clocks = BDK_CSR_READ(node, BDK_LMCX_DCLK_CNT(ddr_interface_num));
        bdk_wait_usec(100000); /* 100ms */
        ddr_clocks = BDK_CSR_READ(node, BDK_LMCX_DCLK_CNT(ddr_interface_num)) - ddr_clocks;
        core_clocks = bdk_clock_get_count(BDK_CLOCK_TIME) - core_clocks;
        calc_ddr_hertz = ddr_clocks * bdk_clock_get_rate(bdk_numa_local(), BDK_CLOCK_TIME) / core_clocks;

	ddr_print("LMC%d: Measured DDR clock: %llu, cpu clock: %u, ddr clocks: %llu\n",
		  ddr_interface_num, calc_ddr_hertz, cpu_hertz, ddr_clocks);

	/* Check for unreasonable settings. */
	if (calc_ddr_hertz == 0) {
	    error_print("DDR clock misconfigured. Exiting.\n");
	    /* FIXME(dhendrix): We don't exit() in coreboot */
//	    exit(1);
	}
	return calc_ddr_hertz;
}

int octeon_ddr_initialize(bdk_node_t node,
			  uint32_t cpu_hertz,
			  uint32_t ddr_hertz,
			  uint32_t ddr_ref_hertz,
			  uint32_t ddr_interface_mask,
			  const ddr_configuration_t *ddr_configuration,
			  uint32_t *measured_ddr_hertz,
			  int board_type,
			  int board_rev_maj,
			  int board_rev_min)
{
    uint32_t ddr_config_valid_mask = 0;
    int memsize_mbytes = 0;
    const char *s;
    int retval;
    int interface_index;
    uint32_t ddr_max_speed = 1210000000; /* needs to be this high for DDR4 */
    uint32_t calc_ddr_hertz = -1;

#ifndef OCTEON_SDK_VERSION_STRING
# define OCTEON_SDK_VERSION_STRING "Development Build"
#endif

    ddr_print(OCTEON_SDK_VERSION_STRING": $Revision: 102369 $\n");

#ifdef CAVIUM_ONLY
    /* Override speed restrictions to support internal testing. */
    ddr_max_speed = 1210000000;
#endif  /* CAVIUM_ONLY */

    if (ddr_hertz > ddr_max_speed) {
	error_print("DDR clock speed %u exceeds maximum speed supported by "
		    "processor, reducing to %uHz\n",
		    ddr_hertz, ddr_max_speed);
	ddr_hertz = ddr_max_speed;
    }

    // Do this earlier so we can return without doing unnecessary things...
    /* Check for DIMM 0 socket populated for each LMC present */
    for (interface_index = 0; interface_index < 4; ++interface_index) {
	if ((ddr_interface_mask & (1 << interface_index)) &&
	    (validate_dimm(node, &ddr_configuration[interface_index].dimm_config_table[0])) == 1)
	{
	    ddr_config_valid_mask |= (1 << interface_index);
	}
    }

    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX)) {
	int four_lmc_mode = 1;

        // Validate that it can only be 2-LMC mode or 4-LMC mode
        if ((ddr_config_valid_mask != 0x03) && (ddr_config_valid_mask != 0x0f)) {
            puts("ERROR: Invalid LMC configuration detected.\n");
            return -1;
        }

	if ((s = lookup_env_parameter("ddr_four_lmc")) != NULL)
	    four_lmc_mode = !!strtoul(s, NULL, 0);

	if (!four_lmc_mode) {
	    puts("Forcing two-LMC Mode.\n");
	    ddr_config_valid_mask &= ~(3<<2); /* Invalidate LMC[2:3] */
	}
    }

    if (!ddr_config_valid_mask) {
	puts("ERROR: No valid DIMMs detected on any DDR interface.\n");
	return -1;
    }

    {
	/*

	rdf_cnt: Defines the sample point of the LMC response data in
	the DDR-clock/core-clock crossing.  For optimal
	performance set to 10 * (DDR-clock period/core-clock
	period) - 1.  To disable set to 0. All other values
	are reserved.
	*/

	uint64_t rdf_cnt;
	BDK_CSR_INIT(l2c_ctl, node, BDK_L2C_CTL);
	/* It is more convenient to compute the ratio using clock
	   frequencies rather than clock periods. */
	rdf_cnt = (((uint64_t) 10 * cpu_hertz) / ddr_hertz) - 1;
	rdf_cnt = rdf_cnt<256 ? rdf_cnt : 255;
	l2c_ctl.s.rdf_cnt = rdf_cnt;

	if ((s = lookup_env_parameter("early_fill_count")) != NULL)
	    l2c_ctl.s.rdf_cnt = strtoul(s, NULL, 0);

	ddr_print("%-45s : %d, cpu_hertz:%u, ddr_hertz:%u\n", "EARLY FILL COUNT  ",
		  l2c_ctl.s.rdf_cnt, cpu_hertz, ddr_hertz);
	DRAM_CSR_WRITE(node, BDK_L2C_CTL, l2c_ctl.u);
    }

    /* Check to see if we should limit the number of L2 ways. */
    if ((s = lookup_env_parameter("limit_l2_ways")) != NULL) {
        int ways = strtoul(s, NULL, 10);
	limit_l2_ways(node, ways, 1);
    }

    /* We measure the DDR frequency by counting DDR clocks.  We can
     * confirm or adjust the expected frequency as necessary.  We use
     * the measured frequency to make accurate timing calculations
     * used to configure the controller.
     */
    for (interface_index = 0; interface_index < 4; ++interface_index) {
	uint32_t tmp_hertz;

	if (! (ddr_config_valid_mask & (1 << interface_index)))
	    continue;

    try_again:
        // if we are LMC0 
        if (interface_index == 0) {
            // if we are asking for 100 MHz refclk, we can only get it via alternate, so switch to it
            if (ddr_ref_hertz == 100000000) {
                DRAM_CSR_MODIFY(c, node, BDK_LMCX_DDR_PLL_CTL(0), c.s.dclk_alt_refclk_sel = 1);
                bdk_wait_usec(1000); // wait 1 msec
            } else {
                // if we are NOT asking for 100MHz, then reset to (assumed) 50MHz and go on
                DRAM_CSR_MODIFY(c, node, BDK_LMCX_DDR_PLL_CTL(0), c.s.dclk_alt_refclk_sel = 0);
                bdk_wait_usec(1000); // wait 1 msec
            }
        }

	tmp_hertz = measure_octeon_ddr_clock(node,
					     &ddr_configuration[interface_index],
					     cpu_hertz,
					     ddr_hertz,
					     ddr_ref_hertz,
					     interface_index,
					     ddr_config_valid_mask);

        // if we are LMC0 and we are asked for 100 MHz refclk,
        // we must be sure it is available
        // If not, we print an error message, set to 50MHz, and go on...
        if ((interface_index == 0) && (ddr_ref_hertz == 100000000)) {
            // validate that the clock returned is close enough to the clock desired
            // FIXME: is 5% close enough?
            int hertz_diff = _abs((int)tmp_hertz - (int)ddr_hertz);
            if (hertz_diff > ((int)ddr_hertz * 5 / 100)) { // nope, diff is greater than than 5%
                ddr_print("N%d: DRAM init: requested 100 MHz refclk NOT FOUND\n", node);
                ddr_ref_hertz = bdk_clock_get_rate(node, BDK_CLOCK_MAIN_REF);
                set_ddr_clock_initialized(node, 0, 0); // clear the flag before trying again!!
                goto try_again;
            } else {
                ddr_print("N%d: DRAM Init: requested 100 MHz refclk FOUND and SELECTED.\n", node);
            }
        }

	if (tmp_hertz > 0)
	    calc_ddr_hertz = tmp_hertz;

    } /* for (interface_index = 0; interface_index < 4; ++interface_index) */

    if (measured_ddr_hertz)
	*measured_ddr_hertz = calc_ddr_hertz;

    memsize_mbytes = 0;
    for (interface_index = 0; interface_index < 4; ++interface_index) {
	if (! (ddr_config_valid_mask & (1 << interface_index))) { // if LMC has no DIMMs found
            if (ddr_interface_mask & (1 << interface_index)) { // but the LMC is present
                for (int i = 0; i < DDR_CFG_T_MAX_DIMMS; i++) {
                    // check for slot presence
                    if (validate_dimm(node, &ddr_configuration[interface_index].dimm_config_table[i]) == 0)
                        printf("N%d.LMC%d.DIMM%d: Not Present\n", node, interface_index, i);
                }
                error_print("N%d.LMC%d Configuration Completed: 0 MB\n", node, interface_index);
            }
	    continue;
        }

	retval = init_octeon_dram_interface(node,
					    &ddr_configuration[interface_index],
					    calc_ddr_hertz, /* Configure using measured value */
					    cpu_hertz,
					    ddr_ref_hertz,
					    board_type,
					    board_rev_maj,
					    board_rev_min,
					    interface_index,
					    ddr_config_valid_mask);
	if (retval > 0)
	    memsize_mbytes += retval;
    }

    if (memsize_mbytes == 0)
	/* All interfaces failed to initialize, so return error */
	return -1;

    // switch over to DBI mode only for chips that support it, and enabled by envvar
    if (! CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) { // added 81xx and 83xx
        int do_dbi = 0;
        if ((s = lookup_env_parameter("ddr_dbi_switchover")) != NULL) {
            do_dbi = !!strtoul(s, NULL, 10);
        }
        if (do_dbi) {
            ddr_print("DBI Switchover starting...\n");
            for (interface_index = 0; interface_index < 4; ++interface_index) {
                if (! (ddr_config_valid_mask & (1 << interface_index)))
                    continue;
                dbi_switchover_interface(node, interface_index);
            }
            printf("DBI Switchover finished.\n");
        }
    }

    // limit memory size if desired...
    if ((s = lookup_env_parameter("limit_dram_mbytes")) != NULL) {
	unsigned int mbytes = strtoul(s, NULL, 10);
	if (mbytes > 0) {
	    memsize_mbytes = mbytes;
	    printf("Limiting DRAM size to %d MBytes based on limit_dram_mbytes env. variable\n",
		   mbytes);
	}
    }

    return memsize_mbytes;
}

