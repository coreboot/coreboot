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
#include <bdk.h>
#include <bdk-coreboot.h>
#include "libbdk-arch/bdk-csrs-l2c_tad.h"
#include "libbdk-arch/bdk-csrs-mio_fus.h"
#include "dram-internal.h"

#include <stdlib.h>
#include <string.h>
#include <libbdk-hal/bdk-config.h>
#include <libbdk-hal/bdk-l2c.h>
#include <libbdk-hal/bdk-rng.h>
#include <libbdk-trust/bdk-trust.h>
#include <lame_string.h>

#define WODT_MASK_2R_1S 1 // FIXME: did not seem to make much difference with #152 1-slot?

#define DESKEW_RODT_CTL 1

// Set to 1 to use the feature whenever possible automatically.
// When 0, however, the feature is still available, and it can
// be enabled via envvar override "ddr_enable_write_deskew=1".
#define ENABLE_WRITE_DESKEW_DEFAULT 0

#define ENABLE_COMPUTED_VREF_ADJUSTMENT 1

#define RLEXTRAS_PATCH     1 // write to unused RL rank entries
#define WLEXTRAS_PATCH     1 // write to unused WL rank entries
#define ADD_48_OHM_SKIP    1
#define NOSKIP_40_48_OHM   1
#define NOSKIP_48_STACKED  1
#define NOSKIP_FOR_MINI    1
#define NOSKIP_FOR_2S_1R   1
#define MAJORITY_OVER_AVG  1
#define RANK_MAJORITY      MAJORITY_OVER_AVG && 1
#define SW_WL_CHECK_PATCH  1 // check validity after SW adjust
#define HW_WL_MAJORITY     1
#define SWL_TRY_HWL_ALT    HW_WL_MAJORITY && 1 // try HW WL base alternate if available when SW WL fails
#define DISABLE_SW_WL_PASS_2 1

#define HWL_BY_BYTE 0 // FIXME? set to 1 to do HWL a byte at a time (seemed to work better earlier?)

#define USE_ORIG_TEST_DRAM_BYTE 1

// collect and print LMC utilization using SWL software algorithm
#define ENABLE_SW_WLEVEL_UTILIZATION 0

#define COUNT_RL_CANDIDATES 1

#define LOOK_FOR_STUCK_BYTE      0
#define ENABLE_STUCK_BYTE_RESET  0

#define FAILSAFE_CHECK      1

#define PERFECT_BITMASK_COUNTING 1

#define DAC_OVERRIDE_EARLY  1

#define SWL_WITH_HW_ALTS_CHOOSE_SW 0 // FIXME: allow override?

#define DEBUG_VALIDATE_BITMASK 0
#if DEBUG_VALIDATE_BITMASK
#define debug_bitmask_print ddr_print
#else
#define debug_bitmask_print(...)
#endif

#define ENABLE_SLOT_CTL_ACCESS 0
#undef ENABLE_CUSTOM_RLEVEL_TABLE

#define ENABLE_DISPLAY_MPR_PAGE 0
#if ENABLE_DISPLAY_MPR_PAGE
static void Display_MPR_Page_Location(bdk_node_t node, int rank,
                                      int ddr_interface_num, int dimm_count,
                                      int page, int location, uint64_t *mpr_data);
#endif

#define USE_L2_WAYS_LIMIT 1

/* Read out Deskew Settings for DDR */

typedef struct {
    uint16_t bits[8];
} deskew_bytes_t;
typedef struct {
    deskew_bytes_t bytes[9];
} deskew_data_t;

static void
Get_Deskew_Settings(bdk_node_t node, int ddr_interface_num, deskew_data_t *dskdat)
{
    bdk_lmcx_phy_ctl_t phy_ctl;
    bdk_lmcx_config_t  lmc_config;
    int bit_num, bit_index;
    int byte_lane, byte_limit;
    // NOTE: these are for pass 2.x
    int is_t88p2 = !CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X); // added 81xx and 83xx
    int bit_end = (is_t88p2) ? 9 : 8;

    lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
    byte_limit = ((lmc_config.s.mode32b) ? 4 : 8) + lmc_config.s.ecc_ena;

    memset(dskdat, 0, sizeof(*dskdat));

    BDK_CSR_MODIFY(_phy_ctl, node, BDK_LMCX_PHY_CTL(ddr_interface_num),
                   _phy_ctl.s.dsk_dbg_clk_scaler = 3);

    for (byte_lane = 0; byte_lane < byte_limit; byte_lane++) {
        bit_index = 0;
        for (bit_num = 0; bit_num <= bit_end; ++bit_num) {        // NOTE: this is for pass 2.x

            if (bit_num == 4) continue;
            if ((bit_num == 5) && is_t88p2) continue;        // NOTE: this is for pass 2.x

            // set byte lane and bit to read
            BDK_CSR_MODIFY(_phy_ctl, node, BDK_LMCX_PHY_CTL(ddr_interface_num),
                           (_phy_ctl.s.dsk_dbg_bit_sel = bit_num,
                            _phy_ctl.s.dsk_dbg_byte_sel = byte_lane));

            // start read sequence
            BDK_CSR_MODIFY(_phy_ctl, node, BDK_LMCX_PHY_CTL(ddr_interface_num),
                           _phy_ctl.s.dsk_dbg_rd_start = 1);

            // poll for read sequence to complete
            do {
                phy_ctl.u = BDK_CSR_READ(node, BDK_LMCX_PHY_CTL(ddr_interface_num));
            } while (phy_ctl.s.dsk_dbg_rd_complete != 1);

            // record the data
            dskdat->bytes[byte_lane].bits[bit_index] = phy_ctl.s.dsk_dbg_rd_data & 0x3ff;
            bit_index++;

        } /* for (bit_num = 0; bit_num <= bit_end; ++bit_num) */
    } /* for (byte_lane = 0; byte_lane < byte_limit; byte_lane++) */

    return;
}

static void
Display_Deskew_Data(bdk_node_t node, int ddr_interface_num,
                    deskew_data_t *dskdat, int print_enable)
{
    int byte_lane;
    int bit_num;
    uint16_t flags, deskew;
    bdk_lmcx_config_t lmc_config;
    int byte_limit;
    const char *fc = " ?-=+*#&";

    lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
    byte_limit = ((lmc_config.s.mode32b) ? 4 : 8) + lmc_config.s.ecc_ena;

    if (print_enable) {
        VB_PRT(print_enable, "N%d.LMC%d: Deskew Data:              Bit =>      :",
                node, ddr_interface_num);
        for (bit_num = 7; bit_num >= 0; --bit_num)
            VB_PRT(print_enable, " %3d  ", bit_num);
        VB_PRT(print_enable, "\n");
    }

    for (byte_lane = 0; byte_lane < byte_limit; byte_lane++) {
        if (print_enable)
            VB_PRT(print_enable, "N%d.LMC%d: Bit Deskew Byte %d %s               :",
                   node, ddr_interface_num, byte_lane,
                   (print_enable >= VBL_TME) ? "FINAL" : "     ");

        for (bit_num = 7; bit_num >= 0; --bit_num) {

            flags = dskdat->bytes[byte_lane].bits[bit_num] & 7;
            deskew = dskdat->bytes[byte_lane].bits[bit_num] >> 3;

            if (print_enable)
                VB_PRT(print_enable, " %3d %c", deskew, fc[flags^1]);

        } /* for (bit_num = 7; bit_num >= 0; --bit_num) */

        if (print_enable)
            VB_PRT(print_enable, "\n");

    } /* for (byte_lane = 0; byte_lane < byte_limit; byte_lane++) */

    return;
}

static int
change_wr_deskew_ena(bdk_node_t node, int ddr_interface_num, int new_state)
{
    bdk_lmcx_dll_ctl3_t ddr_dll_ctl3;
    int saved_wr_deskew_ena;

    // return original WR_DESKEW_ENA setting
    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
    saved_wr_deskew_ena = !!GET_DDR_DLL_CTL3(wr_deskew_ena);
    if (saved_wr_deskew_ena != !!new_state) { // write it only when changing it
        SET_DDR_DLL_CTL3(wr_deskew_ena, !!new_state);
        DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num), ddr_dll_ctl3.u);
    }
    return saved_wr_deskew_ena;
}

typedef struct {
    int saturated;   // number saturated
    int unlocked;    // number unlocked
    int nibrng_errs; // nibble range errors
    int nibunl_errs; // nibble unlocked errors
    //int nibsat_errs; // nibble saturation errors
    int bitval_errs; // bit value errors
#if LOOK_FOR_STUCK_BYTE
    int bytes_stuck; // byte(s) stuck
#endif
} deskew_counts_t;

#define MIN_BITVAL  17
#define MAX_BITVAL 110

static deskew_counts_t deskew_training_results;
static int deskew_validation_delay = 10000; // FIXME: make this a var for overriding

static void
Validate_Read_Deskew_Training(bdk_node_t node, int rank_mask, int ddr_interface_num,
                              deskew_counts_t *counts, int print_enable)
{
    int byte_lane, bit_num, nib_num;
    int nibrng_errs, nibunl_errs, bitval_errs;
    //int nibsat_errs;
    bdk_lmcx_config_t  lmc_config;
    int16_t nib_min[2], nib_max[2], nib_unl[2]/*, nib_sat[2]*/;
    // NOTE: these are for pass 2.x
    int is_t88p2 = !CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X); // added 81xx and 83xx
    int bit_start = (is_t88p2) ? 9 : 8;
    int byte_limit;
#if LOOK_FOR_STUCK_BYTE
    uint64_t bl_mask[2]; // enough for 128 values
    int bit_values;
#endif
    deskew_data_t dskdat;
    int bit_index;
    int16_t flags, deskew;
    const char *fc = " ?-=+*#&";
    int saved_wr_deskew_ena;
    int bit_last;

    // save original WR_DESKEW_ENA setting, and disable it for read deskew
    saved_wr_deskew_ena = change_wr_deskew_ena(node, ddr_interface_num, 0);

    lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
    byte_limit = ((!lmc_config.s.mode32b) ? 8 : 4) + lmc_config.s.ecc_ena;

    memset(counts, 0, sizeof(deskew_counts_t));

    Get_Deskew_Settings(node, ddr_interface_num, &dskdat);

    if (print_enable) {
        VB_PRT(print_enable, "N%d.LMC%d: Deskew Settings:          Bit =>      :",
                node, ddr_interface_num);
        for (bit_num = 7; bit_num >= 0; --bit_num)
            VB_PRT(print_enable, " %3d  ", bit_num);
        VB_PRT(print_enable, "\n");
    }

    for (byte_lane = 0; byte_lane < byte_limit; byte_lane++) {
        if (print_enable)
            VB_PRT(print_enable, "N%d.LMC%d: Bit Deskew Byte %d %s               :",
                   node, ddr_interface_num, byte_lane,
                   (print_enable >= VBL_TME) ? "FINAL" : "     ");

        nib_min[0] = 127; nib_min[1] = 127;
        nib_max[0] = 0;   nib_max[1] = 0;
        nib_unl[0] = 0;   nib_unl[1] = 0;
        //nib_sat[0] = 0;   nib_sat[1] = 0;

#if LOOK_FOR_STUCK_BYTE
        bl_mask[0] = bl_mask[1] = 0;
#endif

        if ((lmc_config.s.mode32b == 1) && (byte_lane == 4)) {
            bit_index = 3;
            bit_last  = 3;
            if (print_enable)
                VB_PRT(print_enable, "                        ");
        } else {
            bit_index = 7;
            bit_last  = bit_start;
        }

        for (bit_num = bit_last; bit_num >= 0; --bit_num) {        // NOTE: this is for pass 2.x
            if (bit_num == 4) continue;
            if ((bit_num == 5) && is_t88p2) continue;        // NOTE: this is for pass 2.x

            nib_num = (bit_num > 4) ? 1 : 0;

            flags = dskdat.bytes[byte_lane].bits[bit_index] & 7;
            deskew = dskdat.bytes[byte_lane].bits[bit_index] >> 3;
            bit_index--;

            counts->saturated += !!(flags & 6);
            counts->unlocked  +=  !(flags & 1);

            nib_unl[nib_num]  +=  !(flags & 1);
            //nib_sat[nib_num]  += !!(flags & 6);

            if (flags & 1) { // FIXME? only do range when locked
                nib_min[nib_num] = min(nib_min[nib_num], deskew);
                nib_max[nib_num] = max(nib_max[nib_num], deskew);
            }

#if LOOK_FOR_STUCK_BYTE
            bl_mask[(deskew >> 6) & 1] |= 1UL << (deskew & 0x3f);
#endif

            if (print_enable)
                VB_PRT(print_enable, " %3d %c", deskew, fc[flags^1]);

        } /* for (bit_num = bit_last; bit_num >= 0; --bit_num) */

        /*
          Now look for nibble errors:

          For bit 55, it looks like a bit deskew problem. When the upper nibble of byte 6
           needs to go to saturation, bit 7 of byte 6 locks prematurely at 64.
          For DIMMs with raw card A and B, can we reset the deskew training when we encounter this case?
          The reset criteria should be looking at one nibble at a time for raw card A and B;
          if the bit-deskew setting within a nibble is different by > 33, we'll issue a reset
          to the bit deskew training.

          LMC0 Bit Deskew Byte(6): 64 0 - 0 - 0 - 26 61 35 64
        */
        // upper nibble range, then lower nibble range
        nibrng_errs  = ((nib_max[1] - nib_min[1]) > 33) ? 1 : 0;
        nibrng_errs |= ((nib_max[0] - nib_min[0]) > 33) ? 1 : 0;

        // check for nibble all unlocked
        nibunl_errs  = ((nib_unl[0] == 4) || (nib_unl[1] == 4)) ? 1 : 0;

        // check for nibble all saturated
        //nibsat_errs  = ((nib_sat[0] == 4) || (nib_sat[1] == 4)) ? 1 : 0;

        // check for bit value errors, ie < 17 or > 110
        // FIXME? assume max always > MIN_BITVAL and min < MAX_BITVAL
        bitval_errs  = ((nib_max[1] > MAX_BITVAL) || (nib_max[0] > MAX_BITVAL)) ? 1 : 0;
        bitval_errs |= ((nib_min[1] < MIN_BITVAL) || (nib_min[0] < MIN_BITVAL)) ? 1 : 0;

        if (((nibrng_errs != 0) || (nibunl_errs != 0) /*|| (nibsat_errs != 0)*/ || (bitval_errs != 0))
            && print_enable)
        {
            VB_PRT(print_enable, " %c%c%c%c",
                   (nibrng_errs)?'R':' ',
                   (nibunl_errs)?'U':' ',
                   (bitval_errs)?'V':' ',
                   /*(nibsat_errs)?'S':*/' ');
        }

#if LOOK_FOR_STUCK_BYTE
        bit_values = __builtin_popcountl(bl_mask[0]) + __builtin_popcountl(bl_mask[1]);
        if (bit_values < 3) {
            counts->bytes_stuck |= (1 << byte_lane);
            if (print_enable)
                VB_PRT(print_enable, "X");
        }
#endif
        if (print_enable)
            VB_PRT(print_enable, "\n");

        counts->nibrng_errs |= (nibrng_errs << byte_lane);
        counts->nibunl_errs |= (nibunl_errs << byte_lane);
        //counts->nibsat_errs |= (nibsat_errs << byte_lane);
        counts->bitval_errs |= (bitval_errs << byte_lane);

#if LOOK_FOR_STUCK_BYTE
        // just for completeness, allow print of the stuck values bitmask after the bytelane print
        if ((bit_values < 3) && print_enable) {
            VB_PRT(VBL_DEV, "N%d.LMC%d: Deskew byte %d STUCK on value 0x%016lx.%016lx\n",
                   node, ddr_interface_num, byte_lane,
                   bl_mask[1], bl_mask[0]);
        }
#endif

    } /* for (byte_lane = 0; byte_lane < byte_limit; byte_lane++) */

    // restore original WR_DESKEW_ENA setting
    change_wr_deskew_ena(node, ddr_interface_num, saved_wr_deskew_ena);

    return;
}

unsigned short load_dac_override(int node, int ddr_interface_num,
                                        int dac_value, int byte)
{
    bdk_lmcx_dll_ctl3_t ddr_dll_ctl3;
    int bytex = (byte == 0x0A) ? byte : byte + 1; // single bytelanes incr by 1; A is for ALL

    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));

    SET_DDR_DLL_CTL3(byte_sel, bytex);
    SET_DDR_DLL_CTL3(offset, dac_value >> 1); // only 7-bit field, use MS bits

    ddr_dll_ctl3.s.bit_select    = 0x9; /* No-op */
    DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num), ddr_dll_ctl3.u);

    ddr_dll_ctl3.s.bit_select    = 0xC; /* Vref bypass setting load */
    DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num), ddr_dll_ctl3.u);

    ddr_dll_ctl3.s.bit_select    = 0xD; /* Vref bypass on. */
    DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num), ddr_dll_ctl3.u);

    ddr_dll_ctl3.s.bit_select    = 0x9; /* No-op */
    DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num), ddr_dll_ctl3.u);

    return ((unsigned short) GET_DDR_DLL_CTL3(offset));
}

// arg dac_or_dbi is 1 for DAC, 0 for DBI
// returns 9 entries (bytelanes 0 through 8) in settings[]
// returns 0 if OK, -1 if a problem
int read_DAC_DBI_settings(int node, int ddr_interface_num,
                          int dac_or_dbi, int *settings)
{
    bdk_lmcx_phy_ctl_t phy_ctl;
    int byte_lane, bit_num;
    int deskew;
    int dac_value;
    int is_t88p2 = !CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X); // added 81xx and 83xx

    phy_ctl.u = BDK_CSR_READ(node, BDK_LMCX_PHY_CTL(ddr_interface_num));
    phy_ctl.s.dsk_dbg_clk_scaler = 3;
    DRAM_CSR_WRITE(node, BDK_LMCX_PHY_CTL(ddr_interface_num), phy_ctl.u);

    bit_num = (dac_or_dbi) ? 4 : 5;
    if ((bit_num == 5) && !is_t88p2) { // NOTE: this is for pass 1.x
        return -1;
    }

    for (byte_lane = 8; byte_lane >= 0 ; --byte_lane) { // FIXME: always assume ECC is available

        //set byte lane and bit to read
        phy_ctl.s.dsk_dbg_bit_sel = bit_num;
        phy_ctl.s.dsk_dbg_byte_sel = byte_lane;
        DRAM_CSR_WRITE(node, BDK_LMCX_PHY_CTL(ddr_interface_num), phy_ctl.u);

        //start read sequence
        phy_ctl.u = BDK_CSR_READ(node, BDK_LMCX_PHY_CTL(ddr_interface_num));
        phy_ctl.s.dsk_dbg_rd_start = 1;
        DRAM_CSR_WRITE(node, BDK_LMCX_PHY_CTL(ddr_interface_num), phy_ctl.u);

        //poll for read sequence to complete
        do {
            phy_ctl.u = BDK_CSR_READ(node, BDK_LMCX_PHY_CTL(ddr_interface_num));
        } while (phy_ctl.s.dsk_dbg_rd_complete != 1);

        deskew = phy_ctl.s.dsk_dbg_rd_data /*>> 3*/; // leave the flag bits for DBI
        dac_value = phy_ctl.s.dsk_dbg_rd_data & 0xff;

        settings[byte_lane] = (dac_or_dbi) ? dac_value : deskew;

    } /* for (byte_lane = 8; byte_lane >= 0 ; --byte_lane) { */

    return 0;
}

// print out the DBI settings array
// arg dac_or_dbi is 1 for DAC, 0 for DBI
void
display_DAC_DBI_settings(int node, int lmc, int dac_or_dbi,
                         int ecc_ena, int *settings, const char *title)
{
    int byte;
    int flags;
    int deskew;
    const char *fc = " ?-=+*#&";

    ddr_print("N%d.LMC%d: %s %s Deskew Settings %d:0 :",
              node, lmc, title, (dac_or_dbi)?"DAC":"DBI", 7+ecc_ena);
    for (byte = (7+ecc_ena); byte >= 0; --byte) { // FIXME: what about 32-bit mode?
        if (dac_or_dbi) { // DAC
            flags  = 1; // say its locked to get blank
            deskew = settings[byte] & 0xff;
        } else { // DBI
            flags  = settings[byte] & 7;
            deskew = (settings[byte] >> 3) & 0x7f;
        }
        ddr_print(" %3d %c", deskew, fc[flags^1]);
    }
    ddr_print("\n");
}

// Evaluate the DAC settings array
static int
evaluate_DAC_settings(int ddr_interface_64b, int ecc_ena, int *settings)
{
    int byte, dac;
    int last = (ddr_interface_64b) ? 7 : 3;

    // this looks only for DAC values that are EVEN
    for (byte = (last+ecc_ena); byte >= 0; --byte) {
        dac  = settings[byte] & 0xff;
        if ((dac & 1) == 0)
            return 1;
    }
    return 0;
}

static void
Perform_Offset_Training(bdk_node_t node, int rank_mask, int ddr_interface_num)
{
    bdk_lmcx_phy_ctl_t lmc_phy_ctl;
    uint64_t orig_phy_ctl;
    const char *s;

    /*
     * 6.9.8 LMC Offset Training
     *
     * LMC requires input-receiver offset training.
     *
     * 1. Write LMC(0)_PHY_CTL[DAC_ON] = 1
     */
    lmc_phy_ctl.u = BDK_CSR_READ(node, BDK_LMCX_PHY_CTL(ddr_interface_num));
    orig_phy_ctl = lmc_phy_ctl.u;
    lmc_phy_ctl.s.dac_on = 1;

    // allow full CSR override
    if ((s = lookup_env_parameter_ull("ddr_phy_ctl")) != NULL) {
        lmc_phy_ctl.u    = strtoull(s, NULL, 0);
    }

    // do not print or write if CSR does not change...
    if (lmc_phy_ctl.u != orig_phy_ctl) {
        ddr_print("PHY_CTL                                       : 0x%016llx\n", lmc_phy_ctl.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_PHY_CTL(ddr_interface_num), lmc_phy_ctl.u);
    }

#if 0
    // FIXME? do we really need to show RODT here?
    bdk_lmcx_comp_ctl2_t lmc_comp_ctl2;
    lmc_comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));
    ddr_print("Read ODT_CTL                                  : 0x%x (%d ohms)\n",
              lmc_comp_ctl2.s.rodt_ctl, imp_values->rodt_ohms[lmc_comp_ctl2.s.rodt_ctl]);
#endif

    /*
     * 2. Write LMC(0)_SEQ_CTL[SEQ_SEL] = 0x0B and
     *    LMC(0)_SEQ_CTL[INIT_START] = 1.
     *
     * 3. Wait for LMC(0)_SEQ_CTL[SEQ_COMPLETE] to be set to 1.
     */
    perform_octeon3_ddr3_sequence(node, rank_mask, ddr_interface_num, 0x0B); /* Offset training sequence */

}

static void
Perform_Internal_VREF_Training(bdk_node_t node, int rank_mask, int ddr_interface_num)
{
    bdk_lmcx_ext_config_t ext_config;

    /*
     * 6.9.9 LMC Internal Vref Training
     *
     * LMC requires input-reference-voltage training.
     *
     * 1. Write LMC(0)_EXT_CONFIG[VREFINT_SEQ_DESKEW] = 0.
     */
    ext_config.u = BDK_CSR_READ(node, BDK_LMCX_EXT_CONFIG(ddr_interface_num));
    ext_config.s.vrefint_seq_deskew = 0;

    VB_PRT(VBL_SEQ, "N%d.LMC%d: Performing LMC sequence: vrefint_seq_deskew = %d\n",
           node, ddr_interface_num, ext_config.s.vrefint_seq_deskew);

    DRAM_CSR_WRITE(node, BDK_LMCX_EXT_CONFIG(ddr_interface_num), ext_config.u);

    /*
     * 2. Write LMC(0)_SEQ_CTL[SEQ_SEL] = 0x0a and
     *    LMC(0)_SEQ_CTL[INIT_START] = 1.
     *
     * 3. Wait for LMC(0)_SEQ_CTL[SEQ_COMPLETE] to be set to 1.
     */
    perform_octeon3_ddr3_sequence(node, rank_mask, ddr_interface_num, 0x0A); /* LMC Internal Vref Training */
}

#define dbg_avg(format, ...) VB_PRT(VBL_DEV, format, ##__VA_ARGS__)
static int
process_samples_average(int16_t *bytes, int num_samples, int lmc, int lane_no)
{
    int i, savg, sadj, sum = 0, rng, ret, asum, trunc;
    int16_t smin = 32767, smax = -32768;

    dbg_avg("DBG_AVG%d.%d: ", lmc, lane_no);

    for (i = 0; i < num_samples; i++) {
        sum += bytes[i];
        if (bytes[i] < smin) smin = bytes[i];
        if (bytes[i] > smax) smax = bytes[i];
        dbg_avg(" %3d", bytes[i]);
    }
    rng = smax - smin + 1;

    dbg_avg(" (%3d, %3d, %2d)", smin, smax, rng);

    asum = sum - smin - smax;

    savg = divide_nint(sum * 10, num_samples);

    sadj = divide_nint(asum * 10, (num_samples - 2));

    trunc = asum / (num_samples - 2);

    dbg_avg(" [%3d.%d, %3d.%d, %3d]", savg/10, savg%10, sadj/10, sadj%10, trunc);

    sadj = divide_nint(sadj, 10);
    if (trunc & 1)
        ret = trunc;
    else if (sadj & 1)
        ret = sadj;
    else
        ret = trunc + 1;

    dbg_avg(" -> %3d\n", ret);

    return ret;
}


#define DEFAULT_SAT_RETRY_LIMIT    11    // 1 + 10 retries
static int default_lock_retry_limit = 20;    // 20 retries // FIXME: make a var for overriding

static int
Perform_Read_Deskew_Training(bdk_node_t node, int rank_mask, int ddr_interface_num,
                             int spd_rawcard_AorB, int print_flags, int ddr_interface_64b)
{
    int unsaturated, locked;
    //int nibble_sat;
    int sat_retries, lock_retries, lock_retries_total, lock_retries_limit;
    int print_first;
    int print_them_all;
    deskew_counts_t dsk_counts;
    uint64_t saved_wr_deskew_ena;
#if DESKEW_RODT_CTL
    bdk_lmcx_comp_ctl2_t comp_ctl2;
    int save_deskew_rodt_ctl = -1;
#endif
    int is_t88p2 = !CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X); // added 81xx and 83xx

    VB_PRT(VBL_FAE, "N%d.LMC%d: Performing Read Deskew Training.\n", node, ddr_interface_num);

    // save original WR_DESKEW_ENA setting, and disable it for read deskew
    saved_wr_deskew_ena = change_wr_deskew_ena(node, ddr_interface_num, 0);

    sat_retries = 0;
    lock_retries_total = 0;
    unsaturated = 0;
    print_first = VBL_FAE; // print the first one, FAE and above
    print_them_all = dram_is_verbose(VBL_DEV4); // set to true for printing all normal deskew attempts

    int loops, normal_loops = 1; // default to 1 NORMAL deskew training op...
    const char *s;
    if ((s = getenv("ddr_deskew_normal_loops")) != NULL) {
        normal_loops = strtoul(s, NULL, 0);
    }

#if LOOK_FOR_STUCK_BYTE
    // provide override for STUCK BYTE RESETS
    int do_stuck_reset = ENABLE_STUCK_BYTE_RESET;
    if ((s = getenv("ddr_enable_stuck_byte_reset")) != NULL) {
        do_stuck_reset = !!strtoul(s, NULL, 0);
    }
#endif

#if DESKEW_RODT_CTL
    if ((s = getenv("ddr_deskew_rodt_ctl")) != NULL) {
        int deskew_rodt_ctl = strtoul(s, NULL, 0);
        comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));
        save_deskew_rodt_ctl = comp_ctl2.s.rodt_ctl;
        comp_ctl2.s.rodt_ctl = deskew_rodt_ctl;
        DRAM_CSR_WRITE(node, BDK_LMCX_COMP_CTL2(ddr_interface_num), comp_ctl2.u);
    }
#endif

    lock_retries_limit = default_lock_retry_limit;
    if (! CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) // added 81xx and 83xx
        lock_retries_limit *= 2; // give pass 2.0 twice as many

    do { /* while (sat_retries < sat_retry_limit) */

        /*
         * 6.9.10 LMC Deskew Training
         *
         * LMC requires input-read-data deskew training.
         *
         * 1. Write LMC(0)_EXT_CONFIG[VREFINT_SEQ_DESKEW] = 1.
         */
        VB_PRT(VBL_SEQ, "N%d.LMC%d: Performing LMC sequence: Set vrefint_seq_deskew = 1\n",
                node, ddr_interface_num);
        DRAM_CSR_MODIFY(ext_config, node, BDK_LMCX_EXT_CONFIG(ddr_interface_num),
                        ext_config.s.vrefint_seq_deskew = 1); /* Set Deskew sequence */

        /*
         * 2. Write LMC(0)_SEQ_CTL[SEQ_SEL] = 0x0A and
         *    LMC(0)_SEQ_CTL[INIT_START] = 1.
         *
         * 3. Wait for LMC(0)_SEQ_CTL[SEQ_COMPLETE] to be set to 1.
         */
        DRAM_CSR_MODIFY(phy_ctl, node, BDK_LMCX_PHY_CTL(ddr_interface_num),
                        phy_ctl.s.phy_dsk_reset = 1); /* RESET Deskew sequence */
        perform_octeon3_ddr3_sequence(node, rank_mask, ddr_interface_num, 0x0A); /* LMC Deskew Training */

        lock_retries = 0;

    perform_read_deskew_training:
        // maybe perform the NORMAL deskew training sequence multiple times before looking at lock status
        for (loops = 0; loops < normal_loops; loops++) {
            DRAM_CSR_MODIFY(phy_ctl, node, BDK_LMCX_PHY_CTL(ddr_interface_num),
                            phy_ctl.s.phy_dsk_reset = 0); /* Normal Deskew sequence */
            perform_octeon3_ddr3_sequence(node, rank_mask, ddr_interface_num, 0x0A); /* LMC Deskew Training */
        }
        // Moved this from Validate_Read_Deskew_Training
        /* Allow deskew results to stabilize before evaluating them. */
        bdk_wait_usec(deskew_validation_delay);

        // Now go look at lock and saturation status...
        Validate_Read_Deskew_Training(node, rank_mask, ddr_interface_num, &dsk_counts, print_first);
        if (print_first && !print_them_all) // after printing the first and not doing them all, no more
            print_first = 0;

        unsaturated = (dsk_counts.saturated == 0);
        locked = (dsk_counts.unlocked == 0);
        //nibble_sat = (dsk_counts.nibsat_errs != 0);

        // only do locking retries if unsaturated or rawcard A or B, otherwise full SAT retry
        if (unsaturated || (spd_rawcard_AorB && !is_t88p2 /*&& !nibble_sat*/)) {
            if (!locked) { // and not locked
                lock_retries++;
                lock_retries_total++;
                if (lock_retries <= lock_retries_limit) {
                    goto perform_read_deskew_training;
                } else {
                    VB_PRT(VBL_TME, "N%d.LMC%d: LOCK RETRIES failed after %d retries\n",
                            node, ddr_interface_num, lock_retries_limit);
                }
            } else {
                if (lock_retries_total > 0) // only print if we did try
                    VB_PRT(VBL_TME, "N%d.LMC%d: LOCK RETRIES successful after %d retries\n",
                            node, ddr_interface_num, lock_retries);
            }
        } /* if (unsaturated || spd_rawcard_AorB) */

        ++sat_retries;

#if LOOK_FOR_STUCK_BYTE
        // FIXME: this is a bit of a hack at the moment...
        // We want to force a Deskew RESET hopefully to unstick the bytes values
        // and then resume normal deskew training as usual.
        // For now, do only if it is all locked...
        if (locked && (dsk_counts.bytes_stuck != 0)) {
            BDK_CSR_INIT(lmc_config, node, BDK_LMCX_CONFIG(ddr_interface_num));
            if (do_stuck_reset && lmc_config.s.mode_x4dev) { // FIXME: only when x4!!
                unsaturated = 0; // to always make sure the while continues
                VB_PRT(VBL_TME, "N%d.LMC%d: STUCK BYTE (0x%x), forcing deskew RESET\n",
                          node, ddr_interface_num, dsk_counts.bytes_stuck);
                continue; // bypass the rest to get back to the RESET
            } else {
                VB_PRT(VBL_TME, "N%d.LMC%d: STUCK BYTE (0x%x), ignoring deskew RESET\n",
                          node, ddr_interface_num, dsk_counts.bytes_stuck);
            }
        }
#endif
        /*
         * At this point, check for a DDR4 RDIMM that will not benefit from SAT retries; if so, no retries
         */
        if (spd_rawcard_AorB && !is_t88p2 /*&& !nibble_sat*/) {
            VB_PRT(VBL_TME, "N%d.LMC%d: Read Deskew Training Loop: Exiting for RAWCARD == A or B.\n",
                    node, ddr_interface_num);
            break; // no sat or lock retries
        }

    } while (!unsaturated && (sat_retries < DEFAULT_SAT_RETRY_LIMIT));

#if DESKEW_RODT_CTL
    if (save_deskew_rodt_ctl != -1) {
        comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));
        comp_ctl2.s.rodt_ctl = save_deskew_rodt_ctl;
        DRAM_CSR_WRITE(node, BDK_LMCX_COMP_CTL2(ddr_interface_num), comp_ctl2.u);
    }
#endif

    VB_PRT(VBL_FAE, "N%d.LMC%d: Read Deskew Training %s. %d sat-retries, %d lock-retries\n",
           node, ddr_interface_num,
           (sat_retries >= DEFAULT_SAT_RETRY_LIMIT) ? "Timed Out" : "Completed",
           sat_retries-1, lock_retries_total);

    // restore original WR_DESKEW_ENA setting
    change_wr_deskew_ena(node, ddr_interface_num, saved_wr_deskew_ena);

    if ((dsk_counts.nibrng_errs != 0) || (dsk_counts.nibunl_errs != 0)) {
        debug_print("N%d.LMC%d: NIBBLE ERROR(S) found, returning FAULT\n",
                  node, ddr_interface_num);
        return -1; // we did retry locally, they did not help
    }

    // NOTE: we (currently) always print one last training validation before starting Read Leveling...

    return 0;
}

static void
do_write_deskew_op(bdk_node_t node, int ddr_interface_num,
                   int bit_sel, int byte_sel, int ena)
{
    bdk_lmcx_dll_ctl3_t ddr_dll_ctl3;

    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
    SET_DDR_DLL_CTL3(bit_select,    bit_sel);
    SET_DDR_DLL_CTL3(byte_sel,      byte_sel);
    SET_DDR_DLL_CTL3(wr_deskew_ena, ena);
    DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num),        ddr_dll_ctl3.u);

    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
}

static void
set_write_deskew_offset(bdk_node_t node, int ddr_interface_num,
                        int bit_sel, int byte_sel, int offset)
{
    bdk_lmcx_dll_ctl3_t ddr_dll_ctl3;

    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
    SET_DDR_DLL_CTL3(bit_select, bit_sel);
    SET_DDR_DLL_CTL3(byte_sel,   byte_sel);
    SET_DDR_DLL_CTL3(offset,     offset);
    DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num),        ddr_dll_ctl3.u);

    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
    SET_DDR_DLL_CTL3(wr_deskew_ld, 1);
    DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num),        ddr_dll_ctl3.u);

    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
}

static void
Update_Write_Deskew_Settings(bdk_node_t node, int ddr_interface_num, deskew_data_t *dskdat)
{
    bdk_lmcx_config_t lmc_config;
    int bit_num;
    int byte_lane, byte_limit;

    lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
    byte_limit = ((lmc_config.s.mode32b) ? 4 : 8) + lmc_config.s.ecc_ena;

    for (byte_lane = 0; byte_lane < byte_limit; byte_lane++) {
        for (bit_num = 0; bit_num <= 7; ++bit_num) {

            set_write_deskew_offset(node, ddr_interface_num, bit_num, byte_lane + 1,
                                    dskdat->bytes[byte_lane].bits[bit_num]);

        } /* for (bit_num = 0; bit_num <= 7; ++bit_num) */
    } /* for (byte_lane = 0; byte_lane < byte_limit; byte_lane++) */

    return;
}

#define ALL_BYTES 0x0A
#define BS_NOOP  0x09
#define BS_RESET 0x0F
#define BS_REUSE 0x0A

// set all entries to the same value (used during training)
static void
Set_Write_Deskew_Settings(bdk_node_t node, int ddr_interface_num, int value)
{
    bdk_lmcx_dll_ctl3_t ddr_dll_ctl3;
    int bit_num;

    VB_PRT(VBL_DEV2, "N%d.LMC%d: SetWriteDeskew: WRITE %d\n", node, ddr_interface_num, value);

    for (bit_num = 0; bit_num <= 7; ++bit_num) {

        // write a bit-deskew value to all bit-lanes of all bytes
        ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
        SET_DDR_DLL_CTL3(bit_select, bit_num);
        SET_DDR_DLL_CTL3(byte_sel,   ALL_BYTES); // FIXME? will this work in 32-bit mode?
        SET_DDR_DLL_CTL3(offset,     value);
        DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num), ddr_dll_ctl3.u);

        ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
        SET_DDR_DLL_CTL3(wr_deskew_ld, 1);
        DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num), ddr_dll_ctl3.u);

    } /* for (bit_num = 0; bit_num <= 7; ++bit_num) */

#if 0
    // FIXME: for debug use only
    Get_Deskew_Settings(node, ddr_interface_num, &dskdat);
    Display_Deskew_Data(node, ddr_interface_num, &dskdat, VBL_NORM);
#endif

    return;
}

typedef struct {
    uint8_t count[8];
    uint8_t start[8];
    uint8_t best_count[8];
    uint8_t best_start[8];
} deskew_bytelane_t;
typedef struct {
    deskew_bytelane_t bytes[9];
} deskew_rank_t;

deskew_rank_t deskew_history[4];

#define DSKVAL_INCR 4

static void
Neutral_Write_Deskew_Setup(bdk_node_t node, int ddr_interface_num)
{
    // first: NO-OP, Select all bytes, Disable write bit-deskew
    ddr_print("N%d.LMC%d: NEUTRAL Write Deskew Setup: first: NOOP\n", node, ddr_interface_num);
    do_write_deskew_op(node, ddr_interface_num, BS_NOOP, ALL_BYTES, 0);
    //Get_Deskew_Settings(node, ddr_interface_num, &dskdat);
    //Display_Deskew_Data(node, ddr_interface_num, &dskdat, VBL_NORM);

    // enable write bit-deskew and RESET the settings
    ddr_print("N%d.LMC%d: NEUTRAL Write Deskew Setup: wr_ena: RESET\n", node, ddr_interface_num);
    do_write_deskew_op(node, ddr_interface_num, BS_RESET, ALL_BYTES, 1);
    //Get_Deskew_Settings(node, ddr_interface_num, &dskdat);
    //Display_Deskew_Data(node, ddr_interface_num, &dskdat, VBL_NORM);
}

static void
Perform_Write_Deskew_Training(bdk_node_t node, int ddr_interface_num)
{
    deskew_data_t dskdat;
    int byte, bit_num;
    int dskval, rankx, rank_mask, active_ranks, errors, bit_errs;
    uint64_t hw_rank_offset;
    uint64_t bad_bits[2];
    uint64_t phys_addr;
    deskew_rank_t *dhp;
    int num_lmcs = __bdk_dram_get_num_lmc(node);

    BDK_CSR_INIT(lmcx_config, node, BDK_LMCX_CONFIG(ddr_interface_num));
    rank_mask = lmcx_config.s.init_status; // FIXME: is this right when we run?

    // this should be correct for 1 or 2 ranks, 1 or 2 DIMMs
    hw_rank_offset = 1ull << (28 + lmcx_config.s.pbank_lsb - lmcx_config.s.rank_ena + (num_lmcs/2));

    VB_PRT(VBL_FAE, "N%d.LMC%d: Performing Write Deskew Training.\n", node, ddr_interface_num);

    // first: NO-OP, Select all bytes, Disable write bit-deskew
    ddr_print("N%d.LMC%d: WriteDeskewConfig: first: NOOP\n", node, ddr_interface_num);
    do_write_deskew_op(node, ddr_interface_num, BS_NOOP, ALL_BYTES, 0);
    //Get_Deskew_Settings(node, ddr_interface_num, &dskdat);
    //Display_Deskew_Data(node, ddr_interface_num, &dskdat, VBL_NORM);

    // enable write bit-deskew and RESET the settings
    ddr_print("N%d.LMC%d: WriteDeskewConfig: wr_ena: RESET\n", node, ddr_interface_num);
    do_write_deskew_op(node, ddr_interface_num, BS_RESET, ALL_BYTES, 1);
    //Get_Deskew_Settings(node, ddr_interface_num, &dskdat);
    //Display_Deskew_Data(node, ddr_interface_num, &dskdat, VBL_NORM);

#if 0
    // enable write bit-deskew and REUSE read bit-deskew settings
    ddr_print("N%d.LMC%d: WriteDeskewConfig: wr_ena: REUSE\n", node, ddr_interface_num);
    do_write_deskew_op(node, ddr_interface_num, BS_REUSE, ALL_BYTES, 1);
    Get_Deskew_Settings(node, ddr_interface_num, &dskdat);
    Display_Deskew_Data(node, ddr_interface_num, &dskdat, VBL_NORM);
#endif

#if 1
    memset(deskew_history, 0, sizeof(deskew_history));

    for (dskval = 0; dskval < 128; dskval += DSKVAL_INCR) {

        Set_Write_Deskew_Settings(node, ddr_interface_num, dskval);

        active_ranks = 0;
        for (rankx = 0; rankx < 4; rankx++) {
            if (!(rank_mask & (1 << rankx)))
                continue;
            dhp = &deskew_history[rankx];
            phys_addr = hw_rank_offset * active_ranks;
            active_ranks++;

            errors = test_dram_byte_hw(node, ddr_interface_num, phys_addr, 0, bad_bits);

            for (byte = 0; byte <= 8; byte++) { // do bytelane(s)

                // check errors
                if (errors & (1 << byte)) { // yes, error(s) in the byte lane in this rank
                    bit_errs = ((byte == 8) ? bad_bits[1] : bad_bits[0] >> (8 * byte)) & 0xFFULL;

                    VB_PRT(VBL_DEV2, "N%d.LMC%d.R%d: Byte %d Value %d: Address 0x%012llx errors 0x%x/0x%x\n",
                           node, ddr_interface_num, rankx, byte,
                           dskval, phys_addr, errors, bit_errs);

                    for (bit_num = 0; bit_num <= 7; bit_num++) {
                        if (!(bit_errs & (1 << bit_num)))
                            continue;
                        if (dhp->bytes[byte].count[bit_num] > 0) { // had started run
                            VB_PRT(VBL_DEV2, "N%d.LMC%d.R%d: Byte %d Bit %d Value %d: stopping a run here\n",
                                   node, ddr_interface_num, rankx, byte, bit_num, dskval);
                            dhp->bytes[byte].count[bit_num] = 0;   // stop now
                        }
                    } /* for (bit_num = 0; bit_num <= 7; bit_num++) */

                    // FIXME: else had not started run - nothing else to do?
                } else { // no error in the byte lane
                    for (bit_num = 0; bit_num <= 7; bit_num++) {
                        if (dhp->bytes[byte].count[bit_num] == 0) { // first success, set run start
                            VB_PRT(VBL_DEV2, "N%d.LMC%d.R%d: Byte %d Bit %d Value %d: starting a run here\n",
                                   node, ddr_interface_num, rankx, byte, bit_num, dskval);
                            dhp->bytes[byte].start[bit_num] = dskval;
                        }
                        dhp->bytes[byte].count[bit_num] += DSKVAL_INCR; // bump run length

                        // is this now the biggest window?
                        if (dhp->bytes[byte].count[bit_num] > dhp->bytes[byte].best_count[bit_num]) {
                            dhp->bytes[byte].best_count[bit_num] = dhp->bytes[byte].count[bit_num];
                            dhp->bytes[byte].best_start[bit_num] = dhp->bytes[byte].start[bit_num];
                            VB_PRT(VBL_DEV2, "N%d.LMC%d.R%d: Byte %d Bit %d Value %d: updating best to %d/%d\n",
                                   node, ddr_interface_num, rankx, byte, bit_num, dskval,
                                   dhp->bytes[byte].best_start[bit_num],
                                   dhp->bytes[byte].best_count[bit_num]);
                        }
                    } /* for (bit_num = 0; bit_num <= 7; bit_num++) */
                } /* error in the byte lane */
            } /* for (byte = 0; byte <= 8; byte++) */
        } /* for (rankx = 0; rankx < 4; rankx++) */
    } /* for (dskval = 0; dskval < 128; dskval++) */


    for (byte = 0; byte <= 8; byte++) { // do bytelane(s)

        for (bit_num = 0; bit_num <= 7; bit_num++) { // do bits
            int bit_beg, bit_end;

            bit_beg = 0;
            bit_end = 128;

            for (rankx = 0; rankx < 4; rankx++) { // merge ranks
                int rank_beg, rank_end, rank_count;
                if (!(rank_mask & (1 << rankx)))
                    continue;

                dhp = &deskew_history[rankx];
                rank_beg = dhp->bytes[byte].best_start[bit_num];
                rank_count = dhp->bytes[byte].best_count[bit_num];

                if (!rank_count) {
                    VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: Byte %d Bit %d: EMPTY\n",
                           node, ddr_interface_num, rankx, byte, bit_num);
                    continue;
                }

                bit_beg = max(bit_beg, rank_beg);
                rank_end = rank_beg + rank_count - DSKVAL_INCR;
                bit_end = min(bit_end, rank_end);

            } /* for (rankx = 0; rankx < 4; rankx++) */

            dskdat.bytes[byte].bits[bit_num] = (bit_end + bit_beg) / 2;

        } /* for (bit_num = 0; bit_num <= 7; bit_num++) */
    } /* for (byte = 0; byte <= 8; byte++) */

#endif

    // update the write bit-deskew settings with final settings
    ddr_print("N%d.LMC%d: WriteDeskewConfig: wr_ena: UPDATE\n", node, ddr_interface_num);
    Update_Write_Deskew_Settings(node, ddr_interface_num, &dskdat);
    Get_Deskew_Settings(node, ddr_interface_num, &dskdat);
    Display_Deskew_Data(node, ddr_interface_num, &dskdat, VBL_NORM);

    // last: NO-OP, Select all bytes, MUST leave write bit-deskew enabled
    ddr_print("N%d.LMC%d: WriteDeskewConfig: last: wr_ena: NOOP\n", node, ddr_interface_num);
    do_write_deskew_op(node, ddr_interface_num, BS_NOOP, ALL_BYTES, 1);
    //Get_Deskew_Settings(node, ddr_interface_num, &dskdat);
    //Display_Deskew_Data(node, ddr_interface_num, &dskdat, VBL_NORM);

#if 0
    // FIXME: disable/delete this when write bit-deskew works...
    // final: NO-OP, Select all bytes, do NOT leave write bit-deskew enabled
    ddr_print("N%d.LMC%d: WriteDeskewConfig: final: read: NOOP\n", node, ddr_interface_num);
    do_write_deskew_op(node, ddr_interface_num, BS_NOOP, ALL_BYTES, 0);
    Get_Deskew_Settings(node, ddr_interface_num, &dskdat);
    Display_Deskew_Data(node, ddr_interface_num, &dskdat, VBL_NORM);
#endif
}

#define SCALING_FACTOR (1000)
#define Dprintf debug_print // make this "ddr_print" for extra debug output below
static int compute_Vref_1slot_2rank(int rtt_wr, int rtt_park, int dqx_ctl, int rank_count)
{
    uint64_t Reff_s;
    uint64_t Rser_s = 15;
    uint64_t Vdd = 1200;
    uint64_t Vref;
    //uint64_t Vl;
    uint64_t rtt_wr_s = (((rtt_wr == 0) || (rtt_wr == 99)) ? 1*1024*1024 : rtt_wr); // 99 == HiZ
    uint64_t rtt_park_s = (((rtt_park == 0) || ((rank_count == 1) && (rtt_wr != 0))) ? 1*1024*1024 : rtt_park);
    uint64_t dqx_ctl_s = (dqx_ctl == 0 ? 1*1024*1024 : dqx_ctl);
    int Vref_value;
    uint64_t Rangepc = 6000; // range1 base is 60%
    uint64_t Vrefpc;
    int Vref_range = 0;

    Dprintf("rtt_wr = %d, rtt_park = %d, dqx_ctl = %d\n", rtt_wr, rtt_park, dqx_ctl);
    Dprintf("rtt_wr_s = %d, rtt_park_s = %d, dqx_ctl_s = %d\n", rtt_wr_s, rtt_park_s, dqx_ctl_s);

    Reff_s = divide_nint((rtt_wr_s * rtt_park_s) , (rtt_wr_s + rtt_park_s));
    Dprintf("Reff_s = %d\n", Reff_s);

    //Vl = (((Rser_s + dqx_ctl_s) * SCALING_FACTOR) / (Rser_s + dqx_ctl_s + Reff_s)) * Vdd / SCALING_FACTOR;
    //printf("Vl = %d\n", Vl);

    Vref = (((Rser_s + dqx_ctl_s) * SCALING_FACTOR) / (Rser_s + dqx_ctl_s + Reff_s)) + SCALING_FACTOR;
    Dprintf("Vref = %d\n", Vref);

    Vref = (Vref * Vdd) / 2 / SCALING_FACTOR;
    Dprintf("Vref = %d\n", Vref);

    Vrefpc = (Vref * 100 * 100) / Vdd;
    Dprintf("Vrefpc = %d\n", Vrefpc);

    if (Vrefpc < Rangepc) { // < range1 base, use range2
        Vref_range = 1 << 6; // set bit A6 for range2
        Rangepc = 4500; // range2 base is 45%
    }

    Vref_value = divide_nint(Vrefpc - Rangepc, 65);
    if (Vref_value < 0)
        Vref_value = Vref_range; // set to base of range as lowest value
    else
        Vref_value |= Vref_range;
    Dprintf("Vref_value = %d (0x%02x)\n", Vref_value, Vref_value);

    debug_print("rtt_wr:%d, rtt_park:%d, dqx_ctl:%d, Vref_value:%d (0x%x)\n",
           rtt_wr, rtt_park, dqx_ctl, Vref_value, Vref_value);

    return Vref_value;
}
static int compute_Vref_2slot_2rank(int rtt_wr, int rtt_park_00, int rtt_park_01, int dqx_ctl, int rtt_nom)
{
    //uint64_t Rser = 15;
    uint64_t Vdd = 1200;
    //uint64_t Vref;
    uint64_t Vl, Vlp, Vcm;
    uint64_t Rd0, Rd1, Rpullup;
    uint64_t rtt_wr_s = (((rtt_wr == 0) || (rtt_wr == 99)) ? 1*1024*1024 : rtt_wr); // 99 == HiZ
    uint64_t rtt_park_00_s = (rtt_park_00 == 0 ? 1*1024*1024 : rtt_park_00);
    uint64_t rtt_park_01_s = (rtt_park_01 == 0 ? 1*1024*1024 : rtt_park_01);
    uint64_t dqx_ctl_s = (dqx_ctl == 0 ? 1*1024*1024 : dqx_ctl);
    uint64_t rtt_nom_s = (rtt_nom == 0 ? 1*1024*1024 : rtt_nom);
    int Vref_value;
    uint64_t Rangepc = 6000; // range1 base is 60%
    uint64_t Vrefpc;
    int Vref_range = 0;

    // Rd0 = (RTT_NOM /*parallel*/ RTT_WR) + 15 = ((RTT_NOM * RTT_WR) / (RTT_NOM + RTT_WR)) + 15
    Rd0 = divide_nint((rtt_nom_s * rtt_wr_s), (rtt_nom_s + rtt_wr_s)) + 15;
    //printf("Rd0 = %ld\n", Rd0);

    // Rd1 = (RTT_PARK_00 /*parallel*/ RTT_PARK_01) + 15 = ((RTT_PARK_00 * RTT_PARK_01) / (RTT_PARK_00 + RTT_PARK_01)) + 15
    Rd1 = divide_nint((rtt_park_00_s * rtt_park_01_s), (rtt_park_00_s + rtt_park_01_s)) + 15;
    //printf("Rd1 = %ld\n", Rd1);

    // Rpullup = Rd0 /*parallel*/ Rd1 = (Rd0 * Rd1) / (Rd0 + Rd1)
    Rpullup = divide_nint((Rd0 * Rd1), (Rd0 + Rd1));
    //printf("Rpullup = %ld\n", Rpullup);

    // Vl = (DQX_CTL / (DQX_CTL + Rpullup)) * 1.2
    Vl = divide_nint((dqx_ctl_s * Vdd), (dqx_ctl_s + Rpullup));
    //printf("Vl = %ld\n", Vl);

    // Vlp = ((15 / Rd0) * (1.2 - Vl)) + Vl
    Vlp = divide_nint((15 * (Vdd - Vl)), Rd0) + Vl;
    //printf("Vlp = %ld\n", Vlp);

    // Vcm = (Vlp + 1.2) / 2
    Vcm = divide_nint((Vlp + Vdd), 2);
    //printf("Vcm = %ld\n", Vcm);

    // Vrefpc = (Vcm / 1.2) * 100
    Vrefpc = divide_nint((Vcm * 100 * 100), Vdd);
    //printf("Vrefpc = %ld\n", Vrefpc);

    if (Vrefpc < Rangepc) { // < range1 base, use range2
        Vref_range = 1 << 6; // set bit A6 for range2
        Rangepc = 4500; // range2 base is 45%
    }

    Vref_value = divide_nint(Vrefpc - Rangepc, 65);
    if (Vref_value < 0)
        Vref_value = Vref_range; // set to base of range as lowest value
    else
        Vref_value |= Vref_range;
    //printf("Vref_value = %d (0x%02x)\n", Vref_value, Vref_value);

    debug_print("rtt_wr:%d, rtt_park_00:%d, rtt_park_01:%d, dqx_ctl:%d, rtt_nom:%d, Vref_value:%d (0x%x)\n",
           rtt_wr, rtt_park_00, rtt_park_01, dqx_ctl, rtt_nom, Vref_value, Vref_value);

    return Vref_value;
}

// NOTE: only call this for DIMMs with 1 or 2 ranks, not 4.
int
compute_vref_value(bdk_node_t node, int ddr_interface_num,
                   int rankx, int dimm_count, int rank_count,
                   impedence_values_t *imp_values, int is_stacked_die)
{
    int computed_final_vref_value = 0;

    /* Calculate an override of the measured Vref value
       but only for configurations we know how to...*/
    // we have code for 2-rank DIMMs in both 1-slot or 2-slot configs,
    // and can use the 2-rank 1-slot code for 1-rank DIMMs in 1-slot configs
    // and can use the 2-rank 2-slot code for 1-rank DIMMs in 2-slot configs

    int rtt_wr, dqx_ctl, rtt_nom, index;
    bdk_lmcx_modereg_params1_t lmc_modereg_params1;
    bdk_lmcx_modereg_params2_t lmc_modereg_params2;
    bdk_lmcx_comp_ctl2_t comp_ctl2;

    lmc_modereg_params1.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS1(ddr_interface_num));
    lmc_modereg_params2.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS2(ddr_interface_num));
    comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));
    dqx_ctl = imp_values->dqx_strength[comp_ctl2.s.dqx_ctl];

    // WR always comes from the current rank
    index   = (lmc_modereg_params1.u >> (rankx * 12 + 5)) & 0x03;
    if (!CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) {
        index |= lmc_modereg_params1.u >> (51+rankx-2) & 0x04;
    }
    rtt_wr  = imp_values->rtt_wr_ohms [index];

    // separate calculations for 1 vs 2 DIMMs per LMC
    if (dimm_count == 1) {
        // PARK comes from this rank if 1-rank, otherwise other rank
        index = (lmc_modereg_params2.u >> ((rankx ^ (rank_count - 1)) * 10 + 0)) & 0x07;
        int rtt_park   = imp_values->rtt_nom_ohms[index];
        computed_final_vref_value = compute_Vref_1slot_2rank(rtt_wr, rtt_park, dqx_ctl, rank_count);
    } else {
        // get both PARK values from the other DIMM
        index = (lmc_modereg_params2.u >> ((rankx ^ 0x02) * 10 + 0)) & 0x07;
        int rtt_park_00 = imp_values->rtt_nom_ohms[index];
        index = (lmc_modereg_params2.u >> ((rankx ^ 0x03) * 10 + 0)) & 0x07;
        int rtt_park_01 = imp_values->rtt_nom_ohms[index];
        // NOM comes from this rank if 1-rank, otherwise other rank
        index   = (lmc_modereg_params1.u >> ((rankx ^ (rank_count - 1)) * 12 + 9)) & 0x07;
        rtt_nom = imp_values->rtt_nom_ohms[index];
        computed_final_vref_value = compute_Vref_2slot_2rank(rtt_wr, rtt_park_00, rtt_park_01, dqx_ctl, rtt_nom);
    }

#if ENABLE_COMPUTED_VREF_ADJUSTMENT
    {
        int saved_final_vref_value = computed_final_vref_value;
        BDK_CSR_INIT(lmc_config, node, BDK_LMCX_CONFIG(ddr_interface_num));
        /*
          New computed Vref = existing computed Vref – X

          The value of X is depending on different conditions. Both #122 and #139 are 2Rx4 RDIMM,
          while #124 is stacked die 2Rx4, so I conclude the results into two conditions:

          1. Stacked Die: 2Rx4
             1-slot: offset = 7. i, e New computed Vref = existing computed Vref – 7
             2-slot: offset = 6

          2. Regular: 2Rx4
             1-slot: offset = 3
             2-slot:  offset = 2
        */
        // we know we never get called unless DDR4, so test just the other conditions
        if((!!__bdk_dram_is_rdimm(node, 0)) &&
           (rank_count == 2) &&
           (lmc_config.s.mode_x4dev))
        { // it must first be RDIMM and 2-rank and x4
            if (is_stacked_die) { // now do according to stacked die or not...
                computed_final_vref_value -= (dimm_count == 1) ? 7 : 6;
            } else {
                computed_final_vref_value -= (dimm_count == 1) ? 3 : 2;
            }
            // we have adjusted it, so print it out if verbosity is right
            VB_PRT(VBL_TME, "N%d.LMC%d.R%d: adjusting computed vref from %2d (0x%02x) to %2d (0x%02x)\n",
                   node, ddr_interface_num, rankx,
                   saved_final_vref_value, saved_final_vref_value,
                   computed_final_vref_value, computed_final_vref_value);
        }
    }
#endif
    return computed_final_vref_value;
}

static unsigned int EXTR_WR(uint64_t u, int x)
{
    return (unsigned int)(((u >> (x*12+5)) & 0x3UL) | ((u >> (51+x-2)) & 0x4UL));
}
static void INSRT_WR(uint64_t *up, int x, int v)
{
    uint64_t u = *up;
    u &= ~(((0x3UL) << (x*12+5)) | ((0x1UL) << (51+x)));
    *up = (u | ((v & 0x3UL) << (x*12+5)) | ((v & 0x4UL) << (51+x-2)));
    return;
}

static int encode_row_lsb_ddr3(int row_lsb, int ddr_interface_wide)
{
    int encoded_row_lsb;
    int row_lsb_start = 14;

    /*  Decoding for row_lsb             */
    /*       000: row_lsb = mem_adr[14]  */
    /*       001: row_lsb = mem_adr[15]  */
    /*       010: row_lsb = mem_adr[16]  */
    /*       011: row_lsb = mem_adr[17]  */
    /*       100: row_lsb = mem_adr[18]  */
    /*       101: row_lsb = mem_adr[19]  */
    /*       110: row_lsb = mem_adr[20]  */
    /*       111: RESERVED               */

    row_lsb_start = 14;

    encoded_row_lsb      = row_lsb - row_lsb_start ;

    return encoded_row_lsb;
}

static int encode_pbank_lsb_ddr3(int pbank_lsb, int ddr_interface_wide)
{
    int encoded_pbank_lsb;

    /*  Decoding for pbank_lsb                                             */
    /*       0000:DIMM = mem_adr[28]    / rank = mem_adr[27] (if RANK_ENA) */
    /*       0001:DIMM = mem_adr[29]    / rank = mem_adr[28]      "        */
    /*       0010:DIMM = mem_adr[30]    / rank = mem_adr[29]      "        */
    /*       0011:DIMM = mem_adr[31]    / rank = mem_adr[30]      "        */
    /*       0100:DIMM = mem_adr[32]    / rank = mem_adr[31]      "        */
    /*       0101:DIMM = mem_adr[33]    / rank = mem_adr[32]      "        */
    /*       0110:DIMM = mem_adr[34]    / rank = mem_adr[33]      "        */
    /*       0111:DIMM = 0              / rank = mem_adr[34]      "        */
    /*       1000-1111: RESERVED                                           */

    int pbank_lsb_start = 0;

    pbank_lsb_start = 28;

    encoded_pbank_lsb      = pbank_lsb - pbank_lsb_start;

    return encoded_pbank_lsb;
}

static uint64_t octeon_read_lmcx_ddr3_rlevel_dbg(bdk_node_t node, int ddr_interface_num, int idx)
{
    DRAM_CSR_MODIFY(c, node, BDK_LMCX_RLEVEL_CTL(ddr_interface_num),
                    c.s.byte = idx);
    BDK_CSR_READ(node, BDK_LMCX_RLEVEL_CTL(ddr_interface_num));
    BDK_CSR_INIT(rlevel_dbg, node, BDK_LMCX_RLEVEL_DBG(ddr_interface_num));
    return rlevel_dbg.s.bitmask;
}

static uint64_t octeon_read_lmcx_ddr3_wlevel_dbg(bdk_node_t node, int ddr_interface_num, int idx)
{
    bdk_lmcx_wlevel_dbg_t wlevel_dbg;

    wlevel_dbg.u = 0;
    wlevel_dbg.s.byte = idx;

    DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_DBG(ddr_interface_num), wlevel_dbg.u);
    BDK_CSR_READ(node, BDK_LMCX_WLEVEL_DBG(ddr_interface_num));

    wlevel_dbg.u = BDK_CSR_READ(node, BDK_LMCX_WLEVEL_DBG(ddr_interface_num));
    return wlevel_dbg.s.bitmask;
}


/*
 * Apply a filter to the BITMASK results returned from Octeon
 * read-leveling to determine the most likely delay result.  This
 * computed delay may be used to qualify the delay result returned by
 * Octeon. Accumulate an error penalty for invalid characteristics of
 * the bitmask so that they can be used to select the most reliable
 * results.
 *
 * The algorithm searches for the largest contiguous MASK within a
 * maximum RANGE of bits beginning with the MSB.
 *
 * 1. a MASK with a WIDTH less than 4 will be penalized
 * 2. Bubbles in the bitmask that occur before or after the MASK
 *    will be penalized
 * 3. If there are no trailing bubbles then extra bits that occur
 *    beyond the maximum RANGE will be penalized.
 *
 *   +++++++++++++++++++++++++++++++++++++++++++++++++++
 *   +                                                 +
 *   +   e.g. bitmask = 27B00                          +
 *   +                                                 +
 *   +   63                  +--- mstart           0   +
 *   +   |                   |                     |   +
 *   +   |         +---------+     +--- fb         |   +
 *   +   |         |  range  |     |               |   +
 *   +   V         V         V     V               V   +
 *   +                                                 +
 *   +   0 0 ... 1 0 0 1 1 1 1 0 1 1 0 0 0 0 0 0 0 0   +
 *   +                                                 +
 *   +           ^     ^     ^                         +
 *   +           |     | mask|                         +
 *   +     lb ---+     +-----+                         +
 *   +                  width                          +
 *   +                                                 +
 *   +++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#define RLEVEL_BITMASK_TRAILING_BITS_ERROR      5
#define RLEVEL_BITMASK_BUBBLE_BITS_ERROR        11 // FIXME? now less than TOOLONG
#define RLEVEL_BITMASK_NARROW_ERROR             6
#define RLEVEL_BITMASK_BLANK_ERROR              100
#define RLEVEL_BITMASK_TOOLONG_ERROR            12

#define MASKRANGE_BITS  6
#define MASKRANGE       ((1 << MASKRANGE_BITS) - 1)

static int
validate_ddr3_rlevel_bitmask(rlevel_bitmask_t *rlevel_bitmask_p, int ddr_type)
{
    int i;
    int errors  = 0;
    uint64_t mask = 0;      /* Used in 64-bit comparisons */
    int8_t  mstart = 0;
    uint8_t width = 0;
    uint8_t firstbit = 0;
    uint8_t lastbit = 0;
    uint8_t bubble = 0;
    uint8_t tbubble = 0;
    uint8_t blank = 0;
    uint8_t narrow = 0;
    uint8_t trailing = 0;
    uint64_t bitmask = rlevel_bitmask_p->bm;
    uint8_t extras = 0;
    uint8_t toolong = 0;
    uint64_t temp;

    if (bitmask == 0) {
        blank += RLEVEL_BITMASK_BLANK_ERROR;
    } else {

        /* Look for fb, the first bit */
        temp = bitmask;
        while (!(temp & 1)) {
            firstbit++;
            temp >>= 1;
        }

        /* Look for lb, the last bit */
        lastbit = firstbit;
        while ((temp >>= 1))
            lastbit++;

        /* Start with the max range to try to find the largest mask within the bitmask data */
        width = MASKRANGE_BITS;
        for (mask = MASKRANGE; mask > 0; mask >>= 1, --width) {
            for (mstart = lastbit - width + 1; mstart >= firstbit; --mstart) {
                temp = mask << mstart;
                if ((bitmask & temp) == temp)
                    goto done_now;
            }
        }
    done_now:
        /* look for any more contiguous 1's to the right of mstart */
        if (width == MASKRANGE_BITS) { // only when maximum mask
            while ((bitmask >> (mstart - 1)) & 1) { // slide right over more 1's
                --mstart;
                if (ddr_type == DDR4_DRAM) // only for DDR4
                    extras++; // count the number of extra bits
            }
        }

        /* Penalize any extra 1's beyond the maximum desired mask */
        if (extras > 0)
            toolong = RLEVEL_BITMASK_TOOLONG_ERROR * ((1 << extras) - 1);

        /* Detect if bitmask is too narrow. */
        if (width < 4)
            narrow = (4 - width) * RLEVEL_BITMASK_NARROW_ERROR;

        /* detect leading bubble bits, that is, any 0's between first and mstart */
        temp = bitmask >> (firstbit + 1);
        i = mstart - firstbit - 1;
        while (--i >= 0) {
            if ((temp & 1) == 0)
                bubble += RLEVEL_BITMASK_BUBBLE_BITS_ERROR;
            temp >>= 1;
        }

        temp = bitmask >> (mstart + width + extras);
        i = lastbit - (mstart + width + extras - 1);
        while (--i >= 0) {
            if (temp & 1) { /* Detect 1 bits after the trailing end of the mask, including last. */
                trailing += RLEVEL_BITMASK_TRAILING_BITS_ERROR;
            } else { /* Detect trailing bubble bits, that is, any 0's between end-of-mask and last */
                tbubble  += RLEVEL_BITMASK_BUBBLE_BITS_ERROR;
            }
            temp >>= 1;
        }
    }

    errors = bubble + tbubble + blank + narrow + trailing + toolong;

    /* Pass out useful statistics */
    rlevel_bitmask_p->mstart = mstart;
    rlevel_bitmask_p->width  = width;

    VB_PRT(VBL_DEV2, "bm:%08lx mask:%02llx, width:%2u, mstart:%2d, fb:%2u, lb:%2u"
           " (bu:%2d, tb:%2d, bl:%2d, n:%2d, t:%2d, x:%2d) errors:%3d %s\n",
           (unsigned long) bitmask, mask, width, mstart,
           firstbit, lastbit, bubble, tbubble, blank, narrow,
           trailing, toolong, errors, (errors) ? "=> invalid" : "");

    return errors;
}

static int compute_ddr3_rlevel_delay(uint8_t mstart, uint8_t width, bdk_lmcx_rlevel_ctl_t rlevel_ctl)
{
    int delay;

    debug_bitmask_print("  offset_en:%d", rlevel_ctl.cn8.offset_en);

    if (rlevel_ctl.s.offset_en) {
        delay = max(mstart, mstart + width - 1 - rlevel_ctl.s.offset);
    } else {
        /* if (rlevel_ctl.s.offset) { */ /* Experimental */
        if (0) {
            delay = max(mstart + rlevel_ctl.s.offset, mstart + 1);
            /* Insure that the offset delay falls within the bitmask */
            delay = min(delay, mstart + width-1);
        } else {
            delay = (width - 1) / 2 + mstart; /* Round down */
            /* delay = (width/2) + mstart; */  /* Round up */
        }
    }

    return delay;
}

#define WLEVEL_BYTE_BITS 5
#define WLEVEL_BYTE_MSK  ((1UL << 5) - 1)

static void update_wlevel_rank_struct(bdk_lmcx_wlevel_rankx_t *lmc_wlevel_rank,
                                      int byte, int delay)
{
    bdk_lmcx_wlevel_rankx_t temp_wlevel_rank;
    if (byte >= 0 && byte <= 8) {
        temp_wlevel_rank.u = lmc_wlevel_rank->u;
        temp_wlevel_rank.u &= ~(WLEVEL_BYTE_MSK << (WLEVEL_BYTE_BITS * byte));
        temp_wlevel_rank.u |= ((delay & WLEVEL_BYTE_MSK) << (WLEVEL_BYTE_BITS * byte));
        lmc_wlevel_rank->u = temp_wlevel_rank.u;
    }
}

static int  get_wlevel_rank_struct(bdk_lmcx_wlevel_rankx_t *lmc_wlevel_rank,
                                   int byte)
{
    int delay = 0;
    if (byte >= 0 && byte <= 8) {
        delay = ((lmc_wlevel_rank->u) >> (WLEVEL_BYTE_BITS * byte)) & WLEVEL_BYTE_MSK;
    }
    return delay;
}

#if 0
// entry = 1 is valid, entry = 0 is invalid
static int
validity_matrix[4][4] = {[0] {1,1,1,0},  // valid pairs when cv == 0: 0,0 + 0,1 + 0,2 == "7"
                         [1] {0,1,1,1},  // valid pairs when cv == 1: 1,1 + 1,2 + 1,3 == "E"
                         [2] {1,0,1,1},  // valid pairs when cv == 2: 2,2 + 2,3 + 2,0 == "D"
                         [3] {1,1,0,1}}; // valid pairs when cv == 3: 3,3 + 3,0 + 3,1 == "B"
#endif
static int
validate_seq(int *wl, int *seq)
{
    int seqx; // sequence index, step through the sequence array
    int bitnum;
    seqx = 0;
    while (seq[seqx+1] >= 0) { // stop on next seq entry == -1
        // but now, check current versus next
#if 0
        if ( !validity_matrix [wl[seq[seqx]]] [wl[seq[seqx+1]]] )
            return 1;
#else
        bitnum = (wl[seq[seqx]] << 2) | wl[seq[seqx+1]];
        if (!((1 << bitnum) & 0xBDE7)) // magic validity number (see matrix above)
            return 1;
#endif
        seqx++;
    }
    return 0;
}

static int
Validate_HW_WL_Settings(bdk_node_t node, int ddr_interface_num,
                        bdk_lmcx_wlevel_rankx_t *lmc_wlevel_rank,
                        int ecc_ena)
{
    int wl[9], byte, errors;

    // arrange the sequences so
    int useq[] = { 0,1,2,3,8,4,5,6,7,-1 }; // index 0 has byte 0, etc, ECC in middle
    int rseq1[] = { 8,3,2,1,0,-1 }; // index 0 is ECC, then go down
    int rseq2[] = { 4,5,6,7,-1 }; // index 0 has byte 4, then go up
    int useqno[] = { 0,1,2,3,4,5,6,7,-1 }; // index 0 has byte 0, etc, no ECC
    int rseq1no[] = { 3,2,1,0,-1 }; // index 0 is byte 3, then go down, no ECC

    // in the CSR, bytes 0-7 are always data, byte 8 is ECC
    for (byte = 0; byte < 8+ecc_ena; byte++) {
        wl[byte] = (get_wlevel_rank_struct(lmc_wlevel_rank, byte) >> 1) & 3; // preprocess :-)
    }

    errors = 0;
    if (__bdk_dram_is_rdimm(node, 0) != 0) { // RDIMM order
        errors  = validate_seq(wl, (ecc_ena) ? rseq1 : rseq1no);
        errors += validate_seq(wl, rseq2);
    } else { // UDIMM order
        errors  = validate_seq(wl, (ecc_ena) ? useq : useqno);
    }

    return errors;
}

#define RLEVEL_BYTE_BITS 6
#define RLEVEL_BYTE_MSK  ((1UL << 6) - 1)

static void update_rlevel_rank_struct(bdk_lmcx_rlevel_rankx_t *lmc_rlevel_rank,
                                      int byte, int delay)
{
    bdk_lmcx_rlevel_rankx_t temp_rlevel_rank;
    if (byte >= 0 && byte <= 8) {
        temp_rlevel_rank.u = lmc_rlevel_rank->u & ~(RLEVEL_BYTE_MSK << (RLEVEL_BYTE_BITS * byte));
        temp_rlevel_rank.u |= ((delay & RLEVEL_BYTE_MSK) << (RLEVEL_BYTE_BITS * byte));
        lmc_rlevel_rank->u = temp_rlevel_rank.u;
    }
}

#if RLEXTRAS_PATCH || !DISABLE_SW_WL_PASS_2
static int  get_rlevel_rank_struct(bdk_lmcx_rlevel_rankx_t *lmc_rlevel_rank,
                                   int byte)
{
    int delay = 0;
    if (byte >= 0 && byte <= 8) {
        delay = ((lmc_rlevel_rank->u) >> (RLEVEL_BYTE_BITS * byte)) & RLEVEL_BYTE_MSK;
    }
    return delay;
}
#endif

static void unpack_rlevel_settings(int ddr_interface_bytemask, int ecc_ena,
                                   rlevel_byte_data_t *rlevel_byte,
                                   bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank)
{
    if ((ddr_interface_bytemask & 0xff) == 0xff) {
        if (ecc_ena) {
            rlevel_byte[8].delay = lmc_rlevel_rank.cn83xx.byte7;
            rlevel_byte[7].delay = lmc_rlevel_rank.cn83xx.byte6;
            rlevel_byte[6].delay = lmc_rlevel_rank.cn83xx.byte5;
            rlevel_byte[5].delay = lmc_rlevel_rank.cn83xx.byte4;
            rlevel_byte[4].delay = lmc_rlevel_rank.cn83xx.byte8; /* ECC */
        } else {
            rlevel_byte[7].delay = lmc_rlevel_rank.cn83xx.byte7;
            rlevel_byte[6].delay = lmc_rlevel_rank.cn83xx.byte6;
            rlevel_byte[5].delay = lmc_rlevel_rank.cn83xx.byte5;
            rlevel_byte[4].delay = lmc_rlevel_rank.cn83xx.byte4;
        }
    } else {
        rlevel_byte[8].delay = lmc_rlevel_rank.cn83xx.byte8; /* unused */
        rlevel_byte[7].delay = lmc_rlevel_rank.cn83xx.byte7; /* unused */
        rlevel_byte[6].delay = lmc_rlevel_rank.cn83xx.byte6; /* unused */
        rlevel_byte[5].delay = lmc_rlevel_rank.cn83xx.byte5; /* unused */
        rlevel_byte[4].delay = lmc_rlevel_rank.cn83xx.byte4; /* ECC */
    }
    rlevel_byte[3].delay = lmc_rlevel_rank.cn83xx.byte3;
    rlevel_byte[2].delay = lmc_rlevel_rank.cn83xx.byte2;
    rlevel_byte[1].delay = lmc_rlevel_rank.cn83xx.byte1;
    rlevel_byte[0].delay = lmc_rlevel_rank.cn83xx.byte0;
}

static void pack_rlevel_settings(int ddr_interface_bytemask, int ecc_ena,
                                 rlevel_byte_data_t *rlevel_byte,
                                 bdk_lmcx_rlevel_rankx_t *final_rlevel_rank)
{
    bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank = *final_rlevel_rank;

    if ((ddr_interface_bytemask & 0xff) == 0xff) {
        if (ecc_ena) {
            lmc_rlevel_rank.cn83xx.byte7 = rlevel_byte[8].delay;
            lmc_rlevel_rank.cn83xx.byte6 = rlevel_byte[7].delay;
            lmc_rlevel_rank.cn83xx.byte5 = rlevel_byte[6].delay;
            lmc_rlevel_rank.cn83xx.byte4 = rlevel_byte[5].delay;
            lmc_rlevel_rank.cn83xx.byte8 = rlevel_byte[4].delay; /* ECC */
        } else {
            lmc_rlevel_rank.cn83xx.byte7 = rlevel_byte[7].delay;
            lmc_rlevel_rank.cn83xx.byte6 = rlevel_byte[6].delay;
            lmc_rlevel_rank.cn83xx.byte5 = rlevel_byte[5].delay;
            lmc_rlevel_rank.cn83xx.byte4 = rlevel_byte[4].delay;
        }
    } else {
        lmc_rlevel_rank.cn83xx.byte8 = rlevel_byte[8].delay;
        lmc_rlevel_rank.cn83xx.byte7 = rlevel_byte[7].delay;
        lmc_rlevel_rank.cn83xx.byte6 = rlevel_byte[6].delay;
        lmc_rlevel_rank.cn83xx.byte5 = rlevel_byte[5].delay;
        lmc_rlevel_rank.cn83xx.byte4 = rlevel_byte[4].delay;
    }
    lmc_rlevel_rank.cn83xx.byte3 = rlevel_byte[3].delay;
    lmc_rlevel_rank.cn83xx.byte2 = rlevel_byte[2].delay;
    lmc_rlevel_rank.cn83xx.byte1 = rlevel_byte[1].delay;
    lmc_rlevel_rank.cn83xx.byte0 = rlevel_byte[0].delay;

    *final_rlevel_rank = lmc_rlevel_rank;
}

#if !DISABLE_SW_WL_PASS_2
static void rlevel_to_wlevel(bdk_lmcx_rlevel_rankx_t *lmc_rlevel_rank,
                             bdk_lmcx_wlevel_rankx_t *lmc_wlevel_rank, int byte)
{
    int byte_delay = get_rlevel_rank_struct(lmc_rlevel_rank, byte);

    debug_print("Estimating Wlevel delay byte %d: ", byte);
    debug_print("Rlevel=%d => ", byte_delay);
    byte_delay = divide_roundup(byte_delay,2) & 0x1e;
    debug_print("Wlevel=%d\n", byte_delay);
    update_wlevel_rank_struct(lmc_wlevel_rank, byte, byte_delay);
}
#endif /* !DISABLE_SW_WL_PASS_2 */

/* Delay trend: constant=0, decreasing=-1, increasing=1 */
static int calc_delay_trend(int v)
{
    if (v == 0)
        return (0);
    if (v < 0)
        return (-1);
    return 1;
}

/* Evaluate delay sequence across the whole range of byte delays while
** keeping track of the overall delay trend, increasing or decreasing.
** If the trend changes charge an error amount to the score.
*/

// NOTE: "max_adj_delay_inc" argument is, by default, 1 for DDR3 and 2 for DDR4

static int nonsequential_delays(rlevel_byte_data_t *rlevel_byte,
                                int start, int end, int max_adj_delay_inc)
{
    int error = 0;
    int delay_trend, prev_trend = 0;
    int byte_idx;
    int delay_inc;
    int delay_diff;
    int byte_err;

    for (byte_idx = start; byte_idx < end; ++byte_idx) {
        byte_err = 0;

        delay_diff = rlevel_byte[byte_idx+1].delay - rlevel_byte[byte_idx].delay;
        delay_trend = calc_delay_trend(delay_diff);

        debug_bitmask_print("Byte %d: %2d, Byte %d: %2d, delay_trend: %2d, prev_trend: %2d",
                            byte_idx+0, rlevel_byte[byte_idx+0].delay,
                            byte_idx+1, rlevel_byte[byte_idx+1].delay,
                            delay_trend, prev_trend);

        /* Increment error each time the trend changes to the opposite direction.
         */
        if ((prev_trend != 0) && (delay_trend != 0) && (prev_trend != delay_trend)) {
            byte_err += RLEVEL_NONSEQUENTIAL_DELAY_ERROR;
            prev_trend = delay_trend;
            debug_bitmask_print(" => Nonsequential byte delay");
        }

        delay_inc = _abs(delay_diff); // how big was the delay change, if any

        /* Even if the trend did not change to the opposite direction, check for
           the magnitude of the change, and scale the penalty by the amount that
           the size is larger than the provided limit.
         */
        if ((max_adj_delay_inc != 0) && (delay_inc > max_adj_delay_inc)) {
            byte_err += (delay_inc - max_adj_delay_inc) * RLEVEL_ADJACENT_DELAY_ERROR;
            debug_bitmask_print(" => Adjacent delay error");
        }

        debug_bitmask_print("\n");
        if (delay_trend != 0)
            prev_trend = delay_trend;

        rlevel_byte[byte_idx+1].sqerrs = byte_err;
        error += byte_err;
    }
    return error;
}

static int roundup_ddr3_wlevel_bitmask(int bitmask)
{
    int shifted_bitmask;
    int leader;
    int delay;

    for (leader=0; leader<8; ++leader) {
        shifted_bitmask = (bitmask>>leader);
        if ((shifted_bitmask&1) == 0)
            break;
    }

    for (/*leader=leader*/; leader<16; ++leader) {
        shifted_bitmask = (bitmask>>(leader%8));
        if (shifted_bitmask&1)
            break;
    }

    delay = (leader & 1) ? leader + 1 : leader;
    delay = delay % 8;

    return delay;
}

/* Check to see if any custom offset values are provided */
static int is_dll_offset_provided(const int8_t *dll_offset_table)
{
    int i;
    if (dll_offset_table != NULL) {
        for (i=0; i<9; ++i) {
            if (dll_offset_table[i] != 0)
                return (1);
        }
    }
    return (0);
}

/////////////////// These are the RLEVEL settings display routines

// flags
#define WITH_NOTHING 0
#define WITH_SCORE   1
#define WITH_AVERAGE 2
#define WITH_FINAL   4
#define WITH_COMPUTE 8
static void do_display_RL(bdk_node_t node, int ddr_interface_num,
                          bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank,
                          int rank, int flags, int score)
{
    char score_buf[16];
    if (flags & WITH_SCORE)
        snprintf(score_buf, sizeof(score_buf), "(%d)", score);
    else {
        score_buf[0] = ' '; score_buf[1] = 0;
    }

    const char *msg_buf;
    char hex_buf[20];
    if (flags & WITH_AVERAGE) {
        msg_buf = "  DELAY AVERAGES  ";
    } else if (flags & WITH_FINAL) {
        msg_buf = "  FINAL SETTINGS  ";
    } else if (flags & WITH_COMPUTE) {
        msg_buf = "  COMPUTED DELAYS ";
    } else {
        snprintf(hex_buf, sizeof(hex_buf), "0x%016lX", lmc_rlevel_rank.u);
        msg_buf = hex_buf;
    }

    ddr_print("N%d.LMC%d.R%d: Rlevel Rank %#4x, %s  : %5d %5d %5d %5d %5d %5d %5d %5d %5d %s\n",
              node, ddr_interface_num, rank,
              lmc_rlevel_rank.s.status,
              msg_buf,
              lmc_rlevel_rank.cn83xx.byte8,
              lmc_rlevel_rank.cn83xx.byte7,
              lmc_rlevel_rank.cn83xx.byte6,
              lmc_rlevel_rank.cn83xx.byte5,
              lmc_rlevel_rank.cn83xx.byte4,
              lmc_rlevel_rank.cn83xx.byte3,
              lmc_rlevel_rank.cn83xx.byte2,
              lmc_rlevel_rank.cn83xx.byte1,
              lmc_rlevel_rank.cn83xx.byte0,
              score_buf
              );
}

static inline void
display_RL(bdk_node_t node, int ddr_interface_num, bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank, int rank)
{
    do_display_RL(node, ddr_interface_num, lmc_rlevel_rank, rank, 0, 0);
}

static inline void
display_RL_with_score(bdk_node_t node, int ddr_interface_num, bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank, int rank, int score)
{
    do_display_RL(node, ddr_interface_num, lmc_rlevel_rank, rank, 1, score);
}

#if !PICK_BEST_RANK_SCORE_NOT_AVG
static inline void
display_RL_with_average(bdk_node_t node, int ddr_interface_num, bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank, int rank, int score)
{
    do_display_RL(node, ddr_interface_num, lmc_rlevel_rank, rank, 3, score);
}
#endif

static inline void
display_RL_with_final(bdk_node_t node, int ddr_interface_num, bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank, int rank)
{
    do_display_RL(node, ddr_interface_num, lmc_rlevel_rank, rank, 4, 0);
}

static inline void
display_RL_with_computed(bdk_node_t node, int ddr_interface_num, bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank, int rank, int score)
{
    do_display_RL(node, ddr_interface_num, lmc_rlevel_rank, rank, 9, score);
}

// flag values
#define WITH_RODT_BLANK      0
#define WITH_RODT_SKIPPING   1
#define WITH_RODT_BESTROW    2
#define WITH_RODT_BESTSCORE  3
// control
#define SKIP_SKIPPING 1

static const char *with_rodt_canned_msgs[4] = { "          ", "SKIPPING  ", "BEST ROW  ", "BEST SCORE" };

static void display_RL_with_RODT(bdk_node_t node, int ddr_interface_num,
                                 bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank, int rank, int score,
                                 int nom_ohms, int rodt_ohms, int flag)
{
    const char *msg_buf;
    char set_buf[20];
#if SKIP_SKIPPING
    if (flag == WITH_RODT_SKIPPING) return;
#endif
    msg_buf = with_rodt_canned_msgs[flag];
    if (nom_ohms < 0) {
        snprintf(set_buf, sizeof(set_buf), "    RODT %3d    ", rodt_ohms);
    } else {
        snprintf(set_buf, sizeof(set_buf), "NOM %3d RODT %3d", nom_ohms, rodt_ohms);
    }

    VB_PRT(VBL_TME, "N%d.LMC%d.R%d: Rlevel %s   %s  : %5d %5d %5d %5d %5d %5d %5d %5d %5d (%d)\n",
           node, ddr_interface_num, rank,
           set_buf, msg_buf,
           lmc_rlevel_rank.cn83xx.byte8,
           lmc_rlevel_rank.cn83xx.byte7,
           lmc_rlevel_rank.cn83xx.byte6,
           lmc_rlevel_rank.cn83xx.byte5,
           lmc_rlevel_rank.cn83xx.byte4,
           lmc_rlevel_rank.cn83xx.byte3,
           lmc_rlevel_rank.cn83xx.byte2,
           lmc_rlevel_rank.cn83xx.byte1,
           lmc_rlevel_rank.cn83xx.byte0,
           score
           );

    // FIXME: does this help make the output a little easier to focus?
    if (flag == WITH_RODT_BESTSCORE) {
        VB_PRT(VBL_DEV, "-----------\n");
    }
}

static void
do_display_WL(bdk_node_t node, int ddr_interface_num, bdk_lmcx_wlevel_rankx_t lmc_wlevel_rank, int rank, int flags)
{
    const char *msg_buf;
    char hex_buf[20];
    int vbl;
    if (flags & WITH_FINAL) {
        msg_buf = "  FINAL SETTINGS  ";
        vbl = VBL_NORM;
    } else {
        snprintf(hex_buf, sizeof(hex_buf), "0x%016lX", lmc_wlevel_rank.u);
        msg_buf = hex_buf;
        vbl = VBL_FAE;
    }

    VB_PRT(vbl, "N%d.LMC%d.R%d: Wlevel Rank %#4x, %s  : %5d %5d %5d %5d %5d %5d %5d %5d %5d\n",
            node, ddr_interface_num, rank,
            lmc_wlevel_rank.s.status,
            msg_buf,
            lmc_wlevel_rank.s.byte8,
            lmc_wlevel_rank.s.byte7,
            lmc_wlevel_rank.s.byte6,
            lmc_wlevel_rank.s.byte5,
            lmc_wlevel_rank.s.byte4,
            lmc_wlevel_rank.s.byte3,
            lmc_wlevel_rank.s.byte2,
            lmc_wlevel_rank.s.byte1,
            lmc_wlevel_rank.s.byte0
            );
}

static inline void
display_WL(bdk_node_t node, int ddr_interface_num, bdk_lmcx_wlevel_rankx_t lmc_wlevel_rank, int rank)
{
    do_display_WL(node, ddr_interface_num, lmc_wlevel_rank, rank, WITH_NOTHING);
}

static inline void
display_WL_with_final(bdk_node_t node, int ddr_interface_num, bdk_lmcx_wlevel_rankx_t lmc_wlevel_rank, int rank)
{
    do_display_WL(node, ddr_interface_num, lmc_wlevel_rank, rank, WITH_FINAL);
}

// pretty-print bitmask adjuster
static uint64_t
PPBM(uint64_t bm)
{
    if (bm != 0ul) {
        while ((bm & 0x0fful) == 0ul)
            bm >>= 4;
    }
    return bm;
}

// xlate PACKED index to UNPACKED index to use with rlevel_byte
#define XPU(i,e) (((i) < 4)?(i):((i)<8)?(i)+(e):4)
// xlate UNPACKED index to PACKED index to use with rlevel_bitmask
#define XUP(i,e) (((i) < 4)?(i):((i)>4)?(i)-(e):8)

// flag values
#define WITH_WL_BITMASKS      0
#define WITH_RL_BITMASKS      1
#define WITH_RL_MASK_SCORES   2
#define WITH_RL_SEQ_SCORES    3
static void
do_display_BM(bdk_node_t node, int ddr_interface_num, int rank, void *bm, int flags, int ecc_ena)
{
    int ecc = !!ecc_ena;
    if (flags == WITH_WL_BITMASKS) { // wlevel_bitmask array in PACKED index order, so just print them
        int *bitmasks = (int *)bm;

        ddr_print("N%d.LMC%d.R%d: Wlevel Debug Results                  : %05x %05x %05x %05x %05x %05x %05x %05x %05x\n",
              node, ddr_interface_num, rank,
              bitmasks[8],
              bitmasks[7],
              bitmasks[6],
              bitmasks[5],
              bitmasks[4],
              bitmasks[3],
              bitmasks[2],
              bitmasks[1],
              bitmasks[0]
              );
    } else
    if (flags == WITH_RL_BITMASKS) { // rlevel_bitmask array in PACKED index order, so just print them
        rlevel_bitmask_t *rlevel_bitmask = (rlevel_bitmask_t *)bm;
        ddr_print("N%d.LMC%d.R%d: Rlevel Debug Bitmasks        8:0      : %05llx %05llx %05llx %05llx %05llx %05llx %05llx %05llx %05llx\n",
                  node, ddr_interface_num, rank,
                  PPBM(rlevel_bitmask[8].bm),
                  PPBM(rlevel_bitmask[7].bm),
                  PPBM(rlevel_bitmask[6].bm),
                  PPBM(rlevel_bitmask[5].bm),
                  PPBM(rlevel_bitmask[4].bm),
                  PPBM(rlevel_bitmask[3].bm),
                  PPBM(rlevel_bitmask[2].bm),
                  PPBM(rlevel_bitmask[1].bm),
                  PPBM(rlevel_bitmask[0].bm)
                  );
    } else
        if (flags == WITH_RL_MASK_SCORES) { // rlevel_bitmask array in PACKED index order, so just print them
        rlevel_bitmask_t *rlevel_bitmask = (rlevel_bitmask_t *)bm;
        ddr_print("N%d.LMC%d.R%d: Rlevel Debug Bitmask Scores  8:0      : %5d %5d %5d %5d %5d %5d %5d %5d %5d\n",
                  node, ddr_interface_num, rank,
                  rlevel_bitmask[8].errs,
                  rlevel_bitmask[7].errs,
                  rlevel_bitmask[6].errs,
                  rlevel_bitmask[5].errs,
                  rlevel_bitmask[4].errs,
                  rlevel_bitmask[3].errs,
                  rlevel_bitmask[2].errs,
                  rlevel_bitmask[1].errs,
                  rlevel_bitmask[0].errs
                  );
    } else
    if (flags == WITH_RL_SEQ_SCORES) { // rlevel_byte array in UNPACKED index order, so xlate and print them
        rlevel_byte_data_t *rlevel_byte = (rlevel_byte_data_t *)bm;
        ddr_print("N%d.LMC%d.R%d: Rlevel Debug Non-seq Scores  8:0      : %5d %5d %5d %5d %5d %5d %5d %5d %5d\n",
                  node, ddr_interface_num, rank,
                  rlevel_byte[XPU(8,ecc)].sqerrs,
                  rlevel_byte[XPU(7,ecc)].sqerrs,
                  rlevel_byte[XPU(6,ecc)].sqerrs,
                  rlevel_byte[XPU(5,ecc)].sqerrs,
                  rlevel_byte[XPU(4,ecc)].sqerrs,
                  rlevel_byte[XPU(3,ecc)].sqerrs,
                  rlevel_byte[XPU(2,ecc)].sqerrs,
                  rlevel_byte[XPU(1,ecc)].sqerrs,
                  rlevel_byte[XPU(0,ecc)].sqerrs
                  );
    }
}

static inline void
display_WL_BM(bdk_node_t node, int ddr_interface_num, int rank, int *bitmasks)
{
    do_display_BM(node, ddr_interface_num, rank, (void *)bitmasks, WITH_WL_BITMASKS, 0);
}

static inline void
display_RL_BM(bdk_node_t node, int ddr_interface_num, int rank, rlevel_bitmask_t *bitmasks, int ecc_ena)
{
    do_display_BM(node, ddr_interface_num, rank, (void *)bitmasks, WITH_RL_BITMASKS, ecc_ena);
}

static inline void
display_RL_BM_scores(bdk_node_t node, int ddr_interface_num, int rank, rlevel_bitmask_t *bitmasks, int ecc_ena)
{
    do_display_BM(node, ddr_interface_num, rank, (void *)bitmasks, WITH_RL_MASK_SCORES, ecc_ena);
}

static inline void
display_RL_SEQ_scores(bdk_node_t node, int ddr_interface_num, int rank, rlevel_byte_data_t *bytes, int ecc_ena)
{
    do_display_BM(node, ddr_interface_num, rank, (void *)bytes, WITH_RL_SEQ_SCORES, ecc_ena);
}

unsigned short load_dll_offset(bdk_node_t node, int ddr_interface_num,
                               int dll_offset_mode, int byte_offset, int byte)
{
    bdk_lmcx_dll_ctl3_t ddr_dll_ctl3;
    /* byte_sel:
       0x1 = byte 0, ..., 0x9 = byte 8
       0xA = all bytes */
    int byte_sel = (byte == 10) ? byte : byte + 1;

    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
    SET_DDR_DLL_CTL3(load_offset, 0);
    DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num),        ddr_dll_ctl3.u);
    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));

    SET_DDR_DLL_CTL3(mode_sel, dll_offset_mode);
    SET_DDR_DLL_CTL3(offset, (_abs(byte_offset)&0x3f) | (_sign(byte_offset) << 6)); /* Always 6-bit field? */
    SET_DDR_DLL_CTL3(byte_sel, byte_sel);
    DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num),        ddr_dll_ctl3.u);
    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));

    SET_DDR_DLL_CTL3(load_offset, 1);
    DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num),        ddr_dll_ctl3.u);
    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));

    return ((unsigned short) GET_DDR_DLL_CTL3(offset));
}

void change_dll_offset_enable(bdk_node_t node, int ddr_interface_num, int change)
{
    bdk_lmcx_dll_ctl3_t ddr_dll_ctl3;

    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
    SET_DDR_DLL_CTL3(offset_ena, !!change);
    DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num),        ddr_dll_ctl3.u);
    ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
}

static void process_custom_dll_offsets(bdk_node_t node, int ddr_interface_num, const char *enable_str,
                                       const int8_t *offsets, const char *byte_str, int mode)
{
    const char *s;
    int enabled;
    int provided;

    if ((s = lookup_env_parameter("%s", enable_str)) != NULL) {
        enabled = !!strtol(s, NULL, 0);
    } else
        enabled = -1;

    // enabled == -1: no override, do only configured offsets if provided
    // enabled ==  0: override OFF, do NOT do it even if configured offsets provided
    // enabled ==  1: override ON, do it for overrides plus configured offsets

    if (enabled == 0)
        return;

    provided = is_dll_offset_provided(offsets);

    if (enabled < 0 && !provided)
        return;

    int byte_offset;
    unsigned short offset[9] = {0};
    int byte;

    // offsets need to be disabled while loading
    change_dll_offset_enable(node, ddr_interface_num, 0);

    for (byte = 0; byte < 9; ++byte) {

        // always take the provided, if available
        byte_offset = (provided) ? offsets[byte] : 0;

        // then, if enabled, use any overrides present
        if (enabled > 0) {
            if ((s = lookup_env_parameter(byte_str, ddr_interface_num, byte)) != NULL) {
                byte_offset = strtol(s, NULL, 0);
            }
        }

        offset[byte] = load_dll_offset(node, ddr_interface_num, mode, byte_offset, byte);
    }

    // re-enable offsets after loading
    change_dll_offset_enable(node, ddr_interface_num, 1);

    ddr_print("N%d.LMC%d: DLL %s Offset 8:0       :"
              "  0x%02x  0x%02x  0x%02x  0x%02x  0x%02x  0x%02x  0x%02x  0x%02x  0x%02x\n",
              node, ddr_interface_num, (mode == 2) ? "Read " : "Write",
              offset[8], offset[7], offset[6], offset[5], offset[4],
              offset[3], offset[2], offset[1], offset[0]);
}

void perform_octeon3_ddr3_sequence(bdk_node_t node, int rank_mask, int ddr_interface_num, int sequence)
{
    /*
     * 3. Without changing any other fields in LMC(0)_CONFIG, write
     *    LMC(0)_CONFIG[RANKMASK] then write both
     *    LMC(0)_SEQ_CTL[SEQ_SEL,INIT_START] = 1 with a single CSR write
     *    operation. LMC(0)_CONFIG[RANKMASK] bits should be set to indicate
     *    the ranks that will participate in the sequence.
     *
     *    The LMC(0)_SEQ_CTL[SEQ_SEL] value should select power-up/init or
     *    selfrefresh exit, depending on whether the DRAM parts are in
     *    self-refresh and whether their contents should be preserved. While
     *    LMC performs these sequences, it will not perform any other DDR3
     *    transactions. When the sequence is complete, hardware sets the
     *    LMC(0)_CONFIG[INIT_STATUS] bits for the ranks that have been
     *    initialized.
     *
     *    If power-up/init is selected immediately following a DRESET
     *    assertion, LMC executes the sequence described in the "Reset and
     *    Initialization Procedure" section of the JEDEC DDR3
     *    specification. This includes activating CKE, writing all four DDR3
     *    mode registers on all selected ranks, and issuing the required ZQCL
     *    command. The LMC(0)_CONFIG[RANKMASK] value should select all ranks
     *    with attached DRAM in this case. If LMC(0)_CONTROL[RDIMM_ENA] = 1,
     *    LMC writes the JEDEC standard SSTE32882 control words selected by
     *    LMC(0)_DIMM_CTL[DIMM*_WMASK] between DDR_CKE* signal assertion and
     *    the first DDR3 mode register write operation.
     *    LMC(0)_DIMM_CTL[DIMM*_WMASK] should be cleared to 0 if the
     *    corresponding DIMM is not present.
     *
     *    If self-refresh exit is selected, LMC executes the required SRX
     *    command followed by a refresh and ZQ calibration. Section 4.5
     *    describes behavior of a REF + ZQCS.  LMC does not write the DDR3
     *    mode registers as part of this sequence, and the mode register
     *    parameters must match at self-refresh entry and exit times.
     *
     * 4. Read LMC(0)_SEQ_CTL and wait for LMC(0)_SEQ_CTL[SEQ_COMPLETE] to be
     *    set.
     *
     * 5. Read LMC(0)_CONFIG[INIT_STATUS] and confirm that all ranks have
     *    been initialized.
     */

    const char *s;
    static const char *sequence_str[] = {
        "Power-up/init",
        "Read-leveling",
        "Self-refresh entry",
        "Self-refresh exit",
        "Illegal",
        "Illegal",
        "Write-leveling",
        "Init Register Control Words",
        "Mode Register Write",
        "MPR Register Access",
        "LMC Deskew/Internal Vref training",
        "Offset Training"
    };

    bdk_lmcx_seq_ctl_t seq_ctl;
    bdk_lmcx_config_t  lmc_config;

    lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
    lmc_config.s.rankmask     = rank_mask;
    DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(ddr_interface_num), lmc_config.u);

    seq_ctl.u    = 0;

    seq_ctl.s.init_start  = 1;
    seq_ctl.s.seq_sel    = sequence;

    VB_PRT(VBL_SEQ, "N%d.LMC%d: Performing LMC sequence=%x: rank_mask=0x%02x, %s\n",
            node, ddr_interface_num, sequence, rank_mask, sequence < 12 ? sequence_str[sequence] : "");

    if ((s = lookup_env_parameter("ddr_trigger_sequence%d", sequence)) != NULL) {
            /* FIXME(dhendrix): this appears to be meant for the eval board */
#if 0
        int trigger = strtoul(s, NULL, 0);
        if (trigger)
            pulse_gpio_pin(node, 1, 2);
#endif
        error_print("env parameter ddr_trigger_sequence%d not found\n", sequence);
    }

    DRAM_CSR_WRITE(node, BDK_LMCX_SEQ_CTL(ddr_interface_num), seq_ctl.u);
    BDK_CSR_READ(node, BDK_LMCX_SEQ_CTL(ddr_interface_num));

    /* Wait 100us minimum before checking for sequence complete */
    bdk_wait_usec(100);
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_LMCX_SEQ_CTL(ddr_interface_num), seq_complete, ==, 1, 1000000))
    {
        error_print("N%d.LMC%d: Timeout waiting for LMC sequence=%x, rank_mask=0x%02x, ignoring...\n",
                    node, ddr_interface_num, sequence, rank_mask);
    }
    else {
        VB_PRT(VBL_SEQ, "N%d.LMC%d: LMC sequence=%x: Completed.\n", node, ddr_interface_num, sequence);
    }
}

void ddr4_mrw(bdk_node_t node, int ddr_interface_num, int rank,
              int mr_wr_addr, int mr_wr_sel, int mr_wr_bg1)
{
    bdk_lmcx_mr_mpr_ctl_t lmc_mr_mpr_ctl;

    lmc_mr_mpr_ctl.u = 0;
    lmc_mr_mpr_ctl.s.mr_wr_addr                 = (mr_wr_addr == -1) ? 0 : mr_wr_addr;
    lmc_mr_mpr_ctl.s.mr_wr_sel                  = mr_wr_sel;
    lmc_mr_mpr_ctl.s.mr_wr_rank                 = rank;
    //lmc_mr_mpr_ctl.s.mr_wr_pda_mask           =
    //lmc_mr_mpr_ctl.s.mr_wr_pda_enable         =
    //lmc_mr_mpr_ctl.s.mpr_loc                  =
    //lmc_mr_mpr_ctl.s.mpr_wr                   =
    //lmc_mr_mpr_ctl.s.mpr_bit_select           =
    //lmc_mr_mpr_ctl.s.mpr_byte_select          =
    //lmc_mr_mpr_ctl.s.mpr_whole_byte_enable    =
    lmc_mr_mpr_ctl.s.mr_wr_use_default_value    = (mr_wr_addr == -1) ? 1 : 0;
    lmc_mr_mpr_ctl.s.mr_wr_bg1                  = mr_wr_bg1;
    DRAM_CSR_WRITE(node, BDK_LMCX_MR_MPR_CTL(ddr_interface_num), lmc_mr_mpr_ctl.u);

    /* Mode Register Write */
    perform_octeon3_ddr3_sequence(node, 1 << rank, ddr_interface_num, 0x8);
}

#define InvA0_17(x) (x ^ 0x22bf8)
static void set_mpr_mode (bdk_node_t node, int rank_mask,
                          int ddr_interface_num, int dimm_count, int mpr, int bg1)
{
    int rankx;

    ddr_print("All Ranks: Set mpr mode = %x %c-side\n",
              mpr, (bg1==0) ? 'A' : 'B');

    for (rankx = 0; rankx < dimm_count*4; rankx++) {
        if (!(rank_mask & (1 << rankx)))
            continue;
        if (bg1 == 0)
            ddr4_mrw(node, ddr_interface_num, rankx,          mpr<<2,   3, bg1); /* MR3 A-side */
        else
            ddr4_mrw(node, ddr_interface_num, rankx, InvA0_17(mpr<<2), ~3, bg1); /* MR3 B-side */
    }
}

#if ENABLE_DISPLAY_MPR_PAGE
static void do_ddr4_mpr_read(bdk_node_t node, int ddr_interface_num, int rank,
                      int page, int location)
{
    bdk_lmcx_mr_mpr_ctl_t lmc_mr_mpr_ctl;

    lmc_mr_mpr_ctl.u = BDK_CSR_READ(node, BDK_LMCX_MR_MPR_CTL(ddr_interface_num));

    lmc_mr_mpr_ctl.s.mr_wr_addr                 = 0;
    lmc_mr_mpr_ctl.s.mr_wr_sel                  = page; /* Page */
    lmc_mr_mpr_ctl.s.mr_wr_rank                 = rank;
    //lmc_mr_mpr_ctl.s.mr_wr_pda_mask           =
    //lmc_mr_mpr_ctl.s.mr_wr_pda_enable         =
    lmc_mr_mpr_ctl.s.mpr_loc                    = location;
    lmc_mr_mpr_ctl.s.mpr_wr                     = 0; /* Read=0, Write=1 */
    //lmc_mr_mpr_ctl.s.mpr_bit_select           =
    //lmc_mr_mpr_ctl.s.mpr_byte_select          =
    //lmc_mr_mpr_ctl.s.mpr_whole_byte_enable    =
    //lmc_mr_mpr_ctl.s.mr_wr_use_default_value  =
    //lmc_mr_mpr_ctl.s.mr_wr_bg1                =

    DRAM_CSR_WRITE(node, BDK_LMCX_MR_MPR_CTL(ddr_interface_num), lmc_mr_mpr_ctl.u);

    /* MPR register access sequence */
    perform_octeon3_ddr3_sequence(node, 1 << rank, ddr_interface_num, 0x9);

    debug_print("LMC_MR_MPR_CTL             : 0x%016lx\n", lmc_mr_mpr_ctl.u);
    debug_print("lmc_mr_mpr_ctl.s.mr_wr_addr: 0x%02x\n", lmc_mr_mpr_ctl.s.mr_wr_addr);
    debug_print("lmc_mr_mpr_ctl.s.mr_wr_sel : 0x%02x\n", lmc_mr_mpr_ctl.s.mr_wr_sel);
    debug_print("lmc_mr_mpr_ctl.s.mpr_loc   : 0x%02x\n", lmc_mr_mpr_ctl.s.mpr_loc);
    debug_print("lmc_mr_mpr_ctl.s.mpr_wr    : 0x%02x\n", lmc_mr_mpr_ctl.s.mpr_wr);

}
#endif

static int set_rdimm_mode(bdk_node_t node, int ddr_interface_num, int enable)
{
    bdk_lmcx_control_t lmc_control;
    int save_rdimm_mode;

    lmc_control.u = BDK_CSR_READ(node, BDK_LMCX_CONTROL(ddr_interface_num));
    save_rdimm_mode = lmc_control.s.rdimm_ena;
    lmc_control.s.rdimm_ena       = enable;
    VB_PRT(VBL_FAE, "Setting RDIMM_ENA = %x\n", enable);
    DRAM_CSR_WRITE(node, BDK_LMCX_CONTROL(ddr_interface_num), lmc_control.u);

    return (save_rdimm_mode);
}

#if ENABLE_DISPLAY_MPR_PAGE
static void ddr4_mpr_read(bdk_node_t node, int ddr_interface_num, int rank,
                          int page, int location, uint64_t *mpr_data)
{
    do_ddr4_mpr_read(node, ddr_interface_num, rank, page, location);

    mpr_data[0] = BDK_CSR_READ(node, BDK_LMCX_MPR_DATA0(ddr_interface_num));
    mpr_data[1] = BDK_CSR_READ(node, BDK_LMCX_MPR_DATA1(ddr_interface_num));
    mpr_data[2] = BDK_CSR_READ(node, BDK_LMCX_MPR_DATA2(ddr_interface_num));

    debug_print("MPR Read %016lx.%016lx.%016lx\n", mpr_data[2], mpr_data[1], mpr_data[0]);
}

/* Display MPR values for Page Location */
static void Display_MPR_Page_Location(bdk_node_t node, int rank,
                                      int ddr_interface_num, int dimm_count,
                                      int page, int location, uint64_t *mpr_data)
{
    ddr4_mpr_read(node, ddr_interface_num, rank, page, location, mpr_data);
    ddr_print("MPR Page %d, Loc %d %016lx.%016lx.%016lx\n",
              page, location, mpr_data[2], mpr_data[1], mpr_data[0]);
}

/* Display MPR values for Page */
static void Display_MPR_Page(bdk_node_t node, int rank_mask,
                      int ddr_interface_num, int dimm_count, int page)
{
    int rankx;
    uint64_t mpr_data[3];

    for (rankx = 0; rankx < dimm_count * 4;rankx++) {
        if (!(rank_mask & (1 << rankx)))
            continue;

        ddr_print("Rank %d: MPR values for Page %d\n", rankx, page);
        for (int location = 0; location < 4; location++) {
            Display_MPR_Page_Location(node, rankx, ddr_interface_num, dimm_count,
                                      page, location, &mpr_data[0]);
        }

    } /* for (rankx = 0; rankx < dimm_count * 4; rankx++) */
}
#endif

static void ddr4_mpr_write(bdk_node_t node, int ddr_interface_num, int rank,
                    int page, int location, uint8_t mpr_data)
{
    bdk_lmcx_mr_mpr_ctl_t lmc_mr_mpr_ctl;

    lmc_mr_mpr_ctl.u = 0;
    lmc_mr_mpr_ctl.s.mr_wr_addr                 = mpr_data;
    lmc_mr_mpr_ctl.s.mr_wr_sel                  = page; /* Page */
    lmc_mr_mpr_ctl.s.mr_wr_rank                 = rank;
    //lmc_mr_mpr_ctl.s.mr_wr_pda_mask           =
    //lmc_mr_mpr_ctl.s.mr_wr_pda_enable         =
    lmc_mr_mpr_ctl.s.mpr_loc                    = location;
    lmc_mr_mpr_ctl.s.mpr_wr                     = 1; /* Read=0, Write=1 */
    //lmc_mr_mpr_ctl.s.mpr_bit_select           =
    //lmc_mr_mpr_ctl.s.mpr_byte_select          =
    //lmc_mr_mpr_ctl.s.mpr_whole_byte_enable    =
    //lmc_mr_mpr_ctl.s.mr_wr_use_default_value  =
    //lmc_mr_mpr_ctl.s.mr_wr_bg1                =
    DRAM_CSR_WRITE(node, BDK_LMCX_MR_MPR_CTL(ddr_interface_num), lmc_mr_mpr_ctl.u);

    /* MPR register access sequence */
    perform_octeon3_ddr3_sequence(node, (1 << rank), ddr_interface_num, 0x9);

    debug_print("LMC_MR_MPR_CTL             : 0x%016lx\n", lmc_mr_mpr_ctl.u);
    debug_print("lmc_mr_mpr_ctl.s.mr_wr_addr: 0x%02x\n", lmc_mr_mpr_ctl.s.mr_wr_addr);
    debug_print("lmc_mr_mpr_ctl.s.mr_wr_sel : 0x%02x\n", lmc_mr_mpr_ctl.s.mr_wr_sel);
    debug_print("lmc_mr_mpr_ctl.s.mpr_loc   : 0x%02x\n", lmc_mr_mpr_ctl.s.mpr_loc);
    debug_print("lmc_mr_mpr_ctl.s.mpr_wr    : 0x%02x\n", lmc_mr_mpr_ctl.s.mpr_wr);
}

void set_vref(bdk_node_t node, int ddr_interface_num, int rank,
              int range, int value)
{
    bdk_lmcx_mr_mpr_ctl_t lmc_mr_mpr_ctl;
    bdk_lmcx_modereg_params3_t lmc_modereg_params3;
    int mr_wr_addr = 0;

    lmc_mr_mpr_ctl.u = 0;
    lmc_modereg_params3.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS3(ddr_interface_num));

    mr_wr_addr |= lmc_modereg_params3.s.tccd_l<<10; /* A12:A10 tCCD_L */
    mr_wr_addr |= 1<<7;         /* A7 1 = Enable(Training Mode) */
    mr_wr_addr |= range<<6;     /* A6 VrefDQ Training Range */
    mr_wr_addr |= value<<0;     /* A5:A0 VrefDQ Training Value */

    lmc_mr_mpr_ctl.s.mr_wr_addr               = mr_wr_addr;
    lmc_mr_mpr_ctl.s.mr_wr_sel                = 6; /* Write MR6 */
    lmc_mr_mpr_ctl.s.mr_wr_rank               = rank;
    //lmc_mr_mpr_ctl.s.mr_wr_pda_mask           =
    //lmc_mr_mpr_ctl.s.mr_wr_pda_enable         =
    //lmc_mr_mpr_ctl.s.mpr_loc                  = location;
    //lmc_mr_mpr_ctl.s.mpr_wr                   = 0; /* Read=0, Write=1 */
    //lmc_mr_mpr_ctl.s.mpr_bit_select           =
    //lmc_mr_mpr_ctl.s.mpr_byte_select          =
    //lmc_mr_mpr_ctl.s.mpr_whole_byte_enable    =
    //lmc_mr_mpr_ctl.s.mr_wr_use_default_value  =
    //lmc_mr_mpr_ctl.s.mr_wr_bg1                =
    DRAM_CSR_WRITE(node, BDK_LMCX_MR_MPR_CTL(ddr_interface_num), lmc_mr_mpr_ctl.u);

    /* 0x8 = Mode Register Write */
    perform_octeon3_ddr3_sequence(node, 1<<rank, ddr_interface_num, 0x8);

    /* It is vendor specific whether Vref_value is captured with A7=1.
       A subsequent MRS might be necessary. */
    perform_octeon3_ddr3_sequence(node, 1<<rank, ddr_interface_num, 0x8);

    mr_wr_addr &= ~(1<<7);         /* A7 0 = Disable(Training Mode) */
    lmc_mr_mpr_ctl.s.mr_wr_addr               = mr_wr_addr;
    DRAM_CSR_WRITE(node, BDK_LMCX_MR_MPR_CTL(ddr_interface_num), lmc_mr_mpr_ctl.u);
}

static void set_DRAM_output_inversion (bdk_node_t node,
                                       int ddr_interface_num,
                                       int dimm_count,
                                       int rank_mask,
                                       int inversion)
{
    bdk_lmcx_ddr4_dimm_ctl_t lmc_ddr4_dimm_ctl;
    bdk_lmcx_dimmx_params_t lmc_dimmx_params;
    bdk_lmcx_dimm_ctl_t lmc_dimm_ctl;
    int dimm_no;

    lmc_ddr4_dimm_ctl.u = 0;  /* Don't touch extended register control words */
    DRAM_CSR_WRITE(node, BDK_LMCX_DDR4_DIMM_CTL(ddr_interface_num), lmc_ddr4_dimm_ctl.u);

    ddr_print("All DIMMs: Register Control Word          RC0 : %x\n", (inversion & 1));

    for (dimm_no = 0; dimm_no < dimm_count; ++dimm_no) {
        lmc_dimmx_params.u = BDK_CSR_READ(node, BDK_LMCX_DIMMX_PARAMS(ddr_interface_num, dimm_no));
        lmc_dimmx_params.s.rc0  = (lmc_dimmx_params.s.rc0 & ~1) | (inversion & 1);
        DRAM_CSR_WRITE(node, BDK_LMCX_DIMMX_PARAMS(ddr_interface_num, dimm_no), lmc_dimmx_params.u);
    }

    /* LMC0_DIMM_CTL */
    lmc_dimm_ctl.u = BDK_CSR_READ(node, BDK_LMCX_DIMM_CTL(ddr_interface_num));
    lmc_dimm_ctl.s.dimm0_wmask         = 0x1;
    lmc_dimm_ctl.s.dimm1_wmask         = (dimm_count > 1) ? 0x0001 : 0x0000;

    ddr_print("LMC DIMM_CTL                                  : 0x%016llx\n",
              lmc_dimm_ctl.u);
    DRAM_CSR_WRITE(node, BDK_LMCX_DIMM_CTL(ddr_interface_num), lmc_dimm_ctl.u);

    perform_octeon3_ddr3_sequence(node, rank_mask, ddr_interface_num, 0x7 ); /* Init RCW */
}

static void write_mpr_page0_pattern (bdk_node_t node, int rank_mask,
                                     int ddr_interface_num, int dimm_count, int pattern, int location_mask)
{
    int rankx;
    int location;

    for (rankx = 0; rankx < dimm_count*4; rankx++) {
        if (!(rank_mask & (1 << rankx)))
            continue;
        for (location = 0; location < 4; ++location) {
            if (!(location_mask & (1 << location)))
                continue;

            ddr4_mpr_write(node, ddr_interface_num, rankx,
                           /* page */ 0, /* location */ location, pattern);
        }
    }
}

static void change_rdimm_mpr_pattern (bdk_node_t node, int rank_mask,
                                      int ddr_interface_num, int dimm_count)
{
    int save_ref_zqcs_int;
    bdk_lmcx_config_t  lmc_config;

    /*
      Okay, here is the latest sequence.  This should work for all
      chips and passes (78,88,73,etc).  This sequence should be run
      immediately after DRAM INIT.  The basic idea is to write the
      same pattern into each of the 4 MPR locations in the DRAM, so
      that the same value is returned when doing MPR reads regardless
      of the inversion state.  My advice is to put this into a
      function, change_rdimm_mpr_pattern or something like that, so
      that it can be called multiple times, as I think David wants a
      clock-like pattern for OFFSET training, but does not want a
      clock pattern for Bit-Deskew.  You should then be able to call
      this at any point in the init sequence (after DRAM init) to
      change the pattern to a new value.
      Mike

      A correction: PHY doesn't need any pattern during offset
      training, but needs clock like pattern for internal vref and
      bit-dskew training.  So for that reason, these steps below have
      to be conducted before those trainings to pre-condition
      the pattern.  David

      Note: Step 3, 4, 8 and 9 have to be done through RDIMM
      sequence. If you issue MRW sequence to do RCW write (in o78 pass
      1 at least), LMC will still do two commands because
      CONTROL[RDIMM_ENA] is still set high. We don't want it to have
      any unintentional mode register write so it's best to do what
      Mike is doing here.
      Andrew
    */


    /* 1) Disable refresh (REF_ZQCS_INT = 0) */

    debug_print("1) Disable refresh (REF_ZQCS_INT = 0)\n");

    lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
    save_ref_zqcs_int         = lmc_config.s.ref_zqcs_int;
    lmc_config.s.ref_zqcs_int = 0;
    DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(ddr_interface_num), lmc_config.u);


    /* 2) Put all devices in MPR mode (Run MRW sequence (sequence=8)
       with MODEREG_PARAMS0[MPRLOC]=0,
       MODEREG_PARAMS0[MPR]=1, MR_MPR_CTL[MR_WR_SEL]=3, and
       MR_MPR_CTL[MR_WR_USE_DEFAULT_VALUE]=1) */

    debug_print("2) Put all devices in MPR mode (Run MRW sequence (sequence=8)\n");

    set_mpr_mode(node, rank_mask, ddr_interface_num, dimm_count, /* mpr */ 1, /* bg1 */ 0); /* A-side */
    set_mpr_mode(node, rank_mask, ddr_interface_num, dimm_count, /* mpr */ 1, /* bg1 */ 1); /* B-side */

    /*    a. Or you can set MR_MPR_CTL[MR_WR_USE_DEFAULT_VALUE]=0 and set
          the value you would like directly into
          MR_MPR_CTL[MR_WR_ADDR] */

    /* 3) Disable RCD Parity (if previously enabled) - parity does not
       work if inversion disabled */

    debug_print("3) Disable RCD Parity\n");

    /* 4) Disable Inversion in the RCD. */
    /*    a. I did (3&4) via the RDIMM sequence (seq_sel=7), but it
          may be easier to use the MRW sequence (seq_sel=8).  Just set
          MR_MPR_CTL[MR_WR_SEL]=7, MR_MPR_CTL[MR_WR_ADDR][3:0]=data,
          MR_MPR_CTL[MR_WR_ADDR][7:4]=RCD reg */

    debug_print("4) Disable Inversion in the RCD.\n");

    set_DRAM_output_inversion(node, ddr_interface_num, dimm_count, rank_mask,
                               1 /* 1=disable output inversion*/);

    /* 5) Disable CONTROL[RDIMM_ENA] so that MR sequence goes out
       non-inverted.  */

    debug_print("5) Disable CONTROL[RDIMM_ENA]\n");

    set_rdimm_mode(node, ddr_interface_num, 0);

    /* 6) Write all 4 MPR registers with the desired pattern (have to
       do this for all enabled ranks) */
    /*    a. MR_MPR_CTL.MPR_WR=1, MR_MPR_CTL.MPR_LOC=0..3,
          MR_MPR_CTL.MR_WR_SEL=0, MR_MPR_CTL.MR_WR_ADDR[7:0]=pattern */

    debug_print("6) Write all 4 MPR page 0 Training Patterns\n");

    write_mpr_page0_pattern(node, rank_mask,
                             ddr_interface_num, dimm_count, 0x55, 0x8);

    /* 7) Re-enable RDIMM_ENA */

    debug_print("7) Re-enable RDIMM_ENA\n");

    set_rdimm_mode(node, ddr_interface_num, 1);

    /* 8) Re-enable RDIMM inversion */

    debug_print("8) Re-enable RDIMM inversion\n");

    set_DRAM_output_inversion(node, ddr_interface_num, dimm_count, rank_mask,
                               0 /* 0=re-enable output inversion*/);

    /* 9) Re-enable RDIMM parity (if desired) */

    debug_print("9) Re-enable RDIMM parity (if desired)\n");

    /* 10)Take B-side devices out of MPR mode (Run MRW sequence
       (sequence=8) with MODEREG_PARAMS0[MPRLOC]=0,
       MODEREG_PARAMS0[MPR]=0, MR_MPR_CTL[MR_WR_SEL]=3, and
       MR_MPR_CTL[MR_WR_USE_DEFAULT_VALUE]=1) */

    debug_print("10)Take B-side devices out of MPR mode\n");

    set_mpr_mode(node, rank_mask, ddr_interface_num, dimm_count, /* mpr */ 0, /* bg1 */ 1);

    /*    a. Or you can set MR_MPR_CTL[MR_WR_USE_DEFAULT_VALUE]=0 and
          set the value you would like directly into
          MR_MPR_CTL[MR_WR_ADDR] */

    /* 11)Re-enable refresh (REF_ZQCS_INT=previous value) */

    debug_print("11)Re-enable refresh (REF_ZQCS_INT=previous value)\n");

    lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
    lmc_config.s.ref_zqcs_int = save_ref_zqcs_int;
    DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(ddr_interface_num), lmc_config.u);

}

static unsigned char ddr4_rodt_ohms     [RODT_OHMS_COUNT     ] = {  0,  40,  60, 80, 120, 240, 34, 48 };
static unsigned char ddr4_rtt_nom_ohms  [RTT_NOM_OHMS_COUNT  ] = {  0,  60, 120, 40, 240,  48, 80, 34 };
static unsigned char ddr4_rtt_nom_table [RTT_NOM_TABLE_COUNT ] = {  0,   4,   2,  6,   1,   5,  3,  7 };
static unsigned char ddr4_rtt_wr_ohms   [RTT_WR_OHMS_COUNT   ] = {  0, 120, 240, 99,  80 }; // setting HiZ ohms to 99 for computed vref
static unsigned char ddr4_dic_ohms      [DIC_OHMS_COUNT      ] = { 34,  48 };
static short         ddr4_drive_strength[DRIVE_STRENGTH_COUNT] = {  0,   0, 26, 30, 34, 40, 48, 68, 0,0,0,0,0,0,0 };
static short         ddr4_dqx_strength  [DRIVE_STRENGTH_COUNT] = {  0,  24, 27, 30, 34, 40, 48, 60, 0,0,0,0,0,0,0 };

impedence_values_t ddr4_impedence_values = {
    .rodt_ohms             =  ddr4_rodt_ohms     ,
    .rtt_nom_ohms          =  ddr4_rtt_nom_ohms  ,
    .rtt_nom_table         =  ddr4_rtt_nom_table ,
    .rtt_wr_ohms           =  ddr4_rtt_wr_ohms   ,
    .dic_ohms              =  ddr4_dic_ohms      ,
    .drive_strength        =  ddr4_drive_strength,
    .dqx_strength          =  ddr4_dqx_strength  ,
};

static unsigned char ddr3_rodt_ohms     [RODT_OHMS_COUNT     ] = { 0, 20, 30, 40, 60, 120, 0, 0 };
static unsigned char ddr3_rtt_nom_ohms  [RTT_NOM_OHMS_COUNT  ] = { 0, 60, 120, 40, 20, 30, 0, 0 };
static unsigned char ddr3_rtt_nom_table [RTT_NOM_TABLE_COUNT ] = { 0, 2, 1, 3, 5, 4, 0, 0 };
static unsigned char ddr3_rtt_wr_ohms   [RTT_WR_OHMS_COUNT   ] = { 0, 60, 120 };
static unsigned char ddr3_dic_ohms      [DIC_OHMS_COUNT      ] = { 40, 34 };
static short         ddr3_drive_strength[DRIVE_STRENGTH_COUNT] = { 0, 24, 27, 30, 34, 40, 48, 60, 0,0,0,0,0,0,0 };
static impedence_values_t ddr3_impedence_values = {
    .rodt_ohms             =  ddr3_rodt_ohms     ,
    .rtt_nom_ohms          =  ddr3_rtt_nom_ohms  ,
    .rtt_nom_table         =  ddr3_rtt_nom_table ,
    .rtt_wr_ohms           =  ddr3_rtt_wr_ohms   ,
    .dic_ohms              =  ddr3_dic_ohms      ,
    .drive_strength        =  ddr3_drive_strength,
    .dqx_strength          =  ddr3_drive_strength,
};


uint64_t
hertz_to_psecs(uint64_t hertz)
{
    return divide_nint((uint64_t) 1000*1000*1000*1000, hertz); /* Clock in psecs */
}

#define DIVIDEND_SCALE 1000      /* Scale to avoid rounding error. */
uint64_t
psecs_to_mts(uint64_t psecs)
{
    //ddr_print("psecs %ld, divisor %ld\n", psecs, divide_nint((uint64_t)(2 * 1000000 * DIVIDEND_SCALE), psecs));
    return divide_nint(divide_nint((uint64_t)(2 * 1000000 * DIVIDEND_SCALE), psecs), DIVIDEND_SCALE);
}

#define WITHIN(v,b,m) (((v)>=((b)-(m)))&&((v)<=((b)+(m))))

// pretty-print version, only works with what comes from the SPD: tCKmin or tCKAVGmin
unsigned long
pretty_psecs_to_mts(uint64_t psecs)
{
    uint64_t ret = 0; // default to error
    if (WITHIN(psecs, 1250, 1))
        ret = 1600;
    else if (WITHIN(psecs, 1071, 1))
        ret = 1866;
    else if (WITHIN(psecs, 937, 1))
        ret = 2133;
    else if (WITHIN(psecs, 833, 1))
        ret = 2400;
    else if (WITHIN(psecs, 750, 1))
        ret = 2666;
    return ret;
}

uint64_t
mts_to_hertz(uint64_t mts)
{
    return ((mts * 1000 * 1000) / 2);
}

#define DEBUG_RC3X_COMPUTE 0
#define rc3x_print(...) \
    do { if (DEBUG_RC3X_COMPUTE) printf(__VA_ARGS__); } while (0)

static int compute_rc3x (int64_t tclk_psecs)
{
    long speed;
    long tclk_psecs_min, tclk_psecs_max;
    long data_rate_mhz, data_rate_mhz_min, data_rate_mhz_max;
    int rc3x;

#define ENCODING_BASE 1240

    data_rate_mhz = psecs_to_mts(tclk_psecs);

    /* 2400 MT/s is a special case. Using integer arithmetic it rounds
       from 833 psecs to 2401 MT/s. Force it to 2400 to pick the
       proper setting from the table. */
    if (tclk_psecs == 833)
        data_rate_mhz = 2400;

    for (speed = ENCODING_BASE; speed < 3200; speed += 20) {
        int error = 0;

        tclk_psecs_min = hertz_to_psecs(mts_to_hertz(speed + 00)); /* Clock in psecs */
        tclk_psecs_max = hertz_to_psecs(mts_to_hertz(speed + 18)); /* Clock in psecs */

        data_rate_mhz_min = psecs_to_mts(tclk_psecs_min);
        data_rate_mhz_max = psecs_to_mts(tclk_psecs_max);

        /* Force alingment to multiple to avound rounding errors. */
        data_rate_mhz_min = ((data_rate_mhz_min + 18) / 20) * 20;
        data_rate_mhz_max = ((data_rate_mhz_max + 18) / 20) * 20;

        error += (speed + 00 != data_rate_mhz_min);
        error += (speed + 20 != data_rate_mhz_max);

        rc3x = (speed - ENCODING_BASE) / 20;

        rc3x_print("rc3x: %02x speed: %4ld MT/s < f <= %4ld MT/s, psec: %3ld:%3ld %4ld:%4ld %s\n",
                   rc3x,
                   speed, speed + 20,
                   tclk_psecs_min, tclk_psecs_max,
                   data_rate_mhz_min, data_rate_mhz_max,
                   error ? "****" : "");

        if (data_rate_mhz <= (speed + 20)) {
            rc3x_print("rc3x: %4ld MT/s <= %4ld MT/s\n", data_rate_mhz, speed + 20);
            break;
        }
    }
    return rc3x;
}

static const int   rlevel_separate_ab  = 1;

int init_octeon3_ddr3_interface(bdk_node_t node,
                               const ddr_configuration_t *ddr_configuration,
                               uint32_t ddr_hertz,
                               uint32_t cpu_hertz,
                               uint32_t ddr_ref_hertz,
                               int board_type,
                               int board_rev_maj,
                               int board_rev_min,
                               int ddr_interface_num,
                               uint32_t ddr_interface_mask
                               )
{
    const char *s;

    const dimm_odt_config_t *odt_1rank_config = ddr_configuration->odt_1rank_config;
    const dimm_odt_config_t *odt_2rank_config = ddr_configuration->odt_2rank_config;
    const dimm_odt_config_t *odt_4rank_config = ddr_configuration->odt_4rank_config;
    const dimm_config_t *dimm_config_table = ddr_configuration->dimm_config_table;
    const dimm_odt_config_t *odt_config;
    const ddr3_custom_config_t *custom_lmc_config = &ddr_configuration->custom_lmc_config;
    int odt_idx;

    /*
    ** Compute clock rates to the nearest picosecond.
    */
    uint64_t tclk_psecs = hertz_to_psecs(ddr_hertz); /* Clock in psecs */
    uint64_t eclk_psecs = hertz_to_psecs(cpu_hertz); /* Clock in psecs */

    int row_bits, col_bits, num_banks, num_ranks, dram_width;
    int dimm_count = 0;
    int fatal_error = 0;        /* Accumulate and report all the errors before giving up */

    int safe_ddr_flag = 0; /* Flag that indicates safe DDR settings should be used */
    int ddr_interface_64b = 1;  /* THUNDER Default: 64bit interface width */
    int ddr_interface_bytemask;
    uint32_t mem_size_mbytes = 0;
    unsigned int didx;
    int bank_bits = 0;
    int bunk_enable;
    int rank_mask;
    int column_bits_start = 1;
    int row_lsb;
    int pbank_lsb;
    int use_ecc = 1;
    int mtb_psec = 0; /* quiet */
    short ftb_Dividend;
    short ftb_Divisor;
    int tAAmin;
    int tCKmin;
    int CL, min_cas_latency = 0, max_cas_latency = 0, override_cas_latency = 0;
    int ddr_rtt_nom_auto, ddr_rodt_ctl_auto;
    int i;

    int spd_addr;
    int spd_org;
    int spd_banks;
    int spd_rdimm;
    int spd_dimm_type;
    int spd_ecc;
    uint32_t spd_cas_latency;
    int spd_mtb_dividend;
    int spd_mtb_divisor;
    int spd_tck_min;
    int spd_taa_min;
    int spd_twr;
    int spd_trcd;
    int spd_trrd;
    int spd_trp;
    int spd_tras;
    int spd_trc;
    int spd_trfc;
    int spd_twtr;
    int spd_trtp;
    int spd_tfaw;
    int spd_addr_mirror;
    int spd_package = 0;
    int spd_rawcard = 0;
    int spd_rawcard_AorB = 0;
    int is_stacked_die = 0;
    int disable_stacked_die = 0;
    int is_3ds_dimm = 0; // 3DS
    int lranks_per_prank = 1; // 3DS: logical ranks per package rank
    int lranks_bits = 0; // 3DS: logical ranks bits
    int die_capacity = 0; // in Mbits; only used for 3DS

    /* FTB values are two's complement ranging from +127 to -128. */
    typedef signed char SC_t;

    int twr;
    int trcd;
    int trrd;
    int trp;
    int tras;
    int trc;
    int trfc;
    int twtr;
    int trtp = 0;  /* quiet */
    int tfaw;

    int wlevel_bitmask_errors = 0;
    int wlevel_loops;
    int default_rtt_nom[4];
    int dyn_rtt_nom_mask = 0;

    ddr_type_t ddr_type;
    int ddr4_tCKAVGmin = 0; /* quiet */
    int ddr4_tCKAVGmax = 0; /* quiet */
    int ddr4_tRCDmin = 0; /* quiet */
    int ddr4_tRPmin = 0; /* quiet */
    int ddr4_tRASmin = 0; /* quiet */
    int ddr4_tRCmin = 0; /* quiet */
    int ddr4_tRFC1min = 0; /* quiet */
    int ddr4_tRFC2min = 0; /* quiet */
    int ddr4_tRFC4min = 0; /* quiet */
    int ddr4_tFAWmin = 0; /* quiet */
    int ddr4_tRRD_Smin = 0; /* quiet */
    int ddr4_tRRD_Lmin;
    int ddr4_tCCD_Lmin;
    impedence_values_t *imp_values;
    int default_rodt_ctl;
    // default to disabled (ie, LMC restart, not chip reset)
    int ddr_disable_chip_reset = 1;
    int disable_deskew_training = 0;
    const char *dimm_type_name;

    /* Allow the Write bit-deskew feature to be enabled when desired. */
    // NOTE: THUNDER pass 2.x only, 81xx, 83xx
    int enable_write_deskew = ENABLE_WRITE_DESKEW_DEFAULT;

#if SWL_TRY_HWL_ALT
    typedef struct {
        uint16_t hwl_alt_mask; // mask of bytelanes with alternate
        uint16_t  hwl_alt_delay[9]; // bytelane alternate avail if mask=1
    } hwl_alt_by_rank_t;
    hwl_alt_by_rank_t hwl_alts[4];
    memset(hwl_alts, 0, sizeof(hwl_alts));
#endif /* SWL_TRY_HWL_ALT */

    bdk_lmcx_config_t  lmc_config;

    /* Initialize these to shut up the compiler. They are configured
       and used only for DDR4  */
    ddr4_tRRD_Lmin = 6000;
    ddr4_tCCD_Lmin = 6000;

    ddr_print("\nInitializing node %d DDR interface %d, DDR Clock %d, DDR Reference Clock %d\n",
              node, ddr_interface_num, ddr_hertz, ddr_ref_hertz);

    if (dimm_config_table[0].spd_addr == 0 && !dimm_config_table[0].spd_ptr) {
        error_print("ERROR: No dimms specified in the dimm_config_table.\n");
        return (-1);
    }

    // allow some overrides to be done

    // this one controls whether chip RESET is done, or LMC init restarted from step 6.9.6
    if ((s = lookup_env_parameter("ddr_disable_chip_reset")) != NULL) {
        ddr_disable_chip_reset = !!strtoul(s, NULL, 0);
    }
    // this one controls whether Deskew Training is performed
    if ((s = lookup_env_parameter("ddr_disable_deskew_training")) != NULL) {
        disable_deskew_training = !!strtoul(s, NULL, 0);
    }
    // this one is in Validate_Read_Deskew_Training and controls a preliminary delay
    if ((s = lookup_env_parameter("ddr_deskew_validation_delay")) != NULL) {
        deskew_validation_delay = strtoul(s, NULL, 0);
    }
    // this one is in Perform_Read_Deskew_Training and controls lock retries
    if ((s = lookup_env_parameter("ddr_lock_retries")) != NULL) {
        default_lock_retry_limit = strtoul(s, NULL, 0);
    }
    // this one controls whether stacked die status can affect processing
    // disabling it will affect computed vref adjustment, and rodt_row_skip_mask
    if ((s = lookup_env_parameter("ddr_disable_stacked_die")) != NULL) {
        disable_stacked_die = !!strtoul(s, NULL, 0);
    }

    // setup/override for write bit-deskew feature
    if (! CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) { // added 81xx and 83xx
        // FIXME: allow override
        if ((s = lookup_env_parameter("ddr_enable_write_deskew")) != NULL) {
            enable_write_deskew = !!strtoul(s, NULL, 0);
        } // else take default setting
    } else { // not pass 2.x
        enable_write_deskew = 0; // force disabled
    }

#if 0 // FIXME: do we really need this anymore?
    if (dram_is_verbose(VBL_NORM)) {
        printf("DDR SPD Table:");
        for (didx = 0; didx < DDR_CFG_T_MAX_DIMMS; ++didx) {
            if (dimm_config_table[didx].spd_addr == 0) break;
            printf(" --ddr%dspd=0x%02x", ddr_interface_num, dimm_config_table[didx].spd_addr);
        }
        printf("\n");
    }
#endif

    /*
    ** Walk the DRAM Socket Configuration Table to see what is installed.
    */
    for (didx = 0; didx < DDR_CFG_T_MAX_DIMMS; ++didx)
    {
        /* Check for lower DIMM socket populated */
        if (validate_dimm(node, &dimm_config_table[didx]) == 1) {
            // NOTE: DIMM info printing is now done later when more details are available
            ++dimm_count;
        } else { break; }       /* Finished when there is no lower DIMM */
    }


    initialize_ddr_clock(node,
                         ddr_configuration,
                         cpu_hertz,
                         ddr_hertz,
                         ddr_ref_hertz,
                         ddr_interface_num,
                         ddr_interface_mask);

    if (!odt_1rank_config)
        odt_1rank_config = disable_odt_config;
    if (!odt_2rank_config)
        odt_2rank_config = disable_odt_config;
    if (!odt_4rank_config)
        odt_4rank_config = disable_odt_config;

    if ((s = lookup_env_parameter("ddr_safe")) != NULL) {
        safe_ddr_flag = !!strtoul(s, NULL, 0);
    }


    if (dimm_count == 0) {
        error_print("ERROR: DIMM 0 not detected.\n");
        return(-1);
    }

    // look for 32-bit mode specified in the config
    if (custom_lmc_config->mode32b) {
        ddr_interface_64b = 0;
    }

    if (ddr_interface_64b == 0) { // check if 32-bit mode is bad
        if (!CAVIUM_IS_MODEL(CAVIUM_CN81XX)) {
            error_print("32-bit interface width is NOT supported for this Thunder model\n");
            ddr_interface_64b = 1; // force to 64-bit
        }
    } else { // check if 64-bit mode is bad
        if (CAVIUM_IS_MODEL(CAVIUM_CN81XX)) { // check the fuses on 81XX for forced 32-bit mode
            BDK_CSR_INIT(mio_fus_dat2, node, BDK_MIO_FUS_DAT2);
            if (mio_fus_dat2.s.lmc_mode32) {
                error_print("32-bit interface width is ONLY supported for this Thunder model\n");
                ddr_interface_64b = 0; // force to 32-bit
            }
        }
    }

    // finally, say we are in 32-bit mode when it has been validated
    if (ddr_interface_64b == 0) {
        ddr_print("N%d.LMC%d: Setting 32-bit data width\n",
                  node, ddr_interface_num);
    }

    /* ddr_type only indicates DDR4 or DDR3 */
    ddr_type = get_ddr_type(node, &dimm_config_table[0]);
    debug_print("DRAM Device Type: DDR%d\n", ddr_type);

    spd_dimm_type = get_dimm_module_type(node, &dimm_config_table[0], ddr_type);

    if (ddr_type == DDR4_DRAM) {
        int spd_module_type;
        int asymmetric;
        const char *signal_load[4] = {"", "MLS", "3DS", "RSV"};

        imp_values = &ddr4_impedence_values;
        dimm_type_name = ddr4_dimm_types[spd_dimm_type];

        spd_addr =  read_spd(node, &dimm_config_table[0], DDR4_SPD_ADDRESSING_ROW_COL_BITS);
        spd_org  =  read_spd(node, &dimm_config_table[0], DDR4_SPD_MODULE_ORGANIZATION);
        spd_banks = 0xFF & read_spd(node, &dimm_config_table[0], DDR4_SPD_DENSITY_BANKS);

        bank_bits = (2 + ((spd_banks >> 4) & 0x3)) + ((spd_banks >> 6) & 0x3);
        bank_bits = min((int)bank_bits, 4); /* Controller can only address 4 bits. */

        spd_package = 0xFF & read_spd(node, &dimm_config_table[0], DDR4_SPD_PACKAGE_TYPE);
        if (spd_package & 0x80) { // non-monolithic device
            is_stacked_die = (!disable_stacked_die) ? ((spd_package & 0x73) == 0x11) : 0;
            ddr_print("DDR4: Package Type 0x%x (%s), %d die\n", spd_package,
                      signal_load[(spd_package & 3)], ((spd_package >> 4) & 7) + 1);
            is_3ds_dimm = ((spd_package & 3) == 2); // is it 3DS?
            if (is_3ds_dimm) { // is it 3DS?
                lranks_per_prank = ((spd_package >> 4) & 7) + 1;
                // FIXME: should make sure it is only 2H or 4H or 8H?
                lranks_bits = lranks_per_prank >> 1;
                if (lranks_bits == 4) lranks_bits = 3;
            }
        } else if (spd_package != 0) {
            // FIXME: print non-zero monolithic device definition
            ddr_print("DDR4: Package Type MONOLITHIC: %d die, signal load %d\n",
                      ((spd_package >> 4) & 7) + 1, (spd_package & 3));
        }

        asymmetric = (spd_org >> 6) & 1;
        if (asymmetric) {
            int spd_secondary_pkg = read_spd(node, &dimm_config_table[0],
                                             DDR4_SPD_SECONDARY_PACKAGE_TYPE);
            ddr_print("DDR4: Module Organization: ASYMMETRICAL: Secondary Package Type 0x%x\n",
                      spd_secondary_pkg);
        } else {
            uint64_t bus_width = 8 << (0x07 & read_spd(node, &dimm_config_table[0],
                                                  DDR4_SPD_MODULE_MEMORY_BUS_WIDTH));
            uint64_t ddr_width = 4 << ((spd_org >> 0) & 0x7);
            uint64_t module_cap;
            int shift = (spd_banks & 0x0F);
            die_capacity = (shift < 8) ? (256UL << shift) : ((12UL << (shift & 1)) << 10);
            ddr_print("DDR4: Module Organization: SYMMETRICAL: capacity per die %d %cbit\n",
                      (die_capacity > 512) ? (die_capacity >> 10) : die_capacity,
                      (die_capacity > 512) ? 'G' : 'M');
            module_cap = ((uint64_t)die_capacity << 20) / 8UL * bus_width / ddr_width *
                /* no. pkg ranks*/(1UL + ((spd_org >> 3) & 0x7));
            if (is_3ds_dimm) // is it 3DS?
                module_cap *= /* die_count */(uint64_t)(((spd_package >> 4) & 7) + 1);
            ddr_print("DDR4: Module Organization: SYMMETRICAL: capacity per module %lld GB\n",
                      module_cap >> 30);
        }

        spd_rawcard = 0xFF & read_spd(node, &dimm_config_table[0], DDR4_SPD_REFERENCE_RAW_CARD);
        ddr_print("DDR4: Reference Raw Card 0x%x \n", spd_rawcard);

        spd_module_type = read_spd(node, &dimm_config_table[0], DDR4_SPD_KEY_BYTE_MODULE_TYPE);
        if (spd_module_type & 0x80) { // HYBRID module
            ddr_print("DDR4: HYBRID module, type %s\n",
                      ((spd_module_type & 0x70) == 0x10) ? "NVDIMM" : "UNKNOWN");
        }

        spd_dimm_type   = spd_module_type & 0x0F;
        spd_rdimm = (spd_dimm_type == 1) || (spd_dimm_type == 5) || (spd_dimm_type == 8);
        if (spd_rdimm) {
            int spd_mfgr_id = read_spd(node, &dimm_config_table[0], DDR4_SPD_REGISTER_MANUFACTURER_ID_LSB) |
                (read_spd(node, &dimm_config_table[0], DDR4_SPD_REGISTER_MANUFACTURER_ID_MSB) << 8);
            int spd_register_rev = read_spd(node, &dimm_config_table[0], DDR4_SPD_REGISTER_REVISION_NUMBER);
            ddr_print("DDR4: RDIMM Register Manufacturer ID 0x%x Revision 0x%x\n",
                      spd_mfgr_id, spd_register_rev);

            // RAWCARD A or B must be bit 7=0 and bits 4-0 either 00000(A) or 00001(B)
            spd_rawcard_AorB = ((spd_rawcard & 0x9fUL) <= 1);
        }
    } else {
        imp_values = &ddr3_impedence_values;
        dimm_type_name = ddr3_dimm_types[spd_dimm_type];

        spd_addr = read_spd(node, &dimm_config_table[0], DDR3_SPD_ADDRESSING_ROW_COL_BITS);
        spd_org = read_spd(node, &dimm_config_table[0], DDR3_SPD_MODULE_ORGANIZATION);
        spd_banks = read_spd(node, &dimm_config_table[0], DDR3_SPD_DENSITY_BANKS) & 0xff;

        bank_bits = 3 + ((spd_banks >> 4) & 0x7);
        bank_bits = min((int)bank_bits, 3); /* Controller can only address 3 bits. */

        spd_rdimm       = (spd_dimm_type == 1) || (spd_dimm_type == 5) || (spd_dimm_type == 9);
    }

#if 0 // FIXME: why should this be possible OR needed?
    if ((s = lookup_env_parameter("ddr_rdimm_ena")) != NULL) {
        spd_rdimm = !!strtoul(s, NULL, 0);
    }
#endif

    debug_print("spd_addr        : %#06x\n", spd_addr );
    debug_print("spd_org         : %#06x\n", spd_org );
    debug_print("spd_banks       : %#06x\n", spd_banks );

    row_bits = 12 + ((spd_addr >> 3) & 0x7);
    col_bits =  9 + ((spd_addr >> 0) & 0x7);

    num_ranks =  1 + ((spd_org >> 3) & 0x7);
    dram_width = 4 << ((spd_org >> 0) & 0x7);
    num_banks = 1 << bank_bits;

    if ((s = lookup_env_parameter("ddr_num_ranks")) != NULL) {
        num_ranks = strtoul(s, NULL, 0);
    }

    /* FIX
    ** Check that values are within some theoretical limits.
    ** col_bits(min) = row_lsb(min) - bank_bits(max) - bus_bits(max) = 14 - 3 - 4 = 7
    ** col_bits(max) = row_lsb(max) - bank_bits(min) - bus_bits(min) = 18 - 2 - 3 = 13
    */
    if ((col_bits > 13) || (col_bits < 7)) {
        error_print("Unsupported number of Col Bits: %d\n", col_bits);
        ++fatal_error;
    }

    /* FIX
    ** Check that values are within some theoretical limits.
    ** row_bits(min) = pbank_lsb(min) - row_lsb(max) - rank_bits = 26 - 18 - 1 = 7
    ** row_bits(max) = pbank_lsb(max) - row_lsb(min) - rank_bits = 33 - 14 - 1 = 18
    */
    if ((row_bits > 18) || (row_bits < 7)) {
        error_print("Unsupported number of Row Bits: %d\n", row_bits);
        ++fatal_error;
    }

        wlevel_loops = WLEVEL_LOOPS_DEFAULT;
        // accept generic or interface-specific override but not for ASIM...
        if ((s = lookup_env_parameter("ddr_wlevel_loops")) == NULL)
            s = lookup_env_parameter("ddr%d_wlevel_loops", ddr_interface_num);
        if (s != NULL) {
            wlevel_loops = strtoul(s, NULL, 0);
        }

    bunk_enable = (num_ranks > 1);

    column_bits_start = 3;

    row_lsb = column_bits_start + col_bits + bank_bits - (! ddr_interface_64b);
    debug_print("row_lsb = column_bits_start + col_bits + bank_bits = %d\n", row_lsb);

    pbank_lsb = row_lsb + row_bits + bunk_enable;
    debug_print("pbank_lsb = row_lsb + row_bits + bunk_enable = %d\n", pbank_lsb);

    if (lranks_per_prank > 1) {
        pbank_lsb = row_lsb + row_bits + lranks_bits + bunk_enable;
        ddr_print("DDR4: 3DS: pbank_lsb = (%d row_lsb) + (%d row_bits) + (%d lranks_bits) + (%d bunk_enable) = %d\n",
                  row_lsb, row_bits, lranks_bits, bunk_enable, pbank_lsb);
    }

    mem_size_mbytes =  dimm_count * ((1ull << pbank_lsb) >> 20);
    if (num_ranks == 4) {
        /* Quad rank dimm capacity is equivalent to two dual-rank dimms. */
        mem_size_mbytes *= 2;
    }

    /* Mask with 1 bits set for each active rank, allowing 2 bits per dimm.
    ** This makes later calculations simpler, as a variety of CSRs use this layout.
    ** This init needs to be updated for dual configs (ie non-identical DIMMs).
    ** Bit 0 = dimm0, rank 0
    ** Bit 1 = dimm0, rank 1
    ** Bit 2 = dimm1, rank 0
    ** Bit 3 = dimm1, rank 1
    ** ...
    */
    rank_mask = 0x1;
    if (num_ranks > 1)
        rank_mask = 0x3;
    if (num_ranks > 2)
        rank_mask = 0xf;

    for (i = 1; i < dimm_count; i++)
        rank_mask |= ((rank_mask & 0x3) << (2*i));


#ifdef CAVIUM_ONLY
    /* Special request: mismatched DIMM support. Slot 0: 2-Rank, Slot 1: 1-Rank */
    if (0)
    {
        /*
        ** Calculate the total memory size in terms of the total
        ** number of ranks instead of the number of dimms.  The usual
        ** requirement is for both dimms to be identical.  This check
        ** works around that requirement to allow one exception.  The
        ** dimm in the second slot may now have fewer ranks than the
        ** first slot.
        */
        int spd_org_dimm1;
        int num_ranks_dimm1;
        int rank_count;
        int rank_mask_dimm1;

        if (dimm_count > 1) {
            spd_org_dimm1 = read_spd(node, &dimm_config_table[1] /* dimm 1*/,
                                     DDR3_SPD_MODULE_ORGANIZATION);
            num_ranks_dimm1 = 1 + ((spd_org_dimm1 >> 3) & 0x7);
            rank_count =  num_ranks/* dimm 0 */ + num_ranks_dimm1 /* dimm 1 */;

            if (num_ranks != num_ranks_dimm1) {
                mem_size_mbytes =  rank_count * ((1ull << (pbank_lsb-bunk_enable)) >> 20);
                rank_mask = 1 | ((num_ranks > 1) << 1);
                rank_mask_dimm1 = 1 | ((num_ranks_dimm1 > 1) << 1);
                rank_mask |= ((rank_mask_dimm1 & 0x3) << 2);
                ddr_print("DIMM 1 - ranks: %d, size: %d MB\n",
                          num_ranks_dimm1, num_ranks_dimm1 * ((1ull << (pbank_lsb-bunk_enable)) >> 20));
            }
        }
    }
#endif  /* CAVIUM_ONLY */

    spd_ecc = get_dimm_ecc(node, &dimm_config_table[0], ddr_type);

    VB_PRT(VBL_DEV, "Summary: - %d %s%s %dRx%d %s, row bits=%d, col bits=%d, bank bits=%d\n",
              dimm_count, dimm_type_name, (dimm_count > 1) ? "s" : "",
              num_ranks, dram_width, (spd_ecc) ? "ECC" : "non-ECC",
              row_bits, col_bits, bank_bits);

    // always print out the useful DIMM information...
    for (i = 0; i < DDR_CFG_T_MAX_DIMMS; i++) {
        if (i < dimm_count)
            report_dimm(node, &dimm_config_table[i], i, ddr_interface_num,
                        num_ranks, dram_width, mem_size_mbytes / dimm_count);
        else
            if (validate_dimm(node, &dimm_config_table[i]) == 0) // only if there is a slot
                printf("N%d.LMC%d.DIMM%d: Not Present\n", node, ddr_interface_num, i);
    }

    if (ddr_type == DDR4_DRAM) {
        spd_cas_latency  = ((0xff & read_spd(node, &dimm_config_table[0], DDR4_SPD_CAS_LATENCIES_BYTE0)) <<  0);
        spd_cas_latency |= ((0xff & read_spd(node, &dimm_config_table[0], DDR4_SPD_CAS_LATENCIES_BYTE1)) <<  8);
        spd_cas_latency |= ((0xff & read_spd(node, &dimm_config_table[0], DDR4_SPD_CAS_LATENCIES_BYTE2)) << 16);
        spd_cas_latency |= ((0xff & read_spd(node, &dimm_config_table[0], DDR4_SPD_CAS_LATENCIES_BYTE3)) << 24);
    } else {
        spd_cas_latency  = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_CAS_LATENCIES_LSB);
        spd_cas_latency |= ((0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_CAS_LATENCIES_MSB)) << 8);
    }
    debug_print("spd_cas_latency : %#06x\n", spd_cas_latency );

    if (ddr_type == DDR4_DRAM) {

        /* No other values for DDR4 MTB and FTB are specified at the
         * current time so don't bother reading them. Can't speculate how
         * new values will be represented.
         */
        int spdMTB = 125;
        int spdFTB = 1;

        tAAmin
          = spdMTB *        read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_CAS_LATENCY_TAAMIN)
          + spdFTB * (SC_t) read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_CAS_LATENCY_FINE_TAAMIN);

        ddr4_tCKAVGmin
          = spdMTB *        read_spd(node, &dimm_config_table[0], DDR4_SPD_MINIMUM_CYCLE_TIME_TCKAVGMIN)
          + spdFTB * (SC_t) read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_CYCLE_TIME_FINE_TCKAVGMIN);

        ddr4_tCKAVGmax
          = spdMTB *        read_spd(node, &dimm_config_table[0], DDR4_SPD_MAXIMUM_CYCLE_TIME_TCKAVGMAX)
          + spdFTB * (SC_t) read_spd(node, &dimm_config_table[0], DDR4_SPD_MAX_CYCLE_TIME_FINE_TCKAVGMAX);

        ddr4_tRCDmin
          = spdMTB *        read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_RAS_CAS_DELAY_TRCDMIN)
          + spdFTB * (SC_t) read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_RAS_TO_CAS_DELAY_FINE_TRCDMIN);

        ddr4_tRPmin
          = spdMTB *        read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_ROW_PRECHARGE_DELAY_TRPMIN)
          + spdFTB * (SC_t) read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_ROW_PRECHARGE_DELAY_FINE_TRPMIN);

        ddr4_tRASmin
          = spdMTB * (((read_spd(node, &dimm_config_table[0], DDR4_SPD_UPPER_NIBBLES_TRAS_TRC) & 0xf) << 8) +
                      ( read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_ACTIVE_PRECHARGE_LSB_TRASMIN) & 0xff));

        ddr4_tRCmin
          = spdMTB * ((((read_spd(node, &dimm_config_table[0], DDR4_SPD_UPPER_NIBBLES_TRAS_TRC) >> 4) & 0xf) << 8) +
                      (  read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_ACTIVE_REFRESH_LSB_TRCMIN) & 0xff))
          + spdFTB * (SC_t) read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_ACT_TO_ACT_REFRESH_DELAY_FINE_TRCMIN);

        ddr4_tRFC1min
          = spdMTB * (((read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_REFRESH_RECOVERY_MSB_TRFC1MIN) & 0xff) << 8) +
                      ( read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_REFRESH_RECOVERY_LSB_TRFC1MIN) & 0xff));

        ddr4_tRFC2min
            = spdMTB * (((read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_REFRESH_RECOVERY_MSB_TRFC2MIN) & 0xff) << 8) +
                        ( read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_REFRESH_RECOVERY_LSB_TRFC2MIN) & 0xff));

        ddr4_tRFC4min
            = spdMTB * (((read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_REFRESH_RECOVERY_MSB_TRFC4MIN) & 0xff) << 8) +
                        ( read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_REFRESH_RECOVERY_LSB_TRFC4MIN) & 0xff));

        ddr4_tFAWmin
            = spdMTB * (((read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_FOUR_ACTIVE_WINDOW_MSN_TFAWMIN) & 0xf) << 8) +
                        ( read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_FOUR_ACTIVE_WINDOW_LSB_TFAWMIN) & 0xff));

        ddr4_tRRD_Smin
            = spdMTB *        read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_ROW_ACTIVE_DELAY_SAME_TRRD_SMIN)
            + spdFTB * (SC_t) read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_ACT_TO_ACT_DELAY_DIFF_FINE_TRRD_SMIN);

        ddr4_tRRD_Lmin
            = spdMTB *        read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_ROW_ACTIVE_DELAY_DIFF_TRRD_LMIN)
            + spdFTB * (SC_t) read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_ACT_TO_ACT_DELAY_SAME_FINE_TRRD_LMIN);

        ddr4_tCCD_Lmin
            = spdMTB *        read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_CAS_TO_CAS_DELAY_TCCD_LMIN)
            + spdFTB * (SC_t) read_spd(node, &dimm_config_table[0], DDR4_SPD_MIN_CAS_TO_CAS_DELAY_FINE_TCCD_LMIN);

        ddr_print("%-45s : %6d ps\n", "Medium Timebase (MTB)",                             spdMTB);
        ddr_print("%-45s : %6d ps\n", "Fine Timebase   (FTB)",                             spdFTB);

    #define DDR4_TWR 15000
    #define DDR4_TWTR_S 2500


        tCKmin                = ddr4_tCKAVGmin;
        twr             = DDR4_TWR;
        trcd            = ddr4_tRCDmin;
        trrd            = ddr4_tRRD_Smin;
        trp             = ddr4_tRPmin;
        tras            = ddr4_tRASmin;
        trc             = ddr4_tRCmin;
        trfc            = ddr4_tRFC1min;
        twtr            = DDR4_TWTR_S;
        tfaw            = ddr4_tFAWmin;

        if (spd_rdimm) {
            spd_addr_mirror = read_spd(node, &dimm_config_table[0], DDR4_SPD_RDIMM_ADDR_MAPPING_FROM_REGISTER_TO_DRAM) & 0x1;
        } else {
            spd_addr_mirror = read_spd(node, &dimm_config_table[0], DDR4_SPD_UDIMM_ADDR_MAPPING_FROM_EDGE) & 0x1;
        }
        debug_print("spd_addr_mirror : %#06x\n", spd_addr_mirror );

    } else { /* if (ddr_type == DDR4_DRAM) */
        spd_mtb_dividend = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MEDIUM_TIMEBASE_DIVIDEND);
        spd_mtb_divisor  = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MEDIUM_TIMEBASE_DIVISOR);
        spd_tck_min      = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MINIMUM_CYCLE_TIME_TCKMIN);
        spd_taa_min      = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_CAS_LATENCY_TAAMIN);

        spd_twr          = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_WRITE_RECOVERY_TWRMIN);
        spd_trcd         = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_RAS_CAS_DELAY_TRCDMIN);
        spd_trrd         = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_ROW_ACTIVE_DELAY_TRRDMIN);
        spd_trp          = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_ROW_PRECHARGE_DELAY_TRPMIN);
        spd_tras         = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_ACTIVE_PRECHARGE_LSB_TRASMIN);
        spd_tras        |= ((0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_UPPER_NIBBLES_TRAS_TRC)&0xf) << 8);
        spd_trc          = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_ACTIVE_REFRESH_LSB_TRCMIN);
        spd_trc         |= ((0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_UPPER_NIBBLES_TRAS_TRC)&0xf0) << 4);
        spd_trfc         = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_REFRESH_RECOVERY_LSB_TRFCMIN);
        spd_trfc        |= ((0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_REFRESH_RECOVERY_MSB_TRFCMIN)) << 8);
        spd_twtr         = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_INTERNAL_WRITE_READ_CMD_TWTRMIN);
        spd_trtp         = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_INTERNAL_READ_PRECHARGE_CMD_TRTPMIN);
        spd_tfaw         = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_FOUR_ACTIVE_WINDOW_TFAWMIN);
        spd_tfaw        |= ((0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_UPPER_NIBBLE_TFAW)&0xf) << 8);
        spd_addr_mirror  = 0xff & read_spd(node, &dimm_config_table[0], DDR3_SPD_ADDRESS_MAPPING) & 0x1;
        spd_addr_mirror  = spd_addr_mirror && !spd_rdimm; /* Only address mirror unbuffered dimms.  */
        ftb_Dividend     = read_spd(node, &dimm_config_table[0], DDR3_SPD_FINE_TIMEBASE_DIVIDEND_DIVISOR) >> 4;
        ftb_Divisor      = read_spd(node, &dimm_config_table[0], DDR3_SPD_FINE_TIMEBASE_DIVIDEND_DIVISOR) & 0xf;
        ftb_Divisor      = (ftb_Divisor == 0) ? 1 : ftb_Divisor; /* Make sure that it is not 0 */

        debug_print("spd_twr         : %#06x\n", spd_twr );
        debug_print("spd_trcd        : %#06x\n", spd_trcd);
        debug_print("spd_trrd        : %#06x\n", spd_trrd);
        debug_print("spd_trp         : %#06x\n", spd_trp );
        debug_print("spd_tras        : %#06x\n", spd_tras);
        debug_print("spd_trc         : %#06x\n", spd_trc );
        debug_print("spd_trfc        : %#06x\n", spd_trfc);
        debug_print("spd_twtr        : %#06x\n", spd_twtr);
        debug_print("spd_trtp        : %#06x\n", spd_trtp);
        debug_print("spd_tfaw        : %#06x\n", spd_tfaw);
        debug_print("spd_addr_mirror : %#06x\n", spd_addr_mirror);

        mtb_psec        = spd_mtb_dividend * 1000 / spd_mtb_divisor;
        tAAmin          = mtb_psec * spd_taa_min;
        tAAmin         += ftb_Dividend * (SC_t) read_spd(node, &dimm_config_table[0], DDR3_SPD_MIN_CAS_LATENCY_FINE_TAAMIN) / ftb_Divisor;
        tCKmin          = mtb_psec * spd_tck_min;
        tCKmin         += ftb_Dividend * (SC_t) read_spd(node, &dimm_config_table[0], DDR3_SPD_MINIMUM_CYCLE_TIME_FINE_TCKMIN) / ftb_Divisor;

        twr             = spd_twr  * mtb_psec;
        trcd            = spd_trcd * mtb_psec;
        trrd            = spd_trrd * mtb_psec;
        trp             = spd_trp  * mtb_psec;
        tras            = spd_tras * mtb_psec;
        trc             = spd_trc  * mtb_psec;
        trfc            = spd_trfc * mtb_psec;
        twtr            = spd_twtr * mtb_psec;
        trtp            = spd_trtp * mtb_psec;
        tfaw            = spd_tfaw * mtb_psec;

    } /* if (ddr_type == DDR4_DRAM) */

    if (ddr_type == DDR4_DRAM) {
        ddr_print("%-45s : %6d ps (%ld MT/s)\n", "SDRAM Minimum Cycle Time (tCKAVGmin)",ddr4_tCKAVGmin,
                  pretty_psecs_to_mts(ddr4_tCKAVGmin));
        ddr_print("%-45s : %6d ps\n", "SDRAM Maximum Cycle Time (tCKAVGmax)",          ddr4_tCKAVGmax);
        ddr_print("%-45s : %6d ps\n", "Minimum CAS Latency Time (tAAmin)",             tAAmin);
        ddr_print("%-45s : %6d ps\n", "Minimum RAS to CAS Delay Time (tRCDmin)",       ddr4_tRCDmin);
        ddr_print("%-45s : %6d ps\n", "Minimum Row Precharge Delay Time (tRPmin)",     ddr4_tRPmin);
        ddr_print("%-45s : %6d ps\n", "Minimum Active to Precharge Delay (tRASmin)",   ddr4_tRASmin);
        ddr_print("%-45s : %6d ps\n", "Minimum Active to Active/Refr. Delay (tRCmin)", ddr4_tRCmin);
        ddr_print("%-45s : %6d ps\n", "Minimum Refresh Recovery Delay (tRFC1min)",     ddr4_tRFC1min);
        ddr_print("%-45s : %6d ps\n", "Minimum Refresh Recovery Delay (tRFC2min)",     ddr4_tRFC2min);
        ddr_print("%-45s : %6d ps\n", "Minimum Refresh Recovery Delay (tRFC4min)",     ddr4_tRFC4min);
        ddr_print("%-45s : %6d ps\n", "Minimum Four Activate Window Time (tFAWmin)",   ddr4_tFAWmin);
        ddr_print("%-45s : %6d ps\n", "Minimum Act. to Act. Delay (tRRD_Smin)",        ddr4_tRRD_Smin);
        ddr_print("%-45s : %6d ps\n", "Minimum Act. to Act. Delay (tRRD_Lmin)",        ddr4_tRRD_Lmin);
        ddr_print("%-45s : %6d ps\n", "Minimum CAS to CAS Delay Time (tCCD_Lmin)",     ddr4_tCCD_Lmin);
    } else {
        ddr_print("Medium Timebase (MTB)                         : %6d ps\n", mtb_psec);
        ddr_print("Minimum Cycle Time (tCKmin)                   : %6d ps (%ld MT/s)\n", tCKmin,
                  pretty_psecs_to_mts(tCKmin));
        ddr_print("Minimum CAS Latency Time (tAAmin)             : %6d ps\n", tAAmin);
        ddr_print("Write Recovery Time (tWR)                     : %6d ps\n", twr);
        ddr_print("Minimum RAS to CAS delay (tRCD)               : %6d ps\n", trcd);
        ddr_print("Minimum Row Active to Row Active delay (tRRD) : %6d ps\n", trrd);
        ddr_print("Minimum Row Precharge Delay (tRP)             : %6d ps\n", trp);
        ddr_print("Minimum Active to Precharge (tRAS)            : %6d ps\n", tras);
        ddr_print("Minimum Active to Active/Refresh Delay (tRC)  : %6d ps\n", trc);
        ddr_print("Minimum Refresh Recovery Delay (tRFC)         : %6d ps\n", trfc);
        ddr_print("Internal write to read command delay (tWTR)   : %6d ps\n", twtr);
        ddr_print("Min Internal Rd to Precharge Cmd Delay (tRTP) : %6d ps\n", trtp);
        ddr_print("Minimum Four Activate Window Delay (tFAW)     : %6d ps\n", tfaw);
    }


    /* When the cycle time is within 1 psec of the minimum accept it
       as a slight rounding error and adjust it to exactly the minimum
       cycle time. This avoids an unnecessary warning. */
    if (_abs(tclk_psecs - tCKmin) < 2)
        tclk_psecs = tCKmin;

    if (tclk_psecs < (uint64_t)tCKmin) {
        ddr_print("WARNING!!!!: DDR Clock Rate (tCLK: %lld) exceeds DIMM specifications (tCKmin: %lld)!!!!\n",
                    tclk_psecs, (uint64_t)tCKmin);
    }


    ddr_print("DDR Clock Rate (tCLK)                         : %6llu ps\n", tclk_psecs);
    ddr_print("Core Clock Rate (eCLK)                        : %6llu ps\n", eclk_psecs);

    if ((s = lookup_env_parameter("ddr_use_ecc")) != NULL) {
        use_ecc = !!strtoul(s, NULL, 0);
    }
    use_ecc = use_ecc && spd_ecc;

    ddr_interface_bytemask = ddr_interface_64b
        ? (use_ecc ? 0x1ff : 0xff)
        : (use_ecc ? 0x01f : 0x0f); // FIXME? 81xx does diff from 70xx

    ddr_print("DRAM Interface width: %d bits %s bytemask 0x%x\n",
              ddr_interface_64b ? 64 : 32, use_ecc ? "+ECC" : "",
              ddr_interface_bytemask);

    ddr_print("\n------ Board Custom Configuration Settings ------\n");
    ddr_print("%-45s : %d\n", "MIN_RTT_NOM_IDX   ", custom_lmc_config->min_rtt_nom_idx);
    ddr_print("%-45s : %d\n", "MAX_RTT_NOM_IDX   ", custom_lmc_config->max_rtt_nom_idx);
    ddr_print("%-45s : %d\n", "MIN_RODT_CTL      ", custom_lmc_config->min_rodt_ctl);
    ddr_print("%-45s : %d\n", "MAX_RODT_CTL      ", custom_lmc_config->max_rodt_ctl);
    ddr_print("%-45s : %d\n", "MIN_CAS_LATENCY   ", custom_lmc_config->min_cas_latency);
    ddr_print("%-45s : %d\n", "OFFSET_EN         ", custom_lmc_config->offset_en);
    ddr_print("%-45s : %d\n", "OFFSET_UDIMM      ", custom_lmc_config->offset_udimm);
    ddr_print("%-45s : %d\n", "OFFSET_RDIMM      ", custom_lmc_config->offset_rdimm);
    ddr_print("%-45s : %d\n", "DDR_RTT_NOM_AUTO  ", custom_lmc_config->ddr_rtt_nom_auto);
    ddr_print("%-45s : %d\n", "DDR_RODT_CTL_AUTO ", custom_lmc_config->ddr_rodt_ctl_auto);
    if (spd_rdimm)
        ddr_print("%-45s : %d\n", "RLEVEL_COMP_OFFSET", custom_lmc_config->rlevel_comp_offset_rdimm);
    else
        ddr_print("%-45s : %d\n", "RLEVEL_COMP_OFFSET", custom_lmc_config->rlevel_comp_offset_udimm);
    ddr_print("%-45s : %d\n", "RLEVEL_COMPUTE    ", custom_lmc_config->rlevel_compute);
    ddr_print("%-45s : %d\n", "DDR2T_UDIMM       ", custom_lmc_config->ddr2t_udimm);
    ddr_print("%-45s : %d\n", "DDR2T_RDIMM       ", custom_lmc_config->ddr2t_rdimm);
    ddr_print("%-45s : %d\n", "FPRCH2            ", custom_lmc_config->fprch2);
    ddr_print("-------------------------------------------------\n");


    CL              = divide_roundup(tAAmin, tclk_psecs);

    ddr_print("Desired CAS Latency                           : %6d\n", CL);

    min_cas_latency = custom_lmc_config->min_cas_latency;


    if ((s = lookup_env_parameter("ddr_min_cas_latency")) != NULL) {
        min_cas_latency = strtoul(s, NULL, 0);
    }

    {
        int base_CL;
        ddr_print("CAS Latencies supported in DIMM               :");
        base_CL = (ddr_type == DDR4_DRAM) ? 7 : 4;
        for (i=0; i<32; ++i) {
            if ((spd_cas_latency >> i) & 1) {
                ddr_print(" %d", i+base_CL);
                max_cas_latency = i+base_CL;
                if (min_cas_latency == 0)
                    min_cas_latency = i+base_CL;
            }
        }
        ddr_print("\n");

        /* Use relaxed timing when running slower than the minimum
           supported speed.  Adjust timing to match the smallest supported
           CAS Latency. */
        if (CL < min_cas_latency) {
            uint64_t adjusted_tclk = tAAmin / min_cas_latency;
            CL = min_cas_latency;
            ddr_print("Slow clock speed. Adjusting timing: tClk = %llu, Adjusted tClk = %lld\n",
                      tclk_psecs, adjusted_tclk);
            tclk_psecs = adjusted_tclk;
        }

        if ((s = lookup_env_parameter("ddr_cas_latency")) != NULL) {
            override_cas_latency = strtoul(s, NULL, 0);
        }

        /* Make sure that the selected cas latency is legal */
        for (i=(CL-base_CL); i<32; ++i) {
            if ((spd_cas_latency >> i) & 1) {
                CL = i+base_CL;
                break;
            }
        }
    }

    if (CL > max_cas_latency)
        CL = max_cas_latency;

    if (override_cas_latency != 0) {
        CL = override_cas_latency;
    }

    ddr_print("CAS Latency                                   : %6d\n", CL);

    if ((CL * tCKmin) > 20000)
    {
        ddr_print("(CLactual * tCKmin) = %d exceeds 20 ns\n", (CL * tCKmin));
    }

    if ((num_banks != 4) && (num_banks != 8) && (num_banks != 16))
    {
        error_print("Unsupported number of banks %d. Must be 4 or 8 or 16.\n", num_banks);
        ++fatal_error;
    }

    if ((num_ranks != 1) && (num_ranks != 2) && (num_ranks != 4))
    {
        error_print("Unsupported number of ranks: %d\n", num_ranks);
        ++fatal_error;
    }

    if (! CAVIUM_IS_MODEL(CAVIUM_CN81XX)) { // 88XX or 83XX, but not 81XX
        if ((dram_width != 8) && (dram_width != 16) && (dram_width != 4)) {
            error_print("Unsupported SDRAM Width, x%d.  Must be x4, x8 or x16.\n", dram_width);
            ++fatal_error;
        }
    } else if ((dram_width != 8) && (dram_width != 16)) { // 81XX can only do x8 or x16
        error_print("Unsupported SDRAM Width, x%d.  Must be x8 or x16.\n", dram_width);
        ++fatal_error;
    }


    /*
    ** Bail out here if things are not copasetic.
    */
    if (fatal_error)
        return(-1);

    /*
     * 6.9.6 LMC RESET Initialization
     *
     * The purpose of this step is to assert/deassert the RESET# pin at the
     * DDR3/DDR4 parts.
     *
     * This LMC RESET step is done for all enabled LMCs.
     */
    perform_lmc_reset(node, ddr_interface_num);

    // Make sure scrambling is disabled during init...
    {
        bdk_lmcx_control_t lmc_control;

        lmc_control.u = BDK_CSR_READ(node, BDK_LMCX_CONTROL(ddr_interface_num));
        lmc_control.s.scramble_ena = 0;
        DRAM_CSR_WRITE(node, BDK_LMCX_CONTROL(ddr_interface_num), lmc_control.u);

        DRAM_CSR_WRITE(node, BDK_LMCX_SCRAMBLE_CFG0(ddr_interface_num), 0);
        DRAM_CSR_WRITE(node, BDK_LMCX_SCRAMBLE_CFG1(ddr_interface_num), 0);
        DRAM_CSR_WRITE(node, BDK_LMCX_SCRAMBLE_CFG2(ddr_interface_num), 0);
    }


    odt_idx = dimm_count - 1;

    switch (num_ranks) {
    case 1:
        odt_config = odt_1rank_config;
        break;
    case 2:
        odt_config = odt_2rank_config;
        break;
    case 4:
        odt_config = odt_4rank_config;
        break;
    default:
        odt_config = disable_odt_config;
        error_print("Unsupported number of ranks: %d\n", num_ranks);
        ++fatal_error;
    }


    /* Parameters from DDR3 Specifications */
#define DDR3_tREFI         7800000    /* 7.8 us */
#define DDR3_ZQCS          80000ull   /* 80 ns */
#define DDR3_ZQCS_Interval 1280000000 /* 128ms/100 */
#define DDR3_tCKE          5000       /* 5 ns */
#define DDR3_tMRD          4          /* 4 nCK */
#define DDR3_tDLLK         512        /* 512 nCK */
#define DDR3_tMPRR         1          /* 1 nCK */
#define DDR3_tWLMRD        40         /* 40 nCK */
#define DDR3_tWLDQSEN      25         /* 25 nCK */

    /* Parameters from DDR4 Specifications */
#define DDR4_tMRD          8          /* 8 nCK */
#define DDR4_tDLLK         768        /* 768 nCK */

     /*
     * 6.9.7 Early LMC Initialization
     *
     * All of DDR PLL, LMC CK, and LMC DRESET initializations must be
     * completed prior to starting this LMC initialization sequence.
     *
     * Perform the following five substeps for early LMC initialization:
     *
     * 1. Software must ensure there are no pending DRAM transactions.
     *
     * 2. Write LMC(0)_CONFIG, LMC(0)_CONTROL, LMC(0)_TIMING_PARAMS0,
     *    LMC(0)_TIMING_PARAMS1, LMC(0)_MODEREG_PARAMS0,
     *    LMC(0)_MODEREG_PARAMS1, LMC(0)_DUAL_MEMCFG, LMC(0)_NXM,
     *    LMC(0)_WODT_MASK, LMC(0)_RODT_MASK, LMC(0)_COMP_CTL2,
     *    LMC(0)_PHY_CTL, LMC(0)_DIMM0/1_PARAMS, and LMC(0)_DIMM_CTL with
     *    appropriate values. All sections in this chapter can be used to
     *    derive proper register settings.
     */

    /* LMC(0)_CONFIG */
    {
        lmc_config.u = 0;

        lmc_config.s.ecc_ena         = use_ecc;
        lmc_config.s.row_lsb         = encode_row_lsb_ddr3(row_lsb, ddr_interface_64b);
        lmc_config.s.pbank_lsb       = encode_pbank_lsb_ddr3(pbank_lsb, ddr_interface_64b);

        lmc_config.s.idlepower       = 0; /* Disabled */

        if ((s = lookup_env_parameter("ddr_idlepower")) != NULL) {
            lmc_config.s.idlepower = strtoul(s, NULL, 0);
        }

        lmc_config.s.forcewrite      = 0; /* Disabled */
        lmc_config.s.ecc_adr         = 1; /* Include memory reference address in the ECC */

        if ((s = lookup_env_parameter("ddr_ecc_adr")) != NULL) {
            lmc_config.s.ecc_adr = strtoul(s, NULL, 0);
        }

        lmc_config.s.reset           = 0;

        /*
         *  Program LMC0_CONFIG[24:18], ref_zqcs_int(6:0) to
         *  RND-DN(tREFI/clkPeriod/512) Program LMC0_CONFIG[36:25],
         *  ref_zqcs_int(18:7) to
         *  RND-DN(ZQCS_Interval/clkPeriod/(512*128)). Note that this
         *  value should always be greater than 32, to account for
         *  resistor calibration delays.
         */

        lmc_config.s.ref_zqcs_int     = ((DDR3_tREFI/tclk_psecs/512) & 0x7f);
        lmc_config.s.ref_zqcs_int    |= ((max(33ull, (DDR3_ZQCS_Interval/(tclk_psecs/100)/(512*128))) & 0xfff) << 7);


        lmc_config.s.early_dqx       = 1; /* Default to enabled */

        if ((s = lookup_env_parameter("ddr_early_dqx")) == NULL)
            s = lookup_env_parameter("ddr%d_early_dqx", ddr_interface_num);
        if (s != NULL) {
            lmc_config.s.early_dqx = strtoul(s, NULL, 0);
        }

        lmc_config.s.sref_with_dll        = 0;

        lmc_config.s.rank_ena        = bunk_enable;
        lmc_config.s.rankmask        = rank_mask; /* Set later */
        lmc_config.s.mirrmask        = (spd_addr_mirror << 1 | spd_addr_mirror << 3) & rank_mask;
        lmc_config.s.init_status     = rank_mask; /* Set once and don't change it. */
        lmc_config.s.early_unload_d0_r0   = 0;
        lmc_config.s.early_unload_d0_r1   = 0;
        lmc_config.s.early_unload_d1_r0   = 0;
        lmc_config.s.early_unload_d1_r1   = 0;
        lmc_config.s.scrz                 = 0;
        // set 32-bit mode for real only when selected AND 81xx...
        if (!ddr_interface_64b && CAVIUM_IS_MODEL(CAVIUM_CN81XX)) {
            lmc_config.s.mode32b          = 1;
        }
        VB_PRT(VBL_DEV, "%-45s : %d\n", "MODE32B (init)", lmc_config.s.mode32b);
        lmc_config.s.mode_x4dev           = (dram_width == 4) ? 1 : 0;
        lmc_config.s.bg2_enable           = ((ddr_type == DDR4_DRAM) && (dram_width == 16)) ? 0 : 1;

        if ((s = lookup_env_parameter_ull("ddr_config")) != NULL) {
            lmc_config.u    = strtoull(s, NULL, 0);
        }
        ddr_print("LMC_CONFIG                                    : 0x%016llx\n", lmc_config.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(ddr_interface_num), lmc_config.u);
    }

    /* LMC(0)_CONTROL */
    {
        bdk_lmcx_control_t lmc_control;
        lmc_control.u = BDK_CSR_READ(node, BDK_LMCX_CONTROL(ddr_interface_num));
        lmc_control.s.rdimm_ena       = spd_rdimm;
        lmc_control.s.bwcnt           = 0; /* Clear counter later */
        if (spd_rdimm)
            lmc_control.s.ddr2t       = (safe_ddr_flag ? 1 : custom_lmc_config->ddr2t_rdimm );
        else
            lmc_control.s.ddr2t       = (safe_ddr_flag ? 1 : custom_lmc_config->ddr2t_udimm );
        lmc_control.s.pocas           = 0;
        lmc_control.s.fprch2          = (safe_ddr_flag ? 2 : custom_lmc_config->fprch2 );
        lmc_control.s.throttle_rd     = safe_ddr_flag ? 1 : 0;
        lmc_control.s.throttle_wr     = safe_ddr_flag ? 1 : 0;
        lmc_control.s.inorder_rd      = safe_ddr_flag ? 1 : 0;
        lmc_control.s.inorder_wr      = safe_ddr_flag ? 1 : 0;
        lmc_control.cn81xx.elev_prio_dis   = safe_ddr_flag ? 1 : 0;
        lmc_control.s.nxm_write_en    = 0; /* discards writes to
                                            addresses that don't exist
                                            in the DRAM */
        lmc_control.s.max_write_batch = 8;
        lmc_control.s.xor_bank        = 1;
        lmc_control.s.auto_dclkdis    = 1;
        lmc_control.s.int_zqcs_dis    = 0;
        lmc_control.s.ext_zqcs_dis    = 0;
        lmc_control.s.bprch           = 1;
        lmc_control.s.wodt_bprch      = 1;
        lmc_control.s.rodt_bprch      = 1;

        if ((s = lookup_env_parameter("ddr_xor_bank")) != NULL) {
            lmc_control.s.xor_bank = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_2t")) != NULL) {
            lmc_control.s.ddr2t = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_fprch2")) != NULL) {
            lmc_control.s.fprch2 = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_bprch")) != NULL) {
            lmc_control.s.bprch = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_wodt_bprch")) != NULL) {
            lmc_control.s.wodt_bprch = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_rodt_bprch")) != NULL) {
            lmc_control.s.rodt_bprch = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_int_zqcs_dis")) != NULL) {
            lmc_control.s.int_zqcs_dis = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_ext_zqcs_dis")) != NULL) {
            lmc_control.s.ext_zqcs_dis = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter_ull("ddr_control")) != NULL) {
            lmc_control.u    = strtoull(s, NULL, 0);
        }
        ddr_print("LMC_CONTROL                                   : 0x%016llx\n", lmc_control.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_CONTROL(ddr_interface_num), lmc_control.u);
    }

    /* LMC(0)_TIMING_PARAMS0 */
    {
        unsigned trp_value;
        bdk_lmcx_timing_params0_t lmc_timing_params0;
        lmc_timing_params0.u = BDK_CSR_READ(node, BDK_LMCX_TIMING_PARAMS0(ddr_interface_num));

        trp_value = divide_roundup(trp, tclk_psecs) - 1;
        ddr_print("TIMING_PARAMS0[TRP]: NEW 0x%x, OLD 0x%x\n", trp_value,
                  trp_value + (unsigned)(divide_roundup(max(4*tclk_psecs, 7500ull), tclk_psecs)) - 4);
#if 1
        if ((s = lookup_env_parameter_ull("ddr_use_old_trp")) != NULL) {
            if (!!strtoull(s, NULL, 0)) {
                trp_value += divide_roundup(max(4*tclk_psecs, 7500ull), tclk_psecs) - 4;
                ddr_print("TIMING_PARAMS0[trp]: USING OLD 0x%x\n", trp_value);
            }
        }
#endif

        lmc_timing_params0.s.txpr     = divide_roundup(max(5*tclk_psecs, trfc+10000ull), 16*tclk_psecs);
        lmc_timing_params0.s.tzqinit  = divide_roundup(max(512*tclk_psecs, 640000ull), (256*tclk_psecs));
        lmc_timing_params0.s.trp      = trp_value & 0x1f;
        lmc_timing_params0.s.tcksre   = divide_roundup(max(5*tclk_psecs, 10000ull), tclk_psecs) - 1;

        if (ddr_type == DDR4_DRAM) {
            lmc_timing_params0.s.tzqcs    = divide_roundup(128*tclk_psecs, (16*tclk_psecs)); /* Always 8. */
            lmc_timing_params0.s.tcke     = divide_roundup(max(3*tclk_psecs, (uint64_t) DDR3_tCKE), tclk_psecs) - 1;
            lmc_timing_params0.s.tmrd     = divide_roundup((DDR4_tMRD*tclk_psecs), tclk_psecs) - 1;
            //lmc_timing_params0.s.tmod     = divide_roundup(max(24*tclk_psecs, 15000ull), tclk_psecs) - 1;
            lmc_timing_params0.s.tmod     = 25; /* 25 is the max allowed */
            lmc_timing_params0.s.tdllk    = divide_roundup(DDR4_tDLLK, 256);
        } else {
            lmc_timing_params0.s.tzqcs    = divide_roundup(max(64*tclk_psecs, DDR3_ZQCS), (16*tclk_psecs));
            lmc_timing_params0.s.tcke     = divide_roundup(DDR3_tCKE, tclk_psecs) - 1;
            lmc_timing_params0.s.tmrd     = divide_roundup((DDR3_tMRD*tclk_psecs), tclk_psecs) - 1;
            lmc_timing_params0.s.tmod     = divide_roundup(max(12*tclk_psecs, 15000ull), tclk_psecs) - 1;
            lmc_timing_params0.s.tdllk    = divide_roundup(DDR3_tDLLK, 256);
        }

        if ((s = lookup_env_parameter_ull("ddr_timing_params0")) != NULL) {
            lmc_timing_params0.u    = strtoull(s, NULL, 0);
        }
        ddr_print("TIMING_PARAMS0                                : 0x%016llx\n", lmc_timing_params0.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_TIMING_PARAMS0(ddr_interface_num), lmc_timing_params0.u);
    }

    /* LMC(0)_TIMING_PARAMS1 */
    {
        int txp, temp_trcd, trfc_dlr;
        bdk_lmcx_timing_params1_t lmc_timing_params1;
        lmc_timing_params1.u = BDK_CSR_READ(node, BDK_LMCX_TIMING_PARAMS1(ddr_interface_num));

        lmc_timing_params1.s.tmprr    = divide_roundup(DDR3_tMPRR*tclk_psecs, tclk_psecs) - 1;

        lmc_timing_params1.s.tras     = divide_roundup(tras, tclk_psecs) - 1;

        // NOTE: this is reworked for pass 2.x
        temp_trcd = divide_roundup(trcd, tclk_psecs);
#if 1
        if (temp_trcd > 15)
            ddr_print("TIMING_PARAMS1[trcd]: need extension bit for 0x%x\n", temp_trcd);
#endif
        if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X) && (temp_trcd > 15)) {
            /* Let .trcd=0 serve as a flag that the field has
               overflowed. Must use Additive Latency mode as a
               workaround. */
            temp_trcd = 0;
        }
        lmc_timing_params1.s.trcd     = temp_trcd & 0x0f;
        lmc_timing_params1.s.trcd_ext = (temp_trcd >> 4) & 1;

        lmc_timing_params1.s.twtr     = divide_roundup(twtr, tclk_psecs) - 1;
        lmc_timing_params1.s.trfc     = divide_roundup(trfc, 8*tclk_psecs);

        // workaround needed for all THUNDER chips thru T88 Pass 2.0,
        // but not 81xx and 83xx...
        if ((ddr_type == DDR4_DRAM) && CAVIUM_IS_MODEL(CAVIUM_CN88XX)) {
            /* Workaround bug 24006. Use Trrd_l. */
            lmc_timing_params1.s.trrd     = divide_roundup(ddr4_tRRD_Lmin, tclk_psecs) - 2;
        } else
            lmc_timing_params1.s.trrd     = divide_roundup(trrd, tclk_psecs) - 2;

        /*
        ** tXP = max( 3nCK, 7.5 ns)     DDR3-800   tCLK = 2500 psec
        ** tXP = max( 3nCK, 7.5 ns)     DDR3-1066  tCLK = 1875 psec
        ** tXP = max( 3nCK, 6.0 ns)     DDR3-1333  tCLK = 1500 psec
        ** tXP = max( 3nCK, 6.0 ns)     DDR3-1600  tCLK = 1250 psec
        ** tXP = max( 3nCK, 6.0 ns)     DDR3-1866  tCLK = 1071 psec
        ** tXP = max( 3nCK, 6.0 ns)     DDR3-2133  tCLK =  937 psec
        */
        txp = (tclk_psecs < 1875) ? 6000 : 7500;
        // NOTE: this is reworked for pass 2.x
        int temp_txp = divide_roundup(max(3*tclk_psecs, (unsigned)txp), tclk_psecs) - 1;
#if 1
        if (temp_txp > 7)
            ddr_print("TIMING_PARAMS1[txp]: need extension bit for 0x%x\n", temp_txp);
#endif
        if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X) && (temp_txp > 7)) {
            temp_txp = 7; // max it out
        }
        lmc_timing_params1.s.txp      = temp_txp & 7;
        lmc_timing_params1.s.txp_ext  = (temp_txp >> 3) & 1;

        lmc_timing_params1.s.twlmrd   = divide_roundup(DDR3_tWLMRD*tclk_psecs, 4*tclk_psecs);
        lmc_timing_params1.s.twldqsen = divide_roundup(DDR3_tWLDQSEN*tclk_psecs, 4*tclk_psecs);
        lmc_timing_params1.s.tfaw     = divide_roundup(tfaw, 4*tclk_psecs);
        lmc_timing_params1.s.txpdll   = divide_roundup(max(10*tclk_psecs, 24000ull), tclk_psecs) - 1;

        if ((ddr_type == DDR4_DRAM) && is_3ds_dimm) {
            /*
              4 Gb: tRFC_DLR = 90 ns
              8 Gb: tRFC_DLR = 120 ns
              16 Gb: tRFC_DLR = 190 ns FIXME?
             */
            // RNDUP[tRFC_DLR(ns) / (8 * TCYC(ns))]
            if (die_capacity == 0x1000) // 4 Gbit
                trfc_dlr = 90;
            else if (die_capacity == 0x2000) // 8 Gbit
                trfc_dlr = 120;
            else if (die_capacity == 0x4000) // 16 Gbit
                trfc_dlr = 190;
            else
                trfc_dlr = 0;

            if (trfc_dlr == 0) {
                ddr_print("N%d.LMC%d: ERROR: tRFC_DLR: die_capacity %u Mbit is illegal\n",
                          node, ddr_interface_num, die_capacity);
            } else {
                lmc_timing_params1.s.trfc_dlr = divide_roundup(trfc_dlr * 1000UL, 8*tclk_psecs);
                ddr_print("N%d.LMC%d: TIMING_PARAMS1[trfc_dlr] set to %u\n",
                          node, ddr_interface_num, lmc_timing_params1.s.trfc_dlr);
            }
        }

        if ((s = lookup_env_parameter_ull("ddr_timing_params1")) != NULL) {
            lmc_timing_params1.u    = strtoull(s, NULL, 0);
        }
        ddr_print("TIMING_PARAMS1                                : 0x%016llx\n", lmc_timing_params1.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_TIMING_PARAMS1(ddr_interface_num), lmc_timing_params1.u);
    }

    /* LMC(0)_TIMING_PARAMS2 */
    if (ddr_type == DDR4_DRAM) {
        bdk_lmcx_timing_params1_t lmc_timing_params1;
        bdk_lmcx_timing_params2_t lmc_timing_params2;
        lmc_timing_params1.u = BDK_CSR_READ(node, BDK_LMCX_TIMING_PARAMS1(ddr_interface_num));
        lmc_timing_params2.u = BDK_CSR_READ(node, BDK_LMCX_TIMING_PARAMS2(ddr_interface_num));
        ddr_print("TIMING_PARAMS2                                : 0x%016llx\n", lmc_timing_params2.u);

        //lmc_timing_params2.s.trrd_l = divide_roundup(ddr4_tRRD_Lmin, tclk_psecs) - 1;
        // NOTE: this is reworked for pass 2.x
        int temp_trrd_l = divide_roundup(ddr4_tRRD_Lmin, tclk_psecs) - 2;
#if 1
        if (temp_trrd_l > 7)
            ddr_print("TIMING_PARAMS2[trrd_l]: need extension bit for 0x%x\n", temp_trrd_l);
#endif
        if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X) && (temp_trrd_l > 7)) {
            temp_trrd_l = 7; // max it out
        }
        lmc_timing_params2.s.trrd_l      = temp_trrd_l & 7;
        lmc_timing_params2.s.trrd_l_ext  = (temp_trrd_l >> 3) & 1;

        lmc_timing_params2.s.twtr_l = divide_nint(max(4*tclk_psecs, 7500ull), tclk_psecs) - 1; // correct for 1600-2400
        lmc_timing_params2.s.t_rw_op_max = 7;
        lmc_timing_params2.s.trtp = divide_roundup(max(4*tclk_psecs, 7500ull), tclk_psecs) - 1;

        ddr_print("TIMING_PARAMS2                                : 0x%016llx\n", lmc_timing_params2.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_TIMING_PARAMS2(ddr_interface_num), lmc_timing_params2.u);

        /* Workaround Errata 25823 - LMC: Possible DDR4 tWTR_L not met
           for Write-to-Read operations to the same Bank Group */
        if (lmc_timing_params1.s.twtr < (lmc_timing_params2.s.twtr_l - 4)) {
            lmc_timing_params1.s.twtr = lmc_timing_params2.s.twtr_l - 4;
            ddr_print("ERRATA 25823: NEW: TWTR: %d, TWTR_L: %d\n", lmc_timing_params1.s.twtr, lmc_timing_params2.s.twtr_l);
            ddr_print("TIMING_PARAMS1                                : 0x%016llx\n", lmc_timing_params1.u);
            DRAM_CSR_WRITE(node, BDK_LMCX_TIMING_PARAMS1(ddr_interface_num), lmc_timing_params1.u);
        }
    }

    /* LMC(0)_MODEREG_PARAMS0 */
    {
        bdk_lmcx_modereg_params0_t lmc_modereg_params0;
        int param;

        lmc_modereg_params0.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num));

        if (ddr_type == DDR4_DRAM) {
            lmc_modereg_params0.s.cwl     = 0; /* 1600 (1250ps) */
            if (tclk_psecs < 1250)
                lmc_modereg_params0.s.cwl = 1; /* 1866 (1072ps) */
            if (tclk_psecs < 1072)
                lmc_modereg_params0.s.cwl = 2; /* 2133 (938ps) */
            if (tclk_psecs < 938)
                lmc_modereg_params0.s.cwl = 3; /* 2400 (833ps) */
            if (tclk_psecs < 833)
                lmc_modereg_params0.s.cwl = 4; /* 2666 (750ps) */
            if (tclk_psecs < 750)
                lmc_modereg_params0.s.cwl = 5; /* 3200 (625ps) */
        } else {
        /*
        ** CSR   CWL         CAS write Latency
        ** ===   ===   =================================
        **  0      5   (           tCK(avg) >=   2.5 ns)
        **  1      6   (2.5 ns   > tCK(avg) >= 1.875 ns)
        **  2      7   (1.875 ns > tCK(avg) >= 1.5   ns)
        **  3      8   (1.5 ns   > tCK(avg) >= 1.25  ns)
        **  4      9   (1.25 ns  > tCK(avg) >= 1.07  ns)
        **  5     10   (1.07 ns  > tCK(avg) >= 0.935 ns)
        **  6     11   (0.935 ns > tCK(avg) >= 0.833 ns)
        **  7     12   (0.833 ns > tCK(avg) >= 0.75  ns)
        */

        lmc_modereg_params0.s.cwl     = 0;
        if (tclk_psecs < 2500)
            lmc_modereg_params0.s.cwl = 1;
        if (tclk_psecs < 1875)
            lmc_modereg_params0.s.cwl = 2;
        if (tclk_psecs < 1500)
            lmc_modereg_params0.s.cwl = 3;
        if (tclk_psecs < 1250)
            lmc_modereg_params0.s.cwl = 4;
        if (tclk_psecs < 1070)
            lmc_modereg_params0.s.cwl = 5;
        if (tclk_psecs <  935)
            lmc_modereg_params0.s.cwl = 6;
        if (tclk_psecs <  833)
            lmc_modereg_params0.s.cwl = 7;
        }

        if ((s = lookup_env_parameter("ddr_cwl")) != NULL) {
            lmc_modereg_params0.s.cwl = strtoul(s, NULL, 0) - 5;
        }

        if (ddr_type == DDR4_DRAM) {
            ddr_print("%-45s : %d, [0x%x]\n", "CAS Write Latency CWL, [CSR]",
                      lmc_modereg_params0.s.cwl + 9
                      + ((lmc_modereg_params0.s.cwl>2) ? (lmc_modereg_params0.s.cwl-3) * 2 : 0),
                      lmc_modereg_params0.s.cwl);
        } else {
            ddr_print("%-45s : %d, [0x%x]\n", "CAS Write Latency CWL, [CSR]",
                      lmc_modereg_params0.s.cwl + 5,
                      lmc_modereg_params0.s.cwl);
        }

        lmc_modereg_params0.s.mprloc  = 0;
        lmc_modereg_params0.s.mpr     = 0;
        lmc_modereg_params0.s.dll     = (ddr_type == DDR4_DRAM)?1:0; /* disable(0) for DDR3 and enable(1) for DDR4 */
        lmc_modereg_params0.s.al      = 0;
        lmc_modereg_params0.s.wlev    = 0; /* Read Only */
        lmc_modereg_params0.s.tdqs    = ((ddr_type == DDR4_DRAM) || (dram_width != 8))?0:1; /* disable(0) for DDR4 and x4/x16 DDR3 */
        lmc_modereg_params0.s.qoff    = 0;
        //lmc_modereg_params0.s.bl      = 0; /* Don't touch block dirty logic */

        if ((s = lookup_env_parameter("ddr_cl")) != NULL) {
            CL = strtoul(s, NULL, 0);
            ddr_print("CAS Latency                                   : %6d\n", CL);
        }

        if (ddr_type == DDR4_DRAM) {
            lmc_modereg_params0.s.cl      = 0x0;
            if (CL > 9)
                lmc_modereg_params0.s.cl  = 0x1;
            if (CL > 10)
                lmc_modereg_params0.s.cl  = 0x2;
            if (CL > 11)
                lmc_modereg_params0.s.cl  = 0x3;
            if (CL > 12)
                lmc_modereg_params0.s.cl  = 0x4;
            if (CL > 13)
                lmc_modereg_params0.s.cl  = 0x5;
            if (CL > 14)
                lmc_modereg_params0.s.cl  = 0x6;
            if (CL > 15)
                lmc_modereg_params0.s.cl  = 0x7;
            if (CL > 16)
                lmc_modereg_params0.s.cl  = 0x8;
            if (CL > 18)
                lmc_modereg_params0.s.cl  = 0x9;
            if (CL > 20)
                lmc_modereg_params0.s.cl  = 0xA;
            if (CL > 24)
                lmc_modereg_params0.s.cl  = 0xB;
        } else {
            lmc_modereg_params0.s.cl      = 0x2;
            if (CL > 5)
                lmc_modereg_params0.s.cl  = 0x4;
            if (CL > 6)
                lmc_modereg_params0.s.cl  = 0x6;
            if (CL > 7)
                lmc_modereg_params0.s.cl  = 0x8;
            if (CL > 8)
                lmc_modereg_params0.s.cl  = 0xA;
            if (CL > 9)
                lmc_modereg_params0.s.cl  = 0xC;
            if (CL > 10)
                lmc_modereg_params0.s.cl  = 0xE;
            if (CL > 11)
                lmc_modereg_params0.s.cl  = 0x1;
            if (CL > 12)
                lmc_modereg_params0.s.cl  = 0x3;
            if (CL > 13)
                lmc_modereg_params0.s.cl  = 0x5;
            if (CL > 14)
                lmc_modereg_params0.s.cl  = 0x7;
            if (CL > 15)
                lmc_modereg_params0.s.cl  = 0x9;
        }

        lmc_modereg_params0.s.rbt     = 0; /* Read Only. */
        lmc_modereg_params0.s.tm      = 0;
        lmc_modereg_params0.s.dllr    = 0;

        param = divide_roundup(twr, tclk_psecs);

        if (ddr_type == DDR4_DRAM) {    /* DDR4 */
            lmc_modereg_params0.s.wrp     = 1;
            if (param > 12)
                lmc_modereg_params0.s.wrp = 2;
            if (param > 14)
                lmc_modereg_params0.s.wrp = 3;
            if (param > 16)
                lmc_modereg_params0.s.wrp = 4;
            if (param > 18)
                lmc_modereg_params0.s.wrp = 5;
            if (param > 20)
                lmc_modereg_params0.s.wrp = 6;
            if (param > 24)         /* RESERVED in DDR4 spec */
                lmc_modereg_params0.s.wrp = 7;
        } else {                /* DDR3 */
            lmc_modereg_params0.s.wrp     = 1;
            if (param > 5)
                lmc_modereg_params0.s.wrp = 2;
            if (param > 6)
                lmc_modereg_params0.s.wrp = 3;
            if (param > 7)
                lmc_modereg_params0.s.wrp = 4;
            if (param > 8)
                lmc_modereg_params0.s.wrp = 5;
            if (param > 10)
                lmc_modereg_params0.s.wrp = 6;
            if (param > 12)
                lmc_modereg_params0.s.wrp = 7;
        }

        lmc_modereg_params0.s.ppd     = 0;

        if ((s = lookup_env_parameter("ddr_wrp")) != NULL) {
            lmc_modereg_params0.s.wrp = strtoul(s, NULL, 0);
        }

        ddr_print("%-45s : %d, [0x%x]\n", "Write recovery for auto precharge WRP, [CSR]",
                  param, lmc_modereg_params0.s.wrp);

        if ((s = lookup_env_parameter_ull("ddr_modereg_params0")) != NULL) {
            lmc_modereg_params0.u    = strtoull(s, NULL, 0);
        }
        ddr_print("MODEREG_PARAMS0                               : 0x%016llx\n", lmc_modereg_params0.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num), lmc_modereg_params0.u);
    }

    /* LMC(0)_MODEREG_PARAMS1 */
    {
        bdk_lmcx_modereg_params1_t lmc_modereg_params1;

        lmc_modereg_params1.u = odt_config[odt_idx].odt_mask1.u;

#ifdef CAVIUM_ONLY
        /* Special request: mismatched DIMM support. Slot 0: 2-Rank, Slot 1: 1-Rank */
        if (rank_mask == 0x7) { /* 2-Rank, 1-Rank */
            lmc_modereg_params1.s.rtt_nom_00 = 0;
            lmc_modereg_params1.s.rtt_nom_01 = 3; /* rttnom_40ohm */
            lmc_modereg_params1.s.rtt_nom_10 = 3; /* rttnom_40ohm */
            lmc_modereg_params1.s.rtt_nom_11 = 0;
            dyn_rtt_nom_mask = 0x6;
        }
#endif  /* CAVIUM_ONLY */

        if ((s = lookup_env_parameter("ddr_rtt_nom_mask")) != NULL) {
            dyn_rtt_nom_mask    = strtoul(s, NULL, 0);
        }


        /* Save the original rtt_nom settings before sweeping through settings. */
        default_rtt_nom[0] = lmc_modereg_params1.s.rtt_nom_00;
        default_rtt_nom[1] = lmc_modereg_params1.s.rtt_nom_01;
        default_rtt_nom[2] = lmc_modereg_params1.s.rtt_nom_10;
        default_rtt_nom[3] = lmc_modereg_params1.s.rtt_nom_11;

        ddr_rtt_nom_auto = custom_lmc_config->ddr_rtt_nom_auto;

        for (i=0; i<4; ++i) {
            uint64_t value;
            if ((s = lookup_env_parameter("ddr_rtt_nom_%1d%1d", !!(i&2), !!(i&1))) == NULL)
                s = lookup_env_parameter("ddr%d_rtt_nom_%1d%1d", ddr_interface_num, !!(i&2), !!(i&1));
            if (s != NULL) {
                value = strtoul(s, NULL, 0);
                lmc_modereg_params1.u &= ~((uint64_t)0x7  << (i*12+9));
                lmc_modereg_params1.u |=  ( (value & 0x7) << (i*12+9));
                default_rtt_nom[i] = value;
                ddr_rtt_nom_auto = 0;
            }
        }

        if ((s = lookup_env_parameter("ddr_rtt_nom")) == NULL)
            s = lookup_env_parameter("ddr%d_rtt_nom", ddr_interface_num);
        if (s != NULL) {
            uint64_t value;
            value = strtoul(s, NULL, 0);

            if (dyn_rtt_nom_mask & 1)
                default_rtt_nom[0] = lmc_modereg_params1.s.rtt_nom_00 = value;
            if (dyn_rtt_nom_mask & 2)
                default_rtt_nom[1] = lmc_modereg_params1.s.rtt_nom_01 = value;
            if (dyn_rtt_nom_mask & 4)
                default_rtt_nom[2] = lmc_modereg_params1.s.rtt_nom_10 = value;
            if (dyn_rtt_nom_mask & 8)
                default_rtt_nom[3] = lmc_modereg_params1.s.rtt_nom_11 = value;

            ddr_rtt_nom_auto = 0;
        }

        if ((s = lookup_env_parameter("ddr_rtt_wr")) != NULL) {
            uint64_t value = strtoul(s, NULL, 0);
            for (i=0; i<4; ++i) {
                INSRT_WR(&lmc_modereg_params1.u, i, value);
            }
        }

        for (i = 0; i < 4; ++i) {
            uint64_t value;
            if ((s = lookup_env_parameter("ddr_rtt_wr_%1d%1d", !!(i&2), !!(i&1))) == NULL)
                s = lookup_env_parameter("ddr%d_rtt_wr_%1d%1d", ddr_interface_num, !!(i&2), !!(i&1));
            if (s != NULL) {
                value = strtoul(s, NULL, 0);
                INSRT_WR(&lmc_modereg_params1.u, i, value);
            }
        }

        // Make sure pass 1 has valid RTT_WR settings, because
        // configuration files may be set-up for pass 2, and
        // pass 1 supports no RTT_WR extension bits
        if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) {
            for (i = 0; i < 4; ++i) {
                if (EXTR_WR(lmc_modereg_params1.u, i) > 3) { // if 80 or undefined
                    INSRT_WR(&lmc_modereg_params1.u, i, 1); // FIXME? always insert 120
                    ddr_print("RTT_WR_%d%d set to 120 for CN88XX pass 1\n", !!(i&2), i&1);
                }
            }
        }
        if ((s = lookup_env_parameter("ddr_dic")) != NULL) {
            uint64_t value = strtoul(s, NULL, 0);
            for (i=0; i<4; ++i) {
                lmc_modereg_params1.u &= ~((uint64_t)0x3  << (i*12+7));
                lmc_modereg_params1.u |=  ( (value & 0x3) << (i*12+7));
            }
        }

        for (i=0; i<4; ++i) {
            uint64_t value;
            if ((s = lookup_env_parameter("ddr_dic_%1d%1d", !!(i&2), !!(i&1))) != NULL) {
                value = strtoul(s, NULL, 0);
                lmc_modereg_params1.u &= ~((uint64_t)0x3  << (i*12+7));
                lmc_modereg_params1.u |=  ( (value & 0x3) << (i*12+7));
            }
        }

        if ((s = lookup_env_parameter_ull("ddr_modereg_params1")) != NULL) {
            lmc_modereg_params1.u    = strtoull(s, NULL, 0);
        }

        ddr_print("RTT_NOM     %3d, %3d, %3d, %3d ohms           :  %x,%x,%x,%x\n",
                  imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_11],
                  imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_10],
                  imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_01],
                  imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_00],
                  lmc_modereg_params1.s.rtt_nom_11,
                  lmc_modereg_params1.s.rtt_nom_10,
                  lmc_modereg_params1.s.rtt_nom_01,
                  lmc_modereg_params1.s.rtt_nom_00);

        ddr_print("RTT_WR      %3d, %3d, %3d, %3d ohms           :  %x,%x,%x,%x\n",
                  imp_values->rtt_wr_ohms[EXTR_WR(lmc_modereg_params1.u, 3)],
                  imp_values->rtt_wr_ohms[EXTR_WR(lmc_modereg_params1.u, 2)],
                  imp_values->rtt_wr_ohms[EXTR_WR(lmc_modereg_params1.u, 1)],
                  imp_values->rtt_wr_ohms[EXTR_WR(lmc_modereg_params1.u, 0)],
                  EXTR_WR(lmc_modereg_params1.u, 3),
                  EXTR_WR(lmc_modereg_params1.u, 2),
                  EXTR_WR(lmc_modereg_params1.u, 1),
                  EXTR_WR(lmc_modereg_params1.u, 0));

        ddr_print("DIC         %3d, %3d, %3d, %3d ohms           :  %x,%x,%x,%x\n",
                  imp_values->dic_ohms[lmc_modereg_params1.s.dic_11],
                  imp_values->dic_ohms[lmc_modereg_params1.s.dic_10],
                  imp_values->dic_ohms[lmc_modereg_params1.s.dic_01],
                  imp_values->dic_ohms[lmc_modereg_params1.s.dic_00],
                  lmc_modereg_params1.s.dic_11,
                  lmc_modereg_params1.s.dic_10,
                  lmc_modereg_params1.s.dic_01,
                  lmc_modereg_params1.s.dic_00);

        ddr_print("MODEREG_PARAMS1                               : 0x%016llx\n", lmc_modereg_params1.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS1(ddr_interface_num), lmc_modereg_params1.u);

    } /* LMC(0)_MODEREG_PARAMS1 */

    /* LMC(0)_MODEREG_PARAMS2 */
    if (ddr_type == DDR4_DRAM) {
        bdk_lmcx_modereg_params2_t lmc_modereg_params2;
        lmc_modereg_params2.u = odt_config[odt_idx].odt_mask2.u;

        for (i=0; i<4; ++i) {
            uint64_t value;
            if ((s = lookup_env_parameter("ddr_rtt_park_%1d%1d", !!(i&2), !!(i&1))) != NULL) {
                value = strtoul(s, NULL, 0);
                lmc_modereg_params2.u &= ~((uint64_t)0x7  << (i*10+0));
                lmc_modereg_params2.u |=  ( (value & 0x7) << (i*10+0));
            }
        }

        if ((s = lookup_env_parameter("ddr_rtt_park")) != NULL) {
            uint64_t value = strtoul(s, NULL, 0);
            for (i=0; i<4; ++i) {
                lmc_modereg_params2.u &= ~((uint64_t)0x7  << (i*10+0));
                lmc_modereg_params2.u |=  ( (value & 0x7) << (i*10+0));
            }
        }

        if ((s = lookup_env_parameter_ull("ddr_modereg_params2")) != NULL) {
            lmc_modereg_params2.u    = strtoull(s, NULL, 0);
        }

        ddr_print("RTT_PARK    %3d, %3d, %3d, %3d ohms           :  %x,%x,%x,%x\n",
                  imp_values->rtt_nom_ohms[lmc_modereg_params2.s.rtt_park_11],
                  imp_values->rtt_nom_ohms[lmc_modereg_params2.s.rtt_park_10],
                  imp_values->rtt_nom_ohms[lmc_modereg_params2.s.rtt_park_01],
                  imp_values->rtt_nom_ohms[lmc_modereg_params2.s.rtt_park_00],
                  lmc_modereg_params2.s.rtt_park_11,
                  lmc_modereg_params2.s.rtt_park_10,
                  lmc_modereg_params2.s.rtt_park_01,
                  lmc_modereg_params2.s.rtt_park_00);

        ddr_print("%-45s :  0x%x,0x%x,0x%x,0x%x\n", "VREF_RANGE",
                  lmc_modereg_params2.s.vref_range_11,
                  lmc_modereg_params2.s.vref_range_10,
                  lmc_modereg_params2.s.vref_range_01,
                  lmc_modereg_params2.s.vref_range_00);

        ddr_print("%-45s :  0x%x,0x%x,0x%x,0x%x\n", "VREF_VALUE",
                  lmc_modereg_params2.s.vref_value_11,
                  lmc_modereg_params2.s.vref_value_10,
                  lmc_modereg_params2.s.vref_value_01,
                  lmc_modereg_params2.s.vref_value_00);

        ddr_print("MODEREG_PARAMS2                               : 0x%016llx\n", lmc_modereg_params2.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS2(ddr_interface_num), lmc_modereg_params2.u);

    } /* LMC(0)_MODEREG_PARAMS2 */

    /* LMC(0)_MODEREG_PARAMS3 */
    if (ddr_type == DDR4_DRAM) {
        bdk_lmcx_modereg_params3_t lmc_modereg_params3;

        lmc_modereg_params3.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS3(ddr_interface_num));

        //lmc_modereg_params3.s.max_pd          =
        //lmc_modereg_params3.s.tc_ref          =
        //lmc_modereg_params3.s.vref_mon        =
        //lmc_modereg_params3.s.cal             =
        //lmc_modereg_params3.s.sre_abort       =
        //lmc_modereg_params3.s.rd_preamble     =
        //lmc_modereg_params3.s.wr_preamble     =
        //lmc_modereg_params3.s.par_lat_mode    =
        //lmc_modereg_params3.s.odt_pd          =
        //lmc_modereg_params3.s.ca_par_pers     =
        //lmc_modereg_params3.s.dm              =
        //lmc_modereg_params3.s.wr_dbi          =
        //lmc_modereg_params3.s.rd_dbi          =
        lmc_modereg_params3.s.tccd_l            = max(divide_roundup(ddr4_tCCD_Lmin, tclk_psecs), 5ull) - 4;
        //lmc_modereg_params3.s.lpasr           =
        //lmc_modereg_params3.s.crc             =
        //lmc_modereg_params3.s.gd              =
        //lmc_modereg_params3.s.pda             =
        //lmc_modereg_params3.s.temp_sense      =
        //lmc_modereg_params3.s.fgrm            =
        //lmc_modereg_params3.s.wr_cmd_lat      =
        //lmc_modereg_params3.s.mpr_fmt         =

        if (!CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) {
            int delay = 0;
            if ((lranks_per_prank == 4) && (ddr_hertz >= 1000000000))
                delay = 1;
            lmc_modereg_params3.s.xrank_add_tccd_l = delay;
            lmc_modereg_params3.s.xrank_add_tccd_s = delay;
        }

        ddr_print("MODEREG_PARAMS3                               : 0x%016llx\n", lmc_modereg_params3.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS3(ddr_interface_num), lmc_modereg_params3.u);
    } /* LMC(0)_MODEREG_PARAMS3 */

    /* LMC(0)_NXM */
    {
        bdk_lmcx_nxm_t lmc_nxm;
        int num_bits = row_lsb + row_bits + lranks_bits - 26;
        lmc_nxm.u = BDK_CSR_READ(node, BDK_LMCX_NXM(ddr_interface_num));

        if (rank_mask & 0x1)
            lmc_nxm.s.mem_msb_d0_r0 = num_bits;
        if (rank_mask & 0x2)
            lmc_nxm.s.mem_msb_d0_r1 = num_bits;
        if (rank_mask & 0x4)
            lmc_nxm.s.mem_msb_d1_r0 = num_bits;
        if (rank_mask & 0x8)
            lmc_nxm.s.mem_msb_d1_r1 = num_bits;

        lmc_nxm.s.cs_mask = ~rank_mask & 0xff; /* Set the mask for non-existant ranks. */

       if ((s = lookup_env_parameter_ull("ddr_nxm")) != NULL) {
            lmc_nxm.u    = strtoull(s, NULL, 0);
        }
        ddr_print("LMC_NXM                                       : 0x%016llx\n", lmc_nxm.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_NXM(ddr_interface_num), lmc_nxm.u);
    }

    /* LMC(0)_WODT_MASK */
    {
        bdk_lmcx_wodt_mask_t lmc_wodt_mask;
        lmc_wodt_mask.u = odt_config[odt_idx].odt_mask;

        if ((s = lookup_env_parameter_ull("ddr_wodt_mask")) != NULL) {
            lmc_wodt_mask.u    = strtoull(s, NULL, 0);
        }

        ddr_print("WODT_MASK                                     : 0x%016llx\n", lmc_wodt_mask.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_WODT_MASK(ddr_interface_num), lmc_wodt_mask.u);
    }

    /* LMC(0)_RODT_MASK */
    {
        int rankx;
        bdk_lmcx_rodt_mask_t lmc_rodt_mask;
        lmc_rodt_mask.u = odt_config[odt_idx].rodt_ctl;

        if ((s = lookup_env_parameter_ull("ddr_rodt_mask")) != NULL) {
            lmc_rodt_mask.u    = strtoull(s, NULL, 0);
        }

        ddr_print("%-45s : 0x%016llx\n", "RODT_MASK", lmc_rodt_mask.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_RODT_MASK(ddr_interface_num), lmc_rodt_mask.u);

        dyn_rtt_nom_mask = 0;
        for (rankx = 0; rankx < dimm_count * 4;rankx++) {
            if (!(rank_mask & (1 << rankx)))
                continue;
            dyn_rtt_nom_mask |= ((lmc_rodt_mask.u >> (8*rankx)) & 0xff);
        }
        if (num_ranks == 4) {
            /* Normally ODT1 is wired to rank 1. For quad-ranked DIMMs
               ODT1 is wired to the third rank (rank 2).  The mask,
               dyn_rtt_nom_mask, is used to indicate for which ranks
               to sweep RTT_NOM during read-leveling. Shift the bit
               from the ODT1 position over to the "ODT2" position so
               that the read-leveling analysis comes out right. */
            int odt1_bit = dyn_rtt_nom_mask & 2;
            dyn_rtt_nom_mask &= ~2;
            dyn_rtt_nom_mask |= odt1_bit<<1;
        }
        ddr_print("%-45s : 0x%02x\n", "DYN_RTT_NOM_MASK", dyn_rtt_nom_mask);
    }

    /* LMC(0)_COMP_CTL2 */
    {
        bdk_lmcx_comp_ctl2_t comp_ctl2;

        comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));

        comp_ctl2.s.dqx_ctl        = odt_config[odt_idx].odt_ena;
        comp_ctl2.s.ck_ctl        = (custom_lmc_config->ck_ctl  == 0) ? 4 : custom_lmc_config->ck_ctl;  /* Default 4=34.3 ohm */
        comp_ctl2.s.cmd_ctl        = (custom_lmc_config->cmd_ctl == 0) ? 4 : custom_lmc_config->cmd_ctl; /* Default 4=34.3 ohm */
        comp_ctl2.s.control_ctl        = (custom_lmc_config->ctl_ctl == 0) ? 4 : custom_lmc_config->ctl_ctl; /* Default 4=34.3 ohm */

        // NOTE: these are now done earlier, in Step 6.9.3
        // comp_ctl2.s.ntune_offset    = 0;
        // comp_ctl2.s.ptune_offset    = 0;

        ddr_rodt_ctl_auto = custom_lmc_config->ddr_rodt_ctl_auto;
        if ((s = lookup_env_parameter("ddr_rodt_ctl_auto")) != NULL) {
            ddr_rodt_ctl_auto = !!strtoul(s, NULL, 0);
        }

        default_rodt_ctl = odt_config[odt_idx].qs_dic;
        if ((s = lookup_env_parameter("ddr_rodt_ctl")) == NULL)
            s = lookup_env_parameter("ddr%d_rodt_ctl", ddr_interface_num);
        if (s != NULL) {
            default_rodt_ctl    = strtoul(s, NULL, 0);
            ddr_rodt_ctl_auto = 0;
        }

        comp_ctl2.s.rodt_ctl = default_rodt_ctl;

        // if DDR4, force CK_CTL to 26 ohms if it is currently 34 ohms, and DCLK speed is 1 GHz or more...
        if ((ddr_type == DDR4_DRAM) && (comp_ctl2.s.ck_ctl == ddr4_driver_34_ohm) && (ddr_hertz >= 1000000000)) {
            comp_ctl2.s.ck_ctl = ddr4_driver_26_ohm; // lowest for DDR4 is 26 ohms
            ddr_print("Forcing DDR4 COMP_CTL2[CK_CTL] to %d, %d ohms\n", comp_ctl2.s.ck_ctl,
                      imp_values->drive_strength[comp_ctl2.s.ck_ctl]);
        }

        if ((s = lookup_env_parameter("ddr_ck_ctl")) != NULL) {
            comp_ctl2.s.ck_ctl  = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_cmd_ctl")) != NULL) {
            comp_ctl2.s.cmd_ctl  = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_control_ctl")) != NULL) {
            comp_ctl2.s.control_ctl  = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_dqx_ctl")) != NULL) {
            comp_ctl2.s.dqx_ctl  = strtoul(s, NULL, 0);
        }

        ddr_print("%-45s : %d, %d ohms\n", "DQX_CTL           ", comp_ctl2.s.dqx_ctl,
                  imp_values->dqx_strength  [comp_ctl2.s.dqx_ctl    ]);
        ddr_print("%-45s : %d, %d ohms\n", "CK_CTL            ", comp_ctl2.s.ck_ctl,
                  imp_values->drive_strength[comp_ctl2.s.ck_ctl     ]);
        ddr_print("%-45s : %d, %d ohms\n", "CMD_CTL           ", comp_ctl2.s.cmd_ctl,
                  imp_values->drive_strength[comp_ctl2.s.cmd_ctl    ]);
        ddr_print("%-45s : %d, %d ohms\n", "CONTROL_CTL       ", comp_ctl2.s.control_ctl,
                  imp_values->drive_strength[comp_ctl2.s.control_ctl]);
        ddr_print("Read ODT_CTL                                  : 0x%x (%d ohms)\n",
                  comp_ctl2.s.rodt_ctl, imp_values->rodt_ohms[comp_ctl2.s.rodt_ctl]);

        DRAM_CSR_WRITE(node, BDK_LMCX_COMP_CTL2(ddr_interface_num), comp_ctl2.u);
    }

    /* LMC(0)_PHY_CTL */
    {
        bdk_lmcx_phy_ctl_t lmc_phy_ctl;
        lmc_phy_ctl.u = BDK_CSR_READ(node, BDK_LMCX_PHY_CTL(ddr_interface_num));
        lmc_phy_ctl.s.ts_stagger           = 0;

        if (!CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X) && (lranks_per_prank > 1)) {
            lmc_phy_ctl.cn81xx.c0_sel = lmc_phy_ctl.cn81xx.c1_sel = 2; // C0 is TEN, C1 is A17
            ddr_print("N%d.LMC%d: 3DS: setting PHY_CTL[cx_csel] = %d\n",
                      node, ddr_interface_num, lmc_phy_ctl.cn81xx.c1_sel);
        }

        ddr_print("PHY_CTL                                       : 0x%016llx\n", lmc_phy_ctl.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_PHY_CTL(ddr_interface_num), lmc_phy_ctl.u);
    }

    /* LMC(0)_DIMM0/1_PARAMS */
    if (spd_rdimm) {
        bdk_lmcx_dimm_ctl_t lmc_dimm_ctl;

        for (didx = 0; didx < (unsigned)dimm_count; ++didx) {
            bdk_lmcx_dimmx_params_t lmc_dimmx_params;
            int dimm = didx;
            int rc;

            lmc_dimmx_params.u = BDK_CSR_READ(node, BDK_LMCX_DIMMX_PARAMS(ddr_interface_num, dimm));


            if (ddr_type == DDR4_DRAM) {

                bdk_lmcx_dimmx_ddr4_params0_t lmc_dimmx_ddr4_params0;
                bdk_lmcx_dimmx_ddr4_params1_t lmc_dimmx_ddr4_params1;
                bdk_lmcx_ddr4_dimm_ctl_t lmc_ddr4_dimm_ctl;

                lmc_dimmx_params.s.rc0  = 0;
                lmc_dimmx_params.s.rc1  = 0;
                lmc_dimmx_params.s.rc2  = 0;

                rc = read_spd(node, &dimm_config_table[didx], DDR4_SPD_RDIMM_REGISTER_DRIVE_STRENGTH_CTL);
                lmc_dimmx_params.s.rc3  = (rc >> 4) & 0xf;
                lmc_dimmx_params.s.rc4  = ((rc >> 0) & 0x3) << 2;
                lmc_dimmx_params.s.rc4 |= ((rc >> 2) & 0x3) << 0;

                rc = read_spd(node, &dimm_config_table[didx], DDR4_SPD_RDIMM_REGISTER_DRIVE_STRENGTH_CK);
                lmc_dimmx_params.s.rc5  = ((rc >> 0) & 0x3) << 2;
                lmc_dimmx_params.s.rc5 |= ((rc >> 2) & 0x3) << 0;

                lmc_dimmx_params.s.rc6  = 0;
                lmc_dimmx_params.s.rc7  = 0;
                lmc_dimmx_params.s.rc8  = 0;
                lmc_dimmx_params.s.rc9  = 0;

                /*
                ** rc10               DDR4 RDIMM Operating Speed
                ** ====   =========================================================
                **  0                 tclk_psecs >= 1250 psec DDR4-1600 (1250 ps)
                **  1     1250 psec > tclk_psecs >= 1071 psec DDR4-1866 (1071 ps)
                **  2     1071 psec > tclk_psecs >=  938 psec DDR4-2133 ( 938 ps)
                **  3      938 psec > tclk_psecs >=  833 psec DDR4-2400 ( 833 ps)
                **  4      833 psec > tclk_psecs >=  750 psec DDR4-2666 ( 750 ps)
                **  5      750 psec > tclk_psecs >=  625 psec DDR4-3200 ( 625 ps)
                */
                lmc_dimmx_params.s.rc10        = 0;
                if (1250 > tclk_psecs)
                    lmc_dimmx_params.s.rc10    = 1;
                if (1071 > tclk_psecs)
                    lmc_dimmx_params.s.rc10    = 2;
                if (938 > tclk_psecs)
                    lmc_dimmx_params.s.rc10    = 3;
                if (833 > tclk_psecs)
                    lmc_dimmx_params.s.rc10    = 4;
                if (750 > tclk_psecs)
                    lmc_dimmx_params.s.rc10    = 5;

                lmc_dimmx_params.s.rc11 = 0;
                lmc_dimmx_params.s.rc12 = 0;
                lmc_dimmx_params.s.rc13 = (spd_dimm_type == 4) ? 0 : 4; /* 0=LRDIMM, 1=RDIMM */
                lmc_dimmx_params.s.rc13 |= (ddr_type == DDR4_DRAM) ? (spd_addr_mirror << 3) : 0;
                lmc_dimmx_params.s.rc14 = 0;
                //lmc_dimmx_params.s.rc15 = 4; /* 0 nCK latency adder */
                lmc_dimmx_params.s.rc15 = 0; /* 1 nCK latency adder */

                lmc_dimmx_ddr4_params0.u = 0;

                lmc_dimmx_ddr4_params0.s.rc8x = 0;
                lmc_dimmx_ddr4_params0.s.rc7x = 0;
                lmc_dimmx_ddr4_params0.s.rc6x = 0;
                lmc_dimmx_ddr4_params0.s.rc5x = 0;
                lmc_dimmx_ddr4_params0.s.rc4x = 0;

                lmc_dimmx_ddr4_params0.s.rc3x = compute_rc3x(tclk_psecs);

                lmc_dimmx_ddr4_params0.s.rc2x = 0;
                lmc_dimmx_ddr4_params0.s.rc1x = 0;

                lmc_dimmx_ddr4_params1.u = 0;

                lmc_dimmx_ddr4_params1.s.rcbx = 0;
                lmc_dimmx_ddr4_params1.s.rcax = 0;
                lmc_dimmx_ddr4_params1.s.rc9x = 0;

                lmc_ddr4_dimm_ctl.u = 0;
                lmc_ddr4_dimm_ctl.s.ddr4_dimm0_wmask = 0x004;
                lmc_ddr4_dimm_ctl.s.ddr4_dimm1_wmask = (dimm_count > 1) ? 0x004 : 0x0000;

                /*
                 * Handle any overrides from envvars here...
                 */
                if ((s = lookup_env_parameter("ddr_ddr4_params0")) != NULL) {
                    lmc_dimmx_ddr4_params0.u = strtoul(s, NULL, 0);
                }

                if ((s = lookup_env_parameter("ddr_ddr4_params1")) != NULL) {
                    lmc_dimmx_ddr4_params1.u = strtoul(s, NULL, 0);
                }

                if ((s = lookup_env_parameter("ddr_ddr4_dimm_ctl")) != NULL) {
                    lmc_ddr4_dimm_ctl.u = strtoul(s, NULL, 0);
                }

                for (i=0; i<11; ++i) {
                    uint64_t value;
                    if ((s = lookup_env_parameter("ddr_ddr4_rc%1xx", i+1)) != NULL) {
                        value = strtoul(s, NULL, 0);
                        if (i < 8) {
                            lmc_dimmx_ddr4_params0.u &= ~((uint64_t)0xff << (i*8));
                            lmc_dimmx_ddr4_params0.u |=           (value << (i*8));
                        } else {
                            lmc_dimmx_ddr4_params1.u &= ~((uint64_t)0xff << ((i-8)*8));
                            lmc_dimmx_ddr4_params1.u |=           (value << ((i-8)*8));
                        }
                    }
                }

                /*
                 * write the final CSR values
                 */
                DRAM_CSR_WRITE(node, BDK_LMCX_DIMMX_DDR4_PARAMS0(ddr_interface_num, dimm), lmc_dimmx_ddr4_params0.u);

                DRAM_CSR_WRITE(node, BDK_LMCX_DDR4_DIMM_CTL(ddr_interface_num), lmc_ddr4_dimm_ctl.u);

                DRAM_CSR_WRITE(node, BDK_LMCX_DIMMX_DDR4_PARAMS1(ddr_interface_num, dimm), lmc_dimmx_ddr4_params1.u);

                ddr_print("DIMM%d Register Control Words        RCBx:RC1x : %x %x %x %x %x %x %x %x %x %x %x\n",
                          dimm,
                          lmc_dimmx_ddr4_params1.s.rcbx,
                          lmc_dimmx_ddr4_params1.s.rcax,
                          lmc_dimmx_ddr4_params1.s.rc9x,
                          lmc_dimmx_ddr4_params0.s.rc8x,
                          lmc_dimmx_ddr4_params0.s.rc7x,
                          lmc_dimmx_ddr4_params0.s.rc6x,
                          lmc_dimmx_ddr4_params0.s.rc5x,
                          lmc_dimmx_ddr4_params0.s.rc4x,
                          lmc_dimmx_ddr4_params0.s.rc3x,
                          lmc_dimmx_ddr4_params0.s.rc2x,
                          lmc_dimmx_ddr4_params0.s.rc1x );

            } else { /* if (ddr_type == DDR4_DRAM) */
                rc = read_spd(node, &dimm_config_table[didx], 69);
                lmc_dimmx_params.s.rc0         = (rc >> 0) & 0xf;
                lmc_dimmx_params.s.rc1         = (rc >> 4) & 0xf;

                rc = read_spd(node, &dimm_config_table[didx], 70);
                lmc_dimmx_params.s.rc2         = (rc >> 0) & 0xf;
                lmc_dimmx_params.s.rc3         = (rc >> 4) & 0xf;

                rc = read_spd(node, &dimm_config_table[didx], 71);
                lmc_dimmx_params.s.rc4         = (rc >> 0) & 0xf;
                lmc_dimmx_params.s.rc5         = (rc >> 4) & 0xf;

                rc = read_spd(node, &dimm_config_table[didx], 72);
                lmc_dimmx_params.s.rc6         = (rc >> 0) & 0xf;
                lmc_dimmx_params.s.rc7         = (rc >> 4) & 0xf;

                rc = read_spd(node, &dimm_config_table[didx], 73);
                lmc_dimmx_params.s.rc8         = (rc >> 0) & 0xf;
                lmc_dimmx_params.s.rc9         = (rc >> 4) & 0xf;

                rc = read_spd(node, &dimm_config_table[didx], 74);
                lmc_dimmx_params.s.rc10        = (rc >> 0) & 0xf;
                lmc_dimmx_params.s.rc11        = (rc >> 4) & 0xf;

                rc = read_spd(node, &dimm_config_table[didx], 75);
                lmc_dimmx_params.s.rc12        = (rc >> 0) & 0xf;
                lmc_dimmx_params.s.rc13        = (rc >> 4) & 0xf;

                rc = read_spd(node, &dimm_config_table[didx], 76);
                lmc_dimmx_params.s.rc14        = (rc >> 0) & 0xf;
                lmc_dimmx_params.s.rc15        = (rc >> 4) & 0xf;


                if ((s = lookup_env_parameter("ddr_clk_drive")) != NULL) {
                    if (strcmp(s,"light") == 0) {
                        lmc_dimmx_params.s.rc5         = 0x0; /* Light Drive */
                    }
                    if (strcmp(s,"moderate") == 0) {
                        lmc_dimmx_params.s.rc5         = 0x5; /* Moderate Drive */
                    }
                    if (strcmp(s,"strong") == 0) {
                        lmc_dimmx_params.s.rc5         = 0xA; /* Strong Drive */
                    }
                }

                if ((s = lookup_env_parameter("ddr_cmd_drive")) != NULL) {
                    if (strcmp(s,"light") == 0) {
                        lmc_dimmx_params.s.rc3         = 0x0; /* Light Drive */
                    }
                    if (strcmp(s,"moderate") == 0) {
                        lmc_dimmx_params.s.rc3         = 0x5; /* Moderate Drive */
                    }
                    if (strcmp(s,"strong") == 0) {
                        lmc_dimmx_params.s.rc3         = 0xA; /* Strong Drive */
                    }
                }

                if ((s = lookup_env_parameter("ddr_ctl_drive")) != NULL) {
                    if (strcmp(s,"light") == 0) {
                        lmc_dimmx_params.s.rc4         = 0x0; /* Light Drive */
                    }
                    if (strcmp(s,"moderate") == 0) {
                        lmc_dimmx_params.s.rc4         = 0x5; /* Moderate Drive */
                    }
                }


                /*
                ** rc10               DDR3 RDIMM Operating Speed
                ** ====   =========================================================
                **  0                 tclk_psecs >= 2500 psec DDR3/DDR3L-800 (default)
                **  1     2500 psec > tclk_psecs >= 1875 psec DDR3/DDR3L-1066
                **  2     1875 psec > tclk_psecs >= 1500 psec DDR3/DDR3L-1333
                **  3     1500 psec > tclk_psecs >= 1250 psec DDR3/DDR3L-1600
                **  4     1250 psec > tclk_psecs >= 1071 psec DDR3-1866
                */
                lmc_dimmx_params.s.rc10        = 0;
                if (2500 > tclk_psecs)
                    lmc_dimmx_params.s.rc10    = 1;
                if (1875 > tclk_psecs)
                    lmc_dimmx_params.s.rc10    = 2;
                if (1500 > tclk_psecs)
                    lmc_dimmx_params.s.rc10    = 3;
                if (1250 > tclk_psecs)
                    lmc_dimmx_params.s.rc10    = 4;

            } /* if (ddr_type == DDR4_DRAM) */

            if ((s = lookup_env_parameter("ddr_dimmx_params")) != NULL) {
                lmc_dimmx_params.u = strtoul(s, NULL, 0);
            }

            for (i=0; i<16; ++i) {
                uint64_t value;
                if ((s = lookup_env_parameter("ddr_rc%d", i)) != NULL) {
                    value = strtoul(s, NULL, 0);
                    lmc_dimmx_params.u &= ~((uint64_t)0xf << (i*4));
                    lmc_dimmx_params.u |=           (  value << (i*4));
                }
            }

            DRAM_CSR_WRITE(node, BDK_LMCX_DIMMX_PARAMS(ddr_interface_num, dimm), lmc_dimmx_params.u);

            ddr_print("DIMM%d Register Control Words         RC15:RC0 : %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
                      dimm,
                      lmc_dimmx_params.s.rc15,
                      lmc_dimmx_params.s.rc14,
                      lmc_dimmx_params.s.rc13,
                      lmc_dimmx_params.s.rc12,
                      lmc_dimmx_params.s.rc11,
                      lmc_dimmx_params.s.rc10,
                      lmc_dimmx_params.s.rc9 ,
                      lmc_dimmx_params.s.rc8 ,
                      lmc_dimmx_params.s.rc7 ,
                      lmc_dimmx_params.s.rc6 ,
                      lmc_dimmx_params.s.rc5 ,
                      lmc_dimmx_params.s.rc4 ,
                      lmc_dimmx_params.s.rc3 ,
                      lmc_dimmx_params.s.rc2 ,
                      lmc_dimmx_params.s.rc1 ,
                      lmc_dimmx_params.s.rc0 );
        } /* for didx */

        if (ddr_type == DDR4_DRAM) {

            /* LMC0_DIMM_CTL */
            lmc_dimm_ctl.u = BDK_CSR_READ(node, BDK_LMCX_DIMM_CTL(ddr_interface_num));
            lmc_dimm_ctl.s.dimm0_wmask         = 0xdf3f;
            lmc_dimm_ctl.s.dimm1_wmask         = (dimm_count > 1) ? 0xdf3f : 0x0000;
            lmc_dimm_ctl.s.tcws                = 0x4e0;
            lmc_dimm_ctl.cn88xx.parity         = custom_lmc_config->parity;

            if ((s = lookup_env_parameter("ddr_dimm0_wmask")) != NULL) {
                lmc_dimm_ctl.s.dimm0_wmask    = strtoul(s, NULL, 0);
            }

            if ((s = lookup_env_parameter("ddr_dimm1_wmask")) != NULL) {
                lmc_dimm_ctl.s.dimm1_wmask    = strtoul(s, NULL, 0);
            }

            if ((s = lookup_env_parameter("ddr_dimm_ctl_parity")) != NULL) {
                lmc_dimm_ctl.cn88xx.parity = strtoul(s, NULL, 0);
            }

            if ((s = lookup_env_parameter("ddr_dimm_ctl_tcws")) != NULL) {
                lmc_dimm_ctl.s.tcws = strtoul(s, NULL, 0);
            }

            ddr_print("LMC DIMM_CTL                                  : 0x%016llx\n", lmc_dimm_ctl.u);
            DRAM_CSR_WRITE(node, BDK_LMCX_DIMM_CTL(ddr_interface_num), lmc_dimm_ctl.u);

            perform_octeon3_ddr3_sequence(node, rank_mask,
                                          ddr_interface_num, 0x7 ); /* Init RCW */

            /* Write RC0D last */
            lmc_dimm_ctl.s.dimm0_wmask         = 0x2000;
            lmc_dimm_ctl.s.dimm1_wmask         = (dimm_count > 1) ? 0x2000 : 0x0000;
            ddr_print("LMC DIMM_CTL                                  : 0x%016llx\n", lmc_dimm_ctl.u);
            DRAM_CSR_WRITE(node, BDK_LMCX_DIMM_CTL(ddr_interface_num), lmc_dimm_ctl.u);

            /* Don't write any extended registers the second time */
            DRAM_CSR_WRITE(node, BDK_LMCX_DDR4_DIMM_CTL(ddr_interface_num), 0);

            perform_octeon3_ddr3_sequence(node, rank_mask,
                                          ddr_interface_num, 0x7 ); /* Init RCW */
        } else {

            /* LMC0_DIMM_CTL */
            lmc_dimm_ctl.u = BDK_CSR_READ(node, BDK_LMCX_DIMM_CTL(ddr_interface_num));
            lmc_dimm_ctl.s.dimm0_wmask         = 0xffff;
            lmc_dimm_ctl.s.dimm1_wmask         = (dimm_count > 1) ? 0xffff : 0x0000;
            lmc_dimm_ctl.s.tcws                = 0x4e0;
            lmc_dimm_ctl.cn88xx.parity         = custom_lmc_config->parity;

            if ((s = lookup_env_parameter("ddr_dimm0_wmask")) != NULL) {
                lmc_dimm_ctl.s.dimm0_wmask    = strtoul(s, NULL, 0);
            }

            if ((s = lookup_env_parameter("ddr_dimm1_wmask")) != NULL) {
                lmc_dimm_ctl.s.dimm1_wmask    = strtoul(s, NULL, 0);
            }

            if ((s = lookup_env_parameter("ddr_dimm_ctl_parity")) != NULL) {
                lmc_dimm_ctl.cn88xx.parity = strtoul(s, NULL, 0);
            }

            if ((s = lookup_env_parameter("ddr_dimm_ctl_tcws")) != NULL) {
                lmc_dimm_ctl.s.tcws = strtoul(s, NULL, 0);
            }

            ddr_print("LMC DIMM_CTL                                : 0x%016llx\n", lmc_dimm_ctl.u);
            DRAM_CSR_WRITE(node, BDK_LMCX_DIMM_CTL(ddr_interface_num), lmc_dimm_ctl.u);

            perform_octeon3_ddr3_sequence(node, rank_mask,
                                          ddr_interface_num, 0x7 ); /* Init RCW */
        }
    } else { /* if (spd_rdimm) */
        /* Disable register control writes for unbuffered */
        bdk_lmcx_dimm_ctl_t lmc_dimm_ctl;
        lmc_dimm_ctl.u = BDK_CSR_READ(node, BDK_LMCX_DIMM_CTL(ddr_interface_num));
        lmc_dimm_ctl.s.dimm0_wmask         = 0;
        lmc_dimm_ctl.s.dimm1_wmask         = 0;
        DRAM_CSR_WRITE(node, BDK_LMCX_DIMM_CTL(ddr_interface_num), lmc_dimm_ctl.u);
    } /* if (spd_rdimm) */

    /*
     * Comments (steps 3 through 5) continue in perform_octeon3_ddr3_sequence()
     */
    {
        bdk_lmcx_modereg_params0_t lmc_modereg_params0;

        if (ddr_memory_preserved(node)) {
            /* Contents are being preserved. Take DRAM out of
               self-refresh first. Then init steps can procede
               normally */
            perform_octeon3_ddr3_sequence(node, rank_mask,
                                          ddr_interface_num, 3); /* self-refresh exit */
        }

        lmc_modereg_params0.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num));

        lmc_modereg_params0.s.dllr = 1; /* Set during first init sequence */
        DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num), lmc_modereg_params0.u);

        perform_ddr_init_sequence(node, rank_mask, ddr_interface_num);

        lmc_modereg_params0.s.dllr = 0; /* Clear for normal operation */
        DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num), lmc_modereg_params0.u);
    }

    // NOTE: this must be done for pass 2.x and pass 1.x
    if ((spd_rdimm) && (ddr_type == DDR4_DRAM)) {
        VB_PRT(VBL_FAE, "Running init sequence 1\n");
        change_rdimm_mpr_pattern(node, rank_mask, ddr_interface_num, dimm_count);
    }

#define DEFAULT_INTERNAL_VREF_TRAINING_LIMIT 5
    int internal_retries = 0;
    int deskew_training_errors;
    int dac_eval_retries;
    int dac_settings[9];
    int num_samples;
    int sample, lane;
    int last_lane = ((ddr_interface_64b) ? 8 : 4) + use_ecc;

#define DEFAULT_DAC_SAMPLES 7 // originally was 5
#define DAC_RETRIES_LIMIT   2

    typedef struct {
        int16_t bytes[DEFAULT_DAC_SAMPLES];
    } bytelane_sample_t;
    bytelane_sample_t lanes[9];

    memset(lanes, 0, sizeof(lanes));

    if ((ddr_type == DDR4_DRAM) && !CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) {
        num_samples = DEFAULT_DAC_SAMPLES;
    } else {
        num_samples = 1; // if DDR3 or no ability to write DAC values
    }

 perform_internal_vref_training:

    for (sample = 0; sample < num_samples; sample++) {

        dac_eval_retries = 0;

        do { // make offset and internal vref training repeatable

            /* 6.9.8 LMC Offset Training
               LMC requires input-receiver offset training. */
            Perform_Offset_Training(node, rank_mask, ddr_interface_num);

            /* 6.9.9 LMC Internal Vref Training
               LMC requires input-reference-voltage training. */
            Perform_Internal_VREF_Training(node, rank_mask, ddr_interface_num);

            // read and maybe display the DAC values for a sample
            read_DAC_DBI_settings(node, ddr_interface_num, /*DAC*/1, dac_settings);
            if ((num_samples == 1) || dram_is_verbose(VBL_DEV)) {
                display_DAC_DBI_settings(node, ddr_interface_num, /*DAC*/1, use_ecc,
                                         dac_settings, (char *)"Internal VREF");
            }

            // for DDR4, evaluate the DAC settings and retry if any issues
            if (ddr_type == DDR4_DRAM) {
                if (evaluate_DAC_settings(ddr_interface_64b, use_ecc, dac_settings)) {
                    if (++dac_eval_retries > DAC_RETRIES_LIMIT) {
                        ddr_print("N%d.LMC%d: DDR4 internal VREF DAC settings: retries exhausted; continuing...\n",
                                  node, ddr_interface_num);
                    } else {
                        ddr_print("N%d.LMC%d: DDR4 internal VREF DAC settings inconsistent; retrying....\n",
                                  node, ddr_interface_num); // FIXME? verbosity!!!
                        continue;
                    }
                }
                if (num_samples > 1) { // taking multiple samples, otherwise do nothing
                    // good sample or exhausted retries, record it
                    for (lane = 0; lane < last_lane; lane++) {
                        lanes[lane].bytes[sample] = dac_settings[lane];
                    }
                }
            }
            break; // done if DDR3, or good sample, or exhausted retries

        } while (1);

    } /* for (sample = 0; sample < num_samples; sample++) */

    if (num_samples > 1) {
        debug_print("N%d.LMC%d: DDR4 internal VREF DAC settings: processing multiple samples...\n",
                    node, ddr_interface_num);

        for (lane = 0; lane < last_lane; lane++) {
            dac_settings[lane] = process_samples_average(&lanes[lane].bytes[0], num_samples,
                                                         ddr_interface_num, lane);
        }
        display_DAC_DBI_settings(node, ddr_interface_num, /*DAC*/1, use_ecc, dac_settings, (char *)"Averaged VREF");

        // finally, write the final DAC values
        for (lane = 0; lane < last_lane; lane++) {
            load_dac_override(node, ddr_interface_num, dac_settings[lane], lane);
        }
    }

#if DAC_OVERRIDE_EARLY
    // as a second step, after internal VREF training, before starting deskew training:
    // for DDR3 and THUNDER pass 2.x, override the DAC setting to 127
    if ((ddr_type == DDR3_DRAM) && !CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) { // added 81xx and 83xx
        load_dac_override(node, ddr_interface_num, 127, /* all */0x0A);
        ddr_print("N%d.LMC%d: Overriding DDR3 internal VREF DAC settings to 127 (early).\n",
                  node, ddr_interface_num);
    }
#endif

    /*
     * 6.9.10 LMC Read Deskew Training
     * LMC requires input-read-data deskew training.
     */
    if (! disable_deskew_training) {

        deskew_training_errors = Perform_Read_Deskew_Training(node, rank_mask, ddr_interface_num,
                                                         spd_rawcard_AorB, 0, ddr_interface_64b);

        // All the Deskew lock and saturation retries (may) have been done,
        //  but we ended up with nibble errors; so, as a last ditch effort,
        //  enable retries of the Internal Vref Training...
        if (deskew_training_errors) {
            if (internal_retries < DEFAULT_INTERNAL_VREF_TRAINING_LIMIT) {
                internal_retries++;
                VB_PRT(VBL_FAE, "N%d.LMC%d: Deskew training results still unsettled - retrying internal Vref training (%d)\n",
                       node, ddr_interface_num, internal_retries);
                goto perform_internal_vref_training;
            } else {
                VB_PRT(VBL_FAE, "N%d.LMC%d: Deskew training incomplete - %d retries exhausted, but continuing...\n",
                       node, ddr_interface_num, internal_retries);
            }
        }

        // FIXME: treat this as the final DSK print from now on, and print if VBL_NORM or above
        // also, save the results of the original training
        Validate_Read_Deskew_Training(node, rank_mask, ddr_interface_num, &deskew_training_results, VBL_NORM);

        // setup write bit-deskew if enabled...
        if (enable_write_deskew) {
            ddr_print("N%d.LMC%d: WRITE BIT-DESKEW feature enabled- going NEUTRAL.\n",
                      node, ddr_interface_num);
            Neutral_Write_Deskew_Setup(node, ddr_interface_num);
        } /* if (enable_write_deskew) */

    } /* if (! disable_deskew_training) */

#if !DAC_OVERRIDE_EARLY
    // as a final step in internal VREF training, after deskew training but before HW WL:
    // for DDR3 and THUNDER pass 2.x, override the DAC setting to 127
    if ((ddr_type == DDR3_DRAM) && !CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X)) { // added 81xx and 83xx
        load_dac_override(node, ddr_interface_num, 127, /* all */0x0A);
        ddr_print("N%d.LMC%d, Overriding DDR3 internal VREF DAC settings to 127 (late).\n",
                  node, ddr_interface_num);
    }
#endif


    /* LMC(0)_EXT_CONFIG */
    {
        bdk_lmcx_ext_config_t ext_config;
        ext_config.u = BDK_CSR_READ(node, BDK_LMCX_EXT_CONFIG(ddr_interface_num));
        ext_config.s.vrefint_seq_deskew = 0;
        ext_config.s.read_ena_bprch = 1;
        ext_config.s.read_ena_fprch = 1;
        ext_config.s.drive_ena_fprch = 1;
        ext_config.s.drive_ena_bprch = 1;
        ext_config.s.invert_data = 0; // make sure this is OFF for all current chips

        if ((s = lookup_env_parameter("ddr_read_fprch")) != NULL) {
            ext_config.s.read_ena_fprch = strtoul(s, NULL, 0);
        }
        if ((s = lookup_env_parameter("ddr_read_bprch")) != NULL) {
            ext_config.s.read_ena_bprch = strtoul(s, NULL, 0);
        }
        if ((s = lookup_env_parameter("ddr_drive_fprch")) != NULL) {
            ext_config.s.drive_ena_fprch = strtoul(s, NULL, 0);
        }
        if ((s = lookup_env_parameter("ddr_drive_bprch")) != NULL) {
            ext_config.s.drive_ena_bprch = strtoul(s, NULL, 0);
        }

        if (!CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X) && (lranks_per_prank > 1)) {
            ext_config.s.dimm0_cid = ext_config.s.dimm1_cid = lranks_bits;
            ddr_print("N%d.LMC%d: 3DS: setting EXT_CONFIG[dimmx_cid] = %d\n",
                      node, ddr_interface_num, ext_config.s.dimm0_cid);
        }

        DRAM_CSR_WRITE(node, BDK_LMCX_EXT_CONFIG(ddr_interface_num), ext_config.u);
        ddr_print("%-45s : 0x%016llx\n", "EXT_CONFIG", ext_config.u);
    }


    {
        int save_ref_zqcs_int;
        uint64_t temp_delay_usecs;

        lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));

        /* Temporarily select the minimum ZQCS interval and wait
           long enough for a few ZQCS calibrations to occur.  This
           should ensure that the calibration circuitry is
           stabilized before read/write leveling occurs. */
        save_ref_zqcs_int         = lmc_config.s.ref_zqcs_int;
        lmc_config.s.ref_zqcs_int = 1 | (32<<7); /* set smallest interval */

        DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(ddr_interface_num), lmc_config.u);
        BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));

        /* Compute an appropriate delay based on the current ZQCS
           interval. The delay should be long enough for the
           current ZQCS delay counter to expire plus ten of the
           minimum intarvals to ensure that some calibrations
           occur. */
        temp_delay_usecs = (((uint64_t)save_ref_zqcs_int >> 7)
                            * tclk_psecs * 100 * 512 * 128) / (10000*10000)
            + 10 * ((uint64_t)32 * tclk_psecs * 100 * 512 * 128) / (10000*10000);

        VB_PRT(VBL_FAE, "N%d.LMC%d: Waiting %lld usecs for ZQCS calibrations to start\n",
                node, ddr_interface_num, temp_delay_usecs);
        bdk_wait_usec(temp_delay_usecs);

        lmc_config.s.ref_zqcs_int = save_ref_zqcs_int; /* Restore computed interval */

        DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(ddr_interface_num), lmc_config.u);
        BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
    }

    /*
     * 6.9.11 LMC Write Leveling
     *
     * LMC supports an automatic write leveling like that described in the
     * JEDEC DDR3 specifications separately per byte-lane.
     *
     * All of DDR PLL, LMC CK, LMC DRESET, and early LMC initializations must
     * be completed prior to starting this LMC write-leveling sequence.
     *
     * There are many possible procedures that will write-level all the
     * attached DDR3 DRAM parts. One possibility is for software to simply
     * write the desired values into LMC(0)_WLEVEL_RANK(0..3). This section
     * describes one possible sequence that uses LMC's autowrite-leveling
     * capabilities.
     *
     * 1. If the DQS/DQ delays on the board may be more than the ADD/CMD
     *    delays, then ensure that LMC(0)_CONFIG[EARLY_DQX] is set at this
     *    point.
     *
     * Do the remaining steps 2-7 separately for each rank i with attached
     * DRAM.
     *
     * 2. Write LMC(0)_WLEVEL_RANKi = 0.
     *
     * 3. For ×8 parts:
     *
     *    Without changing any other fields in LMC(0)_WLEVEL_CTL, write
     *    LMC(0)_WLEVEL_CTL[LANEMASK] to select all byte lanes with attached
     *    DRAM.
     *
     *    For ×16 parts:
     *
     *    Without changing any other fields in LMC(0)_WLEVEL_CTL, write
     *    LMC(0)_WLEVEL_CTL[LANEMASK] to select all even byte lanes with
     *    attached DRAM.
     *
     * 4. Without changing any other fields in LMC(0)_CONFIG,
     *
     *    o write LMC(0)_SEQ_CTL[SEQ_SEL] to select write-leveling
     *
     *    o write LMC(0)_CONFIG[RANKMASK] = (1 << i)
     *
     *    o write LMC(0)_SEQ_CTL[INIT_START] = 1
     *
     *    LMC will initiate write-leveling at this point. Assuming
     *    LMC(0)_WLEVEL_CTL [SSET] = 0, LMC first enables write-leveling on
     *    the selected DRAM rank via a DDR3 MR1 write, then sequences through
     *    and accumulates write-leveling results for eight different delay
     *    settings twice, starting at a delay of zero in this case since
     *    LMC(0)_WLEVEL_RANKi[BYTE*<4:3>] = 0, increasing by 1/8 CK each
     *    setting, covering a total distance of one CK, then disables the
     *    write-leveling via another DDR3 MR1 write.
     *
     *    After the sequence through 16 delay settings is complete:
     *
     *    o LMC sets LMC(0)_WLEVEL_RANKi[STATUS] = 3
     *
     *    o LMC sets LMC(0)_WLEVEL_RANKi[BYTE*<2:0>] (for all ranks selected
     *      by LMC(0)_WLEVEL_CTL[LANEMASK]) to indicate the first write
     *      leveling result of 1 that followed result of 0 during the
     *      sequence, except that the LMC always writes
     *      LMC(0)_WLEVEL_RANKi[BYTE*<0>]=0.
     *
     *    o Software can read the eight write-leveling results from the first
     *      pass through the delay settings by reading
     *      LMC(0)_WLEVEL_DBG[BITMASK] (after writing
     *      LMC(0)_WLEVEL_DBG[BYTE]). (LMC does not retain the writeleveling
     *      results from the second pass through the eight delay
     *      settings. They should often be identical to the
     *      LMC(0)_WLEVEL_DBG[BITMASK] results, though.)
     *
     * 5. Wait until LMC(0)_WLEVEL_RANKi[STATUS] != 2.
     *
     *    LMC will have updated LMC(0)_WLEVEL_RANKi[BYTE*<2:0>] for all byte
     *    lanes selected by LMC(0)_WLEVEL_CTL[LANEMASK] at this point.
     *    LMC(0)_WLEVEL_RANKi[BYTE*<4:3>] will still be the value that
     *    software wrote in substep 2 above, which is 0.
     *
     * 6. For ×16 parts:
     *
     *    Without changing any other fields in LMC(0)_WLEVEL_CTL, write
     *    LMC(0)_WLEVEL_CTL[LANEMASK] to select all odd byte lanes with
     *    attached DRAM.
     *
     *    Repeat substeps 4 and 5 with this new LMC(0)_WLEVEL_CTL[LANEMASK]
     *    setting. Skip to substep 7 if this has already been done.
     *
     *    For ×8 parts:
     *
     *    Skip this substep. Go to substep 7.
     *
     * 7. Calculate LMC(0)_WLEVEL_RANKi[BYTE*<4:3>] settings for all byte
     *    lanes on all ranks with attached DRAM.
     *
     *    At this point, all byte lanes on rank i with attached DRAM should
     *    have been write-leveled, and LMC(0)_WLEVEL_RANKi[BYTE*<2:0>] has
     *    the result for each byte lane.
     *
     *    But note that the DDR3 write-leveling sequence will only determine
     *    the delay modulo the CK cycle time, and cannot determine how many
     *    additional CK cycles of delay are present. Software must calculate
     *    the number of CK cycles, or equivalently, the
     *    LMC(0)_WLEVEL_RANKi[BYTE*<4:3>] settings.
     *
     *    This BYTE*<4:3> calculation is system/board specific.
     *
     * Many techniques can be used to calculate write-leveling BYTE*<4:3> values,
     * including:
     *
     *    o Known values for some byte lanes.
     *
     *    o Relative values for some byte lanes relative to others.
     *
     *    For example, suppose lane X is likely to require a larger
     *    write-leveling delay than lane Y. A BYTEX<2:0> value that is much
     *    smaller than the BYTEY<2:0> value may then indicate that the
     *    required lane X delay wrapped into the next CK, so BYTEX<4:3>
     *    should be set to BYTEY<4:3>+1.
     *
     *    When ECC DRAM is not present (i.e. when DRAM is not attached to the
     *    DDR_CBS_0_* and DDR_CB<7:0> chip signals, or the DDR_DQS_<4>_* and
     *    DDR_DQ<35:32> chip signals), write LMC(0)_WLEVEL_RANK*[BYTE8] =
     *    LMC(0)_WLEVEL_RANK*[BYTE0], using the final calculated BYTE0 value.
     *    Write LMC(0)_WLEVEL_RANK*[BYTE4] = LMC(0)_WLEVEL_RANK*[BYTE0],
     *    using the final calculated BYTE0 value.
     *
     * 8. Initialize LMC(0)_WLEVEL_RANK* values for all unused ranks.
     *
     *    Let rank i be a rank with attached DRAM.
     *
     *    For all ranks j that do not have attached DRAM, set
     *    LMC(0)_WLEVEL_RANKj = LMC(0)_WLEVEL_RANKi.
     */
    { // Start HW write-leveling block
#pragma pack(push,1)
        bdk_lmcx_wlevel_ctl_t wlevel_ctl;
        bdk_lmcx_wlevel_rankx_t lmc_wlevel_rank;
        int rankx = 0;
        int wlevel_bitmask[9];
        int byte_idx;
        int ecc_ena;
        int ddr_wlevel_roundup = 0;
        int ddr_wlevel_printall = (dram_is_verbose(VBL_FAE)); // or default to 1 to print all HW WL samples
        int disable_hwl_validity = 0;
        int default_wlevel_rtt_nom;
#if WODT_MASK_2R_1S
        uint64_t saved_wodt_mask = 0;
#endif
#pragma pack(pop)

        if (wlevel_loops)
            ddr_print("N%d.LMC%d: Performing Hardware Write-Leveling\n", node, ddr_interface_num);
        else {
            wlevel_bitmask_errors = 1; /* Force software write-leveling to run */
            ddr_print("N%d.LMC%d: Forcing software Write-Leveling\n", node, ddr_interface_num);
        }

        default_wlevel_rtt_nom = (ddr_type == DDR3_DRAM) ? rttnom_20ohm : ddr4_rttnom_40ohm ; /* FIXME? */

#if WODT_MASK_2R_1S
        if ((ddr_type == DDR4_DRAM) && (num_ranks == 2) && (dimm_count == 1)) {
            /* LMC(0)_WODT_MASK */
            bdk_lmcx_wodt_mask_t lmc_wodt_mask;
            // always save original so we can always restore later
            saved_wodt_mask = BDK_CSR_READ(node, BDK_LMCX_WODT_MASK(ddr_interface_num));
            if ((s = lookup_env_parameter_ull("ddr_hwl_wodt_mask")) != NULL) {
                lmc_wodt_mask.u = strtoull(s, NULL, 0);
                if (lmc_wodt_mask.u != saved_wodt_mask) { // print/store only when diff
                    ddr_print("WODT_MASK                                     : 0x%016llx\n", lmc_wodt_mask.u);
                    DRAM_CSR_WRITE(node, BDK_LMCX_WODT_MASK(ddr_interface_num), lmc_wodt_mask.u);
                }
            }
        }
#endif /* WODT_MASK_2R_1S */

        lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
        ecc_ena = lmc_config.s.ecc_ena;

        if ((s = lookup_env_parameter("ddr_wlevel_roundup")) != NULL) {
            ddr_wlevel_roundup = strtoul(s, NULL, 0);
        }
        if ((s = lookup_env_parameter("ddr_wlevel_printall")) != NULL) {
            ddr_wlevel_printall = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_disable_hwl_validity")) != NULL) {
            disable_hwl_validity = !!strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_wlevel_rtt_nom")) != NULL) {
            default_wlevel_rtt_nom = strtoul(s, NULL, 0);
        }

        // For DDR3, we leave the WLEVEL_CTL fields at default settings
        // For DDR4, we touch WLEVEL_CTL fields OR_DIS or BITMASK here
        if (ddr_type == DDR4_DRAM) {
            int default_or_dis  = 1;
            int default_bitmask = 0xFF;

            // when x4, use only the lower nibble bits
            if (dram_width == 4) {
                default_bitmask = 0x0F;
                VB_PRT(VBL_DEV, "N%d.LMC%d: WLEVEL_CTL: default bitmask is 0x%2x for DDR4 x4\n",
                          node, ddr_interface_num, default_bitmask);
            }

            wlevel_ctl.u = BDK_CSR_READ(node, BDK_LMCX_WLEVEL_CTL(ddr_interface_num));
            wlevel_ctl.s.or_dis  = default_or_dis;
            wlevel_ctl.s.bitmask = default_bitmask;

            // allow overrides
            if ((s = lookup_env_parameter("ddr_wlevel_ctl_or_dis")) != NULL) {
                wlevel_ctl.s.or_dis = !!strtoul(s, NULL, 0);
            }
            if ((s = lookup_env_parameter("ddr_wlevel_ctl_bitmask")) != NULL) {
                wlevel_ctl.s.bitmask = strtoul(s, NULL, 0);
            }

            // print only if not defaults
            if ((wlevel_ctl.s.or_dis != default_or_dis) || (wlevel_ctl.s.bitmask != default_bitmask)) {
                ddr_print("N%d.LMC%d: WLEVEL_CTL: or_dis=%d, bitmask=0x%02x\n",
                          node, ddr_interface_num, wlevel_ctl.s.or_dis, wlevel_ctl.s.bitmask);
            }
            // always write
            DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_CTL(ddr_interface_num), wlevel_ctl.u);
        }

        // Start the hardware write-leveling loop per rank
        for (rankx = 0; rankx < dimm_count * 4; rankx++) {

            if (!(rank_mask & (1 << rankx)))
                continue;

#if HW_WL_MAJORITY
            // array to collect counts of byte-lane values
            // assume low-order 3 bits and even, so really only 2 bit values
            int wlevel_bytes[9][4];
            memset(wlevel_bytes, 0, sizeof(wlevel_bytes));
#endif

            // restructure the looping so we can keep trying until we get the samples we want
            //for (int wloop = 0; wloop < wlevel_loops; wloop++) {
            int wloop = 0;
            int wloop_retries = 0; // retries per sample for HW-related issues with bitmasks or values
            int wloop_retries_total = 0;
            int wloop_retries_exhausted = 0;
#define WLOOP_RETRIES_DEFAULT 5
            int wlevel_validity_errors;
            int wlevel_bitmask_errors_rank = 0;
            int wlevel_validity_errors_rank = 0;

            while (wloop < wlevel_loops) {

                wlevel_ctl.u = BDK_CSR_READ(node, BDK_LMCX_WLEVEL_CTL(ddr_interface_num));

                wlevel_ctl.s.rtt_nom = (default_wlevel_rtt_nom > 0) ? (default_wlevel_rtt_nom - 1) : 7;


                DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx), 0); /* Clear write-level delays */

                wlevel_bitmask_errors = 0; /* Reset error counters */
                wlevel_validity_errors = 0;

                for (byte_idx=0; byte_idx<9; ++byte_idx) {
                    wlevel_bitmask[byte_idx] = 0; /* Reset bitmasks */
                }

#if HWL_BY_BYTE // FIXME???
                /* Make a separate pass for each byte to reduce power. */
                for (byte_idx=0; byte_idx<(8+ecc_ena); ++byte_idx) {

                    if (!(ddr_interface_bytemask&(1<<byte_idx)))
                        continue;

                    wlevel_ctl.s.lanemask = (1<<byte_idx);

                    DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_CTL(ddr_interface_num), wlevel_ctl.u);

                    /* Read and write values back in order to update the
                       status field. This insures that we read the updated
                       values after write-leveling has completed. */
                    DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx),
                                   BDK_CSR_READ(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx)));

                    perform_octeon3_ddr3_sequence(node, 1 << rankx, ddr_interface_num, 6); /* write-leveling */

                    if (!bdk_is_platform(BDK_PLATFORM_ASIM) &&
                        BDK_CSR_WAIT_FOR_FIELD(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx),
                                               status, ==, 3, 1000000))
                    {
                        error_print("ERROR: Timeout waiting for WLEVEL\n");
                    }
                    lmc_wlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx));

                    wlevel_bitmask[byte_idx] = octeon_read_lmcx_ddr3_wlevel_dbg(node, ddr_interface_num, byte_idx);
                    if (wlevel_bitmask[byte_idx] == 0)
                        ++wlevel_bitmask_errors;
                } /* for (byte_idx=0; byte_idx<(8+ecc_ena); ++byte_idx) */

                wlevel_ctl.s.lanemask = /*0x1ff*/ddr_interface_bytemask; // restore for RL
                DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_CTL(ddr_interface_num), wlevel_ctl.u);
#else
                // do all the byte-lanes at the same time
                wlevel_ctl.s.lanemask = /*0x1ff*/ddr_interface_bytemask; // FIXME?

                DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_CTL(ddr_interface_num), wlevel_ctl.u);

                /* Read and write values back in order to update the
                   status field. This insures that we read the updated
                   values after write-leveling has completed. */
                DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx),
                               BDK_CSR_READ(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx)));

                perform_octeon3_ddr3_sequence(node, 1 << rankx, ddr_interface_num, 6); /* write-leveling */

                if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx),
                                           status, ==, 3, 1000000))
                {
                    error_print("ERROR: Timeout waiting for WLEVEL\n");
                }

                lmc_wlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx));

                for (byte_idx=0; byte_idx<(8+ecc_ena); ++byte_idx) {
                    if (!(ddr_interface_bytemask&(1<<byte_idx)))
                        continue;
                    wlevel_bitmask[byte_idx] = octeon_read_lmcx_ddr3_wlevel_dbg(node, ddr_interface_num, byte_idx);
                    if (wlevel_bitmask[byte_idx] == 0)
                        ++wlevel_bitmask_errors;
                } /* for (byte_idx=0; byte_idx<(8+ecc_ena); ++byte_idx) */
#endif

                // check validity only if no bitmask errors
                if (wlevel_bitmask_errors == 0) {
                    if ((spd_dimm_type != 5) &&
                        (spd_dimm_type != 6) &&
                        (spd_dimm_type != 8) &&
                        (spd_dimm_type != 9) &&
                        (dram_width != 16)   &&
                        (ddr_interface_64b)  &&
                        !(disable_hwl_validity))
                    { // bypass if mini-[RU]DIMM or x16 or 32-bit or SO-[RU]DIMM
                        wlevel_validity_errors =
                            Validate_HW_WL_Settings(node, ddr_interface_num,
                                                    &lmc_wlevel_rank, ecc_ena);
                        wlevel_validity_errors_rank += (wlevel_validity_errors != 0);
                    }
                } else
                    wlevel_bitmask_errors_rank++;

                // before we print, if we had bitmask or validity errors, do a retry...
                if ((wlevel_bitmask_errors != 0) || (wlevel_validity_errors != 0)) {
                    // VBL must be high to show the bad bitmaps or delays here also
                    if (dram_is_verbose(VBL_DEV2)) {
                        display_WL_BM(node, ddr_interface_num, rankx, wlevel_bitmask);
                        display_WL(node, ddr_interface_num, lmc_wlevel_rank, rankx);
                    }
                    if (wloop_retries < WLOOP_RETRIES_DEFAULT) {
                        wloop_retries++;
                        wloop_retries_total++;
                        // this printout is per-retry: only when VBL is high enough (DEV2?)
                        VB_PRT(VBL_DEV2, "N%d.LMC%d.R%d: H/W Write-Leveling had %s errors - retrying...\n",
                                  node, ddr_interface_num, rankx,
                                  (wlevel_bitmask_errors) ? "Bitmask" : "Validity");
                        continue; // this takes us back to the top without counting a sample
                    } else { // ran out of retries for this sample
                        // retries exhausted, do not print at normal VBL
                        VB_PRT(VBL_DEV2, "N%d.LMC%d.R%d: H/W Write-Leveling issues: %s errors\n",
                                  node, ddr_interface_num, rankx,
                                  (wlevel_bitmask_errors) ? "Bitmask" : "Validity");
                        wloop_retries_exhausted++;
                    }
                }
                // no errors or exhausted retries, use this sample
                wloop_retries = 0; //reset for next sample

                // when only 1 sample or forced, print the bitmasks first and current HW WL
                if ((wlevel_loops == 1) || ddr_wlevel_printall) {
                    display_WL_BM(node, ddr_interface_num, rankx, wlevel_bitmask);
                    display_WL(node, ddr_interface_num, lmc_wlevel_rank, rankx);
                }

                if (ddr_wlevel_roundup) { /* Round up odd bitmask delays */
                    for (byte_idx=0; byte_idx<(8+ecc_ena); ++byte_idx) {
                        if (!(ddr_interface_bytemask&(1<<byte_idx)))
                            continue;
                        update_wlevel_rank_struct(&lmc_wlevel_rank,
                                                  byte_idx,
                                                  roundup_ddr3_wlevel_bitmask(wlevel_bitmask[byte_idx]));
                    } /* for (byte_idx=0; byte_idx<(8+ecc_ena); ++byte_idx) */
                    DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx), lmc_wlevel_rank.u);
                    display_WL(node, ddr_interface_num, lmc_wlevel_rank, rankx);
                }

#if HW_WL_MAJORITY
                // OK, we have a decent sample, no bitmask or validity errors
                for (byte_idx=0; byte_idx<(8+ecc_ena); ++byte_idx) {
                    if (!(ddr_interface_bytemask&(1<<byte_idx)))
                        continue;
                    // increment count of byte-lane value
                    int ix = (get_wlevel_rank_struct(&lmc_wlevel_rank, byte_idx) >> 1) & 3; // only 4 values
                    wlevel_bytes[byte_idx][ix]++;
                } /* for (byte_idx=0; byte_idx<(8+ecc_ena); ++byte_idx) */
#endif

                wloop++; // if we get here, we have taken a decent sample

            } /* while (wloop < wlevel_loops) */

#if HW_WL_MAJORITY
            // if we did sample more than once, try to pick a majority vote
            if (wlevel_loops > 1) {
                // look for the majority in each byte-lane
                for (byte_idx = 0; byte_idx < (8+ecc_ena); ++byte_idx) {
                    int mx = -1, mc = 0, xc = 0, cc = 0;
                    int ix, ic;
                    if (!(ddr_interface_bytemask&(1<<byte_idx)))
                        continue;
                    for (ix = 0; ix < 4; ix++) {
                        ic = wlevel_bytes[byte_idx][ix];
                        // make a bitmask of the ones with a count
                        if (ic > 0) {
                            mc |= (1 << ix);
                            cc++; // count how many had non-zero counts
                        }
                        // find the majority
                        if (ic > xc) { // new max?
                            xc = ic; // yes
                            mx = ix; // set its index
                        }
                    }
#if SWL_TRY_HWL_ALT
                    // see if there was an alternate
                    int alts = (mc & ~(1 << mx)); // take out the majority choice
                    if (alts != 0) {
                        for (ix = 0; ix < 4; ix++) {
                            if (alts & (1 << ix)) { // FIXME: could be done multiple times? bad if so
                                hwl_alts[rankx].hwl_alt_mask |= (1 << byte_idx); // set the mask
                                hwl_alts[rankx].hwl_alt_delay[byte_idx] = ix << 1; // record the value
                                VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: SWL_TRY_HWL_ALT: Byte %d maj %d (%d) alt %d (%d).\n",
                                       node, ddr_interface_num, rankx, byte_idx, mx << 1, xc,
                                       ix << 1, wlevel_bytes[byte_idx][ix]);
                            }
                        }
                    } else {
                        debug_print("N%d.LMC%d.R%d: SWL_TRY_HWL_ALT: Byte %d maj %d alt NONE.\n",
                                    node, ddr_interface_num, rankx, byte_idx, mx << 1);
                    }
#endif /* SWL_TRY_HWL_ALT */
                    if (cc > 2) { // unlikely, but...
                        // assume: counts for 3 indices are all 1
                        // possiblities are: 0/2/4, 2/4/6, 0/4/6, 0/2/6
                        // and the desired?:   2  ,   4  ,     6, 0
                        // we choose the middle, assuming one of the outliers is bad
                        // NOTE: this is an ugly hack at the moment; there must be a better way
                        switch (mc) {
                        case 0x7: mx = 1; break; // was 0/2/4, choose 2
                        case 0xb: mx = 0; break; // was 0/2/6, choose 0
                        case 0xd: mx = 3; break; // was 0/4/6, choose 6
                        case 0xe: mx = 2; break; // was 2/4/6, choose 4
                        default:
                        case 0xf: mx = 1; break; // was 0/2/4/6, choose 2?
                        }
                        error_print("N%d.LMC%d.R%d: HW WL MAJORITY: bad byte-lane %d (0x%x), using %d.\n",
                                  node, ddr_interface_num, rankx, byte_idx, mc, mx << 1);
                    }
                    update_wlevel_rank_struct(&lmc_wlevel_rank, byte_idx, mx << 1);
                } /* for (byte_idx=0; byte_idx<(8+ecc_ena); ++byte_idx) */

                DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx), lmc_wlevel_rank.u);
                display_WL_with_final(node, ddr_interface_num, lmc_wlevel_rank, rankx);
            } /* if (wlevel_loops > 1) */
#endif /* HW_WL_MAJORITY */
            // maybe print an error summary for the rank
            if ((wlevel_bitmask_errors_rank != 0) || (wlevel_validity_errors_rank != 0)) {
                VB_PRT(VBL_FAE, "N%d.LMC%d.R%d: H/W Write-Leveling errors - %d bitmask, %d validity, %d retries, %d exhausted\n",
                       node, ddr_interface_num, rankx,
                       wlevel_bitmask_errors_rank, wlevel_validity_errors_rank,
                       wloop_retries_total, wloop_retries_exhausted);
            }

        } /* for (rankx = 0; rankx < dimm_count * 4;rankx++) */

#if WODT_MASK_2R_1S
        if ((ddr_type == DDR4_DRAM) && (num_ranks == 2) && (dimm_count == 1)) {
            /* LMC(0)_WODT_MASK */
            bdk_lmcx_wodt_mask_t lmc_wodt_mask;
            // always read current so we can see if its different from saved
            lmc_wodt_mask.u = BDK_CSR_READ(node, BDK_LMCX_WODT_MASK(ddr_interface_num));
            if (lmc_wodt_mask.u != saved_wodt_mask) { // always restore what was saved if diff
                lmc_wodt_mask.u = saved_wodt_mask;
                ddr_print("WODT_MASK                                     : 0x%016llx\n", lmc_wodt_mask.u);
                DRAM_CSR_WRITE(node, BDK_LMCX_WODT_MASK(ddr_interface_num), lmc_wodt_mask.u);
            }
        }
#endif /* WODT_MASK_2R_1S */

    } // End HW write-leveling block

    // At the end of HW Write Leveling, check on some things...
    if (! disable_deskew_training) {

        deskew_counts_t dsk_counts;
        int retry_count = 0;

        VB_PRT(VBL_FAE, "N%d.LMC%d: Check Deskew Settings before Read-Leveling.\n", node, ddr_interface_num);

        do {
            Validate_Read_Deskew_Training(node, rank_mask, ddr_interface_num, &dsk_counts, VBL_FAE);

            // only RAWCARD A or B will not benefit from retraining if there's only saturation
            // or any rawcard if there is a nibble error
            if ((!spd_rawcard_AorB && dsk_counts.saturated > 0) ||
                ((dsk_counts.nibrng_errs != 0) || (dsk_counts.nibunl_errs != 0)))
            {
                retry_count++;
                VB_PRT(VBL_FAE, "N%d.LMC%d: Deskew Status indicates saturation or nibble errors - retry %d Training.\n",
                          node, ddr_interface_num, retry_count);
                Perform_Read_Deskew_Training(node, rank_mask, ddr_interface_num,
                                             spd_rawcard_AorB, 0, ddr_interface_64b);
            } else
                break;
        } while (retry_count < 5);

        // print the last setting only if we had to do retries here
        if (retry_count > 0)
            Validate_Read_Deskew_Training(node, rank_mask, ddr_interface_num, &dsk_counts, VBL_NORM);
    }

    /*
     * 6.9.12 LMC Read Leveling
     *
     * LMC supports an automatic read-leveling separately per byte-lane using
     * the DDR3 multipurpose register predefined pattern for system
     * calibration defined in the JEDEC DDR3 specifications.
     *
     * All of DDR PLL, LMC CK, and LMC DRESET, and early LMC initializations
     * must be completed prior to starting this LMC read-leveling sequence.
     *
     * Software could simply write the desired read-leveling values into
     * LMC(0)_RLEVEL_RANK(0..3). This section describes a sequence that uses
     * LMC's autoread-leveling capabilities.
     *
     * When LMC does the read-leveling sequence for a rank, it first enables
     * the DDR3 multipurpose register predefined pattern for system
     * calibration on the selected DRAM rank via a DDR3 MR3 write, then
     * executes 64 RD operations at different internal delay settings, then
     * disables the predefined pattern via another DDR3 MR3 write
     * operation. LMC determines the pass or fail of each of the 64 settings
     * independently for each byte lane, then writes appropriate
     * LMC(0)_RLEVEL_RANK(0..3)[BYTE*] values for the rank.
     *
     * After read-leveling for a rank, software can read the 64 pass/fail
     * indications for one byte lane via LMC(0)_RLEVEL_DBG[BITMASK]. Software
     * can observe all pass/fail results for all byte lanes in a rank via
     * separate read-leveling sequences on the rank with different
     * LMC(0)_RLEVEL_CTL[BYTE] values.
     *
     * The 64 pass/fail results will typically have failures for the low
     * delays, followed by a run of some passing settings, followed by more
     * failures in the remaining high delays.  LMC sets
     * LMC(0)_RLEVEL_RANK(0..3)[BYTE*] to one of the passing settings.
     * First, LMC selects the longest run of successes in the 64 results. (In
     * the unlikely event that there is more than one longest run, LMC
     * selects the first one.) Then if LMC(0)_RLEVEL_CTL[OFFSET_EN] = 1 and
     * the selected run has more than LMC(0)_RLEVEL_CTL[OFFSET] successes,
     * LMC selects the last passing setting in the run minus
     * LMC(0)_RLEVEL_CTL[OFFSET]. Otherwise LMC selects the middle setting in
     * the run (rounding earlier when necessary). We expect the read-leveling
     * sequence to produce good results with the reset values
     * LMC(0)_RLEVEL_CTL [OFFSET_EN]=1, LMC(0)_RLEVEL_CTL[OFFSET] = 2.
     *
     * The read-leveling sequence has the following steps:
     *
     * 1. Select desired LMC(0)_RLEVEL_CTL[OFFSET_EN,OFFSET,BYTE] settings.
     *    Do the remaining substeps 2-4 separately for each rank i with
     *    attached DRAM.
     *
     * 2. Without changing any other fields in LMC(0)_CONFIG,
     *
     *    o write LMC(0)_SEQ_CTL[SEQ_SEL] to select read-leveling
     *
     *    o write LMC(0)_CONFIG[RANKMASK] = (1 << i)
     *
     *    o write LMC(0)_SEQ_CTL[INIT_START] = 1
     *
     *    This initiates the previously-described read-leveling.
     *
     * 3. Wait until LMC(0)_RLEVEL_RANKi[STATUS] != 2
     *
     *    LMC will have updated LMC(0)_RLEVEL_RANKi[BYTE*] for all byte lanes
     *    at this point.
     *
     *    If ECC DRAM is not present (i.e. when DRAM is not attached to the
     *    DDR_CBS_0_* and DDR_CB<7:0> chip signals, or the DDR_DQS_<4>_* and
     *    DDR_DQ<35:32> chip signals), write LMC(0)_RLEVEL_RANK*[BYTE8] =
     *    LMC(0)_RLEVEL_RANK*[BYTE0]. Write LMC(0)_RLEVEL_RANK*[BYTE4] =
     *    LMC(0)_RLEVEL_RANK*[BYTE0].
     *
     * 4. If desired, consult LMC(0)_RLEVEL_DBG[BITMASK] and compare to
     *    LMC(0)_RLEVEL_RANKi[BYTE*] for the lane selected by
     *    LMC(0)_RLEVEL_CTL[BYTE]. If desired, modify LMC(0)_RLEVEL_CTL[BYTE]
     *    to a new value and repeat so that all BITMASKs can be observed.
     *
     * 5. Initialize LMC(0)_RLEVEL_RANK* values for all unused ranks.
     *
     *    Let rank i be a rank with attached DRAM.
     *
     *    For all ranks j that do not have attached DRAM, set
     *    LMC(0)_RLEVEL_RANKj = LMC(0)_RLEVEL_RANKi.
     *
     * This read-leveling sequence can help select the proper CN70XX ODT
     * resistance value (LMC(0)_COMP_CTL2[RODT_CTL]). A hardware-generated
     * LMC(0)_RLEVEL_RANKi[BYTEj] value (for a used byte lane j) that is
     * drastically different from a neighboring LMC(0)_RLEVEL_RANKi[BYTEk]
     * (for a used byte lane k) can indicate that the CN70XX ODT value is
     * bad. It is possible to simultaneously optimize both
     * LMC(0)_COMP_CTL2[RODT_CTL] and LMC(0)_RLEVEL_RANKn[BYTE*] values by
     * performing this read-leveling sequence for several
     * LMC(0)_COMP_CTL2[RODT_CTL] values and selecting the one with the best
     * LMC(0)_RLEVEL_RANKn[BYTE*] profile for the ranks.
     */

    {
#pragma pack(push,4)
        bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank;
        bdk_lmcx_comp_ctl2_t lmc_comp_ctl2;
        bdk_lmcx_rlevel_ctl_t rlevel_ctl;
        bdk_lmcx_control_t lmc_control;
        bdk_lmcx_modereg_params1_t lmc_modereg_params1;
        unsigned char rodt_ctl;
        unsigned char rankx = 0;
        int rlevel_rodt_errors = 0;
        unsigned char ecc_ena;
        unsigned char rtt_nom;
        unsigned char rtt_idx;
        int min_rtt_nom_idx;
        int max_rtt_nom_idx;
        int min_rodt_ctl;
        int max_rodt_ctl;
        int rlevel_debug_loops = 1;
        unsigned char save_ddr2t;
        int rlevel_avg_loops;
        int ddr_rlevel_compute;
        int saved_ddr__ptune, saved_ddr__ntune, rlevel_comp_offset;
        int saved_int_zqcs_dis = 0;
        int disable_sequential_delay_check = 0;
        int maximum_adjacent_rlevel_delay_increment = 0;
        struct {
            uint64_t setting;
            int      score;
        } rlevel_scoreboard[RTT_NOM_OHMS_COUNT][RODT_OHMS_COUNT][4];
        int print_nom_ohms;
#if PERFECT_BITMASK_COUNTING
        typedef struct {
            uint8_t count[9][32]; // 8+ECC by 32 values
            uint8_t total[9];     // 8+ECC
        } rank_perfect_t;
        rank_perfect_t rank_perfect_counts[4];
#endif

#pragma pack(pop)

#if PERFECT_BITMASK_COUNTING
        memset(rank_perfect_counts, 0, sizeof(rank_perfect_counts));
#endif /* PERFECT_BITMASK_COUNTING */

        lmc_control.u = BDK_CSR_READ(node, BDK_LMCX_CONTROL(ddr_interface_num));
        save_ddr2t    = lmc_control.s.ddr2t;

        lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
        ecc_ena      = lmc_config.s.ecc_ena;

#if 0
        {
            int save_ref_zqcs_int;
            uint64_t temp_delay_usecs;

            /* Temporarily select the minimum ZQCS interval and wait
               long enough for a few ZQCS calibrations to occur.  This
               should ensure that the calibration circuitry is
               stabilized before read-leveling occurs. */
            save_ref_zqcs_int         = lmc_config.s.ref_zqcs_int;
            lmc_config.s.ref_zqcs_int = 1 | (32<<7); /* set smallest interval */
            DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(ddr_interface_num), lmc_config.u);
            BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));

            /* Compute an appropriate delay based on the current ZQCS
               interval. The delay should be long enough for the
               current ZQCS delay counter to expire plus ten of the
               minimum intarvals to ensure that some calibrations
               occur. */
            temp_delay_usecs = (((uint64_t)save_ref_zqcs_int >> 7)
                                * tclk_psecs * 100 * 512 * 128) / (10000*10000)
                + 10 * ((uint64_t)32 * tclk_psecs * 100 * 512 * 128) / (10000*10000);

            ddr_print ("Waiting %lu usecs for ZQCS calibrations to start\n",
                         temp_delay_usecs);
            bdk_wait_usec(temp_delay_usecs);

            lmc_config.s.ref_zqcs_int = save_ref_zqcs_int; /* Restore computed interval */
            DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(ddr_interface_num), lmc_config.u);
            BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
        }
#endif

        if ((s = lookup_env_parameter("ddr_rlevel_2t")) != NULL) {
            lmc_control.s.ddr2t = strtoul(s, NULL, 0);
        }

        DRAM_CSR_WRITE(node, BDK_LMCX_CONTROL(ddr_interface_num), lmc_control.u);

        ddr_print("N%d.LMC%d: Performing Read-Leveling\n", node, ddr_interface_num);

        rlevel_ctl.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_CTL(ddr_interface_num));

        rlevel_avg_loops = custom_lmc_config->rlevel_average_loops;
        if (rlevel_avg_loops == 0) {
            rlevel_avg_loops = RLEVEL_AVG_LOOPS_DEFAULT;
            if ((dimm_count == 1) || (num_ranks == 1)) // up the samples for these cases
                rlevel_avg_loops = rlevel_avg_loops * 2 + 1;
        }

        ddr_rlevel_compute = custom_lmc_config->rlevel_compute;
        rlevel_ctl.s.offset_en = custom_lmc_config->offset_en;
        rlevel_ctl.s.offset    = spd_rdimm
            ? custom_lmc_config->offset_rdimm
            : custom_lmc_config->offset_udimm;

        rlevel_ctl.s.delay_unload_0 = 1; /* should normally be set */
        rlevel_ctl.s.delay_unload_1 = 1; /* should normally be set */
        rlevel_ctl.s.delay_unload_2 = 1; /* should normally be set */
        rlevel_ctl.s.delay_unload_3 = 1; /* should normally be set */

        rlevel_ctl.s.or_dis = 1; // default to get best bitmasks
        if ((s = lookup_env_parameter("ddr_rlevel_or_dis")) != NULL) {
            rlevel_ctl.s.or_dis = !!strtoul(s, NULL, 0);
        }
        rlevel_ctl.s.bitmask = 0xff; // should work in 32b mode also
        if ((s = lookup_env_parameter("ddr_rlevel_ctl_bitmask")) != NULL) {
            rlevel_ctl.s.bitmask = strtoul(s, NULL, 0);
        }
        debug_print("N%d.LMC%d: RLEVEL_CTL: or_dis=%d, bitmask=0x%02x\n",
                    node, ddr_interface_num,
                    rlevel_ctl.s.or_dis, rlevel_ctl.s.bitmask);

        rlevel_comp_offset = spd_rdimm
            ? custom_lmc_config->rlevel_comp_offset_rdimm
            : custom_lmc_config->rlevel_comp_offset_udimm;

        if ((s = lookup_env_parameter("ddr_rlevel_offset")) != NULL) {
            rlevel_ctl.s.offset   = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_rlevel_offset_en")) != NULL) {
            rlevel_ctl.s.offset_en   = strtoul(s, NULL, 0);
        }
        if ((s = lookup_env_parameter("ddr_rlevel_ctl")) != NULL) {
            rlevel_ctl.u   = strtoul(s, NULL, 0);
        }

        DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_CTL(ddr_interface_num), rlevel_ctl.u);

        if ((s = lookup_env_parameter("ddr%d_rlevel_debug_loops", ddr_interface_num)) != NULL) {
            rlevel_debug_loops = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_rtt_nom_auto")) != NULL) {
            ddr_rtt_nom_auto = !!strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_rlevel_average")) != NULL) {
            rlevel_avg_loops = strtoul(s, NULL, 0);
        }

        if ((s = lookup_env_parameter("ddr_rlevel_compute")) != NULL) {
            ddr_rlevel_compute = strtoul(s, NULL, 0);
        }

        ddr_print("RLEVEL_CTL                                    : 0x%016llx\n", rlevel_ctl.u);
        ddr_print("RLEVEL_OFFSET                                 : %6d\n", rlevel_ctl.s.offset);
        ddr_print("RLEVEL_OFFSET_EN                              : %6d\n", rlevel_ctl.s.offset_en);

        /* The purpose for the indexed table is to sort the settings
        ** by the ohm value to simplify the testing when incrementing
        ** through the settings.  (index => ohms) 1=120, 2=60, 3=40,
        ** 4=30, 5=20 */
        min_rtt_nom_idx = (custom_lmc_config->min_rtt_nom_idx == 0) ? 1 : custom_lmc_config->min_rtt_nom_idx;
        max_rtt_nom_idx = (custom_lmc_config->max_rtt_nom_idx == 0) ? 5 : custom_lmc_config->max_rtt_nom_idx;

        min_rodt_ctl = (custom_lmc_config->min_rodt_ctl == 0) ? 1 : custom_lmc_config->min_rodt_ctl;
        max_rodt_ctl = (custom_lmc_config->max_rodt_ctl == 0) ? 5 : custom_lmc_config->max_rodt_ctl;

        if ((s = lookup_env_parameter("ddr_min_rodt_ctl")) != NULL) {
            min_rodt_ctl = strtoul(s, NULL, 0);
        }
        if ((s = lookup_env_parameter("ddr_max_rodt_ctl")) != NULL) {
            max_rodt_ctl = strtoul(s, NULL, 0);
        }
        if ((s = lookup_env_parameter("ddr_min_rtt_nom_idx")) != NULL) {
            min_rtt_nom_idx = strtoul(s, NULL, 0);
        }
        if ((s = lookup_env_parameter("ddr_max_rtt_nom_idx")) != NULL) {
            max_rtt_nom_idx = strtoul(s, NULL, 0);
        }

#ifdef ENABLE_CUSTOM_RLEVEL_TABLE
        if (custom_lmc_config->rlevel_table != NULL) {
            char part_number[21];
            /* Check for hard-coded read-leveling settings */
            get_dimm_part_number(part_number, node, &dimm_config_table[0], 0, ddr_type);
            for (rankx = 0; rankx < dimm_count * 4;rankx++) {
                if (!(rank_mask & (1 << rankx)))
                    continue;

                lmc_rlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx));

                i = 0;
                while (custom_lmc_config->rlevel_table[i].part != NULL) {
                    debug_print("DIMM part number:\"%s\", SPD: \"%s\"\n", custom_lmc_config->rlevel_table[i].part, part_number);
                    if ((strcmp(part_number, custom_lmc_config->rlevel_table[i].part) == 0)
                        && (_abs(custom_lmc_config->rlevel_table[i].speed - 2*ddr_hertz/(1000*1000)) < 10 ))
                    {
                        ddr_print("Using hard-coded read leveling for DIMM part number: \"%s\"\n", part_number);
                        lmc_rlevel_rank.u = custom_lmc_config->rlevel_table[i].rlevel_rank[ddr_interface_num][rankx];
                        DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx), lmc_rlevel_rank.u);
                        lmc_rlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx));
                        display_RL(node, ddr_interface_num, lmc_rlevel_rank, rankx);
                        rlevel_debug_loops = 0; /* Disable h/w read-leveling */
                        break;
                    }
                    ++i;
                }
            }
        }
#endif /* ENABLE_CUSTOM_RLEVEL_TABLE */

        while(rlevel_debug_loops--) {
            /* Initialize the error scoreboard */
            memset(rlevel_scoreboard, 0, sizeof(rlevel_scoreboard));

            if ((s = lookup_env_parameter("ddr_rlevel_comp_offset")) != NULL) {
                rlevel_comp_offset = strtoul(s, NULL, 0);
            }

            disable_sequential_delay_check = custom_lmc_config->disable_sequential_delay_check;

            if ((s = lookup_env_parameter("ddr_disable_sequential_delay_check")) != NULL) {
                disable_sequential_delay_check = strtoul(s, NULL, 0);
            }

            maximum_adjacent_rlevel_delay_increment = custom_lmc_config->maximum_adjacent_rlevel_delay_increment;

            if ((s = lookup_env_parameter("ddr_maximum_adjacent_rlevel_delay_increment")) != NULL) {
                maximum_adjacent_rlevel_delay_increment = strtoul(s, NULL, 0);
            }

            lmc_comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));
            saved_ddr__ptune = lmc_comp_ctl2.s.ddr__ptune;
            saved_ddr__ntune = lmc_comp_ctl2.s.ddr__ntune;

            /* Disable dynamic compensation settings */
            if (rlevel_comp_offset != 0) {
                lmc_comp_ctl2.s.ptune = saved_ddr__ptune;
                lmc_comp_ctl2.s.ntune = saved_ddr__ntune;

                /* Round up the ptune calculation to bias the odd cases toward ptune */
                lmc_comp_ctl2.s.ptune += divide_roundup(rlevel_comp_offset, 2);
                lmc_comp_ctl2.s.ntune -= rlevel_comp_offset/2;

                lmc_control.u = BDK_CSR_READ(node, BDK_LMCX_CONTROL(ddr_interface_num));
                saved_int_zqcs_dis = lmc_control.s.int_zqcs_dis;
                lmc_control.s.int_zqcs_dis    = 1; /* Disable ZQCS while in bypass. */
                DRAM_CSR_WRITE(node, BDK_LMCX_CONTROL(ddr_interface_num), lmc_control.u);

                lmc_comp_ctl2.s.byp = 1; /* Enable bypass mode */
                DRAM_CSR_WRITE(node, BDK_LMCX_COMP_CTL2(ddr_interface_num), lmc_comp_ctl2.u);
                BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));
                lmc_comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num)); /* Read again */
                ddr_print("DDR__PTUNE/DDR__NTUNE                         : %d/%d\n",
                          lmc_comp_ctl2.s.ddr__ptune, lmc_comp_ctl2.s.ddr__ntune);
            }

            lmc_modereg_params1.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS1(ddr_interface_num));

            for (rtt_idx = min_rtt_nom_idx; rtt_idx <= max_rtt_nom_idx; ++rtt_idx) {
                rtt_nom = imp_values->rtt_nom_table[rtt_idx];

                /* When the read ODT mask is zero the dyn_rtt_nom_mask is
                   zero than RTT_NOM will not be changing during
                   read-leveling.  Since the value is fixed we only need
                   to test it once. */
                if (dyn_rtt_nom_mask == 0) {
                    print_nom_ohms = -1; // flag not to print NOM ohms
                    if (rtt_idx != min_rtt_nom_idx)
                        continue;
                } else {
                    if (dyn_rtt_nom_mask & 1) lmc_modereg_params1.s.rtt_nom_00 = rtt_nom;
                    if (dyn_rtt_nom_mask & 2) lmc_modereg_params1.s.rtt_nom_01 = rtt_nom;
                    if (dyn_rtt_nom_mask & 4) lmc_modereg_params1.s.rtt_nom_10 = rtt_nom;
                    if (dyn_rtt_nom_mask & 8) lmc_modereg_params1.s.rtt_nom_11 = rtt_nom;
                    // FIXME? rank 0 ohms always for the printout?
                    print_nom_ohms = imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_00];
                }

                DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS1(ddr_interface_num), lmc_modereg_params1.u);
                VB_PRT(VBL_TME, "\n");
                VB_PRT(VBL_TME, "RTT_NOM     %3d, %3d, %3d, %3d ohms           :  %x,%x,%x,%x\n",
                          imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_11],
                          imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_10],
                          imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_01],
                          imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_00],
                          lmc_modereg_params1.s.rtt_nom_11,
                          lmc_modereg_params1.s.rtt_nom_10,
                          lmc_modereg_params1.s.rtt_nom_01,
                          lmc_modereg_params1.s.rtt_nom_00);

                perform_ddr_init_sequence(node, rank_mask, ddr_interface_num);

                // Try RANK outside RODT to rearrange the output...
                for (rankx = 0; rankx < dimm_count * 4; rankx++) {
                    int byte_idx;
                    rlevel_byte_data_t rlevel_byte[9];
                    int average_loops;
                    int rlevel_rank_errors, rlevel_bitmask_errors, rlevel_nonseq_errors;
                    rlevel_bitmask_t rlevel_bitmask[9];
#if PICK_BEST_RANK_SCORE_NOT_AVG
                    int rlevel_best_rank_score;
#endif

                    if (!(rank_mask & (1 << rankx)))
                        continue;

                    for (rodt_ctl = max_rodt_ctl; rodt_ctl >= min_rodt_ctl; --rodt_ctl) {
#if PICK_BEST_RANK_SCORE_NOT_AVG
                        rlevel_best_rank_score = DEFAULT_BEST_RANK_SCORE;
#endif
                        rlevel_rodt_errors = 0;
                        lmc_comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));
                        lmc_comp_ctl2.s.rodt_ctl = rodt_ctl;
                        DRAM_CSR_WRITE(node, BDK_LMCX_COMP_CTL2(ddr_interface_num), lmc_comp_ctl2.u);
                        lmc_comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));
                        bdk_wait_usec(1); /* Give it a little time to take affect */
                        VB_PRT(VBL_DEV, "Read ODT_CTL                                  : 0x%x (%d ohms)\n",
                               lmc_comp_ctl2.s.rodt_ctl, imp_values->rodt_ohms[lmc_comp_ctl2.s.rodt_ctl]);

                        memset(rlevel_byte, 0, sizeof(rlevel_byte));

                        for (average_loops = 0; average_loops < rlevel_avg_loops; average_loops++) {
                            rlevel_bitmask_errors = 0;

                            if (! (rlevel_separate_ab && spd_rdimm && (ddr_type == DDR4_DRAM))) {
                                /* Clear read-level delays */
                                DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx), 0);

                                /* read-leveling */
                                perform_octeon3_ddr3_sequence(node, 1 << rankx, ddr_interface_num, 1);

                                if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx),
                                                           status, ==, 3, 1000000))
                                {
                                    error_print("ERROR: Timeout waiting for RLEVEL\n");
                                }
                            }

                            lmc_rlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx));

                            { // start bitmask interpretation block
                                int redoing_nonseq_errs = 0;

                                memset(rlevel_bitmask, 0, sizeof(rlevel_bitmask));

                                if (rlevel_separate_ab && spd_rdimm && (ddr_type == DDR4_DRAM)) {
                                    bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank_aside;
                                    bdk_lmcx_modereg_params0_t lmc_modereg_params0;

                                    /* A-side */
                                    lmc_modereg_params0.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num));
                                    lmc_modereg_params0.s.mprloc = 0; /* MPR Page 0 Location 0 */
                                    DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num), lmc_modereg_params0.u);

                                    /* Clear read-level delays */
                                    DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx), 0);

                                    perform_octeon3_ddr3_sequence(node, 1 << rankx, ddr_interface_num, 1); /* read-leveling */

                                    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx),
                                                               status, ==, 3, 1000000))
                                        {
                                            error_print("ERROR: Timeout waiting for RLEVEL\n");

                                        }
                                    lmc_rlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx));

                                    lmc_rlevel_rank_aside.u = lmc_rlevel_rank.u;

                                    rlevel_bitmask[0].bm = octeon_read_lmcx_ddr3_rlevel_dbg(node, ddr_interface_num, 0);
                                    rlevel_bitmask[1].bm = octeon_read_lmcx_ddr3_rlevel_dbg(node, ddr_interface_num, 1);
                                    rlevel_bitmask[2].bm = octeon_read_lmcx_ddr3_rlevel_dbg(node, ddr_interface_num, 2);
                                    rlevel_bitmask[3].bm = octeon_read_lmcx_ddr3_rlevel_dbg(node, ddr_interface_num, 3);
                                    rlevel_bitmask[8].bm = octeon_read_lmcx_ddr3_rlevel_dbg(node, ddr_interface_num, 8);
                                    /* A-side complete */


                                    /* B-side */
                                    lmc_modereg_params0.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num));
                                    lmc_modereg_params0.s.mprloc = 3; /* MPR Page 0 Location 3 */
                                    DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num), lmc_modereg_params0.u);

                                    /* Clear read-level delays */
                                    DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx), 0);

                                    perform_octeon3_ddr3_sequence(node, 1 << rankx, ddr_interface_num, 1); /* read-leveling */

                                    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx),
                                                               status, ==, 3, 1000000))
                                        {
                                            error_print("ERROR: Timeout waiting for RLEVEL\n");
                                        }
                                    lmc_rlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx));

                                    rlevel_bitmask[4].bm = octeon_read_lmcx_ddr3_rlevel_dbg(node, ddr_interface_num, 4);
                                    rlevel_bitmask[5].bm = octeon_read_lmcx_ddr3_rlevel_dbg(node, ddr_interface_num, 5);
                                    rlevel_bitmask[6].bm = octeon_read_lmcx_ddr3_rlevel_dbg(node, ddr_interface_num, 6);
                                    rlevel_bitmask[7].bm = octeon_read_lmcx_ddr3_rlevel_dbg(node, ddr_interface_num, 7);
                                    /* B-side complete */


                                    update_rlevel_rank_struct(&lmc_rlevel_rank, 0, lmc_rlevel_rank_aside.cn83xx.byte0);
                                    update_rlevel_rank_struct(&lmc_rlevel_rank, 1, lmc_rlevel_rank_aside.cn83xx.byte1);
                                    update_rlevel_rank_struct(&lmc_rlevel_rank, 2, lmc_rlevel_rank_aside.cn83xx.byte2);
                                    update_rlevel_rank_struct(&lmc_rlevel_rank, 3, lmc_rlevel_rank_aside.cn83xx.byte3);
                                    update_rlevel_rank_struct(&lmc_rlevel_rank, 8, lmc_rlevel_rank_aside.cn83xx.byte8); /* ECC A-side */

                                    lmc_modereg_params0.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num));
                                    lmc_modereg_params0.s.mprloc = 0; /* MPR Page 0 Location 0 */
                                    DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num), lmc_modereg_params0.u);

                                } /* if (rlevel_separate_ab && spd_rdimm && (ddr_type == DDR4_DRAM)) */

                                /*
                                 * Evaluate the quality of the read-leveling delays from the bitmasks.
                                 * Also save off a software computed read-leveling mask that may be
                                 * used later to qualify the delay results from Octeon.
                                 */
                                for (byte_idx = 0; byte_idx < (8+ecc_ena); ++byte_idx) {
                                    int bmerr;
                                    if (!(ddr_interface_bytemask&(1<<byte_idx)))
                                        continue;
                                    if (! (rlevel_separate_ab && spd_rdimm && (ddr_type == DDR4_DRAM))) {
                                        rlevel_bitmask[byte_idx].bm =
                                            octeon_read_lmcx_ddr3_rlevel_dbg(node, ddr_interface_num, byte_idx);
                                    }
                                    bmerr = validate_ddr3_rlevel_bitmask(&rlevel_bitmask[byte_idx], ddr_type);
                                    rlevel_bitmask[byte_idx].errs = bmerr;
                                    rlevel_bitmask_errors += bmerr;
#if PERFECT_BITMASK_COUNTING
                                    if ((ddr_type == DDR4_DRAM) && !bmerr) { // count only the "perfect" bitmasks
                                        // FIXME: could optimize this a bit?
                                        int delay = get_rlevel_rank_struct(&lmc_rlevel_rank, byte_idx);
                                        rank_perfect_counts[rankx].count[byte_idx][delay] += 1;
                                        rank_perfect_counts[rankx].total[byte_idx] += 1;
                                    }
#endif /* PERFECT_BITMASK_COUNTING */
                                }

                                /* Set delays for unused bytes to match byte 0. */
                                for (byte_idx = 0; byte_idx < 9; ++byte_idx) {
                                    if (ddr_interface_bytemask & (1 << byte_idx))
                                        continue;
                                    update_rlevel_rank_struct(&lmc_rlevel_rank, byte_idx, lmc_rlevel_rank.cn83xx.byte0);
                                }

                                /* Save a copy of the byte delays in physical
                                   order for sequential evaluation. */
                                unpack_rlevel_settings(ddr_interface_bytemask, ecc_ena, rlevel_byte, lmc_rlevel_rank);
                            redo_nonseq_errs:

                                rlevel_nonseq_errors  = 0;

                                if (! disable_sequential_delay_check) {
                                    if ((ddr_interface_bytemask & 0xff) == 0xff) {
                                        /* Evaluate delay sequence across the whole range of bytes for standard dimms. */
                                        if ((spd_dimm_type == 1) || (spd_dimm_type == 5)) { /* 1=RDIMM, 5=Mini-RDIMM */
                                            int register_adjacent_delay = _abs(rlevel_byte[4].delay - rlevel_byte[5].delay);
                                            /* Registered dimm topology routes from the center. */
                                            rlevel_nonseq_errors += nonsequential_delays(rlevel_byte, 0, 3+ecc_ena,
                                                                                       maximum_adjacent_rlevel_delay_increment);
                                            rlevel_nonseq_errors += nonsequential_delays(rlevel_byte, 5, 7+ecc_ena,
                                                                                       maximum_adjacent_rlevel_delay_increment);
                                            // byte 5 sqerrs never gets cleared for RDIMMs
                                            rlevel_byte[5].sqerrs = 0;
                                            if (register_adjacent_delay > 1) {
                                                /* Assess proximity of bytes on opposite sides of register */
                                                rlevel_nonseq_errors += (register_adjacent_delay-1) * RLEVEL_ADJACENT_DELAY_ERROR;
                                                // update byte 5 error
                                                rlevel_byte[5].sqerrs += (register_adjacent_delay-1) * RLEVEL_ADJACENT_DELAY_ERROR;
                                            }
                                        }
                                        if ((spd_dimm_type == 2) || (spd_dimm_type == 6)) { /* 2=UDIMM, 6=Mini-UDIMM */
                                            /* Unbuffered dimm topology routes from end to end. */
                                            rlevel_nonseq_errors += nonsequential_delays(rlevel_byte, 0, 7+ecc_ena,
                                                                                       maximum_adjacent_rlevel_delay_increment);
                                        }
                                    } else {
                                        rlevel_nonseq_errors += nonsequential_delays(rlevel_byte, 0, 3+ecc_ena,
                                                                                   maximum_adjacent_rlevel_delay_increment);
                                    }
                                } /* if (! disable_sequential_delay_check) */

#if 0
                                // FIXME FIXME: disabled for now, it was too much...

                                // Calculate total errors for the rank:
                                // we do NOT add nonsequential errors if mini-[RU]DIMM or x16;
                                // mini-DIMMs and x16 devices have unusual sequence geometries.
                                // Make the final scores for them depend only on the bitmasks...
                                rlevel_rank_errors = rlevel_bitmask_errors;
                                if ((spd_dimm_type != 5) &&
                                    (spd_dimm_type != 6) &&
                                    (dram_width != 16))
                                {
                                    rlevel_rank_errors += rlevel_nonseq_errors;
                                }
#else
                                rlevel_rank_errors = rlevel_bitmask_errors + rlevel_nonseq_errors;
#endif

                                // print original sample here only if we are not really averaging or picking best
                                // also do not print if we were redoing the NONSEQ score for using COMPUTED
                                if (!redoing_nonseq_errs && ((rlevel_avg_loops < 2) || dram_is_verbose(VBL_DEV2))) {
                                    display_RL_BM(node, ddr_interface_num, rankx, rlevel_bitmask, ecc_ena);
                                    display_RL_BM_scores(node, ddr_interface_num, rankx, rlevel_bitmask, ecc_ena);
                                    display_RL_SEQ_scores(node, ddr_interface_num, rankx, rlevel_byte, ecc_ena);
                                    display_RL_with_score(node, ddr_interface_num, lmc_rlevel_rank, rankx, rlevel_rank_errors);
                                }

                                if (ddr_rlevel_compute) {
                                    if (!redoing_nonseq_errs) {
                                        /* Recompute the delays based on the bitmask */
                                        for (byte_idx = 0; byte_idx < (8+ecc_ena); ++byte_idx) {
                                            if (!(ddr_interface_bytemask & (1 << byte_idx)))
                                                continue;
                                            update_rlevel_rank_struct(&lmc_rlevel_rank, byte_idx,
                                                                      compute_ddr3_rlevel_delay(rlevel_bitmask[byte_idx].mstart,
                                                                                                rlevel_bitmask[byte_idx].width,
                                                                                                rlevel_ctl));
                                        }

                                        /* Override the copy of byte delays with the computed results. */
                                        unpack_rlevel_settings(ddr_interface_bytemask, ecc_ena, rlevel_byte, lmc_rlevel_rank);

                                        redoing_nonseq_errs = 1;
                                        goto redo_nonseq_errs;

                                    } else {
                                        /* now print this if already printed the original sample */
                                        if ((rlevel_avg_loops < 2) || dram_is_verbose(VBL_DEV2)) {
                                            display_RL_with_computed(node, ddr_interface_num,
                                                                     lmc_rlevel_rank, rankx,
                                                                     rlevel_rank_errors);
                                        }
                                    }
                                } /* if (ddr_rlevel_compute) */

                            } // end bitmask interpretation block

#if PICK_BEST_RANK_SCORE_NOT_AVG

                            // if it is a better (lower) score, then  keep it
                            if (rlevel_rank_errors < rlevel_best_rank_score) {
                                rlevel_best_rank_score = rlevel_rank_errors;

                                // save the new best delays and best errors
                                for (byte_idx = 0; byte_idx < 9; ++byte_idx) {
                                    rlevel_byte[byte_idx].best = rlevel_byte[byte_idx].delay;
                                    rlevel_byte[byte_idx].bestsq = rlevel_byte[byte_idx].sqerrs;
                                    // save bitmasks and their scores as well
                                    // xlate UNPACKED index to PACKED index to get from rlevel_bitmask
                                    rlevel_byte[byte_idx].bm     = rlevel_bitmask[XUP(byte_idx, !!ecc_ena)].bm;
                                    rlevel_byte[byte_idx].bmerrs = rlevel_bitmask[XUP(byte_idx, !!ecc_ena)].errs;
                                }
                            }
#else /* PICK_BEST_RANK_SCORE_NOT_AVG */

                            /* Accumulate the total score across averaging loops for this setting */
                            debug_print("rlevel_scoreboard[rtt_nom=%d][rodt_ctl=%d][rankx=%d].score: %d [%d]\n",
                                      rtt_nom, rodt_ctl, rankx, rlevel_rank_errors, average_loops);
                            rlevel_scoreboard[rtt_nom][rodt_ctl][rankx].score += rlevel_rank_errors;

                            /* Accumulate the delay totals and loop counts
                               necessary to compute average delay results */
                            for (byte_idx = 0; byte_idx < 9; ++byte_idx) {
                                if (rlevel_byte[byte_idx].delay != 0) { /* Don't include delay=0 in the average */
                                    ++rlevel_byte[byte_idx].loop_count;
                                    rlevel_byte[byte_idx].loop_total += rlevel_byte[byte_idx].delay;
                                }
                            } /* for (byte_idx = 0; byte_idx < 9; ++byte_idx) */
#endif /* PICK_BEST_RANK_SCORE_NOT_AVG */

                            rlevel_rodt_errors += rlevel_rank_errors;

                        } /* for (average_loops = 0; average_loops < rlevel_avg_loops; average_loops++) */

#if PICK_BEST_RANK_SCORE_NOT_AVG

                        /* We recorded the best score across the averaging loops */
                        rlevel_scoreboard[rtt_nom][rodt_ctl][rankx].score = rlevel_best_rank_score;

                        /* Restore the delays from the best fields that go with the best score */
                        for (byte_idx = 0; byte_idx < 9; ++byte_idx) {
                            rlevel_byte[byte_idx].delay = rlevel_byte[byte_idx].best;
                            rlevel_byte[byte_idx].sqerrs = rlevel_byte[byte_idx].bestsq;
                        }
#else /* PICK_BEST_RANK_SCORE_NOT_AVG */

                        /* Compute the average score across averaging loops */
                        rlevel_scoreboard[rtt_nom][rodt_ctl][rankx].score =
                            divide_nint(rlevel_scoreboard[rtt_nom][rodt_ctl][rankx].score, rlevel_avg_loops);

                        /* Compute the average delay results */
                        for (byte_idx=0; byte_idx < 9; ++byte_idx) {
                            if (rlevel_byte[byte_idx].loop_count == 0)
                                rlevel_byte[byte_idx].loop_count = 1;
                            rlevel_byte[byte_idx].delay = divide_nint(rlevel_byte[byte_idx].loop_total,
                                                                      rlevel_byte[byte_idx].loop_count);
                        }
#endif /* PICK_BEST_RANK_SCORE_NOT_AVG */

                        lmc_rlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx));

                        pack_rlevel_settings(ddr_interface_bytemask, ecc_ena, rlevel_byte, &lmc_rlevel_rank);

                        if (rlevel_avg_loops > 1) {
#if PICK_BEST_RANK_SCORE_NOT_AVG
                            // restore the "best" bitmasks and their scores for printing
                            for (byte_idx = 0; byte_idx < 9; ++byte_idx) {
                                if ((ddr_interface_bytemask & (1 << byte_idx)) == 0)
                                    continue;
                                // xlate PACKED index to UNPACKED index to get from rlevel_byte
                                rlevel_bitmask[byte_idx].bm   = rlevel_byte[XPU(byte_idx, !!ecc_ena)].bm;
                                rlevel_bitmask[byte_idx].errs = rlevel_byte[XPU(byte_idx, !!ecc_ena)].bmerrs;
                            }
                            // print bitmasks/scores here only for DEV // FIXME? lower VBL?
                            if (dram_is_verbose(VBL_DEV)) {
                                display_RL_BM(node, ddr_interface_num, rankx, rlevel_bitmask, ecc_ena);
                                display_RL_BM_scores(node, ddr_interface_num, rankx, rlevel_bitmask, ecc_ena);
                                display_RL_SEQ_scores(node, ddr_interface_num, rankx, rlevel_byte, ecc_ena);
                            }

                            display_RL_with_RODT(node, ddr_interface_num, lmc_rlevel_rank, rankx,
                                                 rlevel_scoreboard[rtt_nom][rodt_ctl][rankx].score,
                                                 print_nom_ohms, imp_values->rodt_ohms[rodt_ctl],
                                                 WITH_RODT_BESTSCORE);

#else /* PICK_BEST_RANK_SCORE_NOT_AVG */
                            display_RL_with_average(node, ddr_interface_num, lmc_rlevel_rank, rankx,
                                                    rlevel_scoreboard[rtt_nom][rodt_ctl][rankx].score);
#endif /* PICK_BEST_RANK_SCORE_NOT_AVG */

                        } /* if (rlevel_avg_loops > 1) */

                        rlevel_scoreboard[rtt_nom][rodt_ctl][rankx].setting = lmc_rlevel_rank.u;

                    } /* for (rodt_ctl = max_rodt_ctl; rodt_ctl >= min_rodt_ctl; --rodt_ctl) */
                } /* for (rankx = 0; rankx < dimm_count*4; rankx++) */
            } /*  for (rtt_idx=min_rtt_nom_idx; rtt_idx<max_rtt_nom_idx; ++rtt_idx) */


            /* Re-enable dynamic compensation settings. */
            if (rlevel_comp_offset != 0) {
                lmc_comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));

                lmc_comp_ctl2.s.ptune = 0;
                lmc_comp_ctl2.s.ntune = 0;
                lmc_comp_ctl2.s.byp = 0; /* Disable bypass mode */
                DRAM_CSR_WRITE(node, BDK_LMCX_COMP_CTL2(ddr_interface_num), lmc_comp_ctl2.u);
                BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num)); /* Read once */

                lmc_comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num)); /* Read again */
                ddr_print("DDR__PTUNE/DDR__NTUNE                         : %d/%d\n",
                          lmc_comp_ctl2.s.ddr__ptune, lmc_comp_ctl2.s.ddr__ntune);

                lmc_control.u = BDK_CSR_READ(node, BDK_LMCX_CONTROL(ddr_interface_num));
                lmc_control.s.int_zqcs_dis    = saved_int_zqcs_dis; /* Restore original setting */
                DRAM_CSR_WRITE(node, BDK_LMCX_CONTROL(ddr_interface_num), lmc_control.u);

            }


            {
                int override_compensation = 0;
                if ((s = lookup_env_parameter("ddr__ptune")) != NULL) {
                    saved_ddr__ptune = strtoul(s, NULL, 0);
                    override_compensation = 1;
                }
                if ((s = lookup_env_parameter("ddr__ntune")) != NULL) {
                    saved_ddr__ntune = strtoul(s, NULL, 0);
                    override_compensation = 1;
                }
                if (override_compensation) {
                    lmc_comp_ctl2.s.ptune = saved_ddr__ptune;
                    lmc_comp_ctl2.s.ntune = saved_ddr__ntune;

                    lmc_control.u = BDK_CSR_READ(node, BDK_LMCX_CONTROL(ddr_interface_num));
                    saved_int_zqcs_dis = lmc_control.s.int_zqcs_dis;
                    lmc_control.s.int_zqcs_dis    = 1; /* Disable ZQCS while in bypass. */
                    DRAM_CSR_WRITE(node, BDK_LMCX_CONTROL(ddr_interface_num), lmc_control.u);

                    lmc_comp_ctl2.s.byp = 1; /* Enable bypass mode */
                    DRAM_CSR_WRITE(node, BDK_LMCX_COMP_CTL2(ddr_interface_num), lmc_comp_ctl2.u);
                    lmc_comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num)); /* Read again */

                    ddr_print("DDR__PTUNE/DDR__NTUNE                         : %d/%d\n",
                              lmc_comp_ctl2.s.ptune, lmc_comp_ctl2.s.ntune);
                }
            }
            { /* Evaluation block */
                int      best_rodt_score = DEFAULT_BEST_RANK_SCORE; /* Start with an arbitrarily high score */
                int      auto_rodt_ctl = 0;
                int      auto_rtt_nom  = 0;
                int      rodt_score;
                int      rodt_row_skip_mask = 0;

                // just add specific RODT rows to the skip mask for DDR4 at this time...
                if (ddr_type == DDR4_DRAM) {
                    rodt_row_skip_mask |= (1 << ddr4_rodt_ctl_34_ohm); // skip RODT row 34 ohms for all DDR4 types
                    rodt_row_skip_mask |= (1 << ddr4_rodt_ctl_40_ohm); // skip RODT row 40 ohms for all DDR4 types
#if ADD_48_OHM_SKIP
                    rodt_row_skip_mask |= (1 << ddr4_rodt_ctl_48_ohm); // skip RODT row 48 ohms for all DDR4 types
#endif /* ADD_48OHM_SKIP */
#if NOSKIP_40_48_OHM
                    // For now, do not skip RODT row 40 or 48 ohm when ddr_hertz is above 1075 MHz
                    if (ddr_hertz > 1075000000) {
                        rodt_row_skip_mask &= ~(1 << ddr4_rodt_ctl_40_ohm); // noskip RODT row 40 ohms
                        rodt_row_skip_mask &= ~(1 << ddr4_rodt_ctl_48_ohm); // noskip RODT row 48 ohms
                    }
#endif /* NOSKIP_40_48_OHM */
#if NOSKIP_48_STACKED
                    // For now, do not skip RODT row 48 ohm for 2Rx4 stacked die DIMMs
                    if ((is_stacked_die) && (num_ranks == 2) && (dram_width == 4)) {
                        rodt_row_skip_mask &= ~(1 << ddr4_rodt_ctl_48_ohm); // noskip RODT row 48 ohms
                    }
#endif /* NOSKIP_48_STACKED */
#if NOSKIP_FOR_MINI
                    // for now, leave all rows eligible when we have mini-DIMMs...
                    if ((spd_dimm_type == 5) || (spd_dimm_type == 6)) {
                        rodt_row_skip_mask = 0;
                    }
#endif /* NOSKIP_FOR_MINI */
#if NOSKIP_FOR_2S_1R
                    // for now, leave all rows eligible when we have a 2-slot 1-rank config
                    if ((dimm_count == 2) && (num_ranks == 1)) {
                        rodt_row_skip_mask = 0;
                    }
#endif /* NOSKIP_FOR_2S_1R */
                }

                VB_PRT(VBL_DEV, "Evaluating Read-Leveling Scoreboard for AUTO settings.\n");
                for (rtt_idx=min_rtt_nom_idx; rtt_idx<=max_rtt_nom_idx; ++rtt_idx) {
                    rtt_nom = imp_values->rtt_nom_table[rtt_idx];

                    /* When the read ODT mask is zero the dyn_rtt_nom_mask is
                       zero than RTT_NOM will not be changing during
                       read-leveling.  Since the value is fixed we only need
                       to test it once. */
                    if ((dyn_rtt_nom_mask == 0) && (rtt_idx != min_rtt_nom_idx))
                        continue;

                    for (rodt_ctl=max_rodt_ctl; rodt_ctl>=min_rodt_ctl; --rodt_ctl) {
                        rodt_score = 0;
                        for (rankx = 0; rankx < dimm_count * 4;rankx++) {
                            if (!(rank_mask & (1 << rankx)))
                                continue;
                            debug_print("rlevel_scoreboard[rtt_nom=%d][rodt_ctl=%d][rankx=%d].score:%d\n",
                                        rtt_nom, rodt_ctl, rankx, rlevel_scoreboard[rtt_nom][rodt_ctl][rankx].score);
                            rodt_score += rlevel_scoreboard[rtt_nom][rodt_ctl][rankx].score;
                        }
                        // FIXME: do we need to skip RODT rows here, like we do below in the by-RANK settings?

                        /* When using automatic ODT settings use the ODT
                           settings associated with the best score for
                           all of the tested ODT combinations. */

                        if ((rodt_score < best_rodt_score) || // always take lower score, OR
                            ((rodt_score == best_rodt_score) && // take same score if RODT ohms are higher
                             (imp_values->rodt_ohms[rodt_ctl] > imp_values->rodt_ohms[auto_rodt_ctl])))
                            {
                                debug_print("AUTO: new best score for rodt:%d (%3d), new score:%d, previous score:%d\n",
                                            rodt_ctl, imp_values->rodt_ohms[rodt_ctl], rodt_score, best_rodt_score);
                                best_rodt_score = rodt_score;
                                auto_rodt_ctl   = rodt_ctl;
                                auto_rtt_nom    = rtt_nom;
                            }
                    } /* for (rodt_ctl=max_rodt_ctl; rodt_ctl>=min_rodt_ctl; --rodt_ctl) */
                } /* for (rtt_idx=min_rtt_nom_idx; rtt_idx<=max_rtt_nom_idx; ++rtt_idx) */

                lmc_modereg_params1.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS1(ddr_interface_num));

                if (ddr_rtt_nom_auto) {
                    /* Store the automatically set RTT_NOM value */
                    if (dyn_rtt_nom_mask & 1) lmc_modereg_params1.s.rtt_nom_00 = auto_rtt_nom;
                    if (dyn_rtt_nom_mask & 2) lmc_modereg_params1.s.rtt_nom_01 = auto_rtt_nom;
                    if (dyn_rtt_nom_mask & 4) lmc_modereg_params1.s.rtt_nom_10 = auto_rtt_nom;
                    if (dyn_rtt_nom_mask & 8) lmc_modereg_params1.s.rtt_nom_11 = auto_rtt_nom;
                } else {
                    /* restore the manual settings to the register */
                    lmc_modereg_params1.s.rtt_nom_00 = default_rtt_nom[0];
                    lmc_modereg_params1.s.rtt_nom_01 = default_rtt_nom[1];
                    lmc_modereg_params1.s.rtt_nom_10 = default_rtt_nom[2];
                    lmc_modereg_params1.s.rtt_nom_11 = default_rtt_nom[3];
                }

                DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS1(ddr_interface_num), lmc_modereg_params1.u);
                VB_PRT(VBL_DEV, "RTT_NOM     %3d, %3d, %3d, %3d ohms           :  %x,%x,%x,%x\n",
                        imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_11],
                        imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_10],
                        imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_01],
                        imp_values->rtt_nom_ohms[lmc_modereg_params1.s.rtt_nom_00],
                        lmc_modereg_params1.s.rtt_nom_11,
                        lmc_modereg_params1.s.rtt_nom_10,
                        lmc_modereg_params1.s.rtt_nom_01,
                        lmc_modereg_params1.s.rtt_nom_00);

                VB_PRT(VBL_DEV, "RTT_WR      %3d, %3d, %3d, %3d ohms           :  %x,%x,%x,%x\n",
                       imp_values->rtt_wr_ohms[EXTR_WR(lmc_modereg_params1.u, 3)],
                       imp_values->rtt_wr_ohms[EXTR_WR(lmc_modereg_params1.u, 2)],
                       imp_values->rtt_wr_ohms[EXTR_WR(lmc_modereg_params1.u, 1)],
                       imp_values->rtt_wr_ohms[EXTR_WR(lmc_modereg_params1.u, 0)],
                       EXTR_WR(lmc_modereg_params1.u, 3),
                       EXTR_WR(lmc_modereg_params1.u, 2),
                       EXTR_WR(lmc_modereg_params1.u, 1),
                       EXTR_WR(lmc_modereg_params1.u, 0));

                VB_PRT(VBL_DEV, "DIC         %3d, %3d, %3d, %3d ohms           :  %x,%x,%x,%x\n",
                        imp_values->dic_ohms[lmc_modereg_params1.s.dic_11],
                        imp_values->dic_ohms[lmc_modereg_params1.s.dic_10],
                        imp_values->dic_ohms[lmc_modereg_params1.s.dic_01],
                        imp_values->dic_ohms[lmc_modereg_params1.s.dic_00],
                        lmc_modereg_params1.s.dic_11,
                        lmc_modereg_params1.s.dic_10,
                        lmc_modereg_params1.s.dic_01,
                        lmc_modereg_params1.s.dic_00);

                if (ddr_type == DDR4_DRAM) {
                    bdk_lmcx_modereg_params2_t lmc_modereg_params2;
                    /*
                     * We must read the CSR, and not depend on odt_config[odt_idx].odt_mask2,
                     * since we could have overridden values with envvars.
                     * NOTE: this corrects the printout, since the CSR is not written with the old values...
                     */
                    lmc_modereg_params2.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS2(ddr_interface_num));

                    VB_PRT(VBL_DEV, "RTT_PARK    %3d, %3d, %3d, %3d ohms           :  %x,%x,%x,%x\n",
                              imp_values->rtt_nom_ohms[lmc_modereg_params2.s.rtt_park_11],
                              imp_values->rtt_nom_ohms[lmc_modereg_params2.s.rtt_park_10],
                              imp_values->rtt_nom_ohms[lmc_modereg_params2.s.rtt_park_01],
                              imp_values->rtt_nom_ohms[lmc_modereg_params2.s.rtt_park_00],
                              lmc_modereg_params2.s.rtt_park_11,
                              lmc_modereg_params2.s.rtt_park_10,
                              lmc_modereg_params2.s.rtt_park_01,
                              lmc_modereg_params2.s.rtt_park_00);

                    VB_PRT(VBL_DEV, "%-45s :  0x%x,0x%x,0x%x,0x%x\n", "VREF_RANGE",
                              lmc_modereg_params2.s.vref_range_11,
                              lmc_modereg_params2.s.vref_range_10,
                              lmc_modereg_params2.s.vref_range_01,
                              lmc_modereg_params2.s.vref_range_00);

                    VB_PRT(VBL_DEV, "%-45s :  0x%x,0x%x,0x%x,0x%x\n", "VREF_VALUE",
                              lmc_modereg_params2.s.vref_value_11,
                              lmc_modereg_params2.s.vref_value_10,
                              lmc_modereg_params2.s.vref_value_01,
                              lmc_modereg_params2.s.vref_value_00);
                }

                lmc_comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));
                if (ddr_rodt_ctl_auto)
                    lmc_comp_ctl2.s.rodt_ctl = auto_rodt_ctl;
                else
                    lmc_comp_ctl2.s.rodt_ctl = default_rodt_ctl; // back to the original setting
                DRAM_CSR_WRITE(node, BDK_LMCX_COMP_CTL2(ddr_interface_num), lmc_comp_ctl2.u);
                lmc_comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(ddr_interface_num));
                VB_PRT(VBL_DEV, "Read ODT_CTL                                  : 0x%x (%d ohms)\n",
                          lmc_comp_ctl2.s.rodt_ctl, imp_values->rodt_ohms[lmc_comp_ctl2.s.rodt_ctl]);

                ////////////////// this is the start of the RANK MAJOR LOOP

                for (rankx = 0; rankx < dimm_count * 4; rankx++) {
                    int best_rank_score = DEFAULT_BEST_RANK_SCORE; /* Start with an arbitrarily high score */
                    int best_rank_rtt_nom = 0;
                    //int best_rank_nom_ohms = 0;
                    int best_rank_ctl = 0;
                    int best_rank_ohms = 0;
                    int best_rankx = 0;

                    if (!(rank_mask & (1 << rankx)))
                        continue;

                    /* Use the delays associated with the best score for each individual rank */
                    VB_PRT(VBL_TME, "Evaluating Read-Leveling Scoreboard for Rank %d settings.\n", rankx);

                    // some of the rank-related loops below need to operate only on the ranks of a single DIMM,
                    // so create a mask for their use here
                    int dimm_rank_mask;
                    if (num_ranks == 4)
                        dimm_rank_mask = rank_mask; // should be 1111
                    else {
                        dimm_rank_mask = rank_mask & 3; // should be 01 or 11
                        if (rankx >= 2)
                            dimm_rank_mask <<= 2; // doing a rank on the second DIMM, should be 0100 or 1100
                    }
                    debug_print("DIMM rank mask: 0x%x, rank mask: 0x%x, rankx: %d\n", dimm_rank_mask, rank_mask, rankx);

                    ////////////////// this is the start of the BEST ROW SCORE LOOP

                    for (rtt_idx = min_rtt_nom_idx; rtt_idx <= max_rtt_nom_idx; ++rtt_idx) {
                        //int rtt_nom_ohms;
                        rtt_nom = imp_values->rtt_nom_table[rtt_idx];
                        //rtt_nom_ohms = imp_values->rtt_nom_ohms[rtt_nom];

                        /* When the read ODT mask is zero the dyn_rtt_nom_mask is
                           zero than RTT_NOM will not be changing during
                           read-leveling.  Since the value is fixed we only need
                           to test it once. */
                        if ((dyn_rtt_nom_mask == 0) && (rtt_idx != min_rtt_nom_idx))
                            continue;

                        debug_print("N%d.LMC%d.R%d: starting RTT_NOM %d (%d)\n",
                                    node, ddr_interface_num, rankx, rtt_nom, rtt_nom_ohms);

                        for (rodt_ctl = max_rodt_ctl; rodt_ctl >= min_rodt_ctl; --rodt_ctl) {
                            int next_ohms = imp_values->rodt_ohms[rodt_ctl];

                            // skip RODT rows in mask, but *NOT* rows with too high a score;
                            // we will not use the skipped ones for printing or evaluating, but
                            // we need to allow all the non-skipped ones to be candidates for "best"
                            if (((1 << rodt_ctl) & rodt_row_skip_mask) != 0) {
                                debug_print("N%d.LMC%d.R%d: SKIPPING rodt:%d (%d) with rank_score:%d\n",
                                            node, ddr_interface_num, rankx, rodt_ctl, next_ohms, next_score);
                                continue;
                            }
                            for (int orankx = 0; orankx < dimm_count * 4; orankx++) { // this is ROFFIX-0528
                                if (!(dimm_rank_mask & (1 << orankx))) // stay on the same DIMM
                                    continue;

                                int next_score = rlevel_scoreboard[rtt_nom][rodt_ctl][orankx].score;

                                if (next_score > best_rank_score) // always skip a higher score
                                    continue;
                                if (next_score == best_rank_score) { // if scores are equal
                                    if (next_ohms < best_rank_ohms) // always skip lower ohms
                                        continue;
                                    if (next_ohms == best_rank_ohms) { // if same ohms
                                        if (orankx != rankx) // always skip the other rank(s)
                                            continue;
                                    }
                                    // else next_ohms are greater, always choose it
                                }
                                // else next_score is less than current best, so always choose it
                                VB_PRT(VBL_DEV2, "N%d.LMC%d.R%d: new best score: rank %d, rodt %d(%3d), new best %d, previous best %d(%d)\n",
                                        node, ddr_interface_num, rankx, orankx, rodt_ctl, next_ohms, next_score,
                                        best_rank_score, best_rank_ohms);
                                best_rank_score            = next_score;
                                best_rank_rtt_nom   = rtt_nom;
                                //best_rank_nom_ohms  = rtt_nom_ohms;
                                best_rank_ctl       = rodt_ctl;
                                best_rank_ohms      = next_ohms;
                                best_rankx          = orankx;
                                lmc_rlevel_rank.u   = rlevel_scoreboard[rtt_nom][rodt_ctl][orankx].setting;

                            } /* for (int orankx = 0; orankx < dimm_count * 4; orankx++) */
                        } /* for (rodt_ctl = max_rodt_ctl; rodt_ctl >= min_rodt_ctl; --rodt_ctl) */
                    } /* for (rtt_idx = min_rtt_nom_idx; rtt_idx <= max_rtt_nom_idx; ++rtt_idx) */

                    ////////////////// this is the end of the BEST ROW SCORE LOOP

                    // DANGER, Will Robinson!! Abort now if we did not find a best score at all...
                    if (best_rank_score == DEFAULT_BEST_RANK_SCORE) {
                        error_print("WARNING: no best rank score found for N%d.LMC%d.R%d - resetting node...\n",
                                    node, ddr_interface_num, rankx);
                        bdk_wait_usec(500000);
                        bdk_reset_chip(node);
                    }

                    // FIXME: relative now, but still arbitrary...
                    // halve the range if 2 DIMMs unless they are single rank...
                    int MAX_RANK_SCORE = best_rank_score;
                    MAX_RANK_SCORE += (MAX_RANK_SCORE_LIMIT / ((num_ranks > 1) ? dimm_count : 1));

                    if (!ecc_ena){
                        lmc_rlevel_rank.cn83xx.byte8 = lmc_rlevel_rank.cn83xx.byte0; /* ECC is not used */
                    }

                    // at the end, write the best row settings to the current rank
                    DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx), lmc_rlevel_rank.u);
                    lmc_rlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx));

                    bdk_lmcx_rlevel_rankx_t saved_rlevel_rank;
                    saved_rlevel_rank.u = lmc_rlevel_rank.u;

                    ////////////////// this is the start of the PRINT LOOP

                    // for pass==0, print current rank, pass==1 print other rank(s)
                    // this is done because we want to show each ranks RODT values together, not interlaced
#if COUNT_RL_CANDIDATES
                    // keep separates for ranks - pass=0 target rank, pass=1 other rank on DIMM
                    int mask_skipped[2] = {0,0};
                    int score_skipped[2] = {0,0};
                    int selected_rows[2] = {0,0};
                    int zero_scores[2] = {0,0};
#endif /* COUNT_RL_CANDIDATES */
                    for (int pass = 0; pass < 2; pass++ ) {
                        for (int orankx = 0; orankx < dimm_count * 4; orankx++) {
                            if (!(dimm_rank_mask & (1 << orankx))) // stay on the same DIMM
                                continue;

                            if (((pass == 0) && (orankx != rankx)) || ((pass != 0) && (orankx == rankx)))
                                continue;

                            for (rtt_idx = min_rtt_nom_idx; rtt_idx <= max_rtt_nom_idx; ++rtt_idx) {
                                rtt_nom = imp_values->rtt_nom_table[rtt_idx];
                                if (dyn_rtt_nom_mask == 0) {
                                    print_nom_ohms = -1;
                                    if (rtt_idx != min_rtt_nom_idx)
                                        continue;
                                } else {
                                    print_nom_ohms = imp_values->rtt_nom_ohms[rtt_nom];
                                }

                                // cycle through all the RODT values...
                                for (rodt_ctl = max_rodt_ctl; rodt_ctl >= min_rodt_ctl; --rodt_ctl) {
                                    bdk_lmcx_rlevel_rankx_t temp_rlevel_rank;
                                    int temp_score = rlevel_scoreboard[rtt_nom][rodt_ctl][orankx].score;
                                    temp_rlevel_rank.u = rlevel_scoreboard[rtt_nom][rodt_ctl][orankx].setting;

                                    // skip RODT rows in mask, or rows with too high a score;
                                    // we will not use them for printing or evaluating...
#if COUNT_RL_CANDIDATES
                                    int skip_row;
                                    if ((1 << rodt_ctl) & rodt_row_skip_mask) {
                                        skip_row = WITH_RODT_SKIPPING;
                                        ++mask_skipped[pass];
                                    } else if (temp_score > MAX_RANK_SCORE) {
                                        skip_row = WITH_RODT_SKIPPING;
                                        ++score_skipped[pass];
                                    } else {
                                        skip_row = WITH_RODT_BLANK;
                                        ++selected_rows[pass];
                                        if (temp_score == 0)
                                            ++zero_scores[pass];
                                    }

#else /* COUNT_RL_CANDIDATES */
                                    int skip_row = (((1 << rodt_ctl) & rodt_row_skip_mask) || (temp_score > MAX_RANK_SCORE))
                                                    ? WITH_RODT_SKIPPING: WITH_RODT_BLANK;
#endif /* COUNT_RL_CANDIDATES */

                                    // identify and print the BEST ROW when it comes up
                                    if ((skip_row == WITH_RODT_BLANK) &&
                                        (best_rankx == orankx) &&
                                        (best_rank_rtt_nom == rtt_nom) &&
                                        (best_rank_ctl == rodt_ctl))
                                    {
                                        skip_row = WITH_RODT_BESTROW;
                                    }

                                    display_RL_with_RODT(node, ddr_interface_num,
                                                         temp_rlevel_rank, orankx, temp_score,
                                                         print_nom_ohms,
                                                         imp_values->rodt_ohms[rodt_ctl],
                                                         skip_row);

                                } /* for (rodt_ctl = max_rodt_ctl; rodt_ctl >= min_rodt_ctl; --rodt_ctl) */
                            } /* for (rtt_idx=min_rtt_nom_idx; rtt_idx<=max_rtt_nom_idx; ++rtt_idx) */
                        } /* for (int orankx = 0; orankx < dimm_count * 4; orankx++) { */
                    } /* for (int pass = 0; pass < 2; pass++ ) */
#if COUNT_RL_CANDIDATES
                    VB_PRT(VBL_TME, "N%d.LMC%d.R%d: RLROWS: selected %d+%d, zero_scores %d+%d, mask_skipped %d+%d, score_skipped %d+%d\n",
                           node, ddr_interface_num, rankx,
                           selected_rows[0], selected_rows[1],
                           zero_scores[0], zero_scores[1],
                           mask_skipped[0], mask_skipped[1],
                           score_skipped[0], score_skipped[1]);
#endif /* COUNT_RL_CANDIDATES */

                    ////////////////// this is the end of the PRINT LOOP

                    // now evaluate which bytes need adjusting
                    uint64_t byte_msk = 0x3f; // 6-bit fields
                    uint64_t best_byte, new_byte, temp_byte, orig_best_byte;

                    uint64_t rank_best_bytes[9]; // collect the new byte values; first init with current best for neighbor use
                    for (int byte_idx = 0, byte_sh = 0; byte_idx < 8+ecc_ena; byte_idx++, byte_sh += 6) {
                        rank_best_bytes[byte_idx] = (lmc_rlevel_rank.u >> byte_sh) & byte_msk;
                    }

                    ////////////////// this is the start of the BEST BYTE LOOP

                    for (int byte_idx = 0, byte_sh = 0; byte_idx < 8+ecc_ena; byte_idx++, byte_sh += 6) {
                        best_byte = orig_best_byte = rank_best_bytes[byte_idx];

                        ////////////////// this is the start of the BEST BYTE AVERAGING LOOP

                        // validate the initial "best" byte by looking at the average of the unskipped byte-column entries
                        // we want to do this before we go further, so we can try to start with a better initial value
                        // this is the so-called "BESTBUY" patch set
                        int sum = 0, count = 0;

                        for (rtt_idx = min_rtt_nom_idx; rtt_idx <= max_rtt_nom_idx; ++rtt_idx) {
                            rtt_nom = imp_values->rtt_nom_table[rtt_idx];
                            if ((dyn_rtt_nom_mask == 0) && (rtt_idx != min_rtt_nom_idx))
                                continue;

                            for (rodt_ctl = max_rodt_ctl; rodt_ctl >= min_rodt_ctl; --rodt_ctl) {
                                bdk_lmcx_rlevel_rankx_t temp_rlevel_rank;
                                int temp_score;
                                for (int orankx = 0; orankx < dimm_count * 4; orankx++) { // average over all the ranks
                                    if (!(dimm_rank_mask & (1 << orankx))) // stay on the same DIMM
                                        continue;
                                    temp_score = rlevel_scoreboard[rtt_nom][rodt_ctl][orankx].score;
                                    // skip RODT rows in mask, or rows with too high a score;
                                    // we will not use them for printing or evaluating...

                                    if (!((1 << rodt_ctl) & rodt_row_skip_mask) &&
                                        (temp_score <= MAX_RANK_SCORE))
                                    {
                                        temp_rlevel_rank.u = rlevel_scoreboard[rtt_nom][rodt_ctl][orankx].setting;
                                        temp_byte = (temp_rlevel_rank.u >> byte_sh) & byte_msk;
                                        sum += temp_byte;
                                        count++;
                                    }
                                } /* for (int orankx = 0; orankx < dimm_count * 4; orankx++) */
                            } /* for (rodt_ctl = max_rodt_ctl; rodt_ctl >= min_rodt_ctl; --rodt_ctl) */
                        } /* for (rtt_idx=min_rtt_nom_idx; rtt_idx<=max_rtt_nom_idx; ++rtt_idx) */

                        ////////////////// this is the end of the BEST BYTE AVERAGING LOOP


                        uint64_t avg_byte = divide_nint(sum, count); // FIXME: validate count and sum??
                        int avg_diff = (int)best_byte - (int)avg_byte;
                        new_byte = best_byte;
                        if (avg_diff != 0) {
                            // bump best up/dn by 1, not necessarily all the way to avg
                            new_byte = best_byte + ((avg_diff > 0) ? -1: 1);
                        }

                        VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: START:   Byte %d: best %d is different by %d from average %d, using %d.\n",
                                        node, ddr_interface_num, rankx,
                                        byte_idx, (int)best_byte, avg_diff, (int)avg_byte, (int)new_byte);
                        best_byte = new_byte;

                        // At this point best_byte is either:
                        // 1. the original byte-column value from the best scoring RODT row, OR
                        // 2. that value bumped toward the average of all the byte-column values
                        //
                        // best_byte will not change from here on...

                        ////////////////// this is the start of the BEST BYTE COUNTING LOOP

                        // NOTE: we do this next loop separately from above, because we count relative to "best_byte"
                        // which may have been modified by the above averaging operation...
                        //
                        // Also, the above only moves toward the average by +- 1, so that we will always have a count
                        // of at least 1 for the original best byte, even if all the others are further away and not counted;
                        // this ensures we will go back to the original if no others are counted...
                        // FIXME: this could cause issue if the range of values for a byte-lane are too disparate...
                        int count_less = 0, count_same = 0, count_more = 0;
#if FAILSAFE_CHECK
                        uint64_t count_byte = new_byte; // save the value we will count around
#endif /* FAILSAFE_CHECK */
#if RANK_MAJORITY
                        int rank_less = 0, rank_same = 0, rank_more = 0;
#endif /* RANK_MAJORITY */

                        for (rtt_idx = min_rtt_nom_idx; rtt_idx <= max_rtt_nom_idx; ++rtt_idx) {
                            rtt_nom = imp_values->rtt_nom_table[rtt_idx];
                            if ((dyn_rtt_nom_mask == 0) && (rtt_idx != min_rtt_nom_idx))
                                continue;

                            for (rodt_ctl = max_rodt_ctl; rodt_ctl >= min_rodt_ctl; --rodt_ctl) {
                                bdk_lmcx_rlevel_rankx_t temp_rlevel_rank;
                                int temp_score;
                                for (int orankx = 0; orankx < dimm_count * 4; orankx++) { // count over all the ranks
                                    if (!(dimm_rank_mask & (1 << orankx))) // stay on the same DIMM
                                        continue;
                                    temp_score = rlevel_scoreboard[rtt_nom][rodt_ctl][orankx].score;
                                    // skip RODT rows in mask, or rows with too high a score;
                                    // we will not use them for printing or evaluating...
                                    if (((1 << rodt_ctl) & rodt_row_skip_mask) ||
                                        (temp_score > MAX_RANK_SCORE))
                                        {
                                            continue;
                                        }
                                    temp_rlevel_rank.u = rlevel_scoreboard[rtt_nom][rodt_ctl][orankx].setting;
                                    temp_byte = (temp_rlevel_rank.u >> byte_sh) & byte_msk;

                                    if (temp_byte == 0) // do not count it if illegal
                                        continue;
                                    else if (temp_byte == best_byte)
                                        count_same++;
                                    else if (temp_byte == best_byte - 1)
                                        count_less++;
                                    else if (temp_byte == best_byte + 1)
                                        count_more++;
                                    // else do not count anything more than 1 away from the best
#if RANK_MAJORITY
                                    // FIXME? count is relative to best_byte; should it be rank-based?
                                    if (orankx != rankx) // rank counts only on main rank
                                        continue;
                                    else if (temp_byte == best_byte)
                                        rank_same++;
                                    else if (temp_byte == best_byte - 1)
                                        rank_less++;
                                    else if (temp_byte == best_byte + 1)
                                        rank_more++;
#endif /* RANK_MAJORITY */
                                } /* for (int orankx = 0; orankx < dimm_count * 4; orankx++) */
                            } /* for (rodt_ctl = max_rodt_ctl; rodt_ctl >= min_rodt_ctl; --rodt_ctl) */
                        } /* for (rtt_idx=min_rtt_nom_idx; rtt_idx<=max_rtt_nom_idx; ++rtt_idx) */

#if RANK_MAJORITY
                        VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: COUNT:   Byte %d: orig %d now %d, more %d same %d less %d (%d/%d/%d)\n",
                                        node, ddr_interface_num, rankx,
                                        byte_idx, (int)orig_best_byte, (int)best_byte,
                                        count_more, count_same, count_less,
                                        rank_more, rank_same, rank_less);
#else /* RANK_MAJORITY */
                        VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: COUNT:   Byte %d: orig %d now %d, more %d same %d less %d\n",
                                        node, ddr_interface_num, rankx,
                                        byte_idx, (int)orig_best_byte, (int)best_byte,
                                        count_more, count_same, count_less);
#endif /* RANK_MAJORITY */
                        ////////////////// this is the end of the BEST BYTE COUNTING LOOP

                        // choose the new byte value
                        // we need to check that there is no gap greater than 2 between adjacent bytes
                        //  (adjacency depends on DIMM type)
                        // use the neighbor value to help decide
                        // initially, the rank_best_bytes[] will contain values from the chosen lowest score rank
                        new_byte = 0;

                        // neighbor is index-1 unless we are index 0 or index 8 (ECC)
                        int neighbor = (byte_idx == 8) ? 3 : ((byte_idx == 0) ? 1 : byte_idx - 1);
                        uint64_t neigh_byte = rank_best_bytes[neighbor];


                        // can go up or down or stay the same, so look at a numeric average to help
                        new_byte = divide_nint(((count_more * (best_byte + 1)) +
                                                (count_same * (best_byte + 0)) +
                                                (count_less * (best_byte - 1))),
                                               max(1, (count_more + count_same + count_less)));

                        // use neighbor to help choose with average
                        if ((byte_idx > 0) && (_abs(neigh_byte - new_byte) > 2)) // but not for byte 0
                        {
                            uint64_t avg_pick = new_byte;
                            if ((new_byte - best_byte) != 0)
                                new_byte = best_byte; // back to best, average did not get better
                            else // avg was the same, still too far, now move it towards the neighbor
                                new_byte += (neigh_byte > new_byte) ? 1 : -1;

                            VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: AVERAGE: Byte %d: neighbor %d too different %d from average %d, picking %d.\n",
                                            node, ddr_interface_num, rankx,
                                            byte_idx, neighbor, (int)neigh_byte, (int)avg_pick, (int)new_byte);
                        }
#if MAJORITY_OVER_AVG
                        // NOTE:
                        // For now, we let the neighbor processing above trump the new simple majority processing here.
                        // This is mostly because we have seen no smoking gun for a neighbor bad choice (yet?).
                        // Also note that we will ALWAYS be using byte 0 majority, because of the if clause above.
                        else {
                            // majority is dependent on the counts, which are relative to best_byte, so start there
                            uint64_t maj_byte = best_byte;
                            if ((count_more > count_same) && (count_more > count_less)) {
                                maj_byte++;
                            } else if ((count_less > count_same) && (count_less > count_more)) {
                                maj_byte--;
                            }
                            if (maj_byte != new_byte) {
                                // print only when majority choice is different from average
                                VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: MAJORTY: Byte %d: picking majority of %d over average %d.\n",
                                                node, ddr_interface_num, rankx,
                                                byte_idx, (int)maj_byte, (int)new_byte);
                                new_byte = maj_byte;
                            } else {
                                VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: AVERAGE: Byte %d: picking average of %d.\n",
                                                node, ddr_interface_num, rankx,
                                                byte_idx, (int)new_byte);
                            }
#if RANK_MAJORITY
                            // rank majority is dependent on the rank counts, which are relative to best_byte,
                            // so start there, and adjust according to the rank counts majority
                            uint64_t rank_maj = best_byte;
                            if ((rank_more > rank_same) && (rank_more > rank_less)) {
                                rank_maj++;
                            } else if ((rank_less > rank_same) && (rank_less > rank_more)) {
                                rank_maj--;
                            }
                            int rank_sum = rank_more + rank_same + rank_less;

                            // now, let rank majority possibly rule over the current new_byte however we got it
                            if (rank_maj != new_byte) { // only if different
                                // Here is where we decide whether to completely apply RANK_MAJORITY or not
                                // FIXME: For the moment, we do it ONLY when running 2-slot configs
                                // FIXME? or when rank_sum is big enough?
                                if ((dimm_count > 1) || (rank_sum > 2)) {
                                    // print only when rank majority choice is selected
                                    VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: RANKMAJ: Byte %d: picking %d over %d.\n",
                                                    node, ddr_interface_num, rankx,
                                                    byte_idx, (int)rank_maj, (int)new_byte);
                                    new_byte = rank_maj;
                                } else { // FIXME: print some info when we could have chosen RANKMAJ but did not
                                    VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: RANKMAJ: Byte %d: NOT using %d over %d (best=%d,sum=%d).\n",
                                                    node, ddr_interface_num, rankx,
                                                    byte_idx, (int)rank_maj, (int)new_byte,
                                                    (int)best_byte, rank_sum);
                                }
                            }
#endif /* RANK_MAJORITY */
                        }
#else
                        else {
                            VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: AVERAGE: Byte %d: picking average of %d.\n",
                                            node, ddr_interface_num, rankx,
                                            byte_idx, (int)new_byte);
                        }
#endif
#if FAILSAFE_CHECK
                        // one last check:
                        // if new_byte is still count_byte, BUT there was no count for that value, DO SOMETHING!!!
                        // FIXME: go back to original best byte from the best row
                        if ((new_byte == count_byte) && (count_same == 0)) {
                            new_byte = orig_best_byte;
                            VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: FAILSAF: Byte %d: going back to original %d.\n",
                                            node, ddr_interface_num, rankx,
                                            byte_idx, (int)new_byte);
                        }
#endif /* FAILSAFE_CHECK */
#if PERFECT_BITMASK_COUNTING
                        // Look at counts for "perfect" bitmasks if we had any for this byte-lane.
                        // Remember, we only counted for DDR4, so zero means none or DDR3, and we bypass this...
                        if (rank_perfect_counts[rankx].total[byte_idx] > 0) {
                            // FIXME: should be more error checking, look for ties, etc...
                            /* FIXME(dhendrix): i shadows another local variable, changed to _i in this block */
//                            int i, delay_count, delay_value, delay_max;
                            int _i, delay_count, delay_value, delay_max;
                            uint32_t ties;
                            delay_value = -1;
                            delay_max = 0;
                            ties = 0;

                            for (_i = 0; _i < 32; _i++) {
                                delay_count = rank_perfect_counts[rankx].count[byte_idx][_i];
                                if (delay_count > 0) { // only look closer if there are any,,,
                                    if (delay_count > delay_max) {
                                        delay_max = delay_count;
                                        delay_value = _i;
                                        ties = 0; // reset ties to none
                                    } else if (delay_count == delay_max) {
                                        if (ties == 0)
                                            ties = 1UL << delay_value; // put in original value
                                        ties |= 1UL << _i; // add new value
                                    }
                                }
                            } /* for (_i = 0; _i < 32; _i++) */

                            if (delay_value >= 0) {
                                if (ties != 0) {
                                    if (ties & (1UL << (int)new_byte)) {
                                        // leave choice as new_byte if any tied one is the same...


                                        delay_value = (int)new_byte;
                                        VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: PERFECT: Byte %d: TIES (0x%x) INCLUDED %d (%d)\n",
                                                  node, ddr_interface_num, rankx, byte_idx, ties, (int)new_byte, delay_max);
                                    } else {
                                        // FIXME: should choose a perfect one!!!
                                        // FIXME: for now, leave the choice as new_byte
                                        delay_value = (int)new_byte;
                                        VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: PERFECT: Byte %d: TIES (0x%x) OMITTED %d (%d)\n",
                                                  node, ddr_interface_num, rankx, byte_idx, ties, (int)new_byte, delay_max);
                                    }
                                } /* if (ties != 0) */

                                if (delay_value != (int)new_byte) {
                                    delay_count = rank_perfect_counts[rankx].count[byte_idx][(int)new_byte];
                                    VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: PERFECT: Byte %d: DIFF from %d (%d), USING %d (%d)\n",
                                           node, ddr_interface_num, rankx, byte_idx, (int)new_byte,
                                           delay_count, delay_value, delay_max);
                                    new_byte = (uint64_t)delay_value; // FIXME: make this optional via envvar?
                                } else {
                                    debug_print("N%d.LMC%d.R%d: PERFECT: Byte %d: SAME as %d (%d)\n",
                                                node, ddr_interface_num, rankx, byte_idx, new_byte, delay_max);
                                }
                            }
                        } /* if (rank_perfect_counts[rankx].total[byte_idx] > 0) */
                        else {
                            if (ddr_type == DDR4_DRAM) { // only report when DDR4
                                // FIXME: remove or increase VBL for this output...
                                VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: PERFECT: Byte %d: ZERO perfect bitmasks\n",
                                          node, ddr_interface_num, rankx, byte_idx);
                            }
                        } /* if (rank_perfect_counts[rankx].total[byte_idx] > 0) */
#endif /* PERFECT_BITMASK_COUNTING */

                        VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: SUMMARY: Byte %d: %s: orig %d now %d, more %d same %d less %d, using %d\n",
                                        node, ddr_interface_num, rankx,
                                        byte_idx, "AVG", (int)orig_best_byte,
                                        (int)best_byte, count_more, count_same, count_less, (int)new_byte);

                        // update the byte with the new value (NOTE: orig value in the CSR may not be current "best")
                        lmc_rlevel_rank.u &= ~(byte_msk << byte_sh);
                        lmc_rlevel_rank.u |= (new_byte << byte_sh);

                        rank_best_bytes[byte_idx] = new_byte; // save new best for neighbor use

                    } /* for (byte_idx = 0; byte_idx < 8+ecc_ena; byte_idx++) */

                    ////////////////// this is the end of the BEST BYTE LOOP

                    if (saved_rlevel_rank.u != lmc_rlevel_rank.u) {
                        DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx), lmc_rlevel_rank.u);
                        lmc_rlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx));
                        debug_print("Adjusting Read-Leveling per-RANK settings.\n");
                    } else {
                        debug_print("Not Adjusting Read-Leveling per-RANK settings.\n");
                    }
                    display_RL_with_final(node, ddr_interface_num, lmc_rlevel_rank, rankx);

#if RLEXTRAS_PATCH
#define RLEVEL_RANKX_EXTRAS_INCR  4
                    if ((rank_mask & 0x0F) != 0x0F) { // if there are unused entries to be filled
                        bdk_lmcx_rlevel_rankx_t temp_rlevel_rank = lmc_rlevel_rank; // copy the current rank
                        int byte, delay;
                        if (rankx < 3) {
                            debug_print("N%d.LMC%d.R%d: checking for RLEVEL_RANK unused entries.\n",
                                      node, ddr_interface_num, rankx);
                            for (byte = 0; byte < 9; byte++) { // modify the copy in prep for writing to empty slot(s)
                                delay = get_rlevel_rank_struct(&temp_rlevel_rank, byte) + RLEVEL_RANKX_EXTRAS_INCR;
                                if (delay > (int)RLEVEL_BYTE_MSK) delay = RLEVEL_BYTE_MSK;
                                update_rlevel_rank_struct(&temp_rlevel_rank, byte, delay);
                            }
                            if (rankx == 0) { // if rank 0, write rank 1 and rank 2 here if empty
                                if (!(rank_mask & (1<<1))) { // check that rank 1 is empty
                                    VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: writing RLEVEL_RANK unused entry R%d.\n",
                                                    node, ddr_interface_num, rankx, 1);
                                    DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, 1), temp_rlevel_rank.u);
                                }
                                if (!(rank_mask & (1<<2))) { // check that rank 2 is empty
                                    VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: writing RLEVEL_RANK unused entry R%d.\n",
                                                    node, ddr_interface_num, rankx, 2);
                                    DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, 2), temp_rlevel_rank.u);
                                }
                            }
                            // if ranks 0, 1 or 2, write rank 3 here if empty
                            if (!(rank_mask & (1<<3))) { // check that rank 3 is empty
                                VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: writing RLEVEL_RANK unused entry R%d.\n",
                                          node, ddr_interface_num, rankx, 3);
                                DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, 3), temp_rlevel_rank.u);
                            }
                        }
                    }
#endif /* RLEXTRAS_PATCH */
                } /* for (rankx = 0; rankx < dimm_count * 4; rankx++) */

                ////////////////// this is the end of the RANK MAJOR LOOP

            }  /* Evaluation block */
        } /* while(rlevel_debug_loops--) */

        lmc_control.s.ddr2t           = save_ddr2t;
        DRAM_CSR_WRITE(node, BDK_LMCX_CONTROL(ddr_interface_num), lmc_control.u);
        lmc_control.u = BDK_CSR_READ(node, BDK_LMCX_CONTROL(ddr_interface_num));
        ddr_print("%-45s : %6d\n", "DDR2T", lmc_control.s.ddr2t); /* Display final 2T value */


        perform_ddr_init_sequence(node, rank_mask, ddr_interface_num);

        for (rankx = 0; rankx < dimm_count * 4;rankx++) {
            uint64_t value;
            int parameter_set = 0;
            if (!(rank_mask & (1 << rankx)))
                continue;

            lmc_rlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx));

            for (i=0; i<9; ++i) {
                if ((s = lookup_env_parameter("ddr%d_rlevel_rank%d_byte%d", ddr_interface_num, rankx, i)) != NULL) {
                    parameter_set |= 1;
                    value = strtoul(s, NULL, 0);

                    update_rlevel_rank_struct(&lmc_rlevel_rank, i, value);
                }
            }

            if ((s = lookup_env_parameter_ull("ddr%d_rlevel_rank%d", ddr_interface_num, rankx)) != NULL) {
                parameter_set |= 1;
                value = strtoull(s, NULL, 0);
                lmc_rlevel_rank.u = value;
            }

            if (parameter_set) {
                DRAM_CSR_WRITE(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx), lmc_rlevel_rank.u);
                display_RL(node, ddr_interface_num, lmc_rlevel_rank, rankx);
            }
        }
    }

    /* Workaround Trcd overflow by using Additive latency. */
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X))
    {
        bdk_lmcx_modereg_params0_t lmc_modereg_params0;
        bdk_lmcx_timing_params1_t lmc_timing_params1;
        bdk_lmcx_control_t lmc_control;
        int rankx;

        lmc_timing_params1.u = BDK_CSR_READ(node, BDK_LMCX_TIMING_PARAMS1(ddr_interface_num));
        lmc_modereg_params0.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num));
        lmc_control.u = BDK_CSR_READ(node, BDK_LMCX_CONTROL(ddr_interface_num));

        if (lmc_timing_params1.s.trcd == 0) {
            ddr_print("Workaround Trcd overflow by using Additive latency.\n");
            lmc_timing_params1.s.trcd     = 12; /* Hard code this to 12 and enable additive latency */
            lmc_modereg_params0.s.al      = 2; /* CL-2 */
            lmc_control.s.pocas           = 1;

            ddr_print("MODEREG_PARAMS0                               : 0x%016llx\n", lmc_modereg_params0.u);
            DRAM_CSR_WRITE(node, BDK_LMCX_MODEREG_PARAMS0(ddr_interface_num), lmc_modereg_params0.u);
            ddr_print("TIMING_PARAMS1                                : 0x%016llx\n", lmc_timing_params1.u);
            DRAM_CSR_WRITE(node, BDK_LMCX_TIMING_PARAMS1(ddr_interface_num), lmc_timing_params1.u);

            ddr_print("LMC_CONTROL                                   : 0x%016llx\n", lmc_control.u);
            DRAM_CSR_WRITE(node, BDK_LMCX_CONTROL(ddr_interface_num), lmc_control.u);

            for (rankx = 0; rankx < dimm_count * 4; rankx++) {
                if (!(rank_mask & (1 << rankx)))
                    continue;

                ddr4_mrw(node, ddr_interface_num, rankx, -1, 1, 0); /* MR1 */
            }
        }
    }

    // this is here just for output, to allow check of the Deskew settings one last time...
    if (! disable_deskew_training) {
        deskew_counts_t dsk_counts;
        VB_PRT(VBL_TME, "N%d.LMC%d: Check Deskew Settings before software Write-Leveling.\n",
                  node, ddr_interface_num);
        Validate_Read_Deskew_Training(node, rank_mask, ddr_interface_num, &dsk_counts, VBL_TME); // TME for FINAL
    }


    /* Workaround Errata 26304 (T88@2.0)

       When the CSRs LMCX_DLL_CTL3[WR_DESKEW_ENA] = 1 AND
       LMCX_PHY_CTL2[DQS[0..8]_DSK_ADJ] > 4, set
       LMCX_EXT_CONFIG[DRIVE_ENA_BPRCH] = 1.
    */
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS2_X)) { // only for 88XX pass 2, not 81xx or 83xx
        bdk_lmcx_dll_ctl3_t dll_ctl3;
        bdk_lmcx_phy_ctl2_t phy_ctl2;
        bdk_lmcx_ext_config_t ext_config;
        int increased_dsk_adj = 0;
        int byte;

        phy_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_PHY_CTL2(ddr_interface_num));
        ext_config.u = BDK_CSR_READ(node, BDK_LMCX_EXT_CONFIG(ddr_interface_num));
        dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));

        for (byte = 0; byte < 8; ++byte) {
            if (!(ddr_interface_bytemask&(1<<byte)))
                continue;
            increased_dsk_adj |= (((phy_ctl2.u >> (byte*3)) & 0x7) > 4);
        }

        if ((dll_ctl3.s.wr_deskew_ena == 1) && increased_dsk_adj) {
            ext_config.s.drive_ena_bprch = 1;
            DRAM_CSR_WRITE(node, BDK_LMCX_EXT_CONFIG(ddr_interface_num),
                                      ext_config.u);
        }
    }

    /*
     * 6.9.13 DRAM Vref Training for DDR4
     *
     * This includes software write-leveling
     */

    { // Software Write-Leveling block

        /* Try to determine/optimize write-level delays experimentally. */
#pragma pack(push,1)
        bdk_lmcx_wlevel_rankx_t lmc_wlevel_rank;
        bdk_lmcx_wlevel_rankx_t lmc_wlevel_rank_hw_results;
        int byte;
        int delay;
        int rankx = 0;
        int active_rank;
#if !DISABLE_SW_WL_PASS_2
        bdk_lmcx_rlevel_rankx_t lmc_rlevel_rank;
        int sw_wlevel_offset = 1;
#endif
        int sw_wlevel_enable = 1; /* FIX... Should be customizable. */
        int interfaces;
        int measured_vref_flag;
        typedef enum {
            WL_ESTIMATED = 0,   /* HW/SW wleveling failed. Results
                                   estimated. */
            WL_HARDWARE  = 1,   /* H/W wleveling succeeded */
            WL_SOFTWARE  = 2,   /* S/W wleveling passed 2 contiguous
                                   settings. */
            WL_SOFTWARE1 = 3,   /* S/W wleveling passed 1 marginal
                                   setting. */
        } sw_wl_status_t;

        static const char *wl_status_strings[] = {
            "(e)",
            "   ",
            "   ",
            "(1)"
        };
        int sw_wlevel_hw_default = 1; // FIXME: make H/W assist the default now
#pragma pack(pop)

        if ((s = lookup_env_parameter("ddr_sw_wlevel_hw")) != NULL) {
            sw_wlevel_hw_default = !!strtoul(s, NULL, 0);
        }

         // cannot use hw-assist when doing 32-bit
        if (! ddr_interface_64b) {
            sw_wlevel_hw_default = 0;
        }

        if ((s = lookup_env_parameter("ddr_software_wlevel")) != NULL) {
            sw_wlevel_enable = strtoul(s, NULL, 0);
        }

#if SWL_WITH_HW_ALTS_CHOOSE_SW
        // Choose the SW algo for SWL if any HWL alternates were found
        // NOTE: we have to do this here, and for all, since HW-assist including ECC requires ECC enable
        for (rankx = 0; rankx < dimm_count * 4; rankx++) {
            if (!sw_wlevel_enable)
                break;
            if (!(rank_mask & (1 << rankx)))
                continue;

            // if we are doing HW-assist, and there are alternates, switch to SW-algorithm for all
            if (sw_wlevel_hw && hwl_alts[rankx].hwl_alt_mask) {
                ddr_print("N%d.LMC%d.R%d: Using SW algorithm for write-leveling this rank\n",
                          node, ddr_interface_num, rankx);
                sw_wlevel_hw_default = 0;
                break;
            }
        } /* for (rankx = 0; rankx < dimm_count * 4; rankx++) */
#endif

        /* Get the measured_vref setting from the config, check for an override... */
        /* NOTE: measured_vref=1 (ON) means force use of MEASURED Vref... */
        // NOTE: measured VREF can only be done for DDR4
        if (ddr_type == DDR4_DRAM) {
            measured_vref_flag = custom_lmc_config->measured_vref;
            if ((s = lookup_env_parameter("ddr_measured_vref")) != NULL) {
                measured_vref_flag = !!strtoul(s, NULL, 0);
            }
        } else {
            measured_vref_flag = 0; // OFF for DDR3
        }

        /* Ensure disabled ECC for DRAM tests using the SW algo, else leave it untouched */
        if (!sw_wlevel_hw_default) {
            lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(ddr_interface_num));
            lmc_config.s.ecc_ena = 0;
            DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(ddr_interface_num), lmc_config.u);
        }

#if USE_L2_WAYS_LIMIT
        limit_l2_ways(node, 0, 0);       /* Disable l2 sets for DRAM testing */
#endif

        /* We need to track absolute rank number, as well as how many
        ** active ranks we have.  Two single rank DIMMs show up as
        ** ranks 0 and 2, but only 2 ranks are active. */
        active_rank = 0;

        interfaces = bdk_pop(ddr_interface_mask);

#define VREF_RANGE1_LIMIT 0x33 // range1 is valid for 0x00 - 0x32
#define VREF_RANGE2_LIMIT 0x18 // range2 is valid for 0x00 - 0x17
// full window is valid for 0x00 to 0x4A
// let 0x00 - 0x17 be range2, 0x18 - 0x4a be range 1
#define VREF_LIMIT        (VREF_RANGE1_LIMIT + VREF_RANGE2_LIMIT)
#define VREF_FINAL        (VREF_LIMIT - 1)

        for (rankx = 0; rankx < dimm_count * 4; rankx++) {
            uint64_t rank_addr;
            int vref_value, final_vref_value, final_vref_range = 0;
            int start_vref_value = 0, computed_final_vref_value = -1;
            char best_vref_values_count, vref_values_count;
            char best_vref_values_start, vref_values_start;

            int bytes_failed;
            sw_wl_status_t byte_test_status[9];
            sw_wl_status_t sw_wl_rank_status = WL_HARDWARE;
            int sw_wl_failed = 0;
            int sw_wlevel_hw = sw_wlevel_hw_default;

            if (!sw_wlevel_enable)
                break;

            if (!(rank_mask & (1 << rankx)))
                continue;

            ddr_print("N%d.LMC%d.R%d: Performing Software Write-Leveling %s\n",
                      node, ddr_interface_num, rankx,
                      (sw_wlevel_hw) ? "with H/W assist" : "with S/W algorithm");

            if ((ddr_type == DDR4_DRAM) && (num_ranks != 4)) {
                // always compute when we can...
                computed_final_vref_value = compute_vref_value(node, ddr_interface_num, rankx,
                                                               dimm_count, num_ranks, imp_values,
                                                               is_stacked_die);
                if (!measured_vref_flag) // but only use it if allowed
                    start_vref_value = VREF_FINAL; // skip all the measured Vref processing, just the final setting
            }

            /* Save off the h/w wl results */
            lmc_wlevel_rank_hw_results.u = BDK_CSR_READ(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx));

            vref_values_count = 0;
            vref_values_start = 0;
            best_vref_values_count = 0;
            best_vref_values_start = 0;

            /* Loop one extra time using the Final Vref value. */
            for (vref_value = start_vref_value; vref_value < VREF_LIMIT; ++vref_value) {
                if (ddr_type == DDR4_DRAM) {
                    if (vref_value < VREF_FINAL) {
                        int vrange, vvalue;
                        if (vref_value < VREF_RANGE2_LIMIT) {
                            vrange = 1; vvalue = vref_value;
                        } else {
                            vrange = 0; vvalue = vref_value - VREF_RANGE2_LIMIT;
                        }
                        set_vref(node, ddr_interface_num, rankx,
                                 vrange, vvalue);
                    } else { /* if (vref_value < VREF_FINAL) */
                        /* Print the final Vref value first. */

                        /* Always print the computed first if its valid */
                        if (computed_final_vref_value >= 0) {
                            ddr_print("N%d.LMC%d.R%d: Vref Computed Summary                 :"
                                      "              %2d (0x%02x)\n",
                                      node, ddr_interface_num,
                                      rankx, computed_final_vref_value,
                                      computed_final_vref_value);
                        }
                        if (!measured_vref_flag) { // setup to use the computed
                            best_vref_values_count = 1;
                            final_vref_value = computed_final_vref_value;
                        } else { // setup to use the measured
                            if (best_vref_values_count > 0) {
                                best_vref_values_count = max(best_vref_values_count, 2);
#if 0
                                // NOTE: this already adjusts VREF via calculating 40% rather than 50%
                                final_vref_value = best_vref_values_start + divide_roundup((best_vref_values_count-1)*4,10);
                                ddr_print("N%d.LMC%d.R%d: Vref Training Summary                 :"
                                          "    %2d <----- %2d (0x%02x) -----> %2d range: %2d\n",
                                          node, ddr_interface_num, rankx, best_vref_values_start,
                                          final_vref_value, final_vref_value,
                                          best_vref_values_start+best_vref_values_count-1,
                                          best_vref_values_count-1);
#else
                                final_vref_value = best_vref_values_start + divide_nint(best_vref_values_count - 1, 2);
                                if (final_vref_value < VREF_RANGE2_LIMIT) {
                                    final_vref_range = 1;
                                } else {
                                    final_vref_range = 0; final_vref_value -= VREF_RANGE2_LIMIT;
                                }
                                {
                                    int vvlo = best_vref_values_start;
                                    int vrlo;
                                    if (vvlo < VREF_RANGE2_LIMIT) {
                                        vrlo = 2;
                                    } else {
                                        vrlo = 1; vvlo -= VREF_RANGE2_LIMIT;
                                    }

                                    int vvhi = best_vref_values_start + best_vref_values_count - 1;
                                    int vrhi;
                                    if (vvhi < VREF_RANGE2_LIMIT) {
                                        vrhi = 2;
                                    } else {
                                        vrhi = 1; vvhi -= VREF_RANGE2_LIMIT;
                                    }
                                    ddr_print("N%d.LMC%d.R%d: Vref Training Summary                 :"
                                              "  0x%02x/%1d <----- 0x%02x/%1d -----> 0x%02x/%1d, range: %2d\n",
                                              node, ddr_interface_num, rankx,
                                              vvlo, vrlo,
                                              final_vref_value, final_vref_range + 1,
                                              vvhi, vrhi,
                                              best_vref_values_count-1);
                                }
#endif

                            } else {
                                /* If nothing passed use the default Vref value for this rank */
                                bdk_lmcx_modereg_params2_t lmc_modereg_params2;
                                lmc_modereg_params2.u = BDK_CSR_READ(node, BDK_LMCX_MODEREG_PARAMS2(ddr_interface_num));
                                final_vref_value = (lmc_modereg_params2.u >> (rankx * 10 + 3)) & 0x3f;
                                final_vref_range = (lmc_modereg_params2.u >> (rankx * 10 + 9)) & 0x01;

                                ddr_print("N%d.LMC%d.R%d: Vref Using Default                    :"
                                          "    %2d <----- %2d (0x%02x) -----> %2d, range%1d\n",
                                          node, ddr_interface_num, rankx,
                                          final_vref_value, final_vref_value,
                                          final_vref_value, final_vref_value, final_vref_range+1);
                            }
                        }

                        // allow override
                        if ((s = lookup_env_parameter("ddr%d_vref_value_%1d%1d",
                                                      ddr_interface_num, !!(rankx&2), !!(rankx&1))) != NULL) {
                            final_vref_value = strtoul(s, NULL, 0);
                        }

                        set_vref(node, ddr_interface_num, rankx, final_vref_range, final_vref_value);

                    } /* if (vref_value < VREF_FINAL) */
                } /* if (ddr_type == DDR4_DRAM) */

                lmc_wlevel_rank.u = lmc_wlevel_rank_hw_results.u; /* Restore the saved value */

                for (byte = 0; byte < 9; ++byte)
                    byte_test_status[byte] = WL_ESTIMATED;

                if (wlevel_bitmask_errors == 0) {

                    /* Determine address of DRAM to test for pass 1 of software write leveling. */
                    rank_addr  = active_rank * (1ull << (pbank_lsb - bunk_enable + (interfaces/2)));
                    // FIXME: these now put in by test_dram_byte()
                    //rank_addr |= (ddr_interface_num<<7); /* Map address into proper interface */
                    //rank_addr = bdk_numa_get_address(node, rank_addr);
                    VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: Active Rank %d Address: 0x%llx\n",
                           node, ddr_interface_num, rankx, active_rank, rank_addr);

                    { // start parallel write-leveling block for delay high-order bits
                        int errors = 0;
                        int byte_delay[9];
                        uint64_t bytemask;
                        int bytes_todo;

                        if (ddr_interface_64b) {
                            bytes_todo = (sw_wlevel_hw) ? ddr_interface_bytemask : 0xFF;
                            bytemask = ~0ULL;
                        } else { // 32-bit, must be using SW algo, only data bytes
                            bytes_todo = 0x0f;
                            bytemask = 0x00000000ffffffffULL;
                        }

                        for (byte = 0; byte < 9; ++byte) {
                            if (!(bytes_todo & (1 << byte))) {
                                byte_delay[byte] = 0;
                            } else {
                                byte_delay[byte] = get_wlevel_rank_struct(&lmc_wlevel_rank, byte);
                            }
                        } /* for (byte = 0; byte < 9; ++byte) */

#define WL_MIN_NO_ERRORS_COUNT 3  // FIXME? three passes without errors
                        int no_errors_count = 0;

                        // Change verbosity if using measured vs computed VREF or DDR3
                        // measured goes many times through SWL, computed and DDR3 only once
                        // so we want the EXHAUSTED messages at NORM for computed and DDR3,
                        // and at DEV2 for measured, just for completeness
                        int vbl_local = (measured_vref_flag) ? VBL_DEV2 : VBL_NORM;
                        uint64_t bad_bits[2];
#if ENABLE_SW_WLEVEL_UTILIZATION
                        uint64_t sum_dram_dclk = 0, sum_dram_ops = 0;
                        uint64_t start_dram_dclk, stop_dram_dclk;
                        uint64_t start_dram_ops, stop_dram_ops;
#endif
                        do {
                            // write the current set of WL delays
                            DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx), lmc_wlevel_rank.u);
                            lmc_wlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx));

                            bdk_watchdog_poke();

                            // do the test
                            if (sw_wlevel_hw) {
                                errors = run_best_hw_patterns(node, ddr_interface_num, rank_addr,
                                                              DBTRAIN_TEST, bad_bits);
                                errors &= bytes_todo; // keep only the ones we are still doing
                            } else {
#if ENABLE_SW_WLEVEL_UTILIZATION
                                start_dram_dclk = BDK_CSR_READ(node, BDK_LMCX_DCLK_CNT(ddr_interface_num));
                                start_dram_ops  = BDK_CSR_READ(node, BDK_LMCX_OPS_CNT(ddr_interface_num));
#endif
#if USE_ORIG_TEST_DRAM_BYTE
                                errors = test_dram_byte(node, ddr_interface_num, rank_addr, bytemask, bad_bits);
#else
                                errors = dram_tuning_mem_xor(node, ddr_interface_num, rank_addr, bytemask, bad_bits);
#endif
#if ENABLE_SW_WLEVEL_UTILIZATION
                                stop_dram_dclk = BDK_CSR_READ(node, BDK_LMCX_DCLK_CNT(ddr_interface_num));
                                stop_dram_ops  = BDK_CSR_READ(node, BDK_LMCX_OPS_CNT(ddr_interface_num));
                                sum_dram_dclk += stop_dram_dclk - start_dram_dclk;
                                sum_dram_ops  += stop_dram_ops  - start_dram_ops;
#endif
                            }

                            VB_PRT(VBL_DEV2, "N%d.LMC%d.R%d: S/W write-leveling TEST: returned 0x%x\n",
                                   node, ddr_interface_num, rankx, errors);

                            // remember, errors will not be returned for byte-lanes that have maxxed out...
                            if (errors == 0) {
                                no_errors_count++; // bump
                                if (no_errors_count > 1) // bypass check/update completely
                                    continue; // to end of do-while
                            } else
                                no_errors_count = 0; // reset

                            // check errors by byte
                            for (byte = 0; byte < 9; ++byte) {
                                if (!(bytes_todo & (1 << byte)))
                                    continue;

                                delay = byte_delay[byte];
                                if (errors & (1 << byte)) { // yes, an error in this byte lane
                                    debug_print("        byte %d delay %2d Errors\n", byte, delay);
                                    // since this byte had an error, we move to the next delay value, unless maxxed out
                                    delay += 8; // incr by 8 to do only delay high-order bits
                                    if (delay < 32) {
                                        update_wlevel_rank_struct(&lmc_wlevel_rank, byte, delay);
                                        debug_print("        byte %d delay %2d New\n", byte, delay);
                                        byte_delay[byte] = delay;
                                    } else { // reached max delay, maybe really done with this byte
#if SWL_TRY_HWL_ALT
                                        if (!measured_vref_flag && // consider an alt only for computed VREF and
                                            (hwl_alts[rankx].hwl_alt_mask & (1 << byte))) // if an alt exists...
                                        {
                                            int bad_delay = delay & 0x6; // just orig low-3 bits
                                            delay = hwl_alts[rankx].hwl_alt_delay[byte]; // yes, use it
                                            hwl_alts[rankx].hwl_alt_mask &= ~(1 << byte); // clear that flag
                                            update_wlevel_rank_struct(&lmc_wlevel_rank, byte, delay);
                                            byte_delay[byte] = delay;
                                            debug_print("        byte %d delay %2d ALTERNATE\n", byte, delay);
                                            VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: SWL: Byte %d: %d FAIL, trying ALTERNATE %d\n",
                                                   node, ddr_interface_num, rankx, byte, bad_delay, delay);

                                        } else
#endif /* SWL_TRY_HWL_ALT */
                                        {
                                            unsigned bits_bad;
                                            if (byte < 8) {
                                                bytemask &= ~(0xffULL << (8*byte)); // test no longer, remove from byte mask
                                                bits_bad = (unsigned)((bad_bits[0] >> (8 * byte)) & 0xffUL);
                                            } else {
                                                bits_bad = (unsigned)(bad_bits[1] & 0xffUL);
                                            }
                                            bytes_todo &= ~(1 << byte); // remove from bytes to do
                                            byte_test_status[byte] = WL_ESTIMATED; // make sure this is set for this case
                                            debug_print("        byte %d delay %2d Exhausted\n", byte, delay);
                                            VB_PRT(vbl_local, "N%d.LMC%d.R%d: SWL: Byte %d (0x%02x): delay %d EXHAUSTED \n",
                                                   node, ddr_interface_num, rankx, byte, bits_bad, delay);
                                        }
                                    }
                                } else { // no error, stay with current delay, but keep testing it...
                                    debug_print("        byte %d delay %2d Passed\n", byte, delay);
                                    byte_test_status[byte] = WL_HARDWARE; // change status
                                }

                            } /* for (byte = 0; byte < 9; ++byte) */

                        } while (no_errors_count < WL_MIN_NO_ERRORS_COUNT);

#if ENABLE_SW_WLEVEL_UTILIZATION
                        if (! sw_wlevel_hw) {
                            uint64_t percent_x10;
                            if (sum_dram_dclk == 0)
                                sum_dram_dclk = 1;
                            percent_x10 = sum_dram_ops * 1000 / sum_dram_dclk;
                            ddr_print("N%d.LMC%d.R%d: ops %lu, cycles %lu, used %lu.%lu%%\n",
                                      node, ddr_interface_num, rankx, sum_dram_ops, sum_dram_dclk,
                                      percent_x10 / 10, percent_x10 % 10);
                        }
#endif
                        if (errors) {
                            debug_print("End WLEV_64 while loop: vref_value %d(0x%x), errors 0x%02x\n",
                                      vref_value, vref_value, errors);
                        }
                    } // end parallel write-leveling block for delay high-order bits

                    if (sw_wlevel_hw) { // if we used HW-assist, we did the ECC byte when approp.
                        VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: HW-assist SWL - no ECC estimate!!!\n",
                               node, ddr_interface_num, rankx);
                        goto no_ecc_estimate;
                    }

                    if ((ddr_interface_bytemask & 0xff) == 0xff) {
                        if (use_ecc) {
                            int save_byte8 = lmc_wlevel_rank.s.byte8; // save original HW delay
                            byte_test_status[8] = WL_HARDWARE; /* say it is H/W delay value */

                            if ((save_byte8 != lmc_wlevel_rank.s.byte3) &&
                                (save_byte8 != lmc_wlevel_rank.s.byte4))
                            {
                                // try to calculate a workable delay using SW bytes 3 and 4 and HW byte 8
                                int test_byte8 = save_byte8;
                                int test_byte8_error;
                                int byte8_error = 0x1f;
                                int adder;
                                int avg_bytes = divide_nint(lmc_wlevel_rank.s.byte3+lmc_wlevel_rank.s.byte4, 2);
                                for (adder = 0; adder<= 32; adder+=8) {
                                    test_byte8_error = _abs((adder+save_byte8) - avg_bytes);
                                    if (test_byte8_error < byte8_error) {
                                        byte8_error = test_byte8_error;
                                        test_byte8 = save_byte8 + adder;
                                    }
                                }

#if SW_WL_CHECK_PATCH
                                // only do the check if we are not using measured VREF
                                if (!measured_vref_flag) {
                                    test_byte8 &= ~1; /* Use only even settings, rounding down... */

                                    // do validity check on the calculated ECC delay value
                                    // this depends on the DIMM type
                                    if (spd_rdimm) { // RDIMM
                                        if (spd_dimm_type != 5) { // but not mini-RDIMM
                                            // it can be > byte4, but should never be > byte3
                                            if (test_byte8 > lmc_wlevel_rank.s.byte3) {
                                                byte_test_status[8] = WL_ESTIMATED; /* say it is still estimated */
                                            }
                                        }
                                    } else { // UDIMM
                                        if ((test_byte8 < lmc_wlevel_rank.s.byte3) ||
                                            (test_byte8 > lmc_wlevel_rank.s.byte4))
                                            { // should never be outside the byte 3-4 range
                                                byte_test_status[8] = WL_ESTIMATED; /* say it is still estimated */
                                            }
                                    }
                                    /*
                                     * Report whenever the calculation appears bad.
                                     * This happens if some of the original values were off, or unexpected geometry
                                     * from DIMM type, or custom circuitry (NIC225E, I am looking at you!).
                                     * We will trust the calculated value, and depend on later testing to catch
                                     * any instances when that value is truly bad.
                                     */
                                    if (byte_test_status[8] == WL_ESTIMATED) { // ESTIMATED means there may be an issue
                                        ddr_print("N%d.LMC%d.R%d: SWL: (%cDIMM): calculated ECC delay unexpected (%d/%d/%d)\n",
                                                  node, ddr_interface_num, rankx, (spd_rdimm?'R':'U'),
                                                  lmc_wlevel_rank.s.byte4, test_byte8, lmc_wlevel_rank.s.byte3);
                                        byte_test_status[8] = WL_HARDWARE;
                                    }
                                }
#endif /* SW_WL_CHECK_PATCH */
                                lmc_wlevel_rank.s.byte8 = test_byte8 & ~1; /* Use only even settings */
                            }

                            if (lmc_wlevel_rank.s.byte8 != save_byte8) {
                                /* Change the status if s/w adjusted the delay */
                                byte_test_status[8] = WL_SOFTWARE; /* Estimated delay */
                            }
                        } else {
                            byte_test_status[8] = WL_HARDWARE; /* H/W delay value */
                            lmc_wlevel_rank.s.byte8 = lmc_wlevel_rank.s.byte0; /* ECC is not used */
                        }
                    } else { /* if ((ddr_interface_bytemask & 0xff) == 0xff) */
                        if (use_ecc) {
                            /* Estimate the ECC byte delay  */
                            lmc_wlevel_rank.s.byte4 |= (lmc_wlevel_rank.s.byte3 & 0x38); // add hi-order to b4
                            if ((lmc_wlevel_rank.s.byte4 & 0x06) < (lmc_wlevel_rank.s.byte3 & 0x06)) // orig b4 < orig b3
                                lmc_wlevel_rank.s.byte4 += 8; // must be next clock
                        } else {
                            lmc_wlevel_rank.s.byte4 = lmc_wlevel_rank.s.byte0; /* ECC is not used */
                        }
                        /* Change the status if s/w adjusted the delay */
                        byte_test_status[4] = WL_SOFTWARE; /* Estimated delay */
                    } /* if ((ddr_interface_bytemask & 0xff) == 0xff) */
                } /* if (wlevel_bitmask_errors == 0) */

            no_ecc_estimate:

                bytes_failed = 0;
                for (byte = 0; byte < 9; ++byte) {
                    /* Don't accumulate errors for untested bytes. */
                    if (!(ddr_interface_bytemask & (1 << byte)))
                        continue;
                    bytes_failed += (byte_test_status[byte] == WL_ESTIMATED);
                }

                 /* Vref training loop is only used for DDR4  */
                if (ddr_type != DDR4_DRAM)
                        break;

                if (bytes_failed == 0) {
                    if (vref_values_count == 0) {
                        vref_values_start = vref_value;
                    }
                    ++vref_values_count;
                    if (vref_values_count > best_vref_values_count) {
                        best_vref_values_count = vref_values_count;
                        best_vref_values_start = vref_values_start;
                        debug_print("N%d.LMC%d.R%d: Vref Training                    (%2d) :    0x%02x <----- ???? -----> 0x%02x\n",
                                    node, ddr_interface_num,
                                  rankx, vref_value, best_vref_values_start,
                                  best_vref_values_start+best_vref_values_count-1);
                    }
                } else {
                    vref_values_count = 0;
                    debug_print("N%d.LMC%d.R%d: Vref Training                    (%2d) :    failed\n",
                                node, ddr_interface_num,
                                rankx, vref_value);
                }
            } /* for (vref_value=0; vref_value<VREF_LIMIT; ++vref_value) */

            /* Determine address of DRAM to test for pass 2 and final test of software write leveling. */
            rank_addr  = active_rank * (1ull << (pbank_lsb - bunk_enable + (interfaces/2)));
            rank_addr |= (ddr_interface_num<<7); /* Map address into proper interface */
            rank_addr = bdk_numa_get_address(node, rank_addr);
            debug_print("N%d.LMC%d.R%d: Active Rank %d Address: 0x%lx\n",
                        node, ddr_interface_num, rankx, active_rank, rank_addr);

            int errors;

            if (bytes_failed) {

#if !DISABLE_SW_WL_PASS_2

                ddr_print("N%d.LMC%d.R%d: Starting SW Write-leveling pass 2\n",
                          node, ddr_interface_num, rankx);
                sw_wl_rank_status = WL_SOFTWARE;

                /* If previous s/w fixups failed then retry using s/w write-leveling. */
                if (wlevel_bitmask_errors == 0) {
                    /* h/w succeeded but previous s/w fixups failed. So retry s/w. */
                    debug_print("N%d.LMC%d.R%d: Retrying software Write-Leveling.\n",
                                node, ddr_interface_num, rankx);
                }

                { // start parallel write-leveling block for delay low-order bits
                    int byte_delay[8];
                    int byte_passed[8];
                    uint64_t bytemask;
                    uint64_t bitmask;
                    int wl_offset;
                    int bytes_todo;

                    for (byte = 0; byte < 8; ++byte) {
                        byte_passed[byte] = 0;
                    }

                    bytes_todo = ddr_interface_bytemask;

                    for (wl_offset = sw_wlevel_offset; wl_offset >= 0; --wl_offset) {
                        debug_print("Starting wl_offset for-loop: %d\n", wl_offset);

                        bytemask = 0;

                        for (byte = 0; byte < 8; ++byte) {
                            byte_delay[byte] = 0;
                            if (!(bytes_todo & (1 << byte))) // this does not contain fully passed bytes
                                continue;

                            byte_passed[byte] = 0; // reset across passes if not fully passed
                            update_wlevel_rank_struct(&lmc_wlevel_rank, byte, 0); // all delays start at 0
                            bitmask = ((!ddr_interface_64b) && (byte == 4)) ? 0x0f: 0xff;
                            bytemask |= bitmask << (8*byte); // set the bytes bits in the bytemask
                        } /* for (byte = 0; byte < 8; ++byte) */

                        while (bytemask != 0) { // start a pass if there is any byte lane to test

                            debug_print("Starting bytemask while-loop: 0x%lx\n", bytemask);

                            // write this set of WL delays
                            DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx), lmc_wlevel_rank.u);
                            lmc_wlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx));

                            bdk_watchdog_poke();

                            // do the test
                            if (sw_wlevel_hw)
                                errors = run_best_hw_patterns(node, ddr_interface_num, rank_addr,
                                                              DBTRAIN_TEST, NULL);
                            else
                                errors = test_dram_byte(node, ddr_interface_num, rank_addr, bytemask, NULL);

                            debug_print("SWL pass 2: test_dram_byte returned 0x%x\n", errors);

                            // check errors by byte
                            for (byte = 0; byte < 8; ++byte) {
                                if (!(bytes_todo & (1 << byte)))
                                    continue;

                                delay = byte_delay[byte];
                                if (errors & (1 << byte)) { // yes, an error
                                    debug_print("        byte %d delay %2d Errors\n", byte, delay);
                                    byte_passed[byte] = 0;
                                } else { // no error
                                    byte_passed[byte] += 1;
                                    if (byte_passed[byte] == (1 + wl_offset)) { /* Look for consecutive working settings */
                                        debug_print("        byte %d delay %2d FULLY Passed\n", byte, delay);
                                        if (wl_offset == 1) {
                                            byte_test_status[byte] = WL_SOFTWARE;
                                        } else if (wl_offset == 0) {
                                            byte_test_status[byte] = WL_SOFTWARE1;
                                        }
                                        bytemask &= ~(0xffULL << (8*byte)); // test no longer, remove from byte mask this pass
                                        bytes_todo &= ~(1 << byte); // remove completely from concern
                                        continue; // on to the next byte, bypass delay updating!!
                                    } else {
                                        debug_print("        byte %d delay %2d Passed\n", byte, delay);
                                    }
                                }
                                // error or no, here we move to the next delay value for this byte, unless done all delays
                                // only a byte that has "fully passed" will bypass around this,
                                delay += 2;
                                if (delay < 32) {
                                    update_wlevel_rank_struct(&lmc_wlevel_rank, byte, delay);
                                    debug_print("        byte %d delay %2d New\n", byte, delay);
                                    byte_delay[byte] = delay;
                                } else {
                                    // reached max delay, done with this byte
                                    debug_print("        byte %d delay %2d Exhausted\n", byte, delay);
                                    bytemask &= ~(0xffULL << (8*byte)); // test no longer, remove from byte mask this pass
                                }
                            } /* for (byte = 0; byte < 8; ++byte) */
                            debug_print("End of for-loop: bytemask 0x%lx\n", bytemask);

                        } /* while (bytemask != 0) */
                    } /* for (wl_offset = sw_wlevel_offset; wl_offset >= 0; --wl_offset) */

                    for (byte = 0; byte < 8; ++byte) {
                        // any bytes left in bytes_todo did not pass
                        if (bytes_todo & (1 << byte)) {
                            /* Last resort. Use Rlevel settings to estimate
                               Wlevel if software write-leveling fails */
                            debug_print("Using RLEVEL as WLEVEL estimate for byte %d\n", byte);
                            lmc_rlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_RLEVEL_RANKX(ddr_interface_num, rankx));
                            rlevel_to_wlevel(&lmc_rlevel_rank, &lmc_wlevel_rank, byte);
                        }
                    } /* for (byte = 0; byte < 8; ++byte) */

                    sw_wl_failed = (bytes_todo != 0);

                } // end parallel write-leveling block for delay low-order bits

                if (use_ecc) {
                    /* ECC byte has to be estimated. Take the average of the two surrounding bytes. */
                    int test_byte8 = divide_nint(lmc_wlevel_rank.s.byte3
                                                 + lmc_wlevel_rank.s.byte4
                                                 + 2 /* round-up*/ , 2);
                    lmc_wlevel_rank.s.byte8 = test_byte8 & ~1; /* Use only even settings */
                    byte_test_status[8] = WL_ESTIMATED; /* Estimated delay */
                } else {
                    byte_test_status[8] = WL_HARDWARE; /* H/W delay value */
                    lmc_wlevel_rank.s.byte8 = lmc_wlevel_rank.s.byte0; /* ECC is not used */
                }

                /* Set delays for unused bytes to match byte 0. */
                for (byte=0; byte<8; ++byte) {
                    if ((ddr_interface_bytemask & (1 << byte)))
                        continue;
                    update_wlevel_rank_struct(&lmc_wlevel_rank, byte,
                                              lmc_wlevel_rank.s.byte0);
                    byte_test_status[byte] = WL_SOFTWARE;
                }
#else /* !DISABLE_SW_WL_PASS_2 */
                // FIXME? the big hammer, did not even try SW WL pass2, assume only chip reset will help
                ddr_print("N%d.LMC%d.R%d: S/W write-leveling pass 1 failed\n",
                          node, ddr_interface_num, rankx);
                sw_wl_failed = 1;
#endif /* !DISABLE_SW_WL_PASS_2 */

            } else { /* if (bytes_failed) */

                // SW WL pass 1 was OK, write the settings
                DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx), lmc_wlevel_rank.u);
                lmc_wlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx));

#if SW_WL_CHECK_PATCH
                // do validity check on the delay values by running the test 1 more time...
                // FIXME: we really need to check the ECC byte setting here as well,
                //        so we need to enable ECC for this test!!!
                // if there are any errors, claim SW WL failure
                {
                    uint64_t datamask = (ddr_interface_64b) ? 0xffffffffffffffffULL : 0x00000000ffffffffULL;

                    // do the test
                    if (sw_wlevel_hw) {
                        errors = run_best_hw_patterns(node, ddr_interface_num, rank_addr,
                                                      DBTRAIN_TEST, NULL) & 0x0ff;
                    } else {
#if USE_ORIG_TEST_DRAM_BYTE
                        errors = test_dram_byte(node, ddr_interface_num, rank_addr, datamask, NULL);
#else
                        errors = dram_tuning_mem_xor(node, ddr_interface_num, rank_addr, datamask, NULL);
#endif
                    }

                    if (errors) {
                        ddr_print("N%d.LMC%d.R%d: Wlevel Rank Final Test errors 0x%x\n",
                              node, ddr_interface_num, rankx, errors);
                        sw_wl_failed = 1;
                    }
                }
#endif /* SW_WL_CHECK_PATCH */

            } /* if (bytes_failed) */

            // FIXME? dump the WL settings, so we get more of a clue as to what happened where
            ddr_print("N%d.LMC%d.R%d: Wlevel Rank %#4x, 0x%016llX  : %2d%3s %2d%3s %2d%3s %2d%3s %2d%3s %2d%3s %2d%3s %2d%3s %2d%3s %s\n",
                      node, ddr_interface_num, rankx,
                      lmc_wlevel_rank.s.status,
                      lmc_wlevel_rank.u,
                      lmc_wlevel_rank.s.byte8, wl_status_strings[byte_test_status[8]],
                      lmc_wlevel_rank.s.byte7, wl_status_strings[byte_test_status[7]],
                      lmc_wlevel_rank.s.byte6, wl_status_strings[byte_test_status[6]],
                      lmc_wlevel_rank.s.byte5, wl_status_strings[byte_test_status[5]],
                      lmc_wlevel_rank.s.byte4, wl_status_strings[byte_test_status[4]],
                      lmc_wlevel_rank.s.byte3, wl_status_strings[byte_test_status[3]],
                      lmc_wlevel_rank.s.byte2, wl_status_strings[byte_test_status[2]],
                      lmc_wlevel_rank.s.byte1, wl_status_strings[byte_test_status[1]],
                      lmc_wlevel_rank.s.byte0, wl_status_strings[byte_test_status[0]],
                      (sw_wl_rank_status == WL_HARDWARE) ? "" : "(s)"
                      );

            // finally, check for fatal conditions: either chip reset right here, or return error flag
            if (((ddr_type == DDR4_DRAM) && (best_vref_values_count == 0)) || sw_wl_failed) {
                if (!ddr_disable_chip_reset) { // do chip RESET
                    error_print("INFO: Short memory test indicates a retry is needed on N%d.LMC%d.R%d. Resetting node...\n",
                                node, ddr_interface_num, rankx);
                    bdk_wait_usec(500000);
                    bdk_reset_chip(node);
                } else { // return error flag so LMC init can be retried...
                    ddr_print("INFO: Short memory test indicates a retry is needed on N%d.LMC%d.R%d. Restarting LMC init...\n",
                              node, ddr_interface_num, rankx);
                    return 0; // 0 indicates restart possible...
                }
            }

            active_rank++;
        } /* for (rankx = 0; rankx < dimm_count * 4; rankx++) */

        // Finalize the write-leveling settings
        for (rankx = 0; rankx < dimm_count * 4;rankx++) {
            uint64_t value;
            int parameter_set = 0;
            if (!(rank_mask & (1 << rankx)))
                continue;

            lmc_wlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx));

            for (i=0; i<9; ++i) {
                if ((s = lookup_env_parameter("ddr%d_wlevel_rank%d_byte%d", ddr_interface_num, rankx, i)) != NULL) {
                    parameter_set |= 1;
                    value = strtoul(s, NULL, 0);

                    update_wlevel_rank_struct(&lmc_wlevel_rank, i, value);
                }
            }

            if ((s = lookup_env_parameter_ull("ddr%d_wlevel_rank%d", ddr_interface_num, rankx)) != NULL) {
                parameter_set |= 1;
                value = strtoull(s, NULL, 0);
                lmc_wlevel_rank.u = value;
            }

            if (parameter_set) {
                DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx), lmc_wlevel_rank.u);
                lmc_wlevel_rank.u = BDK_CSR_READ(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, rankx));
                display_WL(node, ddr_interface_num, lmc_wlevel_rank, rankx);
            }
#if WLEXTRAS_PATCH
            if ((rank_mask & 0x0F) != 0x0F) { // if there are unused entries to be filled
                if (rankx < 3) {
                    debug_print("N%d.LMC%d.R%d: checking for WLEVEL_RANK unused entries.\n",
                              node, ddr_interface_num, rankx);
                    if (rankx == 0) { // if rank 0, write ranks 1 and 2 here if empty
                        if (!(rank_mask & (1<<1))) { // check that rank 1 is empty
                            DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, 1), lmc_wlevel_rank.u);
                            VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: writing WLEVEL_RANK unused entry R%d.\n",
                                      node, ddr_interface_num, rankx, 1);
                        }
                        if (!(rank_mask & (1<<2))) { // check that rank 2 is empty
                            VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: writing WLEVEL_RANK unused entry R%d.\n",
                                      node, ddr_interface_num, rankx, 2);
                            DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, 2), lmc_wlevel_rank.u);
                        }
                    }
                    // if rank 0, 1 or 2, write rank 3 here if empty
                    if (!(rank_mask & (1<<3))) { // check that rank 3 is empty
                        VB_PRT(VBL_DEV, "N%d.LMC%d.R%d: writing WLEVEL_RANK unused entry R%d.\n",
                                  node, ddr_interface_num, rankx, 3);
                        DRAM_CSR_WRITE(node, BDK_LMCX_WLEVEL_RANKX(ddr_interface_num, 3), lmc_wlevel_rank.u);
                    }
                }
            }
#endif /* WLEXTRAS_PATCH */

        } /* for (rankx = 0; rankx < dimm_count * 4;rankx++) */

        /* Restore the ECC configuration */
        if (!sw_wlevel_hw_default) {
            lmc_config.s.ecc_ena = use_ecc;
            DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(ddr_interface_num), lmc_config.u);
        }

#if USE_L2_WAYS_LIMIT
        /* Restore the l2 set configuration */
        if ((s = lookup_env_parameter("limit_l2_ways")) != NULL) {
            int ways = strtoul(s, NULL, 10);
            limit_l2_ways(node, ways, 1);
        } else {
            limit_l2_ways(node, bdk_l2c_get_num_assoc(node), 0);
        }
#endif

    } // End Software Write-Leveling block

#if ENABLE_DISPLAY_MPR_PAGE
    if (ddr_type == DDR4_DRAM) {
            Display_MPR_Page(node, rank_mask, ddr_interface_num, dimm_count, 2);
            Display_MPR_Page(node, rank_mask, ddr_interface_num, dimm_count, 0);
    }
#endif

#ifdef CAVIUM_ONLY
    {
        int _i;
        int setting[9];
        bdk_lmcx_dll_ctl3_t ddr_dll_ctl3;
        ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));

        for (_i=0; _i<9; ++_i) {
            SET_DDR_DLL_CTL3(dll90_byte_sel, ENCODE_DLL90_BYTE_SEL(_i));
            DRAM_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num),        ddr_dll_ctl3.u);
            BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
            ddr_dll_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_DLL_CTL3(ddr_interface_num));
            setting[_i] = GET_DDR_DLL_CTL3(dll90_setting);
            debug_print("%d. LMC%d_DLL_CTL3[%d] = %016lx %d\n", _i, ddr_interface_num,
                      GET_DDR_DLL_CTL3(dll90_byte_sel), ddr_dll_ctl3.u, setting[_i]);
        }

        VB_PRT(VBL_DEV, "N%d.LMC%d: %-36s : %5d %5d %5d %5d %5d %5d %5d %5d %5d\n",
               node, ddr_interface_num, "DLL90 Setting 8:0",
               setting[8], setting[7], setting[6], setting[5], setting[4],
               setting[3], setting[2], setting[1], setting[0]);

        //BDK_CSR_WRITE(node, BDK_LMCX_DLL_CTL3(ddr_interface_num),        save_ddr_dll_ctl3.u);
    }
#endif  /* CAVIUM_ONLY */

    // any custom DLL read or write offsets, install them
    // FIXME: no need to do these if we are going to auto-tune... ???

    process_custom_dll_offsets(node, ddr_interface_num, "ddr_dll_write_offset",
                               custom_lmc_config->dll_write_offset, "ddr%d_dll_write_offset_byte%d", 1);
    process_custom_dll_offsets(node, ddr_interface_num, "ddr_dll_read_offset",
                               custom_lmc_config->dll_read_offset,  "ddr%d_dll_read_offset_byte%d",  2);

    // we want to train write bit-deskew here...
    if (! disable_deskew_training) {
        if (enable_write_deskew) {
            ddr_print("N%d.LMC%d: WRITE BIT-DESKEW feature training begins.\n",
                      node, ddr_interface_num);
            Perform_Write_Deskew_Training(node, ddr_interface_num);
        } /* if (enable_write_deskew) */
    } /* if (! disable_deskew_training) */

    /*
     * 6.9.14 Final LMC Initialization
     *
     * Early LMC initialization, LMC write-leveling, and LMC read-leveling
     * must be completed prior to starting this final LMC initialization.
     *
     * LMC hardware updates the LMC(0)_SLOT_CTL0, LMC(0)_SLOT_CTL1,
     * LMC(0)_SLOT_CTL2 CSRs with minimum values based on the selected
     * readleveling and write-leveling settings. Software should not write
     * the final LMC(0)_SLOT_CTL0, LMC(0)_SLOT_CTL1, and LMC(0)_SLOT_CTL2
     * values until after the final read-leveling and write-leveling settings
     * are written.
     *
     * Software must ensure the LMC(0)_SLOT_CTL0, LMC(0)_SLOT_CTL1, and
     * LMC(0)_SLOT_CTL2 CSR values are appropriate for this step. These CSRs
     * select the minimum gaps between read operations and write operations
     * of various types.
     *
     * Software must not reduce the values in these CSR fields below the
     * values previously selected by the LMC hardware (during write-leveling
     * and read-leveling steps above).
     *
     * All sections in this chapter may be used to derive proper settings for
     * these registers.
     *
     * For minimal read latency, L2C_CTL[EF_ENA,EF_CNT] should be programmed
     * properly. This should be done prior to the first read.
     */

#if ENABLE_SLOT_CTL_ACCESS
    {
        bdk_lmcx_slot_ctl0_t lmc_slot_ctl0;
        bdk_lmcx_slot_ctl1_t lmc_slot_ctl1;
        bdk_lmcx_slot_ctl2_t lmc_slot_ctl2;
        bdk_lmcx_slot_ctl3_t lmc_slot_ctl3;

        lmc_slot_ctl0.u = BDK_CSR_READ(node, BDK_LMCX_SLOT_CTL0(ddr_interface_num));
        lmc_slot_ctl1.u = BDK_CSR_READ(node, BDK_LMCX_SLOT_CTL1(ddr_interface_num));
        lmc_slot_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_SLOT_CTL2(ddr_interface_num));
        lmc_slot_ctl3.u = BDK_CSR_READ(node, BDK_LMCX_SLOT_CTL3(ddr_interface_num));

        ddr_print("%-45s : 0x%016lx\n", "LMC_SLOT_CTL0", lmc_slot_ctl0.u);
        ddr_print("%-45s : 0x%016lx\n", "LMC_SLOT_CTL1", lmc_slot_ctl1.u);
        ddr_print("%-45s : 0x%016lx\n", "LMC_SLOT_CTL2", lmc_slot_ctl2.u);
        ddr_print("%-45s : 0x%016lx\n", "LMC_SLOT_CTL3", lmc_slot_ctl3.u);

        // for now, look only for SLOT_CTL1 envvar for override of contents
        if ((s = lookup_env_parameter("ddr%d_slot_ctl1", ddr_interface_num)) != NULL) {
            int slot_ctl1_incr = strtoul(s, NULL, 0);
            // validate the value
            if ((slot_ctl1_incr < 0) || (slot_ctl1_incr > 3)) { // allow 0 for printing only
                error_print("ddr%d_slot_ctl1 illegal value (%d); must be 0-3\n",
                            ddr_interface_num, slot_ctl1_incr);
            } else {

#define INCR(csr, chip, field, incr)  \
                csr.chip.field = (csr.chip.field < (64 - incr)) ? (csr.chip.field + incr) : 63

                // only print original when we are changing it!
                if (slot_ctl1_incr)
                    ddr_print("%-45s : 0x%016lx\n", "LMC_SLOT_CTL1", lmc_slot_ctl1.u);

                // modify all the SLOT_CTL1 fields by the increment, for now...
                // but make sure the value will not overflow!!!
                INCR(lmc_slot_ctl1, s, r2r_xrank_init, slot_ctl1_incr);
                INCR(lmc_slot_ctl1, s, r2w_xrank_init, slot_ctl1_incr);
                INCR(lmc_slot_ctl1, s, w2r_xrank_init, slot_ctl1_incr);
                INCR(lmc_slot_ctl1, s, w2w_xrank_init, slot_ctl1_incr);
                DRAM_CSR_WRITE(node, BDK_LMCX_SLOT_CTL1(ddr_interface_num), lmc_slot_ctl1.u);
                lmc_slot_ctl1.u = BDK_CSR_READ(node, BDK_LMCX_SLOT_CTL1(ddr_interface_num));

                // always print when we are changing it!
                printf("%-45s : 0x%016lx\n", "LMC_SLOT_CTL1", lmc_slot_ctl1.u);
            }
        }
    }
#endif /* ENABLE_SLOT_CTL_ACCESS */
    {
        /* Clear any residual ECC errors */
        int num_tads = 1;
        int tad;

        DRAM_CSR_WRITE(node, BDK_LMCX_INT(ddr_interface_num), -1ULL);
        BDK_CSR_READ(node, BDK_LMCX_INT(ddr_interface_num));

        for (tad=0; tad<num_tads; tad++)
            DRAM_CSR_WRITE(node, BDK_L2C_TADX_INT_W1C(tad), BDK_CSR_READ(node, BDK_L2C_TADX_INT_W1C(tad)));

        ddr_print("%-45s : 0x%08llx\n", "LMC_INT",
                  BDK_CSR_READ(node, BDK_LMCX_INT(ddr_interface_num)));

    }

    // Now we can enable scrambling if desired...
    {
        bdk_lmcx_control_t lmc_control;
        bdk_lmcx_scramble_cfg0_t lmc_scramble_cfg0;
        bdk_lmcx_scramble_cfg1_t lmc_scramble_cfg1;
        bdk_lmcx_scramble_cfg2_t lmc_scramble_cfg2;
        bdk_lmcx_ns_ctl_t lmc_ns_ctl;

        lmc_control.u = BDK_CSR_READ(node, BDK_LMCX_CONTROL(ddr_interface_num));
        lmc_scramble_cfg0.u = BDK_CSR_READ(node, BDK_LMCX_SCRAMBLE_CFG0(ddr_interface_num));
        lmc_scramble_cfg1.u = BDK_CSR_READ(node, BDK_LMCX_SCRAMBLE_CFG1(ddr_interface_num));
        lmc_scramble_cfg2.u = BDK_CSR_READ(node, BDK_LMCX_SCRAMBLE_CFG2(ddr_interface_num));
        lmc_ns_ctl.u = BDK_CSR_READ(node, BDK_LMCX_NS_CTL(ddr_interface_num));

        /* Read the scramble setting from the config and see if we
           need scrambling */
        int use_scramble = bdk_config_get_int(BDK_CONFIG_DRAM_SCRAMBLE);
        if (use_scramble == 2)
        {
            if (bdk_trust_get_level() >= BDK_TRUST_LEVEL_SIGNED)
                use_scramble = 1;
            else
                use_scramble = 0;
        }

        /* Generate random values if scrambling is needed */
        if (use_scramble)
        {
            lmc_scramble_cfg0.u = bdk_rng_get_random64();
            lmc_scramble_cfg1.u = bdk_rng_get_random64();
            lmc_scramble_cfg2.u = bdk_rng_get_random64();
            lmc_ns_ctl.s.ns_scramble_dis = 0;
            lmc_ns_ctl.s.adr_offset = 0;
            lmc_control.s.scramble_ena = 1;
        }

        if ((s = lookup_env_parameter_ull("ddr_scramble_cfg0")) != NULL) {
            lmc_scramble_cfg0.u    = strtoull(s, NULL, 0);
            lmc_control.s.scramble_ena = 1;
        }
        ddr_print("%-45s : 0x%016llx\n", "LMC_SCRAMBLE_CFG0", lmc_scramble_cfg0.u);

        DRAM_CSR_WRITE(node, BDK_LMCX_SCRAMBLE_CFG0(ddr_interface_num), lmc_scramble_cfg0.u);

        if ((s = lookup_env_parameter_ull("ddr_scramble_cfg1")) != NULL) {
            lmc_scramble_cfg1.u    = strtoull(s, NULL, 0);
            lmc_control.s.scramble_ena = 1;
        }
        ddr_print("%-45s : 0x%016llx\n", "LMC_SCRAMBLE_CFG1", lmc_scramble_cfg1.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_SCRAMBLE_CFG1(ddr_interface_num), lmc_scramble_cfg1.u);

        if ((s = lookup_env_parameter_ull("ddr_scramble_cfg2")) != NULL) {
            lmc_scramble_cfg2.u    = strtoull(s, NULL, 0);
            lmc_control.s.scramble_ena = 1;
        }
        ddr_print("%-45s : 0x%016llx\n", "LMC_SCRAMBLE_CFG2", lmc_scramble_cfg2.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_SCRAMBLE_CFG2(ddr_interface_num), lmc_scramble_cfg2.u);

        if ((s = lookup_env_parameter_ull("ddr_ns_ctl")) != NULL) {
            lmc_ns_ctl.u    = strtoull(s, NULL, 0);
        }
        ddr_print("%-45s : 0x%016llx\n", "LMC_NS_CTL", lmc_ns_ctl.u);
        DRAM_CSR_WRITE(node, BDK_LMCX_NS_CTL(ddr_interface_num), lmc_ns_ctl.u);

        DRAM_CSR_WRITE(node, BDK_LMCX_CONTROL(ddr_interface_num), lmc_control.u);

    }

    return(mem_size_mbytes);
}
