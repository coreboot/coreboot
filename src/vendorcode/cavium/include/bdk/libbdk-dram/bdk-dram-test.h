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

/**
 * @file
 *
 * Functions for configuring DRAM.
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @defgroup dram DRAM related functions
 * @{
 */

/**
 * Flags to pass to DRAM tests to control behavior
 */
typedef enum
{
    /* Which nodes to check. If none are specified, default to all */
    BDK_DRAM_TEST_NODE0         = 1 << BDK_NODE_0,
    BDK_DRAM_TEST_NODE1         = 1 << BDK_NODE_1,
    BDK_DRAM_TEST_NODE2         = 1 << BDK_NODE_2,
    BDK_DRAM_TEST_NODE3         = 1 << BDK_NODE_3,
    BDK_DRAM_TEST_NO_STOP_ERROR = 1 << 8,  /**< Don't stop running tests on errors, continue counting all errors */
    BDK_DRAM_TEST_NO_PROGRESS   = 1 << 9,  /**< Don't report progress percentage during run, for batch runs */
    BDK_DRAM_TEST_NO_STATS      = 1 << 10, /**< Don't report usage status for LMC, or CCPI with USE_CCPI */
    BDK_DRAM_TEST_NO_BANNERS    = 1 << 11,  /**< Don't display banenrs at beginning of test */
    BDK_DRAM_TEST_USE_CCPI      = 1 << 12, /**< Test using other node across CCPI. Use to verify CCPI. This
                                            automatically enables CCPI usage reporting unless NO_STATS is
                                            also specified */
} bdk_dram_test_flags_t;

/**
 * Convert a test enumeration into a string
 *
 * @param test   Test to convert
 *
 * @return String for display
 */
extern const char* bdk_dram_get_test_name(int test);

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
extern int
bdk_dram_test(int test, uint64_t start_address, uint64_t length,
              bdk_dram_test_flags_t flags);

/**
 * Given a physical DRAM address, extract information about the node, LMC, DIMM,
 * rank, bank, row, and column that was accessed.
 *
 * @param address Physical address to decode
 * @param node    Node the address was for
 * @param lmc     LMC controller the address was for
 * @param dimm    DIMM the address was for
 * @param prank   Physical RANK on the DIMM
 * @param lrank   Logical RANK on the DIMM
 * @param bank    BANK on the DIMM
 * @param row     Row on the DIMM
 * @param col     Column on the DIMM
 */
extern void
bdk_dram_address_extract_info(uint64_t address, int *node, int *lmc, int *dimm,
                              int *prank, int *lrank, int *bank, int *row, int *col);

/**
 * Construct a physical address given the node, LMC, DIMM, rank, bank, row, and column.
 *
 * @param node    Node the address was for
 * @param lmc     LMC controller the address was for
 * @param dimm    DIMM the address was for
 * @param prank   Physical RANK on the DIMM
 * @param lrank   Logical RANK on the DIMM
 * @param bank    BANK on the DIMM
 * @param row     Row on the DIMM
 * @param col     Column on the DIMM
 */
extern uint64_t
bdk_dram_address_construct_info(bdk_node_t node, int lmc, int dimm,
                                int prank, int lrank, int bank, int row, int col);

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
extern void bdk_dram_test_inject_error(uint64_t address, int bit);

/* These variables count the number of ECC errors. They should only be accessed atomically */
/* Keep the counts per memory channel (LMC) for more detail. */
#define BDK_MAX_MEM_CHANS 4
extern int64_t __bdk_dram_ecc_single_bit_errors[BDK_MAX_MEM_CHANS];
extern int64_t __bdk_dram_ecc_double_bit_errors[BDK_MAX_MEM_CHANS];

/* These are internal support functions */
extern void __bdk_dram_flush_to_mem(uint64_t address);
extern void __bdk_dram_flush_to_mem_range(uint64_t area, uint64_t max_address);
extern void __bdk_dram_report_error(uint64_t address, uint64_t data, uint64_t correct, int burst, int fails);
extern void __bdk_dram_report_error2(uint64_t address1, uint64_t data1, uint64_t address2, uint64_t data2, int burst, int fails);
extern int  __bdk_dram_retry_failure(int burst, uint64_t address, uint64_t data, uint64_t expected);
extern int  __bdk_dram_retry_failure2(int burst, uint64_t address1, uint64_t data1, uint64_t address2, uint64_t data2);

static inline void __bdk_dram_write64(uint64_t address, uint64_t data)
{
    /* The DRAM code doesn't use the normal bdk_phys_to_ptr() because of the
       NULL check in it. This greatly slows down the memory tests */
    volatile uint64_t *ptr = (void*)address;
    *ptr = data;
}

static inline uint64_t __bdk_dram_read64(uint64_t address)
{
    /* The DRAM code doesn't use the normal bdk_phys_to_ptr() because of the
       NULL check in it. This greatly slows down the memory tests */
    volatile uint64_t *ptr = (void*)address;
    return *ptr;
}

/* This is the function prototype that all test must use. "start_address" is
   the first byte to be tested (inclusive), "end_address" is the address right
   after the region (exclusive). For example, if start_address equals
   end_address, no memory will be tested */
typedef int (*__bdk_dram_test_t)(uint64_t start_address, uint64_t end_address, int bursts);

/* These are the actual tests that get run. Each test is meant to be run with
   a small range and repeated on lots of cores and large ranges. The return
   value is the number of errors found */
extern int __bdk_dram_test_mem_address_bus(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_mem_checkerboard(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_mem_data_bus(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_mem_leftwalk0(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_mem_leftwalk1(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_mem_random(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_mem_rightwalk0(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_mem_rightwalk1(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_mem_rows(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_mem_self_addr(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_mem_solid(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_mem_xor(uint64_t start_address, uint64_t end_address, int bursts);
extern int __bdk_dram_test_fast_scan(uint64_t area, uint64_t max_address, int bursts);

/** @} */

