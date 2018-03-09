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
#include "bdk.h"
#include <libbdk-hal/bdk-rng.h>
#include <libbdk-hal/bdk-utils.h>

// choose prediction-based algorithms for mem_xor and mem_rows tests
#define USE_PREDICTION_CODE_VERSIONS 1   // change to 0 to go back to the original versions

/* Used for all memory reads/writes related to the test */
#define READ64(address) __bdk_dram_read64(address)
#define WRITE64(address, data) __bdk_dram_write64(address, data)

/**
 * Fill an memory area with the address of each 64-bit word in the area.
 * Reread to confirm the pattern.
 *
 * @param area   Start of the physical memory area
 * @param max_address
 *               End of the physical memory area (exclusive)
 * @param bursts Number of time to repeat the test over the entire area
 *
 * @return Number of errors, zero on success
 */
int __bdk_dram_test_mem_self_addr(uint64_t area, uint64_t max_address, int bursts)
{
    int failures = 0;

    for (int burst = 0; burst < bursts; burst++)
    {
        /* Write the pattern to memory. Each location receives the address
         * of the location.
         */
        for (uint64_t address = area; address < max_address; address+=8)
            WRITE64(address, address);
        __bdk_dram_flush_to_mem_range(area, max_address);
        BDK_DCACHE_INVALIDATE;

        /* Read by ascending address the written memory and confirm that it
         * has the expected data pattern.
         */
        for (uint64_t address = area; address < max_address; )
        {
            if (address + 256 < max_address)
                BDK_PREFETCH(address + 256, 0);
            for (int i=0; i<16; i++)
            {
                uint64_t data = READ64(address);
                if (bdk_unlikely(data != address))
                    failures += __bdk_dram_retry_failure(burst, address, data, address);
                address += 8;
            }
        }
        __bdk_dram_flush_to_mem_range(area, max_address);
        BDK_DCACHE_INVALIDATE;

        /* Read by descending address the written memory and confirm that it
         * has the expected data pattern.
         */
        uint64_t end = max_address - sizeof(uint64_t);
        for (uint64_t address = end; address >= area; )
        {
            if (address - 256 >= area)
                BDK_PREFETCH(address - 256, 0);
            for (int i=0; i<16; i++)
            {
                uint64_t data = READ64(address);
                if (bdk_unlikely(data != address))
                    failures += __bdk_dram_retry_failure(burst, address, data, address);
                address -= 8;
            }
        }
        __bdk_dram_flush_to_mem_range(area, max_address);
        BDK_DCACHE_INVALIDATE;

        /* Read from random addresses within the memory area.
         */
        uint64_t probes = (max_address - area) / 128;
        uint64_t address_ahead1 = area;
        uint64_t address_ahead2 = area;
        for (uint64_t i = 0; i < probes; i++)
        {
            /* Create a pipeline of prefetches:
               address = address read this loop
               address_ahead1 = prefetch started last loop
               address_ahead2 = prefetch started this loop */
            uint64_t address = address_ahead1;
            address_ahead1 = address_ahead2;
            address_ahead2 = bdk_rng_get_random64() % (max_address - area);
            address_ahead2 += area;
            address_ahead2 &= -8;
            BDK_PREFETCH(address_ahead2, 0);

            uint64_t data = READ64(address);
            if (bdk_unlikely(data != address))
                failures += __bdk_dram_retry_failure(burst, address, data, address);
        }
    }
    return failures;
}

/**
 * Write "pattern" and its compliment to memory and verify it was written
 * properly. Memory will be filled with DWORDs pattern, ~pattern, pattern,
 * ~pattern, ...
 *
 * @param area    Start physical address of memory
 * @param max_address
 *                End of physical memory region
 * @param pattern Pattern to write
 * @param passes  Number of time to repeat the test
 *
 * @return Number of errors, zero on success
 */
static uint32_t test_mem_pattern(uint64_t area, uint64_t max_address, uint64_t pattern,
    int passes)
{
    int failures = 0;

    for (int pass = 0; pass < passes; pass++)
    {
        if (pass & 0x1)
            pattern = ~pattern;

        for (uint64_t address = area; address < max_address; address += 8)
            WRITE64(address, pattern);
        __bdk_dram_flush_to_mem_range(area, max_address);
        BDK_DCACHE_INVALIDATE;

        /* Read the written memory and confirm that it has the expected
         * data pattern.
         */
        uint64_t address = area;
        while (address < max_address)
        {
            if (address + 256 < max_address)
                BDK_PREFETCH(address + 256, 0);
            for (int i=0; i<16; i++)
            {
                uint64_t data = READ64(address);
                if (bdk_unlikely(data != pattern))
                    failures += __bdk_dram_retry_failure(pass, address, data, pattern);
                address += 8;
            }
        }
    }
    return failures;
}

/**
 * Walking zero written to memory, left shift
 *
 * @param area   Start of the physical memory area
 * @param max_address
 *               End of the physical memory area
 * @param bursts Number of time to repeat the test over the entire area
 *
 * @return Number of errors, zero on success
 */
int __bdk_dram_test_mem_leftwalk0(uint64_t area, uint64_t max_address, int bursts)
{
    int failures = 0;
    for (int burst = 0; burst < bursts; burst++)
    {
        for (uint64_t pattern = 1; pattern != 0; pattern = pattern << 1)
            failures += test_mem_pattern(area, max_address, ~pattern, 1);
    }
    return failures;
}

/**
 * Walking one written to memory, left shift
 *
 * @param area   Start of the physical memory area
 * @param max_address
 *               End of the physical memory area
 * @param bursts Number of time to repeat the test over the entire area
 *
 * @return Number of errors, zero on success
 */
int __bdk_dram_test_mem_leftwalk1(uint64_t area, uint64_t max_address, int bursts)
{
    int failures = 0;
    for (int burst = 0; burst < bursts; burst++)
    {
        for (uint64_t pattern = 1; pattern != 0; pattern = pattern << 1)
            failures += test_mem_pattern(area, max_address, pattern, 1);
    }
    return failures;
}

/**
 * Walking zero written to memory, right shift
 *
 * @param area   Start of the physical memory area
 * @param max_address
 *               End of the physical memory area
 * @param bursts Number of time to repeat the test over the entire area
 *
 * @return Number of errors, zero on success
 */
int __bdk_dram_test_mem_rightwalk0(uint64_t area, uint64_t max_address, int bursts)
{
    int failures = 0;
    for (int burst = 0; burst < bursts; burst++)
    {
        for (uint64_t pattern = 1ull << 63; pattern != 0; pattern = pattern >> 1)
            failures += test_mem_pattern(area, max_address, ~pattern, 1);
    }
    return failures;
}

/**
 * Walking one written to memory, right shift
 *
 * @param area   Start of the physical memory area
 * @param max_address
 *               End of the physical memory area
 * @param bursts Number of time to repeat the test over the entire area
 *
 * @return Number of errors, zero on success
 */
int __bdk_dram_test_mem_rightwalk1(uint64_t area, uint64_t max_address, int bursts)
{
    int failures = 0;
    for (int burst = 0; burst < bursts; burst++)
    {
        for (uint64_t pattern = 1ull<<63; pattern != 0; pattern = pattern >> 1)
            failures += test_mem_pattern(area, max_address, pattern, 1);
    }
    return failures;
}

/**
 * Apply the March C- testing algorithm to the given memory area.
 * 1) Write "pattern" to memory.
 * 2) Verify "pattern" and write "~pattern".
 * 3) Verify "~pattern" and write "pattern".
 * 4) Verify "pattern" and write "~pattern".
 * 5) Verify "~pattern" and write "pattern".
 * 6) Verify "pattern".
 *
 * @param area    Start of the physical memory area
 * @param max_address
 *                End of the physical memory area
 * @param pattern
 *
 * @return Number of errors, zero on success
 */
static int test_mem_march_c(uint64_t area, uint64_t max_address, uint64_t pattern)
{
    int failures = 0;

    /* Pass 1 ascending addresses, fill memory with pattern. */
    BDK_TRACE(DRAM_TEST, "    [0x%016llx:0x%016llx] Phase1, address incrementing, pattern 0x%016llx\n", area, max_address-1, pattern);
    for (uint64_t address = area; address < max_address; address += 8)
        WRITE64(address, pattern);

    __bdk_dram_flush_to_mem_range(area, max_address);
    BDK_DCACHE_INVALIDATE;

    /* Pass 2: ascending addresses, read pattern and write ~pattern */
    BDK_TRACE(DRAM_TEST, "    [0x%016llx:0x%016llx] Phase2, address incrementing, pattern 0x%016llx\n", area, max_address-1, ~pattern);
    for (uint64_t address = area; address < max_address; address += 8)
    {
        uint64_t data = READ64(address);
        if (bdk_unlikely(data != pattern))
            failures += __bdk_dram_retry_failure(1, address, data, pattern);
        WRITE64(address, ~pattern);
    }

    __bdk_dram_flush_to_mem_range(area, max_address);
    BDK_DCACHE_INVALIDATE;

    /* Pass 3: ascending addresses, read ~pattern and write pattern. */
    BDK_TRACE(DRAM_TEST, "    [0x%016llx:0x%016llx] Phase3, address incrementing, pattern 0x%016llx\n", area, max_address-1, pattern);
    for (uint64_t address = area; address < max_address; address += 8)
    {
        uint64_t data = READ64(address);
        if (bdk_unlikely(data != ~pattern))
            failures += __bdk_dram_retry_failure(1, address, data, ~pattern);
        WRITE64(address, pattern);
    }

    __bdk_dram_flush_to_mem_range(area, max_address);
    BDK_DCACHE_INVALIDATE;

    /* Pass 4: descending addresses, read pattern and write ~pattern. */
    BDK_TRACE(DRAM_TEST, "    [0x%016llx:0x%016llx] Phase4, address decrementing, pattern 0x%016llx\n", area, max_address-1, ~pattern);
    uint64_t end = max_address - sizeof(uint64_t);
    for (uint64_t address = end; address >= area; address -= 8)
    {
        uint64_t data = READ64(address);
        if (bdk_unlikely(data != pattern))
            failures += __bdk_dram_retry_failure(1, address, data, pattern);
        WRITE64(address, ~pattern);
    }

    __bdk_dram_flush_to_mem_range(area, max_address);
    BDK_DCACHE_INVALIDATE;

    /* Pass 5: descending addresses, read ~pattern and write pattern. */
    BDK_TRACE(DRAM_TEST, "    [0x%016llx:0x%016llx] Phase5, address decrementing, pattern 0x%016llx\n", area, max_address-1, pattern);
    for (uint64_t address = end; address >= area; address -= 8)
    {
        uint64_t data = READ64(address);
        if (bdk_unlikely(data != ~pattern))
            failures += __bdk_dram_retry_failure(1, address, data, ~pattern);
        WRITE64(address, pattern);
    }

    __bdk_dram_flush_to_mem_range(area, max_address);
    BDK_DCACHE_INVALIDATE;

    /* Pass 6: ascending addresses, read pattern. */
    BDK_TRACE(DRAM_TEST, "    [0x%016llx:0x%016llx] Phase6, address incrementing\n", area, max_address-1);
    for (uint64_t address = area; address < max_address; address += 8)
    {
        uint64_t data = READ64(address);
        if (bdk_unlikely(data != pattern))
            failures += __bdk_dram_retry_failure(1, address, data, pattern);
    }

    return failures;
}

/**
 * Use test_mem_march_c() with a all ones pattern
 *
 * @param area   Start of the physical memory area
 * @param max_address
 *               End of the physical memory area
 * @param bursts Number of time to repeat the test over the entire area
 *
 * @return Number of errors, zero on success
 */
int __bdk_dram_test_mem_solid(uint64_t area, uint64_t max_address, int bursts)
{
    int failures = 0;
    for (int burst = 0; burst < bursts; burst++)
        failures += test_mem_march_c(area, max_address, -1);
    return failures;
}

/**
 * Use test_mem_march_c() with a 0x55 pattern
 *
 * @param area   Start of the physical memory area
 * @param max_address
 *               End of the physical memory area
 * @param bursts Number of time to repeat the test over the entire area
 *
 * @return Number of errors, zero on success
 */
int __bdk_dram_test_mem_checkerboard(uint64_t area, uint64_t max_address, int bursts)
{
    int failures = 0;
    for (int burst = 0; burst < bursts; burst++)
        failures += test_mem_march_c(area, max_address, 0x5555555555555555L);
    return failures;
}

/**
 * Write a pseudo random pattern to memory and verify it
 *
 * @param area   Start of the physical memory area
 * @param max_address
 *               End of the physical memory area
 * @param bursts Number of time to repeat the test over the entire area
 *
 * @return Number of errors, zero on success
 */
int __bdk_dram_test_mem_random(uint64_t area, uint64_t max_address, int bursts)
{
    /* This constant is used to increment the pattern after every DWORD. This
       makes only the first DWORD truly random, but saves us processing
       power generating the random values */
    const uint64_t INC = 0x1010101010101010ULL;

    int failures = 0;
    for (int burst = 0; burst < bursts; burst++)
    {
        const uint64_t init_pattern = bdk_rng_get_random64();
        uint64_t pattern = init_pattern;

        /* Write the pattern to memory. Each location receives the address
         * of the location. A second write pass is needed to force all of
         * the cached memory out to the DDR.
         */
        for (uint64_t address = area; address < max_address; address += 8)
        {
            WRITE64(address, pattern);
            pattern += INC;
        }

        __bdk_dram_flush_to_mem_range(area, max_address);
        BDK_DCACHE_INVALIDATE;

        /* Read the written memory and confirm that it has the expected
         * data pattern.
         */
        pattern = init_pattern;
        for (uint64_t address = area; address < max_address; address += 8)
        {
            uint64_t data = READ64(address);
            if (bdk_unlikely(data != pattern))
                failures += __bdk_dram_retry_failure(burst, address, data, pattern);
            pattern += INC;
        }
    }
    return failures;
}

#if !USE_PREDICTION_CODE_VERSIONS
/**
 * test_mem_xor
 *
 * @param area   Start of the physical memory area
 * @param max_address
 *               End of the physical memory area
 * @param bursts Number of time to repeat the test over the entire area
 *
 * @return Number of errors, zero on success
 */
int __bdk_dram_test_mem_xor(uint64_t area, uint64_t max_address, int bursts)
{
    int failures = 0;

    uint64_t extent = max_address - area;
    uint64_t count = (extent / sizeof(uint64_t)) / 2;

    /* Fill both halves of the memory area with identical randomized data.
     */
    uint64_t address1 = area;
    uint64_t address2 = area + count * sizeof(uint64_t);

    uint64_t pattern = bdk_rng_get_random64();

    for (uint64_t j = 0; j < count; j++)
    {
        uint64_t p = pattern * address1;
        WRITE64(address1, p);
        WRITE64(address2, p);
        address1 += 8;
        address2 += 8;
    }
    __bdk_dram_flush_to_mem_range(area, max_address);
    BDK_DCACHE_INVALIDATE;

    /* Make a series of passes over the memory areas. */
    for (int burst = 0; burst < bursts; burst++)
    {
        /* XOR the data with a random value, applying the change to both
         * memory areas.
         */
        address1 = area;
        address2 = area + count * sizeof(uint64_t);

        pattern = bdk_rng_get_random64();

        for (uint64_t j = 0; j < count; j++)
        {
            if ((address1 & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address1, BDK_CACHE_LINE_SIZE);
            if ((address2 & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address2, BDK_CACHE_LINE_SIZE);
            WRITE64(address1, READ64(address1) ^ pattern);
            WRITE64(address2, READ64(address2) ^ pattern);
            address1 += 8;
            address2 += 8;
        }

        __bdk_dram_flush_to_mem_range(area, max_address);
        BDK_DCACHE_INVALIDATE;

        /* Look for differences in the areas. If there is a mismatch, reset
         * both memory locations with the same pattern. Failing to do so
         * means that on all subsequent passes the pair of locations remain
         * out of sync giving spurious errors.
         */
        address1 = area;
        address2 = area + count * sizeof(uint64_t);
        for (uint64_t j = 0; j < count; j++)
        {
            if ((address1 & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address1, BDK_CACHE_LINE_SIZE);
            if ((address2 & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address2, BDK_CACHE_LINE_SIZE);
            uint64_t d1 = READ64(address1);
            uint64_t d2 = READ64(address2);
            if (bdk_unlikely(d1 != d2))
            {
		failures += __bdk_dram_retry_failure2(burst, address1, d1, address2, d2);

                // Synchronize the two areas, adjusting for the error.
                WRITE64(address1, d2);
                WRITE64(address2, d2);
            }
            address1 += 8;
            address2 += 8;
        }
    }
    return failures;
}

/**
 * test_mem_rows
 *
 * Write a pattern of alternating 64-bit words of all one bits and then all 0
 * bits. This pattern generates the maximum amount of simultaneous switching
 * activity on the memory channels. Each pass flips the pattern with words
 * going from all ones to all zeros and vice versa.
 *
 * @param area   Start of the physical memory area
 * @param max_address
 *               End of the physical memory area
 * @param bursts Number of times to repeat the test over the entire area
 *
 * @return Number of errors, zero on success
 */
int __bdk_dram_test_mem_rows(uint64_t area, uint64_t max_address, int bursts)
{
    int failures = 0;
    uint64_t pattern = 0x0;
    uint64_t extent = (max_address - area);
    uint64_t count  = (extent / 2) / sizeof(uint64_t); // in terms of 64bit words

    /* Fill both halves of the memory area with identical data pattern. Odd
     * address 64-bit words get the pattern, while even address words get the
     * inverted pattern.
     */
    uint64_t address1 = area;
    uint64_t address2 = area + count * sizeof(uint64_t);

    for (uint64_t j = 0; j < (count / 2); j++)
    {
        WRITE64(address1, pattern);
        WRITE64(address2, pattern);
        address1 += 8;
        address2 += 8;
        WRITE64(address1, ~pattern);
        WRITE64(address2, ~pattern);
        address1 += 8;
        address2 += 8;
    }
    __bdk_dram_flush_to_mem_range(area, max_address);
    BDK_DCACHE_INVALIDATE;

    /* Make a series of passes over the memory areas. */
    for (int burst = 0; burst < bursts; burst++)
    {
        /* Invert the data, applying the change to both memory areas. Thus on
	 * alternate passes, the data flips from 0 to 1 and vice versa.
         */
        address1 = area;
        address2 = area + count * sizeof(uint64_t);
        for (uint64_t j = 0; j < count; j++)
        {
            WRITE64(address1, ~READ64(address1));
            WRITE64(address2, ~READ64(address2));
            address1 += 8;
            address2 += 8;
        }
        __bdk_dram_flush_to_mem_range(area, max_address);
        BDK_DCACHE_INVALIDATE;

        /* Look for differences in the areas. If there is a mismatch, reset
         * both memory locations with the same pattern. Failing to do so
         * means that on all subsequent passes the pair of locations remain
         * out of sync giving spurious errors.
         */
        address1 = area;
        address2 = area + count * sizeof(uint64_t);
        for (uint64_t j = 0; j < count; j++)
        {
            uint64_t d1 = READ64(address1);
            uint64_t d2 = READ64(address2);
            if (bdk_unlikely(d1 != d2))
            {
		failures += __bdk_dram_retry_failure2(burst, address1, d1, address2, d2);

                // Synchronize the two areas, adjusting for the error.
                WRITE64(address1, d2);
                WRITE64(address2, d2);
            }
            address1 += 8;
            address2 += 8;
        }
    }
    return failures;
}
#endif /* !USE_PREDICTION_CODE_VERSIONS */

#if USE_PREDICTION_CODE_VERSIONS
//////////////////////////// this is the new code...

int __bdk_dram_test_mem_xor(uint64_t area, uint64_t max_address, int bursts)
{
    int failures = 0;
    int burst;

    uint64_t extent = max_address - area;
    uint64_t count  = (extent / sizeof(uint64_t)) / 2;
    uint64_t offset = count * sizeof(uint64_t);
    uint64_t area2  = area + offset;

    /* Fill both halves of the memory area with identical randomized data.
     */
    uint64_t address1 = area;

    uint64_t pattern1 = bdk_rng_get_random64();
    uint64_t pattern2 = 0;
    uint64_t this_pattern;

    uint64_t p;
    uint64_t d1, d2;

    // move the multiplies outside the loop
    uint64_t pbase = address1 * pattern1;
    uint64_t pincr = 8 * pattern1;
    uint64_t ppred;

    p = pbase;
    while (address1 < area2)
    {
        WRITE64(address1         , p);
        WRITE64(address1 + offset, p);
        address1 += 8;
        p += pincr;
    }
    __bdk_dram_flush_to_mem_range(area, max_address);
    BDK_DCACHE_INVALIDATE;

    /* Make a series of passes over the memory areas. */
    for (burst = 0; burst < bursts; burst++)
    {
        /* XOR the data with a random value, applying the change to both
         * memory areas.
         */
        address1 = area;

        this_pattern = bdk_rng_get_random64();
        pattern2 ^= this_pattern;

        while (address1 < area2)
        {
#if 1
            if ((address1 & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address1, BDK_CACHE_LINE_SIZE);
            if (((address1 + offset) & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address1 + offset, BDK_CACHE_LINE_SIZE);
#endif
            WRITE64(address1         , READ64(address1         ) ^ this_pattern);
            WRITE64(address1 + offset, READ64(address1 + offset) ^ this_pattern);
            address1 += 8;
        }

        __bdk_dram_flush_to_mem_range(area, max_address);
        BDK_DCACHE_INVALIDATE;

        /* Look for differences from the expected pattern in both areas.
         * If there is a mismatch, reset the appropriate memory location
         * with the correct pattern. Failing to do so
         * means that on all subsequent passes the erroring locations
         * will be out of sync, giving spurious errors.
         */
        address1 = area;
        ppred = pbase;

        while (address1 < area2)
        {
#if 1
            if ((address1 & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address1, BDK_CACHE_LINE_SIZE);
            if (((address1 + offset) & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address1 + offset, BDK_CACHE_LINE_SIZE);
#endif
            d1 = READ64(address1         );
            d2 = READ64(address1 + offset);

            p = ppred ^ pattern2;

            if (bdk_unlikely(d1 != p)) {
                failures += __bdk_dram_retry_failure(burst, address1, d1, p);
                // Synchronize the area, adjusting for the error.
                //WRITE64(address1, p); // retries should do this
            }
            if (bdk_unlikely(d2 != p)) {
                failures += __bdk_dram_retry_failure(burst, address1 + offset, d2, p);
                // Synchronize the area, adjusting for the error.
                //WRITE64(address1 + offset, p); // retries should do this
            }

            address1 += 8;
            ppred += pincr;

        } /* while (address1 < area2) */
    } /* for (int burst = 0; burst < bursts; burst++) */
    return failures;
}

//////////////// this is the new code...

int __bdk_dram_test_mem_rows(uint64_t area, uint64_t max_address, int bursts)
{
    int failures = 0;

    uint64_t pattern1 = 0x0;
    uint64_t extent = (max_address - area);
    uint64_t count  = (extent / 2) / sizeof(uint64_t); // in terms of 64bit words
    uint64_t offset = count * sizeof(uint64_t);
    uint64_t area2 = area + offset;
    uint64_t pattern2;
    uint64_t d1, d2;
    int burst;

    /* Fill both halves of the memory area with identical data pattern. Odd
     * address 64-bit words get the pattern, while even address words get the
     * inverted pattern.
     */
    uint64_t address1 = area;

    pattern2 = pattern1; // start with original pattern

    while (address1 < area2)
    {
        WRITE64(address1         , pattern2);
        WRITE64(address1 + offset, pattern2);
        address1 += 8;
        pattern2 = ~pattern2; // flip for next slots
    }

    __bdk_dram_flush_to_mem_range(area, max_address);
    BDK_DCACHE_INVALIDATE;

    /* Make a series of passes over the memory areas. */
    for (burst = 0; burst < bursts; burst++)
    {
        /* Invert the data, applying the change to both memory areas. Thus on
         * alternate passes, the data flips from 0 to 1 and vice versa.
         */
        address1 = area;

        while (address1 < area2)
        {
            if ((address1 & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address1         , BDK_CACHE_LINE_SIZE);
            if (((address1 + offset) & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address1 + offset, BDK_CACHE_LINE_SIZE);

            WRITE64(address1         , ~READ64(address1         ));
            WRITE64(address1 + offset, ~READ64(address1 + offset));
            address1 += 8;
        }

        __bdk_dram_flush_to_mem_range(area, max_address);
        BDK_DCACHE_INVALIDATE;

        /* Look for differences in the areas. If there is a mismatch, reset
         * both memory locations with the same pattern. Failing to do so
         * means that on all subsequent passes the pair of locations remain
         * out of sync giving spurious errors.
         */
        address1 = area;
        pattern1 = ~pattern1; // flip the starting pattern to match above loop
        pattern2 = pattern1;  // slots have been flipped by the above loop

        while (address1 < area2)
        {
            if ((address1 & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address1         , BDK_CACHE_LINE_SIZE);
            if (((address1 + offset) & BDK_CACHE_LINE_MASK) == 0)
                BDK_PREFETCH(address1 + offset, BDK_CACHE_LINE_SIZE);

            d1 = READ64(address1         );
            d2 = READ64(address1 + offset);

            if (bdk_unlikely(d1 != pattern2)) {
                failures += __bdk_dram_retry_failure(burst, address1, d1, pattern2);
                // Synchronize the area, adjusting for the error.
                //WRITE64(address1, pattern2); // retries should do this
            }
            if (bdk_unlikely(d2 != pattern2)) {
                failures += __bdk_dram_retry_failure(burst, address1 + offset, d2, pattern2);
                // Synchronize the two areas, adjusting for the error.
                //WRITE64(address1 + offset, pattern2); // retries should do this
            }

            address1 += 8;
            pattern2 = ~pattern2; // flip for next pair of slots
        }
    }
    return failures;
}
#endif /* USE_PREDICTION_CODE_VERSIONS */
