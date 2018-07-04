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
#include "dram-internal.h"

#include <string.h>
#include <lame_string.h>                /* for strtoul */
#include <libbdk-hal/bdk-atomic.h>
#include <libbdk-hal/bdk-clock.h>
#include <libbdk-hal/bdk-rng.h>
#include <libbdk-os/bdk-init.h>

// if enhanced verbosity levels are defined, use them 
#if defined(VB_PRT)
#define ddr_print2(format, ...) VB_PRT(VBL_FAE,  format, ##__VA_ARGS__)
#define ddr_print3(format, ...) VB_PRT(VBL_TME,  format, ##__VA_ARGS__)
#define ddr_print4(format, ...) VB_PRT(VBL_DEV,  format, ##__VA_ARGS__)
#define ddr_print5(format, ...) VB_PRT(VBL_DEV3, format, ##__VA_ARGS__)
#else
#define ddr_print2 ddr_print
#define ddr_print4 ddr_print
#define ddr_print5 ddr_print
#endif

static  int64_t test_dram_byte_threads_done;
static uint64_t test_dram_byte_threads_errs;
static uint64_t test_dram_byte_lmc_errs[4];

#if 0
/*
 * Suggested testing patterns.
 */
static const uint64_t test_pattern_2[] = {
    0xFFFFFFFFFFFFFFFFULL,
    0xAAAAAAAAAAAAAAAAULL,
    0xFFFFFFFFFFFFFFFFULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0xFFFFFFFFFFFFFFFFULL,
    0xAAAAAAAAAAAAAAAAULL,
    0xFFFFFFFFFFFFFFFFULL,
    0x5555555555555555ULL,
    0xFFFFFFFFFFFFFFFFULL,
    0x5555555555555555ULL,
    0xAAAAAAAAAAAAAAAAULL,
    0x5555555555555555ULL,
    0xFFFFFFFFFFFFFFFFULL,
    0x5555555555555555ULL,
};
 /*
 *  or possibly
 */
static const uint64_t test_pattern_3[] = {
    0xFDFDFDFDFDFDFDFDULL,
    0x8787878787878787ULL,
    0xFEFEFEFEFEFEFEFEULL,
    0xC3C3C3C3C3C3C3C3ULL,
    0x7F7F7F7F7F7F7F7FULL,
    0xE1E1E1E1E1E1E1E1ULL,
    0xBFBFBFBFBFBFBFBFULL,
    0xF0F0F0F0F0F0F0F0ULL,
    0xDFDFDFDFDFDFDFDFULL,
    0x7878787878787878ULL,
    0xEFEFEFEFEFEFEFEFULL,
    0x3C3C3C3C3C3C3C3CULL,
    0xF7F7F7F7F7F7F7F7ULL,
    0x1E1E1E1E1E1E1E1EULL,
    0xFBFBFBFBFBFBFBFBULL,
    0x0F0F0F0F0F0F0F0FULL,
};

static const uint64_t test_pattern_1[] = {
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
#if 0 // only need a cacheline size
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
#endif
};

// setup default for test pattern array
static const uint64_t *dram_tune_test_pattern = test_pattern_1;
#endif

// set this to 1 to shorten the testing to exit when all byte lanes have errors
// having this at 0 forces the testing to take place over the entire range every iteration,
// hopefully ensuring an even load on the memory subsystem 
#define EXIT_WHEN_ALL_LANES_HAVE_ERRORS 0

#define DEFAULT_TEST_BURSTS 5 // FIXME: this is what works so far...// FIXME: was 7
int dram_tune_use_bursts = DEFAULT_TEST_BURSTS;

// dram_tune_rank_offset is used to offset the second area used in test_dram_mem_xor.
//
// If only a single-rank DIMM, the offset will be 256MB from the start of the first area,
//  which is more than enough for the restricted looping/address range actually tested...
//
// If a 2-rank DIMM, the offset will be the size of a rank's address space, so the effect
//  will be to have the first and second areas in different ranks on the same DIMM.
//
// So, we default this to single-rank, and it will be overridden when 2-ranks are detected.
//

// FIXME: ASSUME that we have DIMMS no less than 4GB in size

// offset to first area that avoids any boot stuff in low range (below 256MB)
#define AREA_BASE_OFFSET (1ULL << 28) // bit 28 always ON

// offset to duplicate area; may coincide with rank 1 base address for 2-rank 4GB DIMM
#define AREA_DUPE_OFFSET (1ULL << 31) // bit 31 always ON

// defaults to DUPE, but will be set elsewhere to offset to next RANK if multi-rank DIMM
static uint64_t dram_tune_rank_offset = AREA_DUPE_OFFSET; // default

// defaults to 0, but will be set elsewhere to the address offset to next DIMM if multi-slot
static uint64_t dram_tune_dimm_offset = 0; // default


static int speed_bin_offset[3] = {25, 20, 15};
static int speed_bin_winlen[3] = {70, 60, 60};

static int
get_speed_bin(bdk_node_t node, int lmc)
{
    uint32_t mts_speed = (libdram_get_freq_from_pll(node, lmc) / 1000000) * 2;
    int ret = 0;

    // FIXME: is this reasonable speed "binning"?
    if (mts_speed >= 1700) {
        if (mts_speed >= 2000)
            ret = 2;
        else
            ret = 1;
    }

    debug_print("N%d.LMC%d: %s: returning bin %d for MTS %d\n", 
                node, lmc, __FUNCTION__, ret, mts_speed);

    return ret;
}

static int is_low_risk_offset(int speed_bin, int offset)
{
    return (_abs(offset) <= speed_bin_offset[speed_bin]);
}
static int is_low_risk_winlen(int speed_bin, int winlen)
{
    return (winlen >= speed_bin_winlen[speed_bin]);
}

#define ENABLE_PREFETCH 0
#define ENABLE_WBIL2    1
#define ENABLE_SBLKDTY  0

#define BDK_SYS_CVMCACHE_INV_L2 "#0,c11,c1,#1"          // L2 Cache Invalidate
#define BDK_CACHE_INV_L2(address) { asm volatile ("sys " BDK_SYS_CVMCACHE_INV_L2 ", %0" : : "r" (address)); }

int dram_tuning_mem_xor(bdk_node_t node, int lmc, uint64_t p, uint64_t bitmask, uint64_t *xor_data)
{
    uint64_t p1, p2, d1, d2;
    uint64_t v, v1;
    uint64_t p2offset = 0x10000000/* was: dram_tune_rank_offset; */; // FIXME?
    uint64_t datamask;
    uint64_t xor;
    uint64_t i, j, k;
    uint64_t ii;
    int errors = 0;
    //uint64_t index;
    uint64_t pattern1 = bdk_rng_get_random64();
    uint64_t pattern2 = 0;
    uint64_t bad_bits[2] = {0,0};

#if ENABLE_SBLKDTY
    BDK_CSR_MODIFY(c, node, BDK_L2C_CTL, c.s.dissblkdty = 0);
#endif

    // Byte lanes may be clear in the mask to indicate no testing on that lane.
    datamask = bitmask;

    // final address must include LMC and node
    p |= (lmc<<7); /* Map address into proper interface */
    p = bdk_numa_get_address(node, p); /* Map to node */

    /* Add offset to both test regions to not clobber boot stuff
     * when running from L2 for NAND boot.
     */
    p += AREA_BASE_OFFSET; // make sure base is out of the way of boot

#define II_INC (1ULL << 29)
#define II_MAX (1ULL << 31)
#define K_INC  (1ULL << 14)
#define K_MAX  (1ULL << 20)
#define J_INC  (1ULL <<  9)
#define J_MAX  (1ULL << 12)
#define I_INC  (1ULL <<  3)
#define I_MAX  (1ULL <<  7)

    debug_print("N%d.LMC%d: dram_tuning_mem_xor: phys_addr=0x%lx\n", 
              node, lmc, p);

#if 0
    int ix;
    // add this loop to fill memory with the test pattern first
    // loops are ordered so that only entire cachelines are written 
    for (ii = 0; ii < II_MAX; ii += II_INC) { // FIXME? extend the range of memory tested!!
        for (k = 0; k < K_MAX; k += K_INC) {
            for (j = 0; j < J_MAX; j += J_INC) {
                p1 = p + ii + k + j;
                p2 = p1 + p2offset;
                for (i = 0, ix = 0; i < I_MAX; i += I_INC, ix++) {

                    v = dram_tune_test_pattern[ix];
                    v1 = v; // write the same thing to both areas

                    __bdk_dram_write64(p1 + i, v);
                    __bdk_dram_write64(p2 + i, v1);

                }
#if ENABLE_WBIL2
                BDK_CACHE_WBI_L2(p1);
                BDK_CACHE_WBI_L2(p2);
#endif
            }
        }
    } /* for (ii = 0; ii < (1ULL << 31); ii += (1ULL << 29)) */
#endif

#if ENABLE_PREFETCH
    BDK_PREFETCH(p           , BDK_CACHE_LINE_SIZE);
    BDK_PREFETCH(p + p2offset, BDK_CACHE_LINE_SIZE);
#endif

    // loops are ordered so that only a single 64-bit slot is written to each cacheline at one time,
    // then the cachelines are forced out; this should maximize read/write traffic
    for (ii = 0; ii < II_MAX; ii += II_INC) { // FIXME? extend the range of memory tested!!
        for (k = 0; k < K_MAX; k += K_INC) {
            for (i = 0; i < I_MAX; i += I_INC) {
                for (j = 0; j < J_MAX; j += J_INC) {

                    p1 = p + ii + k + j;
                    p2 = p1 + p2offset;

#if ENABLE_PREFETCH
                    if (j < (J_MAX - J_INC)) {
                        BDK_PREFETCH(p1 + J_INC, BDK_CACHE_LINE_SIZE);
                        BDK_PREFETCH(p2 + J_INC, BDK_CACHE_LINE_SIZE);
                    }
#endif

                    v = pattern1 * (p1 + i);
                    v1 = v; // write the same thing to both areas

                    __bdk_dram_write64(p1 + i, v);
                    __bdk_dram_write64(p2 + i, v1);

#if ENABLE_WBIL2
                    BDK_CACHE_WBI_L2(p1);
                    BDK_CACHE_WBI_L2(p2);
#endif
                }
            }
        }
    } /* for (ii = 0; ii < (1ULL << 31); ii += (1ULL << 29)) */

    BDK_DCACHE_INVALIDATE;

    debug_print("N%d.LMC%d: dram_tuning_mem_xor: done INIT loop\n", 
              node, lmc);

    /* Make a series of passes over the memory areas. */

    for (int burst = 0; burst < 1/* was: dram_tune_use_bursts*/; burst++)
    {
        uint64_t this_pattern = bdk_rng_get_random64();
        pattern2 ^= this_pattern;

        /* XOR the data with a random value, applying the change to both
         * memory areas.
         */
#if ENABLE_PREFETCH
        BDK_PREFETCH(p           , BDK_CACHE_LINE_SIZE);
        BDK_PREFETCH(p + p2offset, BDK_CACHE_LINE_SIZE);
#endif

        for (ii = 0; ii < II_MAX; ii += II_INC) { // FIXME? extend the range of memory tested!!
            for (k = 0; k < K_MAX; k += K_INC) {
                for (i = 0; i < I_MAX; i += I_INC) { // FIXME: rearranged, did not make much difference?
                    for (j = 0; j < J_MAX; j += J_INC) {

                        p1 = p + ii + k + j;
                        p2 = p1 + p2offset;

#if ENABLE_PREFETCH
                        if (j < (J_MAX - J_INC)) {
                            BDK_PREFETCH(p1 + J_INC, BDK_CACHE_LINE_SIZE);
                            BDK_PREFETCH(p2 + J_INC, BDK_CACHE_LINE_SIZE);
                        }
#endif

                        v  = __bdk_dram_read64(p1 + i) ^ this_pattern;
                        v1 = __bdk_dram_read64(p2 + i) ^ this_pattern;

#if ENABLE_WBIL2
                        BDK_CACHE_INV_L2(p1);
                        BDK_CACHE_INV_L2(p2);
#endif

                        __bdk_dram_write64(p1 + i, v);
                        __bdk_dram_write64(p2 + i, v1);

#if ENABLE_WBIL2
                        BDK_CACHE_WBI_L2(p1);
                        BDK_CACHE_WBI_L2(p2);
#endif
                    }
                }
            }
        } /* for (ii = 0; ii < (1ULL << 31); ii += (1ULL << 29)) */

        BDK_DCACHE_INVALIDATE;

        debug_print("N%d.LMC%d: dram_tuning_mem_xor: done MODIFY loop\n", 
                  node, lmc);

#if ENABLE_PREFETCH
        BDK_PREFETCH(p           , BDK_CACHE_LINE_SIZE);
        BDK_PREFETCH(p + p2offset, BDK_CACHE_LINE_SIZE);
#endif

        /* Look for differences in the areas. If there is a mismatch, reset
         * both memory locations with the same pattern. Failing to do so
         * means that on all subsequent passes the pair of locations remain
         * out of sync giving spurious errors.
         */
        // FIXME: change the loop order so that an entire cache line is compared at one time
        // FIXME: this is so that a read error that occurs *anywhere* on the cacheline will be caught,
        // FIXME: rather than comparing only 1 cacheline slot at a time, where an error on a different
        // FIXME: slot will be missed that time around
        // Does the above make sense?

        for (ii = 0; ii < II_MAX; ii += II_INC) { // FIXME? extend the range of memory tested!!
            for (k = 0; k < K_MAX; k += K_INC) {
                for (j = 0; j < J_MAX; j += J_INC) {

                    p1 = p + ii + k + j;
                    p2 = p1 + p2offset;

#if ENABLE_PREFETCH
                    if (j < (J_MAX - J_INC)) {
                        BDK_PREFETCH(p1 + J_INC, BDK_CACHE_LINE_SIZE);
                        BDK_PREFETCH(p2 + J_INC, BDK_CACHE_LINE_SIZE);
                    }
#endif

                    // process entire cachelines in the innermost loop
                    for (i = 0; i < I_MAX; i += I_INC) {

                        v = ((p1 + i) * pattern1) ^ pattern2; // FIXME: this should predict what we find...???
                        d1 = __bdk_dram_read64(p1 + i);
                        d2 = __bdk_dram_read64(p2 + i);

                        xor = ((d1 ^ v) | (d2 ^ v)) & datamask; // union of error bits only in active byte lanes

                        if (!xor)
                            continue;

                        // accumulate bad bits
                        bad_bits[0] |= xor;
                        //bad_bits[1] |= ~mpr_data1 & 0xffUL; // cannot do ECC here

                        int bybit = 1;
                        uint64_t bymsk = 0xffULL; // start in byte lane 0
                        while (xor != 0) {
                            debug_print("ERROR(%03d): [0x%016lX] [0x%016lX]  expected 0x%016lX d1 %016lX d2 %016lX\n",
                                        burst, p1, p2, v, d1, d2);
                            if (xor & bymsk) { // error(s) in this lane
                                errors |= bybit; // set the byte error bit
                                xor &= ~bymsk; // clear byte lane in error bits
                                datamask &= ~bymsk; // clear the byte lane in the mask
#if EXIT_WHEN_ALL_LANES_HAVE_ERRORS
                                if (datamask == 0) { // nothing left to do
                                    return errors; // completely done when errors found in all byte lanes in datamask
                                }
#endif /* EXIT_WHEN_ALL_LANES_HAVE_ERRORS */
                            }
                            bymsk <<= 8; // move mask into next byte lane
                            bybit <<= 1; // move bit into next byte position
                        }
                    }
#if ENABLE_WBIL2
                    BDK_CACHE_WBI_L2(p1);
                    BDK_CACHE_WBI_L2(p2);
#endif
                }
            }
        } /* for (ii = 0; ii < (1ULL << 31); ii += (1ULL << 29)) */

        debug_print("N%d.LMC%d: dram_tuning_mem_xor: done TEST loop\n", 
                  node, lmc);

    } /* for (int burst = 0; burst < dram_tune_use_bursts; burst++) */

    if (xor_data != NULL) { // send the bad bits back...
        xor_data[0] = bad_bits[0];
        xor_data[1] = bad_bits[1]; // let it be zeroed
    }

#if ENABLE_SBLKDTY
    BDK_CSR_MODIFY(c, node, BDK_L2C_CTL, c.s.dissblkdty = 1);
#endif

    return errors;
}

#undef II_INC
#undef II_MAX

#define EXTRACT(v, lsb, width) (((v) >> (lsb)) & ((1ull << (width)) - 1))
#define LMCNO(address, xbits) (EXTRACT(address, 7, xbits) ^ EXTRACT(address, 20, xbits) ^ EXTRACT(address, 12, xbits))

// cores to use
#define DEFAULT_USE_CORES 44   // FIXME: was (1 << CORE_BITS)
int dram_tune_use_cores = DEFAULT_USE_CORES; // max cores to use, override available
int dram_tune_max_cores; // max cores available on a node
#define CORE_SHIFT 22          // FIXME: offset into rank_address passed to test_dram_byte

typedef void (*__dram_tuning_thread_t)(int arg, void *arg1);

typedef struct
{
    bdk_node_t node;
    int64_t num_lmcs;
    uint64_t byte_mask;
} test_dram_byte_info_t;

static int dram_tune_use_xor2 = 1; // FIXME: do NOT default to original mem_xor (LMC-based) code

static int
run_dram_tuning_threads(bdk_node_t node, int num_lmcs, uint64_t bytemask)
{
    test_dram_byte_info_t test_dram_byte_info;
    test_dram_byte_info_t *test_info = &test_dram_byte_info;
    int total_count = 0;

    test_info->node = node;
    test_info->num_lmcs = num_lmcs;
    test_info->byte_mask = bytemask;

    // init some global data
    bdk_atomic_set64(&test_dram_byte_threads_done, 0);
    bdk_atomic_set64((int64_t *)&test_dram_byte_threads_errs, 0);
    bdk_atomic_set64((int64_t *)&test_dram_byte_lmc_errs[0], 0);
    bdk_atomic_set64((int64_t *)&test_dram_byte_lmc_errs[1], 0);
    bdk_atomic_set64((int64_t *)&test_dram_byte_lmc_errs[2], 0);
    bdk_atomic_set64((int64_t *)&test_dram_byte_lmc_errs[3], 0);

    /* Start threads for cores on the node */
    if (bdk_numa_exists(node)) {
        /* FIXME(dhendrix): We shouldn't hit this. */
        die("bdk_numa_exists() is non-zero\n");
    }

#if 0
    /* Wait for threads to finish */
    while (bdk_atomic_get64(&test_dram_byte_threads_done) < total_count)
        bdk_thread_yield();
#else
#define TIMEOUT_SECS 5  // FIXME: long enough so a pass for a given setting will not print
        /* Wait for threads to finish, with progress */
        int cur_count;
        uint64_t cur_time;
        uint64_t period = bdk_clock_get_rate(bdk_numa_local(), BDK_CLOCK_TIME) * TIMEOUT_SECS; // FIXME? 
        uint64_t timeout = bdk_clock_get_count(BDK_CLOCK_TIME) + period;
        do {
//            bdk_thread_yield();        /* FIXME(dhendrix): don't yield... */
            cur_count = bdk_atomic_get64(&test_dram_byte_threads_done);
            cur_time = bdk_clock_get_count(BDK_CLOCK_TIME);
            if (cur_time >= timeout) {
                printf("Waiting for %d cores\n", total_count - cur_count);
                timeout = cur_time + period;
            }
        } while (cur_count < total_count);
#endif

    // NOTE: this is the summary of errors across all LMCs
    return (int)bdk_atomic_get64((int64_t *)&test_dram_byte_threads_errs);
}

/* These variables count the number of ECC errors. They should only be accessed atomically */
/* FIXME(dhendrix): redundant declaration in original BDK sources */
//extern int64_t __bdk_dram_ecc_single_bit_errors[];
extern int64_t __bdk_dram_ecc_double_bit_errors[];

#define DEFAULT_SAMPLE_GRAN 3 // sample for errors every N offset values
#define MIN_BYTE_OFFSET -63
#define MAX_BYTE_OFFSET +63
int dram_tune_use_gran = DEFAULT_SAMPLE_GRAN;

static int
auto_set_dll_offset(bdk_node_t node, int dll_offset_mode,
                    int num_lmcs, int ddr_interface_64b,
                    int do_tune)
{
    int byte_offset;
    //unsigned short result[9];
    int byte;
    int byte_delay_start[4][9];
    int byte_delay_count[4][9];
    uint64_t byte_delay_windows [4][9];
    int byte_delay_best_start[4][9];
    int byte_delay_best_count[4][9];
    //int this_rodt;
    uint64_t ops_sum[4], dclk_sum[4];
    uint64_t start_dram_dclk[4], stop_dram_dclk[4];
    uint64_t start_dram_ops[4], stop_dram_ops[4];
    int errors, tot_errors;
    int lmc;
    const char *mode_str = (dll_offset_mode == 2) ? "Read" : "Write";        /* FIXME(dhendrix): const */
    int mode_is_read = (dll_offset_mode == 2);
    const char *mode_blk = (dll_offset_mode == 2) ? " " : "";                /* FIXME(dhendrix): const */
    int start_offset, end_offset, incr_offset;

    int speed_bin = get_speed_bin(node, 0); // FIXME: just get from LMC0?
    int low_risk_count = 0, needs_review_count = 0;

    if (dram_tune_use_gran != DEFAULT_SAMPLE_GRAN) {
        ddr_print2("N%d: Changing sample granularity from %d to %d\n",
                  node, DEFAULT_SAMPLE_GRAN, dram_tune_use_gran);
    }
    // ensure sample is taken at 0
    start_offset = MIN_BYTE_OFFSET - (MIN_BYTE_OFFSET % dram_tune_use_gran);
    end_offset   = MAX_BYTE_OFFSET - (MAX_BYTE_OFFSET % dram_tune_use_gran);
    incr_offset  = dram_tune_use_gran;

    memset(ops_sum, 0, sizeof(ops_sum));
    memset(dclk_sum, 0, sizeof(dclk_sum));
    memset(byte_delay_start, 0, sizeof(byte_delay_start));
    memset(byte_delay_count, 0, sizeof(byte_delay_count));
    memset(byte_delay_windows,  0, sizeof(byte_delay_windows));
    memset(byte_delay_best_start, 0, sizeof(byte_delay_best_start));
    memset(byte_delay_best_count, 0, sizeof(byte_delay_best_count));

    // FIXME? consult LMC0 only
    BDK_CSR_INIT(lmcx_config, node, BDK_LMCX_CONFIG(0));
    if (lmcx_config.s.rank_ena) { // replace the default offset when there is more than 1 rank...
        dram_tune_rank_offset = 1ull << (28 + lmcx_config.s.pbank_lsb - lmcx_config.s.rank_ena + (num_lmcs/2));
        /* FIXME(dhendrix): %lx --> %llx */
        ddr_print2("N%d: Tuning multiple ranks per DIMM (rank offset 0x%llx).\n", node, dram_tune_rank_offset);
    }
    if (lmcx_config.s.init_status & 0x0c) { // bit 2 or 3 set indicates 2 DIMMs
        dram_tune_dimm_offset = 1ull << (28 + lmcx_config.s.pbank_lsb + (num_lmcs/2));
        /* FIXME(dhendrix): %lx --> %llx */
        ddr_print2("N%d: Tuning multiple DIMMs per channel (DIMM offset 0x%llx)\n", node, dram_tune_dimm_offset);
    }

    // FIXME? do this for LMC0 only
    //BDK_CSR_INIT(comp_ctl2, node, BDK_LMCX_COMP_CTL2(0));
    //this_rodt = comp_ctl2.s.rodt_ctl;

    // construct the bytemask
    int bytes_todo = (ddr_interface_64b) ? 0xff : 0x0f;
    uint64_t bytemask = 0;
    for (byte = 0; byte < 8; ++byte) {
        if (bytes_todo & (1 << byte)) {
            bytemask |= 0xfful << (8*byte); // set the bytes bits in the bytemask
        }
    } /* for (byte = 0; byte < 8; ++byte) */

    // now loop through selected legal values for the DLL byte offset...

    for (byte_offset = start_offset; byte_offset <= end_offset; byte_offset += incr_offset) {

        // do the setup on active LMCs
        for (lmc = 0; lmc < num_lmcs; lmc++) {
            change_dll_offset_enable(node, lmc, 0);

            // set all byte lanes at once
            load_dll_offset(node, lmc, dll_offset_mode, byte_offset, 10 /* All bytes at once */);
            // but then clear the ECC byte lane so it should be neutral for the test...
            load_dll_offset(node, lmc, dll_offset_mode, 0, 8);

            change_dll_offset_enable(node, lmc, 1);

            // record start cycle CSRs here for utilization measure
            start_dram_dclk[lmc] = BDK_CSR_READ(node, BDK_LMCX_DCLK_CNT(lmc));
            start_dram_ops[lmc]  = BDK_CSR_READ(node, BDK_LMCX_OPS_CNT(lmc));
        } /* for (lmc = 0; lmc < num_lmcs; lmc++) */

        bdk_watchdog_poke();

        // run the test(s)
        // only 1 call should be enough, let the bursts, etc, control the load...
        tot_errors = run_dram_tuning_threads(node, num_lmcs, bytemask);

        for (lmc = 0; lmc < num_lmcs; lmc++) {
            // record stop cycle CSRs here for utilization measure
            stop_dram_dclk[lmc] = BDK_CSR_READ(node, BDK_LMCX_DCLK_CNT(lmc));
            stop_dram_ops[lmc]  = BDK_CSR_READ(node, BDK_LMCX_OPS_CNT(lmc));

            // accumulate...
            ops_sum[lmc]  += stop_dram_ops[lmc]  - start_dram_ops[lmc];
            dclk_sum[lmc] += stop_dram_dclk[lmc] - start_dram_dclk[lmc];

            errors = test_dram_byte_lmc_errs[lmc];

            // check errors by byte, but not ECC
            for (byte = 0; byte < 8; ++byte) {
                if (!(bytes_todo & (1 << byte))) // is this byte lane to be done
                    continue; // no

                byte_delay_windows[lmc][byte] <<= 1; // always put in a zero
                if (errors & (1 << byte)) { // yes, an error in this byte lane
                    byte_delay_count[lmc][byte] = 0; // stop now always
                } else { // no error in this byte lane
                    if (byte_delay_count[lmc][byte] == 0) { // first success, set run start
                        byte_delay_start[lmc][byte] = byte_offset;
                    }
                    byte_delay_count[lmc][byte] += incr_offset; // bump run length

                    if (byte_delay_count[lmc][byte] > byte_delay_best_count[lmc][byte]) {
                        byte_delay_best_count[lmc][byte] = byte_delay_count[lmc][byte];
                        byte_delay_best_start[lmc][byte] = byte_delay_start[lmc][byte];
                    }
                    byte_delay_windows[lmc][byte] |= 1ULL; // for pass, put in a 1
                }
            } /* for (byte = 0; byte < 8; ++byte) */

            // only print when there are errors and verbose...
            if (errors) {
                debug_print("DLL %s Offset Test %3d: errors 0x%x\n",
                            mode_str, byte_offset, errors);
            }
        } /* for (lmc = 0; lmc < num_lmcs; lmc++) */

    } /* for (byte_offset=-63; byte_offset<63; byte_offset += incr_offset) */

    // done with testing, load up and/or print out the offsets we found...

    // only when margining...
    if (!do_tune) {
        printf("  \n");
        printf("-------------------------------------\n");
#if 0
        uint32_t mts_speed = (libdram_get_freq_from_pll(node, 0) * 2) / 1000000; // FIXME: sample LMC0
        printf("N%d: Starting %s Timing Margining for %d MT/s.\n", node, mode_str, mts_speed);
#else
        printf("N%d: Starting %s Timing Margining.\n", node, mode_str);
#endif
        printf("  \n");
    } /* if (!do_tune) */

    for (lmc = 0; lmc < num_lmcs; lmc++) {
#if 1
        // FIXME FIXME
        // FIXME: this just makes ECC always show 0
        byte_delay_best_start[lmc][8] = start_offset;
        byte_delay_best_count[lmc][8] = end_offset - start_offset + incr_offset;
#endif

        // disable offsets while we load...
        change_dll_offset_enable(node, lmc, 0);

        // only when margining...
        if (!do_tune) {
            // print the heading
            printf("  \n");
            printf("N%d.LMC%d: %s Timing Margin     %s : ", node, lmc, mode_str, mode_blk);
            printf("     ECC/8 ");
            for (byte = 7; byte >= 0; byte--) {
                printf("    Byte %d ", byte);
            }
            printf("\n");
        } /* if (!do_tune) */

        // print and load the offset values
        // print the windows bit arrays
        // only when margining...
        if (!do_tune) {
            printf("N%d.LMC%d: DLL %s Offset Amount %s : ", node, lmc, mode_str, mode_blk);
        } else {
            ddr_print("N%d.LMC%d: SW DLL %s Offset Amount %s : ", node, lmc, mode_str, mode_blk);
        }
        for (byte = 8; byte >= 0; --byte) { // print in "normal" reverse index order

            int count = byte_delay_best_count[lmc][byte];
            if (count == 0)
                count = incr_offset; // should make non-tested ECC byte come out 0

            byte_offset =  byte_delay_best_start[lmc][byte] +
                ((count - incr_offset) / 2); // adj by incr

            if (!do_tune) { // do counting and special flag if margining
                int will_need_review = !is_low_risk_winlen(speed_bin, (count - incr_offset)) &&
                                       !is_low_risk_offset(speed_bin, byte_offset);

                printf("%10d%c", byte_offset, (will_need_review) ? '<' :' ');

                if (will_need_review)
                    needs_review_count++;
                else
                    low_risk_count++;
            } else { // if just tuning, make the printout less lengthy
                ddr_print("%5d ", byte_offset);
            }

            // FIXME? should we be able to override this?
            if (mode_is_read) // for READ offsets, always store what we found
                load_dll_offset(node, lmc, dll_offset_mode, byte_offset, byte);
            else // for WRITE offsets, always store 0
                load_dll_offset(node, lmc, dll_offset_mode, 0, byte);

        }
        if (!do_tune) {
            printf("\n");
        } else {
            ddr_print("\n");
        }


        // re-enable the offsets now that we are done loading
        change_dll_offset_enable(node, lmc, 1);

        // only when margining...
        if (!do_tune) {
            // print the window sizes
            printf("N%d.LMC%d: DLL %s Window Length %s : ", node, lmc, mode_str, mode_blk);
            for (byte = 8; byte >= 0; --byte) { // print in "normal" reverse index order
                int count = byte_delay_best_count[lmc][byte];
                if (count == 0)
                    count = incr_offset; // should make non-tested ECC byte come out 0

                // do this again since the "needs review" test is an AND...
                byte_offset =  byte_delay_best_start[lmc][byte] +
                    ((count - incr_offset) / 2); // adj by incr

                int will_need_review = !is_low_risk_winlen(speed_bin, (count - incr_offset)) &&
                    !is_low_risk_offset(speed_bin, byte_offset);

                printf("%10d%c", count - incr_offset, (will_need_review) ? '<' :' ');
            }
            printf("\n");

            // print the window extents
            printf("N%d.LMC%d: DLL %s Window Bounds %s : ", node, lmc, mode_str, mode_blk);
            for (byte = 8; byte >= 0; --byte) { // print in "normal" reverse index order
                int start = byte_delay_best_start[lmc][byte];
                int count = byte_delay_best_count[lmc][byte];
                if (count == 0)
                    count = incr_offset; // should make non-tested ECC byte come out 0
                printf(" %3d to%3d ", start,
                       start + count - incr_offset);
            }
            printf("\n");
#if 0
            // FIXME: should have a way to force these out...
            // print the windows bit arrays
            printf("N%d.LMC%d: DLL %s Window Bitmap%s : ", node, lmc, mode_str, mode_blk);
            for (byte = 8; byte >= 0; --byte) { // print in "normal" reverse index order
                printf("%010lx ", byte_delay_windows[lmc][byte]);
            }
            printf("\n");
#endif
        } /* if (!do_tune) */
    } /* for (lmc = 0; lmc < num_lmcs; lmc++) */

    // only when margining...
    if (!do_tune) {
        // print the Summary line(s) here
        printf("  \n");
        printf("N%d: %s Timing Margining Summary : %s ", node, mode_str,
               (needs_review_count > 0) ? "Needs Review" : "Low Risk");
        if (needs_review_count > 0)
            printf("(%d)", needs_review_count);
        printf("\n");

        // FIXME??? want to print here: "N0: %s Offsets have been applied already"

        printf("-------------------------------------\n");
        printf("  \n");
    } /* if (!do_tune) */

    // FIXME: we probably want this only when doing verbose...
    // finally, print the utilizations all together
    for (lmc = 0; lmc < num_lmcs; lmc++) {
        uint64_t percent_x10 = ops_sum[lmc] * 1000 / dclk_sum[lmc];
        /* FIXME(dhendrix): %lu --> %llu */
        ddr_print2("N%d.LMC%d: ops %llu, cycles %llu, used %llu.%llu%%\n",
                  node, lmc, ops_sum[lmc], dclk_sum[lmc], percent_x10 / 10, percent_x10 % 10);
    } /* for (lmc = 0; lmc < num_lmcs; lmc++) */

    // FIXME: only when verbose, or only when there are errors?
    // run the test one last time 
    // print whether there are errors or not, but only when verbose...
    bdk_watchdog_poke();
    debug_print("N%d: %s: Start running test one last time\n", node, __FUNCTION__);
    tot_errors = run_dram_tuning_threads(node, num_lmcs, bytemask);
    debug_print("N%d: %s: Finished running test one last time\n", node, __FUNCTION__);
    if (tot_errors)
        ddr_print2("%s Timing Final Test: errors 0x%x\n", mode_str, tot_errors);

    return (do_tune) ? tot_errors : !!(needs_review_count > 0);
}

#define USE_L2_WAYS_LIMIT 0 // non-zero to enable L2 ways limiting

/*
 * Automatically adjust the DLL offset for the data bytes
 */
int perform_dll_offset_tuning(bdk_node_t node, int dll_offset_mode, int do_tune)
{
    int ddr_interface_64b;
    int save_ecc_ena[4];
    bdk_lmcx_config_t lmc_config;
    int lmc, num_lmcs = __bdk_dram_get_num_lmc(node);
    const char *s;
#if USE_L2_WAYS_LIMIT
    int ways, ways_print = 0;
#endif
#if 0
    int dram_tune_use_rodt = -1, save_rodt[4];
    bdk_lmcx_comp_ctl2_t comp_ctl2;
#endif
    int loops = 1, loop;
    uint64_t orig_coremask;
    int errs = 0;

    // enable any non-running cores on this node
    orig_coremask = bdk_get_running_coremask(node);
    /* FIXME(dhendrix): %lx --> %llx */
    ddr_print4("N%d: %s: Starting cores (mask was 0x%llx)\n",
              node, __FUNCTION__, orig_coremask);
        /* FIXME(dhendrix): don't call bdk_init_cores(). */
//    bdk_init_cores(node, ~0ULL & ~orig_coremask);
    dram_tune_max_cores = bdk_get_num_running_cores(node);

    // but use only a certain number of cores, at most what is available
    if ((s = getenv("ddr_tune_use_cores")) != NULL) {
        dram_tune_use_cores = strtoul(s, NULL, 0);
        if (dram_tune_use_cores <= 0) // allow 0 or negative to mean all
            dram_tune_use_cores = dram_tune_max_cores;
    }
    if (dram_tune_use_cores > dram_tune_max_cores)
        dram_tune_use_cores = dram_tune_max_cores;

    // see if we want to do the tuning more than once per LMC...
    if ((s = getenv("ddr_tune_use_loops"))) {
        loops = strtoul(s, NULL, 0);
    }

    // see if we want to change the granularity of the byte_offset sampling 
    if ((s = getenv("ddr_tune_use_gran"))) {
        dram_tune_use_gran = strtoul(s, NULL, 0);
    }

    // allow override of the test repeats (bursts) per thread create
    if ((s = getenv("ddr_tune_use_bursts")) != NULL) {
        dram_tune_use_bursts = strtoul(s, NULL, 10);
    }

#if 0
    // allow override of Read ODT setting just during the tuning run(s)
    if ((s = getenv("ddr_tune_use_rodt")) != NULL) {
        int temp = strtoul(s, NULL, 10);
        // validity check
        if (temp >= 0 && temp <= 7)
            dram_tune_use_rodt = temp;
    }
#endif

#if 0
    // allow override of the test pattern
    // FIXME: a bit simplistic...
    if ((s = getenv("ddr_tune_use_pattern")) != NULL) {
        int patno = strtoul(s, NULL, 10);
        if (patno == 2)
            dram_tune_test_pattern = test_pattern_2;
        else if (patno == 3)
            dram_tune_test_pattern = test_pattern_3;
        else // all other values use default
            dram_tune_test_pattern = test_pattern_1;
    }
#endif

    // allow override of the test mem_xor algorithm
    if ((s = getenv("ddr_tune_use_xor2")) != NULL) {
        dram_tune_use_xor2 = !!strtoul(s, NULL, 10);
    }

    // print current working values
    ddr_print2("N%d: Tuning will use %d cores of max %d cores, and use %d repeats.\n",
                node, dram_tune_use_cores, dram_tune_max_cores,
                dram_tune_use_bursts);

#if USE_L2_WAYS_LIMIT
    // see if L2 ways are limited
    if ((s = lookup_env_parameter("limit_l2_ways")) != NULL) {
        ways = strtoul(s, NULL, 10);
        ways_print = 1;
    } else {
        ways = bdk_l2c_get_num_assoc(node);
    }
#endif

#if 0
    // if RODT is to be overridden during tuning, note change
    if (dram_tune_use_rodt >= 0) {
        ddr_print("N%d: using RODT %d for tuning.\n",
                  node, dram_tune_use_rodt);
    }
#endif

    // FIXME? get flag from LMC0 only
    lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(0));
    ddr_interface_64b = !lmc_config.s.mode32b;

    // do setup for each active LMC
    debug_print("N%d: %s: starting LMCs setup.\n", node, __FUNCTION__);
    for (lmc = 0; lmc < num_lmcs; lmc++) {

#if 0
        // if RODT change, save old and set new here...
        if (dram_tune_use_rodt >= 0) {
            comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(lmc));
            save_rodt[lmc] = comp_ctl2.s.rodt_ctl;
            comp_ctl2.s.rodt_ctl = dram_tune_use_rodt;
            DRAM_CSR_WRITE(node, BDK_LMCX_COMP_CTL2(lmc), comp_ctl2.u);
            BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(lmc));
        }
#endif
        /* Disable ECC for DRAM tests */
        lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(lmc));
        save_ecc_ena[lmc] = lmc_config.s.ecc_ena;
        lmc_config.s.ecc_ena = 0;
        DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(lmc), lmc_config.u);
        lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(lmc));

    } /* for (lmc = 0; lmc < num_lmcs; lmc++) */

#if USE_L2_WAYS_LIMIT
    /* Disable l2 sets for DRAM testing */
    limit_l2_ways(node, 0, ways_print);
#endif

    // testing is done on all LMCs simultaneously
    // FIXME: for now, loop here to show what happens multiple times
    for (loop = 0; loop < loops; loop++) {
        /* Perform DLL offset tuning */
        errs = auto_set_dll_offset(node, dll_offset_mode, num_lmcs, ddr_interface_64b, do_tune);
    }

#if USE_L2_WAYS_LIMIT
    /* Restore the l2 set configuration */
    limit_l2_ways(node, ways, ways_print);
#endif

    // perform cleanup on all active LMCs   
    debug_print("N%d: %s: starting LMCs cleanup.\n", node, __FUNCTION__);
    for (lmc = 0; lmc < num_lmcs; lmc++) {

        /* Restore ECC for DRAM tests */
        lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(lmc));
        lmc_config.s.ecc_ena = save_ecc_ena[lmc];
        DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(lmc), lmc_config.u);
        lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(lmc));
#if 0
        // if RODT change, restore old here...
        if (dram_tune_use_rodt >= 0) {
            comp_ctl2.u = BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(lmc));
            comp_ctl2.s.rodt_ctl = save_rodt[lmc];
            DRAM_CSR_WRITE(node, BDK_LMCX_COMP_CTL2(lmc), comp_ctl2.u);
            BDK_CSR_READ(node, BDK_LMCX_COMP_CTL2(lmc));
        }
#endif
        // finally, see if there are any read offset overrides after tuning
        // FIXME: provide a way to do write offsets also??
        if (dll_offset_mode == 2) {
            for (int by = 0; by < 9; by++) {
                if ((s = lookup_env_parameter("ddr%d_tune_byte%d", lmc, by)) != NULL) {
                    int dllro = strtoul(s, NULL, 10);
                    change_dll_offset_enable(node, lmc, 0);
                    load_dll_offset(node, lmc, /* read */2, dllro, by);
                    change_dll_offset_enable(node, lmc, 1);
                }
            }
        }
    } /* for (lmc = 0; lmc < num_lmcs; lmc++) */

    // finish up...

#if 0
    // if RODT was overridden during tuning, note restore
    if (dram_tune_use_rodt >= 0) {
        ddr_print("N%d: restoring RODT %d after tuning.\n",
                  node, save_rodt[0]); // FIXME? use LMC0
    }
#endif

    // put any cores on this node, that were not running at the start, back into reset
    /* FIXME(dhendrix): don't reset cores... */
//    uint64_t reset_coremask = bdk_get_running_coremask(node) & ~orig_coremask;
    uint64_t reset_coremask = 0;
    if (reset_coremask) {
        /* FIXME(dhendrix): %lx --> %llx */
        ddr_print4("N%d: %s: Stopping cores 0x%llx\n", node, __FUNCTION__,
                  reset_coremask);
        bdk_reset_cores(node, reset_coremask);
    } else {
        /* FIXME(dhendrix): %lx --> %llx */
        ddr_print4("N%d: %s: leaving cores set to 0x%llx\n", node, __FUNCTION__,
                  orig_coremask);
    }

    return errs;

} /* perform_dll_offset_tuning */

/////////////////////////////////////////////////////////////////////////////////////////////

/////    HW-assist byte DLL offset tuning   //////

#if 1
// setup defaults for byte test pattern array
// take these first two from the HRM section 6.9.13
static const uint64_t byte_pattern_0[] = {
    0xFFAAFFFFFF55FFFFULL, // GP0
    0x55555555AAAAAAAAULL, // GP1
    0xAA55AAAAULL,         // GP2
};
static const uint64_t byte_pattern_1[] = {
    0xFBF7EFDFBF7FFEFDULL, // GP0
    0x0F1E3C78F0E1C387ULL, // GP1
    0xF0E1BF7FULL,         // GP2
};
// this is from Andrew via LFSR with PRBS=0xFFFFAAAA
static const uint64_t byte_pattern_2[] = {
    0xEE55AADDEE55AADDULL, // GP0
    0x55AADDEE55AADDEEULL, // GP1
    0x55EEULL,             // GP2
};
// this is from Mike via LFSR with PRBS=0x4A519909
static const uint64_t byte_pattern_3[] = {
    0x0088CCEE0088CCEEULL, // GP0
    0xBB552211BB552211ULL, // GP1
    0xBB00ULL,             // GP2
};

static const uint64_t *byte_patterns[] = {
    byte_pattern_0, byte_pattern_1, byte_pattern_2, byte_pattern_3 // FIXME: use all we have
};
#define NUM_BYTE_PATTERNS ((int)(sizeof(byte_patterns)/sizeof(uint64_t *)))

#define DEFAULT_BYTE_BURSTS 32 // FIXME: this is what what the longest test usually has
int dram_tune_byte_bursts = DEFAULT_BYTE_BURSTS;
#endif

static void
setup_hw_pattern(bdk_node_t node, int lmc, const uint64_t *pattern_p)
{
    /*
      3) Setup GENERAL_PURPOSE[0-2] registers with the data pattern of choice.
      a. GENERAL_PURPOSE0[DATA<63:0>] – sets the initial lower (rising edge) 64 bits of data.
      b. GENERAL_PURPOSE1[DATA<63:0>] – sets the initial upper (falling edge) 64 bits of data.
      c. GENERAL_PURPOSE2[DATA<15:0>] – sets the initial lower (rising edge <7:0>) and upper
      (falling edge <15:8>) ECC data.
    */
    DRAM_CSR_WRITE(node, BDK_LMCX_GENERAL_PURPOSE0(lmc), pattern_p[0]);
    DRAM_CSR_WRITE(node, BDK_LMCX_GENERAL_PURPOSE1(lmc), pattern_p[1]);
    DRAM_CSR_WRITE(node, BDK_LMCX_GENERAL_PURPOSE2(lmc), pattern_p[2]);
}

#define DEFAULT_PRBS 0xFFFFAAAAUL /* FIXME: maybe try 0x4A519909UL */

static void
setup_lfsr_pattern(bdk_node_t node, int lmc, uint64_t data)
{
    uint32_t prbs;
    const char *s;

    if ((s = getenv("ddr_lfsr_prbs"))) {
        prbs = strtoul(s, NULL, 0);
    } else
        prbs = DEFAULT_PRBS; // FIXME: from data arg?

    /*
      2) DBTRAIN_CTL[LFSR_PATTERN_SEL] = 1
         here data comes from the LFSR generating a PRBS pattern
         CHAR_CTL.EN = 0
         CHAR_CTL.SEL = 0; // for PRBS
         CHAR_CTL.DR = 1;
         CHAR_CTL.PRBS = setup for whatever type of PRBS to send
         CHAR_CTL.SKEW_ON = 1;
    */
    BDK_CSR_INIT(char_ctl, node, BDK_LMCX_CHAR_CTL(lmc));
    char_ctl.s.en      = 0;
    char_ctl.s.sel     = 0;
    char_ctl.s.dr      = 1;
    char_ctl.s.prbs    = prbs;
    char_ctl.s.skew_on = 1;
    DRAM_CSR_WRITE(node, BDK_LMCX_CHAR_CTL(lmc), char_ctl.u);
}

/* FIXME(dhendrix): made static to avoid need for prototype */
static int
choose_best_hw_patterns(bdk_node_t node, int lmc, int mode)
{
    int new_mode = mode;
    const char *s;

    switch (mode) {
    case DBTRAIN_TEST: // always choose LFSR if chip supports it
        if (! CAVIUM_IS_MODEL(CAVIUM_CN88XX)) {
            int lfsr_enable = 1;
            if ((s = getenv("ddr_allow_lfsr"))) { // override?
                lfsr_enable = !!strtoul(s, NULL, 0);
            }
            if (lfsr_enable)
                new_mode = DBTRAIN_LFSR;
        }
        break;
    case DBTRAIN_DBI: // possibly can allow LFSR use?
        break;
    case DBTRAIN_LFSR: // forced already
        if (CAVIUM_IS_MODEL(CAVIUM_CN88XX)) {
            ddr_print("ERROR: illegal HW assist mode %d\n", mode);
            new_mode = DBTRAIN_TEST;
        }
        break;
    default:
        ddr_print("ERROR: unknown HW assist mode %d\n", mode);
    }

    if (new_mode != mode)
        VB_PRT(VBL_DEV2, "choose_best_hw_patterns: changing mode %d to %d\n", mode, new_mode);

    return new_mode;
}

int
run_best_hw_patterns(bdk_node_t node, int lmc, uint64_t phys_addr,
                     int mode, uint64_t *xor_data)
{
    int pattern;
    const uint64_t *pattern_p;
    int errs, errors = 0;

    // FIXME? always choose LFSR if chip supports it???
    mode = choose_best_hw_patterns(node, lmc, mode);

    if (mode == DBTRAIN_LFSR) {
            setup_lfsr_pattern(node, lmc, 0);
            errors = test_dram_byte_hw(node, lmc, phys_addr, mode, xor_data);
            VB_PRT(VBL_DEV2, "%s: LFSR at A:0x%012llx errors 0x%x\n",
                   __FUNCTION__, phys_addr, errors);
    } else {
        for (pattern = 0; pattern < NUM_BYTE_PATTERNS; pattern++) {
            pattern_p = byte_patterns[pattern];
            setup_hw_pattern(node, lmc, pattern_p);

            errs = test_dram_byte_hw(node, lmc, phys_addr, mode, xor_data);

            VB_PRT(VBL_DEV2, "%s: PATTERN %d at A:0x%012llx errors 0x%x\n",
                   __FUNCTION__, pattern, phys_addr, errs);

            errors |= errs;
        } /* for (pattern = 0; pattern < NUM_BYTE_PATTERNS; pattern++) */
    }
    return errors;
}

static void
hw_assist_test_dll_offset(bdk_node_t node, int dll_offset_mode,
                          int lmc, int bytelane)
{
    int byte_offset, new_best_offset[9];
    int rank_delay_start[4][9];
    int rank_delay_count[4][9];
    int rank_delay_best_start[4][9];
    int rank_delay_best_count[4][9];
    int errors[4], off_errors, tot_errors;
    int num_lmcs = __bdk_dram_get_num_lmc(node);
    int rank_mask, rankx, active_ranks;
    int pattern;
    const uint64_t *pattern_p;
    int byte;
    const char *mode_str = (dll_offset_mode == 2) ? "Read" : "Write";
    int pat_best_offset[9];
    uint64_t phys_addr;
    int pat_beg, pat_end;
    int rank_beg, rank_end;
    int byte_lo, byte_hi;
    uint64_t hw_rank_offset;
    // FIXME? always choose LFSR if chip supports it???
    int mode = choose_best_hw_patterns(node, lmc, DBTRAIN_TEST);

    if (bytelane == 0x0A) { // all bytelanes
        byte_lo = 0;
        byte_hi = 8;
    } else { // just 1
        byte_lo = byte_hi = bytelane;
    }

    BDK_CSR_INIT(lmcx_config, node, BDK_LMCX_CONFIG(lmc));
    rank_mask = lmcx_config.s.init_status;
    // this should be correct for 1 or 2 ranks, 1 or 2 DIMMs
    hw_rank_offset = 1ull << (28 + lmcx_config.s.pbank_lsb - lmcx_config.s.rank_ena + (num_lmcs/2));

    debug_print("N%d: %s: starting LMC%d with rank offset 0x%lx\n",
                node, __FUNCTION__, lmc, hw_rank_offset);

    // start of pattern loop
    // we do the set of tests for each pattern supplied...

    memset(new_best_offset, 0, sizeof(new_best_offset));
    for (pattern = 0; pattern < NUM_BYTE_PATTERNS; pattern++) {

        memset(pat_best_offset, 0, sizeof(pat_best_offset));

        if (mode == DBTRAIN_TEST) {
            pattern_p = byte_patterns[pattern];
            setup_hw_pattern(node, lmc, pattern_p);
        } else {
            setup_lfsr_pattern(node, lmc, 0);
        }

        // now loop through all legal values for the DLL byte offset...

#define BYTE_OFFSET_INCR 3 // FIXME: make this tunable?

        tot_errors = 0;

        memset(rank_delay_count, 0, sizeof(rank_delay_count));
        memset(rank_delay_start, 0, sizeof(rank_delay_start));
        memset(rank_delay_best_count, 0, sizeof(rank_delay_best_count));
        memset(rank_delay_best_start, 0, sizeof(rank_delay_best_start));

        for (byte_offset = -63; byte_offset < 64; byte_offset += BYTE_OFFSET_INCR) {

            // do the setup on the active LMC
            // set the bytelanes DLL offsets
            change_dll_offset_enable(node, lmc, 0);
            load_dll_offset(node, lmc, dll_offset_mode, byte_offset, bytelane); // FIXME? bytelane?
            change_dll_offset_enable(node, lmc, 1);

            bdk_watchdog_poke();

            // run the test on each rank
            // only 1 call per rank should be enough, let the bursts, loops, etc, control the load...

            off_errors = 0; // errors for this byte_offset, all ranks

            active_ranks = 0;

            for (rankx = 0; rankx < 4; rankx++) {
                if (!(rank_mask & (1 << rankx)))
                    continue;

                phys_addr = hw_rank_offset * active_ranks;
                // FIXME: now done by test_dram_byte_hw()
                //phys_addr |= (lmc << 7);
                //phys_addr = bdk_numa_get_address(node, phys_addr); // map to node

                active_ranks++;

                // NOTE: return is a now a bitmask of the erroring bytelanes..
                errors[rankx] = test_dram_byte_hw(node, lmc, phys_addr, mode, NULL);

                for (byte = byte_lo; byte <= byte_hi; byte++) { // do bytelane(s)

                    // check errors
                    if (errors[rankx] & (1 << byte)) { // yes, an error in the byte lane in this rank
                        off_errors |= (1 << byte);

                        ddr_print5("N%d.LMC%d.R%d: Bytelane %d DLL %s Offset Test %3d: Address 0x%012llx errors 0x%x\n",
                                   node, lmc, rankx, bytelane, mode_str,
                                   byte_offset, phys_addr, errors[rankx]);

                        if (rank_delay_count[rankx][byte] > 0) { // had started run
                            ddr_print5("N%d.LMC%d.R%d: Bytelane %d DLL %s Offset Test %3d: stopping a run here\n",
                                       node, lmc, rankx, bytelane, mode_str, byte_offset);
                            rank_delay_count[rankx][byte] = 0;   // stop now
                        }
                        // FIXME: else had not started run - nothing else to do?
                    } else { // no error in the byte lane
                        if (rank_delay_count[rankx][byte] == 0) { // first success, set run start
                            ddr_print5("N%d.LMC%d.R%d: Bytelane %d DLL %s Offset Test %3d: starting a run here\n",
                                       node, lmc, rankx, bytelane, mode_str, byte_offset);
                            rank_delay_start[rankx][byte] = byte_offset;
                        }
                        rank_delay_count[rankx][byte] += BYTE_OFFSET_INCR; // bump run length

                        // is this now the biggest window?
                        if (rank_delay_count[rankx][byte] > rank_delay_best_count[rankx][byte]) {
                            rank_delay_best_count[rankx][byte] = rank_delay_count[rankx][byte];
                            rank_delay_best_start[rankx][byte] = rank_delay_start[rankx][byte];
                            debug_print("N%d.LMC%d.R%d: Bytelane %d DLL %s Offset Test %3d: updating best to %d/%d\n",
                                        node, lmc, rankx, bytelane, mode_str, byte_offset,
                                        rank_delay_best_start[rankx][byte], rank_delay_best_count[rankx][byte]);
                        }
                    }
                } /* for (byte = byte_lo; byte <= byte_hi; byte++) */
            } /* for (rankx = 0; rankx < 4; rankx++) */

            tot_errors |= off_errors;

        } /* for (byte_offset = -63; byte_offset < 64; byte_offset += BYTE_OFFSET_INCR) */

        // now choose the best byte_offsets for this pattern according to the best windows of the tested ranks
        // calculate offset by constructing an average window from the rank windows
        for (byte = byte_lo; byte <= byte_hi; byte++) {

            pat_beg = -999;
            pat_end = 999;

            for (rankx = 0; rankx < 4; rankx++) {
                if (!(rank_mask & (1 << rankx)))
                    continue;

                rank_beg = rank_delay_best_start[rankx][byte];
                pat_beg = max(pat_beg, rank_beg);
                rank_end = rank_beg + rank_delay_best_count[rankx][byte] - BYTE_OFFSET_INCR;
                pat_end = min(pat_end, rank_end);

                ddr_print5("N%d.LMC%d.R%d: Bytelane %d DLL %s Offset Test:  Rank Window %3d:%3d\n",
                           node, lmc, rankx, bytelane, mode_str, rank_beg, rank_end);

            } /* for (rankx = 0; rankx < 4; rankx++) */

            pat_best_offset[byte] = (pat_end + pat_beg) / 2;
            ddr_print4("N%d.LMC%d: Bytelane %d DLL %s Offset Test:  Pattern %d Average %3d\n",
                       node, lmc, byte, mode_str, pattern, pat_best_offset[byte]);

#if 0
            // FIXME: next print the window counts
            sprintf(sbuffer, "N%d.LMC%d Pattern %d: DLL %s Offset Count ",
                    node, lmc, pattern, mode_str);
            printf("%-45s : ", sbuffer);
            printf(" %3d", byte_delay_best_count);
            printf("\n");
#endif

            new_best_offset[byte] += pat_best_offset[byte]; // sum the pattern averages
        } /* for (byte = byte_lo; byte <= byte_hi; byte++) */
    } /* for (pattern = 0; pattern < NUM_BYTE_PATTERNS; pattern++) */
    // end of pattern loop

    ddr_print("N%d.LMC%d: HW DLL %s Offset Amount   : ",
              node, lmc, mode_str);

    for (byte = byte_hi; byte >= byte_lo; --byte) { // print in decending byte index order
        new_best_offset[byte] = divide_nint(new_best_offset[byte], NUM_BYTE_PATTERNS); // create the new average NINT

        // print the best offsets from all patterns

        if (bytelane == 0x0A) // print just the offset of all the bytes
            ddr_print("%5d ", new_best_offset[byte]);
        else
            ddr_print("(byte %d) %5d ", byte, new_best_offset[byte]);
        

#if 1
        // done with testing, load up the best offsets we found...
        change_dll_offset_enable(node, lmc, 0); // disable offsets while we load...
        load_dll_offset(node, lmc, dll_offset_mode, new_best_offset[byte], byte);
        change_dll_offset_enable(node, lmc, 1); // re-enable the offsets now that we are done loading
#endif
    } /* for (byte = byte_hi; byte >= byte_lo; --byte) */

    ddr_print("\n");

#if 0
    // run the test one last time 
    // print whether there are errors or not, but only when verbose...
    tot_errors = run_test_dram_byte_threads(node, num_lmcs, bytemask);
    printf("N%d.LMC%d: Bytelane %d DLL %s Offset Final Test: errors 0x%x\n",
           node, lmc, bytelane, mode_str, tot_errors);
#endif
}

/*
 * Automatically adjust the DLL offset for the selected bytelane using hardware-assist
 */
int perform_HW_dll_offset_tuning(bdk_node_t node, int dll_offset_mode, int bytelane)
{
    int save_ecc_ena[4];
    bdk_lmcx_config_t lmc_config;
    int lmc, num_lmcs = __bdk_dram_get_num_lmc(node);
    const char *s;
    //bdk_lmcx_comp_ctl2_t comp_ctl2;
    int loops = 1, loop;

    // see if we want to do the tuning more than once per LMC...
    if ((s = getenv("ddr_tune_ecc_loops"))) {
        loops = strtoul(s, NULL, 0);
    }

    // allow override of the test repeats (bursts)
    if ((s = getenv("ddr_tune_byte_bursts")) != NULL) {
        dram_tune_byte_bursts = strtoul(s, NULL, 10);
    }

    // print current working values
    ddr_print2("N%d: H/W Tuning for bytelane %d will use %d loops, %d bursts, and %d patterns.\n",
              node, bytelane, loops, dram_tune_byte_bursts,
              NUM_BYTE_PATTERNS);

    // FIXME? get flag from LMC0 only
    lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(0));

    // do once for each active LMC

    for (lmc = 0; lmc < num_lmcs; lmc++) {

        ddr_print4("N%d: H/W Tuning: starting LMC%d bytelane %d tune.\n", node, lmc, bytelane);

        /* Enable ECC for the HW tests */
        // NOTE: we do enable ECC, but the HW tests used will not generate "visible" errors
        lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(lmc));
        save_ecc_ena[lmc] = lmc_config.s.ecc_ena;
        lmc_config.s.ecc_ena = 1;
        DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(lmc), lmc_config.u);
        lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(lmc));

        // testing is done on a single LMC at a time
        // FIXME: for now, loop here to show what happens multiple times
        for (loop = 0; loop < loops; loop++) {
            /* Perform DLL offset tuning */
            //auto_set_dll_offset(node,  1 /* 1=write */, lmc, bytelane);
            hw_assist_test_dll_offset(node,  2 /* 2=read */, lmc, bytelane);
        }

        // perform cleanup on active LMC
        ddr_print4("N%d: H/W Tuning: finishing LMC%d bytelane %d tune.\n", node, lmc, bytelane);

        /* Restore ECC for DRAM tests */
        lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(lmc));
        lmc_config.s.ecc_ena = save_ecc_ena[lmc];
        DRAM_CSR_WRITE(node, BDK_LMCX_CONFIG(lmc), lmc_config.u);
        lmc_config.u = BDK_CSR_READ(node, BDK_LMCX_CONFIG(lmc));

        // finally, see if there are any read offset overrides after tuning
        for (int by = 0; by < 9; by++) {
            if ((s = lookup_env_parameter("ddr%d_tune_byte%d", lmc, by)) != NULL) {
                int dllro = strtoul(s, NULL, 10);
                change_dll_offset_enable(node, lmc, 0);
                load_dll_offset(node, lmc, 2 /* 2=read */, dllro, by);
                change_dll_offset_enable(node, lmc, 1);
            }
        }

    } /* for (lmc = 0; lmc < num_lmcs; lmc++) */

    // finish up...

    return 0;

} /* perform_HW_dll_offset_tuning */
