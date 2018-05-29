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
#include "libbdk-arch/bdk-csrs-gti.h"
#include "libbdk-arch/bdk-csrs-ocx.h"

/* This code is an optional part of the BDK. It is only linked in
    if BDK_REQUIRE() needs it */
BDK_REQUIRE_DEFINE(DRAM_TEST);

#define MAX_ERRORS_TO_REPORT 50
#define RETRY_LIMIT 1000

typedef struct
{
    const char *        name;       /* Friendly name for the test */
    __bdk_dram_test_t   test_func;  /* Function to call */
    int                 bursts;     /* Bursts parameter to pass to the test */
    int                 max_cores;  /* Maximum number of cores the test should be run on in parallel. Zero means all */
} dram_test_info_t;

static const dram_test_info_t TEST_INFO[] = {
    /* Name,                    Test function,                      Bursts, Max Cores */
    { "Data Bus",               __bdk_dram_test_mem_data_bus,       8,      1},
    { "Address Bus",            __bdk_dram_test_mem_address_bus,    0,      1},
    { "Marching Rows",          __bdk_dram_test_mem_rows,           16,     0},
    { "Random Data",            __bdk_dram_test_mem_random,         32,     0},
    { "Random XOR (32 Burst)",  __bdk_dram_test_mem_xor,            32,     0},
    { "Self Address",           __bdk_dram_test_mem_self_addr,      1,      0},
    { "March C- Solid Bits",    __bdk_dram_test_mem_solid,          1,      0},
    { "March C- Checkerboard",  __bdk_dram_test_mem_checkerboard,   1,      0},
    { "Walking Ones Left",      __bdk_dram_test_mem_leftwalk1,      1,      0},
    { "Walking Ones Right",     __bdk_dram_test_mem_rightwalk1,     1,      0},
    { "Walking Zeros Left",     __bdk_dram_test_mem_leftwalk0,      1,      0},
    { "Walking Zeros Right",    __bdk_dram_test_mem_rightwalk0,     1,      0},
    { "Random XOR (224 Burst)", __bdk_dram_test_mem_xor,            224,    0},
    { "Fast Scan",              __bdk_dram_test_fast_scan,          0,      0},
    { NULL,                     NULL,                               0,      0}
};

/* These variables count the number of ECC errors. They should only be accessed atomically */
int64_t __bdk_dram_ecc_single_bit_errors[BDK_MAX_MEM_CHANS];
int64_t __bdk_dram_ecc_double_bit_errors[BDK_MAX_MEM_CHANS];

static int64_t dram_test_thread_done;
static int64_t dram_test_thread_errors;
static uint64_t dram_test_thread_start;
static uint64_t dram_test_thread_end;
static uint64_t dram_test_thread_size;

/**
 * Force the memory at the pointer location to be written to memory and evicted
 * from L2. L1 will be unaffected.
 *
 * @param address Physical memory location
 */
void __bdk_dram_flush_to_mem(uint64_t address)
{
    BDK_MB;
    /* The DRAM code doesn't use the normal bdk_phys_to_ptr() because of the
       NULL check in it. This greatly slows down the memory tests */
    char *ptr = (void*)address;
    BDK_CACHE_WBI_L2(ptr);
}

/**
 * Force a memory region to be written to DRAM and evicted from L2
 *
 * @param area   Start of the region
 * @param max_address
 *               End of the region (exclusive)
 */
void __bdk_dram_flush_to_mem_range(uint64_t area, uint64_t max_address)
{
    /* The DRAM code doesn't use the normal bdk_phys_to_ptr() because of the
       NULL check in it. This greatly slows down the memory tests */
    char *ptr = (void*)area;
    char *end = (void*)max_address;
    BDK_MB;
    while (ptr < end)
    {
        BDK_CACHE_WBI_L2(ptr);
        ptr += 128;
    }
}

/**
 * Convert a test enumeration into a string
 *
 * @param test   Test to convert
 *
 * @return String for display
 */
const char *bdk_dram_get_test_name(int test)
{
    if (test < (int)(sizeof(TEST_INFO) / sizeof(TEST_INFO[0])))
        return TEST_INFO[test].name;
    else
        return NULL;
}

static bdk_dram_test_flags_t dram_test_flags; // FIXME: Don't use global
/**
 * This function is run as a thread to perform memory tests over multiple cores.
 * Each thread gets a section of memory to work on, which is controlled by global
 * variables at the beginning of this file.
 *
 * @param arg    Number of the region we should check
 * @param arg1   Pointer the the test_info structure
 */
static void dram_test_thread(int arg, void *arg1)
{
    const dram_test_info_t *test_info = arg1;
    const int bursts = test_info->bursts;
    const int range_number = arg;

    /* Figure out our work memory range.
     *
     * Note start_address and end_address just provide the physical offset
     * portion of the address and do not have the node bits set. This is
     * to simplify address checks and calculations. Later, when about to run
     * the memory test, the routines adds in the node bits to form the final
     * addresses.
     */
    uint64_t start_address = dram_test_thread_start + dram_test_thread_size * range_number;
    uint64_t end_address = start_address + dram_test_thread_size;
    if (end_address > dram_test_thread_end)
        end_address = dram_test_thread_end;

    bdk_node_t test_node = bdk_numa_local();
    if (dram_test_flags & BDK_DRAM_TEST_USE_CCPI)
        test_node ^= 1;
    /* Insert the node part of the address */
    start_address = bdk_numa_get_address(test_node, start_address);
    end_address = bdk_numa_get_address(test_node, end_address);
    /* Test the region */
    BDK_TRACE(DRAM_TEST, "  Node %d, core %d, Testing [0x%011lx:0x%011lx]\n",
        bdk_numa_local(), bdk_get_core_num() & 127, start_address, end_address - 1);
    test_info->test_func(start_address, end_address, bursts);

    /* Report that we're done */
    BDK_TRACE(DRAM_TEST, "Thread %d on node %d done with memory test\n", range_number, bdk_numa_local());
    bdk_atomic_add64_nosync(&dram_test_thread_done, 1);
}

/**
 * Run the memory test.
 *
 * @param test_info
 * @param start_address
 *                  Physical address to start at
 * @param length    Length of memory block
 * @param flags     Flags to control memory test options. Zero defaults to testing all
 *                  node with statistics and progress output.
 *
 * @return Number of errors found. Zero is success. Negative means the test
 *         did not run due to some other failure.
 */
static int __bdk_dram_run_test(const dram_test_info_t *test_info, uint64_t start_address,
                               uint64_t length, bdk_dram_test_flags_t flags)
{
    /* Figure out the addess of the byte one off the top of memory */
    uint64_t max_address = bdk_dram_get_size_mbytes(bdk_numa_local());
    BDK_TRACE(DRAM_TEST, "DRAM available per node: %lu MB\n", max_address);
    max_address <<= 20;

    /* Make sure we have enough */
    if (max_address < (16<<20))
    {
        bdk_error("DRAM size is too small\n");
        return -1;
    }

    /* Make sure the amount is sane */
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
    {
        if (max_address > (1ull << 40)) /* 40 bits in CN8XXX */
            max_address = 1ull << 40;
    }
    else
    {
        if (max_address > (1ull << 43)) /* 43 bits in CN9XXX */
            max_address = 1ull << 43;
    }
    BDK_TRACE(DRAM_TEST, "DRAM max address: 0x%011lx\n", max_address-1);

    /* Make sure the start address is lower than the top of memory */
    if (start_address >= max_address)
    {
        bdk_error("Start address is larger than the amount of memory: 0x%011lx versus 0x%011lx\n",
	          start_address, max_address);
        return -1;
    }
    if (length == (uint64_t)-1)
        length = max_address - start_address;

    /* Final range checks */
    uint64_t end_address = start_address + length;
    if (end_address > max_address)
    {
        end_address = max_address;
        length = end_address - start_address;
    }
    if (length == 0)
        return 0;

    /* Ready to run the test. Figure out how many cores we need */
    int max_cores = test_info->max_cores;
    int total_cores_all_nodes = max_cores;

    /* Figure out the number of cores available in the system */
    if (max_cores == 0)
    {
        max_cores += bdk_get_num_running_cores(bdk_numa_local());
        /* Calculate the total number of cores being used. The per node number
           is confusing to people */
        for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
            if (flags & (1 << node))
            {
                if (flags & BDK_DRAM_TEST_USE_CCPI)
                    total_cores_all_nodes += bdk_get_num_running_cores(node ^ 1);
                else
                    total_cores_all_nodes += bdk_get_num_running_cores(node);
            }
    }
    if (!(flags & BDK_DRAM_TEST_NO_BANNERS))
        printf("Starting Test \"%s\" for [0x%011lx:0x%011lx] using %d core(s)\n",
	   test_info->name, start_address, end_address - 1, total_cores_all_nodes);

    /* Remember the LMC perf counters for stats after the test */
    uint64_t start_dram_dclk[BDK_NUMA_MAX_NODES][4];
    uint64_t start_dram_ops[BDK_NUMA_MAX_NODES][4];
    uint64_t stop_dram_dclk[BDK_NUMA_MAX_NODES][4];
    uint64_t stop_dram_ops[BDK_NUMA_MAX_NODES][4];
    for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
    {
        if (flags & (1 << node))
        {
            const int num_dram_controllers = __bdk_dram_get_num_lmc(node);
            for (int i = 0; i < num_dram_controllers; i++)
            {
                start_dram_dclk[node][i] = BDK_CSR_READ(node, BDK_LMCX_DCLK_CNT(i));
                start_dram_ops[node][i] = BDK_CSR_READ(node, BDK_LMCX_OPS_CNT(i));
            }
        }
    }
    /* Remember the CCPI link counters for stats after the test */
    uint64_t start_ccpi_data[BDK_NUMA_MAX_NODES][3];
    uint64_t start_ccpi_idle[BDK_NUMA_MAX_NODES][3];
    uint64_t start_ccpi_err[BDK_NUMA_MAX_NODES][3];
    uint64_t stop_ccpi_data[BDK_NUMA_MAX_NODES][3];
    uint64_t stop_ccpi_idle[BDK_NUMA_MAX_NODES][3];
    uint64_t stop_ccpi_err[BDK_NUMA_MAX_NODES][3];
    if (!bdk_numa_is_only_one())
    {
        for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
        {
            if (flags & (1 << node))
            {
                for (int link = 0; link < 3; link++)
                {
                    start_ccpi_data[node][link] = BDK_CSR_READ(node, BDK_OCX_TLKX_STAT_DATA_CNT(link));
                    start_ccpi_idle[node][link] = BDK_CSR_READ(node, BDK_OCX_TLKX_STAT_IDLE_CNT(link));
                    start_ccpi_err[node][link] = BDK_CSR_READ(node, BDK_OCX_TLKX_STAT_ERR_CNT(link));
                }
            }
        }
    }

    /* WARNING: This code assumes the same memory range is being tested on
       all nodes. The same number of cores are used on each node to test
       its local memory */
    uint64_t work_address = start_address;
    dram_test_flags = flags;
    bdk_atomic_set64(&dram_test_thread_errors, 0);
    while ((work_address < end_address) && ((dram_test_thread_errors == 0) || (flags & BDK_DRAM_TEST_NO_STOP_ERROR)))
    {
        /* Check at most MAX_CHUNK_SIZE across each iteration. We only report
           progress between chunks, so keep them reasonably small */
        const uint64_t MAX_CHUNK_SIZE = 1ull << 28; /* 256MB */
        uint64_t size = end_address - work_address;
        if (size > MAX_CHUNK_SIZE)
            size = MAX_CHUNK_SIZE;

        /* Divide memory evenly between the cores. Round the size up so that
           all memory is covered. The last core may have slightly less memory to
           test */
        uint64_t thread_size = (size + (max_cores - 1)) / max_cores;
        thread_size += 127;
        thread_size &= -128;
        dram_test_thread_start = work_address;
        dram_test_thread_end = work_address + size;
        dram_test_thread_size = thread_size;
        BDK_WMB;

        /* Poke the watchdog */
        BDK_CSR_WRITE(bdk_numa_local(), BDK_GTI_CWD_POKEX(0), 0);

	/* disable progress output when batch mode is ON  */
        if (!(flags & BDK_DRAM_TEST_NO_PROGRESS)) {

            /* Report progress percentage */
            int percent_x10 = (work_address - start_address) * 1000 / (end_address - start_address);
            printf("  %3d.%d%% complete, testing [0x%011lx:0x%011lx]\r",
                   percent_x10 / 10, percent_x10 % 10,  work_address, work_address + size - 1);
            fflush(stdout);
	}

        work_address += size;

        /* Start threads for all the cores */
        int total_count = 0;
        bdk_atomic_set64(&dram_test_thread_done, 0);
        for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
        {
            if (flags & (1 << node))
            {
                const int num_cores = bdk_get_num_cores(node);
                int per_node = 0;
                for (int core = 0; core < num_cores; core++)
                {
                    if (per_node >= max_cores)
                        break;
                    int run_node = (flags & BDK_DRAM_TEST_USE_CCPI) ? node ^ 1 : node;
                    BDK_TRACE(DRAM_TEST, "Starting thread %d on node %d for memory test\n", per_node, node);
                    if (bdk_thread_create(run_node, 0, dram_test_thread, per_node, (void *)test_info, 0))
                    {
                        bdk_error("Failed to create thread %d for memory test on node %d\n", per_node, node);
                    }
                    else
                    {
                        per_node++;
                        total_count++;
                    }
                }
            }
        }

#if 0
        /* Wait for threads to finish */
        while (bdk_atomic_get64(&dram_test_thread_done) < total_count)
            bdk_thread_yield();
#else
#define TIMEOUT_SECS 30  // FIXME: long enough so multicore RXOR 224 should not print out
        /* Wait for threads to finish, with progress */
        int cur_count;
        uint64_t cur_time;
        uint64_t period = bdk_clock_get_rate(bdk_numa_local(), BDK_CLOCK_TIME) * TIMEOUT_SECS; // FIXME? 
        uint64_t timeout = bdk_clock_get_count(BDK_CLOCK_TIME) + period;
        do {
            bdk_thread_yield();
            cur_count = bdk_atomic_get64(&dram_test_thread_done);
            cur_time = bdk_clock_get_count(BDK_CLOCK_TIME);
            if (cur_time >= timeout) {
                BDK_TRACE(DRAM_TEST, "N%d: Waiting for %d cores\n",
                          bdk_numa_local(), total_count - cur_count);
                timeout = cur_time + period;
            }
        } while (cur_count < total_count);
#endif
    }

    /* Get the DRAM perf counters */
    for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
    {
        if (flags & (1 << node))
        {
            const int num_dram_controllers = __bdk_dram_get_num_lmc(node);
            for (int i = 0; i < num_dram_controllers; i++)
            {
                stop_dram_dclk[node][i] = BDK_CSR_READ(node, BDK_LMCX_DCLK_CNT(i));
                stop_dram_ops[node][i] = BDK_CSR_READ(node, BDK_LMCX_OPS_CNT(i));
            }
        }
    }
    /* Get the CCPI link counters */
    if (!bdk_numa_is_only_one())
    {
        for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
        {
            if (flags & (1 << node))
            {
                for (int link = 0; link < 3; link++)
                {
                    stop_ccpi_data[node][link] = BDK_CSR_READ(node, BDK_OCX_TLKX_STAT_DATA_CNT(link));
                    stop_ccpi_idle[node][link] = BDK_CSR_READ(node, BDK_OCX_TLKX_STAT_IDLE_CNT(link));
                    stop_ccpi_err[node][link] = BDK_CSR_READ(node, BDK_OCX_TLKX_STAT_ERR_CNT(link));
                }
            }
        }
    }

    /* disable progress output when batch mode is ON  */
    if (!(flags & BDK_DRAM_TEST_NO_PROGRESS)) {

        /* Report progress percentage as complete */
        printf("  %3d.%d%% complete, testing [0x%011lx:0x%011lx]\n",
               100, 0,  start_address, end_address - 1);
        fflush(stdout);
    }

    if (!(flags & BDK_DRAM_TEST_NO_STATS))
    {
        /* Display LMC load */
        for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
        {
            if (flags & (1 << node))
            {
                const int num_dram_controllers = __bdk_dram_get_num_lmc(node);
                for (int i = 0; i < num_dram_controllers; i++)
                {
                    uint64_t ops = stop_dram_ops[node][i] - start_dram_ops[node][i];
                    uint64_t dclk = stop_dram_dclk[node][i] - start_dram_dclk[node][i];
                    if (dclk == 0)
                        dclk = 1;
                    uint64_t percent_x10 = ops * 1000 / dclk;
                    printf("  Node %d, LMC%d: ops %lu, cycles %lu, used %lu.%lu%%\n",
                        node, i, ops, dclk, percent_x10 / 10, percent_x10 % 10);
                }
            }
        }
        if (flags & BDK_DRAM_TEST_USE_CCPI)
        {
            /* Display CCPI load */
            for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
            {
                if (flags & (1 << node))
                {
                    for (int link = 0; link < 3; link++)
                    {
                        uint64_t busy = stop_ccpi_data[node][link] - start_ccpi_data[node][link];
                        busy += stop_ccpi_err[node][link] - start_ccpi_err[node][link];
                        uint64_t total = stop_ccpi_idle[node][link] - start_ccpi_idle[node][link];
                        total += busy;
                        if (total == 0)
                            continue;
                        uint64_t percent_x10 = busy * 1000 / total;
                        printf("  Node %d, CCPI%d: busy %lu, total %lu, used %lu.%lu%%\n",
                            node, link, busy, total, percent_x10 / 10, percent_x10 % 10);
                    }
                }
            }
        }
    }
    return dram_test_thread_errors;
}

/**
 * Perform a memory test.
 *
 * @param test   Test type to run
 * @param start_address
 *               Physical address to start at
 * @param length Length of memory block
 * @param flags  Flags to control memory test options. Zero defaults to testing all
 *               node with statistics and progress output.
 *
 * @return Number of errors found. Zero is success. Negative means the test
 *         did not run due to some other failure.
 */
int bdk_dram_test(int test, uint64_t start_address, uint64_t length, bdk_dram_test_flags_t flags)
{
    /* These limits are arbitrary. They just make sure we aren't doing something
       silly, like test a non cache line aligned memory region */
    if (start_address & 0xffff)
    {
        bdk_error("DRAM test start address must be aligned on a 64KB boundary\n");
        return -1;
    }
    if (length & 0xffff)
    {
        bdk_error("DRAM test length must be a multiple of 64KB\n");
        return -1;
    }

    const char *name = bdk_dram_get_test_name(test);
    if (name == NULL)
    {
        bdk_error("Invalid DRAM test number %d\n", test);
        return -1;
    }

    /* If no nodes are selected assume the user meant all nodes */
    if ((flags & (BDK_DRAM_TEST_NODE0 | BDK_DRAM_TEST_NODE1 | BDK_DRAM_TEST_NODE2 | BDK_DRAM_TEST_NODE3)) == 0)
        flags |= BDK_DRAM_TEST_NODE0 | BDK_DRAM_TEST_NODE1 | BDK_DRAM_TEST_NODE2 | BDK_DRAM_TEST_NODE3;

    /* Remove nodes from the flags that don't exist */
    for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
    {
        if (flags & BDK_DRAM_TEST_USE_CCPI)
        {
            if (!bdk_numa_exists(node ^ 1))
                flags &= ~(1 << node);
        }
        else
        {
            if (!bdk_numa_exists(node))
                flags &= ~(1 << node);
        }
    }


    /* Make sure the start address is higher that the BDK's active range */
    uint64_t top_of_bdk = bdk_dram_get_top_of_bdk();
    if (start_address < top_of_bdk)
        start_address = top_of_bdk;

    /* Clear ECC error counters before starting the test */
    for (int chan = 0; chan < BDK_MAX_MEM_CHANS; chan++) {
	bdk_atomic_set64(&__bdk_dram_ecc_single_bit_errors[chan], 0);
	bdk_atomic_set64(&__bdk_dram_ecc_double_bit_errors[chan], 0);
    }

    /* Make sure at least one core from each node is running */
    for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
    {
        if (flags & (1<<node))
        {
            int use_node = (flags & BDK_DRAM_TEST_USE_CCPI) ? node ^ 1 : node;
            if (bdk_get_running_coremask(use_node) == 0)
                bdk_init_cores(use_node, 1);
        }
    }

    /* This returns any data compare errors found */
    int errors = __bdk_dram_run_test(&TEST_INFO[test], start_address, length, flags);

    /* Poll for any errors right now to make sure any ECC errors are reported */
    for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
    {
        if (bdk_numa_exists(node) && bdk_error_check)
            bdk_error_check(node);
    }

    /* Check ECC error counters after the test */
    int64_t ecc_single = 0;
    int64_t ecc_double = 0;
    int64_t ecc_single_errs[BDK_MAX_MEM_CHANS];
    int64_t ecc_double_errs[BDK_MAX_MEM_CHANS];

    for (int chan = 0; chan < BDK_MAX_MEM_CHANS; chan++) {
        ecc_single += (ecc_single_errs[chan] = bdk_atomic_get64(&__bdk_dram_ecc_single_bit_errors[chan]));
        ecc_double += (ecc_double_errs[chan] = bdk_atomic_get64(&__bdk_dram_ecc_double_bit_errors[chan]));
    }

    /* Always print any ECC errors */
    if (ecc_single || ecc_double)
    {
        printf("Test \"%s\": ECC errors, %ld/%ld/%ld/%ld corrected, %ld/%ld/%ld/%ld uncorrected\n",
	       name,
	       ecc_single_errs[0], ecc_single_errs[1], ecc_single_errs[2], ecc_single_errs[3],
	       ecc_double_errs[0], ecc_double_errs[1], ecc_double_errs[2], ecc_double_errs[3]);
    }
    if (errors || ecc_double || ecc_single) {
	printf("Test \"%s\": FAIL: %ld single, %ld double, %d compare errors\n",
	       name, ecc_single, ecc_double, errors);
    }
    else
        BDK_TRACE(DRAM_TEST, "Test \"%s\": PASS\n", name);

    return (errors + ecc_double + ecc_single);
}

/**
 * Report a DRAM address in decoded format.
 *
 * @param address Physical address the error occurred at
 *
 */
static void __bdk_dram_report_address_decode(uint64_t address, char *buffer, int len)
{
    int node, lmc, dimm, prank, lrank, bank, row, col;

    bdk_dram_address_extract_info(address, &node, &lmc, &dimm, &prank, &lrank, &bank, &row, &col);

    snprintf(buffer, len, "[0x%011lx] (N%d,LMC%d,DIMM%d,Rank%d/%d,Bank%02d,Row 0x%05x,Col 0x%04x)",
	     address, node, lmc, dimm, prank, lrank, bank, row, col);
}

/**
 * Report a DRAM address in a new decoded format.
 *
 * @param address Physical address the error occurred at
 * @param xor     XOR of data read vs expected data
 *
 */
static void __bdk_dram_report_address_decode_new(uint64_t address, uint64_t orig_xor, char *buffer, int len)
{
    int node, lmc, dimm, prank, lrank, bank, row, col;

    int byte = 8; // means no byte-lanes in error, should not happen
    uint64_t bits, print_bits = 0;
    uint64_t xor = orig_xor;

    // find the byte-lane(s) with errors
    for (int i = 0; i < 8; i++) {
        bits = xor & 0xffULL;
        xor >>= 8;
	if (bits) {
	    if (byte != 8) {
		byte = 9; // means more than 1 byte-lane was present
                print_bits = orig_xor; // print the full original
		break; // quit now
	    } else {
		byte = i; // keep checking
                print_bits = bits;
	    }
	}
    }
	
    bdk_dram_address_extract_info(address, &node, &lmc, &dimm, &prank, &lrank, &bank, &row, &col);

    snprintf(buffer, len, "N%d.LMC%d: CMP byte %d xor 0x%02lx (DIMM%d,Rank%d/%d,Bank%02d,Row 0x%05x,Col 0x%04x)[0x%011lx]",
	     node, lmc, byte, print_bits, dimm, prank, lrank, bank, row, col, address);
}

/**
 * Report a DRAM error. Errors are not shown after MAX_ERRORS_TO_REPORT is
 * exceeded. Used when a single address is involved in the failure.
 *
 * @param address Physical address the error occurred at
 * @param data    Data read from memory
 * @param correct Correct data
 * @param burst   Which burst this is from, informational only
 * @param fails   -1 for no retries done, >= 0 number of failures during retries 
 *
 * @return Zero if a message was logged, non-zero if the error limit has been reached
 */
void __bdk_dram_report_error(uint64_t address, uint64_t data, uint64_t correct, int burst, int fails)
{
    char buffer[128];
    char failbuf[32];
    int64_t errors = bdk_atomic_fetch_and_add64(&dram_test_thread_errors, 1);
    uint64_t xor = data ^ correct;

    if (errors < MAX_ERRORS_TO_REPORT)
    {
	if (fails < 0) {
	    snprintf(failbuf, sizeof(failbuf), " ");
	} else {
            int percent_x10 = fails * 1000 / RETRY_LIMIT;
	    snprintf(failbuf, sizeof(failbuf), ", retries failed %3d.%d%%",
                     percent_x10 / 10, percent_x10 % 10);
	}

	__bdk_dram_report_address_decode_new(address, xor, buffer, sizeof(buffer));
        bdk_error("%s%s\n", buffer, failbuf);

        if (errors == MAX_ERRORS_TO_REPORT-1)
            bdk_error("No further DRAM errors will be reported\n");
    }
    return;
}

/**
 * Report a DRAM error. Errors are not shown after MAX_ERRORS_TO_REPORT is
 * exceeded. Used when two addresses might be involved in the failure.
 *
 * @param address1 First address involved in the failure
 * @param data1    Data from the first address
 * @param address2 Second address involved in the failure
 * @param data2    Data from second address
 * @param burst    Which burst this is from, informational only
 * @param fails    -1 for no retries done, >= 0 number of failures during retries 
 *
 * @return Zero if a message was logged, non-zero if the error limit has been reached
 */
void __bdk_dram_report_error2(uint64_t address1, uint64_t data1, uint64_t address2, uint64_t data2,
			      int burst, int fails)
{
    int64_t errors = bdk_atomic_fetch_and_add64(&dram_test_thread_errors, 1);
    if (errors < MAX_ERRORS_TO_REPORT)
    {
	char buffer1[80], buffer2[80];
	char failbuf[32];

	if (fails < 0) {
	    snprintf(failbuf, sizeof(failbuf), " ");
	} else {
	    snprintf(failbuf, sizeof(failbuf), ", retried %d failed %d", RETRY_LIMIT, fails);
	}
	__bdk_dram_report_address_decode(address1, buffer1, sizeof(buffer1));
	__bdk_dram_report_address_decode(address2, buffer2, sizeof(buffer2));

        bdk_error("compare: data1: 0x%016lx, xor: 0x%016lx%s\n"
		  "       %s\n       %s\n",
		  data1, data1 ^ data2, failbuf,
		  buffer1, buffer2);

        if (errors == MAX_ERRORS_TO_REPORT-1)
            bdk_error("No further DRAM errors will be reported\n");
    }
    return;
}

/* Report the circumstances of a failure and try re-reading the memory
 * location to see if the error is transient or permanent.
 *
 * Note: re-reading requires using evicting addresses
 */
int __bdk_dram_retry_failure(int burst, uint64_t address, uint64_t data, uint64_t expected)
{
    int refail = 0;

    // bypass the retries if we are already over the limit...
    if (bdk_atomic_get64(&dram_test_thread_errors) < MAX_ERRORS_TO_REPORT) {

	/* Try re-reading the memory location. A transient error may fail
	 * on one read and work on another. Keep on retrying even when a
	 * read succeeds.
	 */
	for (int i = 0; i < RETRY_LIMIT; i++) {

	    __bdk_dram_flush_to_mem(address);
	    BDK_DCACHE_INVALIDATE;

	    uint64_t new = __bdk_dram_read64(address);

	    if (new != expected) {
		refail++;
	    }
	}
    } else
	refail = -1;

    // this will increment the errors always, but maybe not print...
    __bdk_dram_report_error(address, data, expected, burst, refail);

    return 1;
}

/**
 * retry_failure2
 *
 * @param burst
 * @param address1
 * @param address2
 */
int __bdk_dram_retry_failure2(int burst, uint64_t address1, uint64_t data1, uint64_t address2, uint64_t data2)
{
    int refail = 0;

    // bypass the retries if we are already over the limit...
    if (bdk_atomic_get64(&dram_test_thread_errors) < MAX_ERRORS_TO_REPORT) {

	for (int i = 0; i < RETRY_LIMIT; i++) {
	    __bdk_dram_flush_to_mem(address1);
	    __bdk_dram_flush_to_mem(address2);
	    BDK_DCACHE_INVALIDATE;

	    uint64_t d1 = __bdk_dram_read64(address1);
	    uint64_t d2 = __bdk_dram_read64(address2);

	    if (d1 != d2) {
		refail++;
	    }
	}
    } else
	refail = -1;

    // this will increment the errors always, but maybe not print...
    __bdk_dram_report_error2(address1, data1, address2, data2, burst, refail);

    return 1;
}

/**
 * Inject a DRAM error at a specific address in memory. The injection can either
 * be a single bit inside the byte, or a double bit error in the ECC byte. Double
 * bit errors may corrupt memory, causing software to crash. The corruption is
 * written to memory and will continue to exist until the cache line is written
 * again. After a call to this function, the BDK should report a ECC error. Double
 * bit errors corrupt bits 0-1.
 *
 * @param address Physical address to corrupt. Any byte alignment is supported
 * @param bit     Bit to corrupt in the byte (0-7), or -1 to create a double bit fault in the ECC
 *                byte.
 */
void bdk_dram_test_inject_error(uint64_t address, int bit)
{
    uint64_t aligned_address = address & -16;
    int corrupt_bit = -1;
    if (bit >= 0)
        corrupt_bit = (address & 0xf) * 8 + bit;

    /* Extract the DRAM controller information */
    int node, lmc, dimm, prank, lrank, bank, row, col;
    bdk_dram_address_extract_info(address, &node, &lmc, &dimm, &prank, &lrank, &bank, &row, &col);

    /* Read the current data */
    uint64_t data = __bdk_dram_read64(aligned_address);

    /* Program LMC to inject the error */
    if ((corrupt_bit >= 0) && (corrupt_bit < 64))
        BDK_CSR_WRITE(node, BDK_LMCX_CHAR_MASK0(lmc), 1ull << corrupt_bit);
    else if (bit == -1)
        BDK_CSR_WRITE(node, BDK_LMCX_CHAR_MASK0(lmc), 3);
    else
        BDK_CSR_WRITE(node, BDK_LMCX_CHAR_MASK0(lmc), 0);
    if (corrupt_bit >= 64)
        BDK_CSR_WRITE(node, BDK_LMCX_CHAR_MASK2(lmc), 1ull << (corrupt_bit - 64));
    else
        BDK_CSR_WRITE(node, BDK_LMCX_CHAR_MASK2(lmc), 0);
    BDK_CSR_MODIFY(c, node, BDK_LMCX_ECC_PARITY_TEST(lmc),
        c.s.ecc_corrupt_idx = (address & 0x7f) >> 4;
        c.s.ecc_corrupt_ena = 1);
    BDK_CSR_READ(node, BDK_LMCX_ECC_PARITY_TEST(lmc));

    /* Perform a write and push it to DRAM. This creates the error */
    __bdk_dram_write64(aligned_address, data);
    __bdk_dram_flush_to_mem(aligned_address);

    /* Disable error injection */
    BDK_CSR_MODIFY(c, node, BDK_LMCX_ECC_PARITY_TEST(lmc),
        c.s.ecc_corrupt_ena = 0);
    BDK_CSR_READ(node, BDK_LMCX_ECC_PARITY_TEST(lmc));
    BDK_CSR_WRITE(node, BDK_LMCX_CHAR_MASK0(lmc), 0);
    BDK_CSR_WRITE(node, BDK_LMCX_CHAR_MASK2(lmc), 0);

    /* Read back the data, which should now cause an error */
    printf("Loading the injected error address 0x%lx, node=%d, lmc=%d, dimm=%d, rank=%d/%d, bank=%d, row=%d, col=%d\n",
           address, node, lmc, dimm, prank, lrank, bank, row, col);
    __bdk_dram_read64(aligned_address);
}
