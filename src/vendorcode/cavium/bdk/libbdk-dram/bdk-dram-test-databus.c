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

/* Used for all memory reads/writes related to the test */
#define READ64(address) __bdk_dram_read64(address)
#define WRITE64(address, data) __bdk_dram_write64(address, data)

/* Build a 64bit mask out of a single hex digit */
#define REPEAT2(v) ((((uint64_t)v) << 4) | ((uint64_t)v))
#define REPEAT4(v) ((REPEAT2(v) << 8) | REPEAT2(v))
#define REPEAT8(v) ((REPEAT4(v) << 16) | REPEAT4(v))
#define REPEAT16(v) ((REPEAT8(v) << 32) | REPEAT8(v))

/**
 * Read memory and check that the data bus pattern is present. The pattern is a
 * sequence if 16 dwords created from the 16 hex digits repeated in each word.
 *
 * @param address Physical address to read. This must be cache line aligned.
 * @param bursts  Number of time to repeat the read test to verify stability
 *
 * @return Number of errors, zero means success
 */
static int read_data_bus_burst(uint64_t address, int bursts)
{
    int failures = 0;

    /* Loop over the burst so people using a scope have time to capture
       traces */
    for (int burst = 0; burst < bursts; burst++)
    {
        /* Invalidate all caches so we must read from DRAM */
        __bdk_dram_flush_to_mem(address);
        BDK_DCACHE_INVALIDATE;

        for (uint64_t digit = 0; digit < 16; digit++)
        {
            uint64_t a = address + digit * 8;
            uint64_t data = READ64(a);
            uint64_t correct = REPEAT16(digit);
            if (data != correct)
            {
                failures++;
                __bdk_dram_report_error(a, data, correct, burst, -1);
            }
        }
    }
    return failures;
}

/**
 * Write memory with a data bus pattern and check that it can be read correctly.
 * The pattern is a sequence if 16 dwords created from the 16 hex digits repeated
 * in each word.
 *
 * @param address Physical address to write. This must be cache line aligned. 128 bytes will be
 *                written starting at this address.
 * @param bursts  Number of time to repeat the write+read test to verify stability
 *
 * @return Number of errors, zero means success
 */
static int write_data_bus_burst(uint64_t address, int bursts)
{
    BDK_TRACE(DRAM_TEST, "[0x%016lx:0x%016lx] Writing incrementing digits\n",
        address, address + 127);
    /* Loop over the burst so people using a scope have time to capture
       traces */
    for (int burst = 0; burst < bursts; burst++)
    {
        /* Fill a cache line with an incrementing pattern. Each nibble
           in the 64bit word increments from 0 to 0xf */
        for (uint64_t digit = 0; digit < 16; digit++)
            WRITE64(address + digit * 8, REPEAT16(digit));
        /* Force the cache line out to memory */
        __bdk_dram_flush_to_mem(address);
    }
    return read_data_bus_burst(address, bursts);
}

/**
 * Read back the pattern written by write_data_bus_walk() nad
 * make sure it was stored properly.
 *
 * @param address Physical address to read. This must be cache line aligned.
 * @param bursts  Number of time to repeat the read test to verify stability
 * @param pattern Pattern basis for writes. See
 *                write_data_bus_walk()
 *
 * @return Number of errors, zero means success
 */
static int read_data_bus_walk(uint64_t address, int burst, uint64_t pattern)
{
    int failures = 0;

    /* Invalidate all caches so we must readfrom DRAM */
    __bdk_dram_flush_to_mem(address);
    BDK_DCACHE_INVALIDATE;

    uint64_t correct = pattern;
    for (uint64_t word = 0; word < 16; word++)
    {
        uint64_t a = address + word * 8;
        uint64_t data = READ64(a);
        if (data != correct)
        {
            failures++;
            __bdk_dram_report_error(a, data, correct, burst, -1);
        }
        uint64_t tmp = correct >> 63; /* Save top bit */
        correct <<= 1; /* Shift left one bit */
        correct |= tmp; /* Restore the top bit as bit 0 */
    }

    return failures;
}

/**
 * Write a pattern to a cache line, rotating it one bit for each DWORD. Read back
 * the pattern and make sure it was stored properly. The input pattern is rotated
 * left by one bit for each DWORD written.
 *
 * @param address Physical address to write. This must be cache line aligned. 128 bytes will be
 *                written starting at this address.
 * @param bursts  Number of time to repeat the write+read test to verify stability
 * @param pattern Pattern basis
 *
 * @return Number of errors, zero means success
 */
static void write_data_bus_walk(uint64_t address, int burst, uint64_t pattern)
{
    BDK_TRACE(DRAM_TEST, "[0x%016lx:0x%016lx] Writing walking pattern 0x%016lx\n",
        address, address + 127, pattern);

    uint64_t a = address;
    uint64_t d = pattern;

    /* Fill a cache line with pattern. Each 64bit work will have the
       pattern rotated left one bit */
    for (uint64_t word = 0; word < 16; word++)
    {
        WRITE64(a, d);
        a += 8;
        uint64_t tmp = d >> 63; /* Save top bit */
        d <<= 1; /* Shift left one bit */
        d |= tmp; /* Restore the top bit as bit 0 */
    }
    /* Force the cache line out to memory */
    __bdk_dram_flush_to_mem(address);
}

/**
 * The goal of these tests are to toggle every DDR data pin, one at a time or in
 * related groups, to isolate any short circuits between the data pins or open
 * circuits where the pin is not connected to the DDR memory. A board which fails
 * one of these tests has severe problems and will not be able to run any of the
 * later test patterns.
 *
 * @param start_address
 *               Physical address of a cache line to
 *               use for the test. Only this cache line is
 *               written.
 * @param end_address
 *               Top end of the address range. Currently unused
 * @param bursts Number of time to repeats writes+reads to insure stability
 *
 * @return Number of errors, zero means success
 */
int __bdk_dram_test_mem_data_bus(uint64_t start_address, uint64_t end_address, int bursts)
{
    int failures = 0;

    /* Incrementing pattern: 0x0 - 0xf in each nibble */
    failures += write_data_bus_burst(start_address, bursts);

    /* Walking ones. Run with 1, 2, and 3 bits walking */
    for (int bits = 1; bits <= 3; bits++)
    {
        for (int burst = 0; burst < bursts; burst++)
        {
            /* Each write_data_bus_walk() call write 16 dword, so step by 16 */
            for (int i = 0; i < 64; i += 16)
            {
                uint64_t pattern = bdk_build_mask(bits) << i;
                write_data_bus_walk(start_address + i*8, burst, pattern);
            }
            /* Each read_data_bus_walk() call write 16 dword, so step by 16 */
            for (int i = 0; i < 64; i += 16)
            {
                uint64_t pattern = bdk_build_mask(bits) << i;
                failures += read_data_bus_walk(start_address + i*8, burst, pattern);
            }
        }
    }

    /* Walking zeros. Run with 1, 2, and 3 bits walking */
    for (int bits = 1; bits <= 3; bits++)
    {
        for (int burst = 0; burst < bursts; burst++)
        {
            /* Each write_data_bus_walk() call write 16 dword, so step by 16 */
            for (int i = 0; i < 64; i += 16)
            {
                uint64_t pattern = ~(bdk_build_mask(bits) << i);
                write_data_bus_walk(start_address + i*8, burst, pattern);
            }
            /* Each read_data_bus_walk() call write 16 dword, so step by 16 */
            for (int i = 0; i < 64; i += 16)
            {
                uint64_t pattern = ~(bdk_build_mask(bits) << i);
                failures += read_data_bus_walk(start_address + i*8, burst, pattern);
            }
        }
    }
    return failures;
}

