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
 * Function prototypes for accessing memory and CSRs.
 *
 * <hr>$Revision: 38306 $<hr>
 *
 * @addtogroup hal
 * @{
*/

#define BDK_FUNCTION static inline

/**
 * Convert a memory pointer (void*) into a hardware compatible
 * memory address (uint64_t). Cavium hardware widgets don't
 * understand logical addresses.
 *
 * @param ptr    C style memory pointer
 * @return Hardware physical address
 */
BDK_FUNCTION uint64_t bdk_ptr_to_phys(void *ptr);

/**
 * Convert a hardware physical address (uint64_t) into a
 * memory pointer (void *).
 *
 * @param physical_address
 *               Hardware physical address to memory
 * @return Pointer to memory
 */
BDK_FUNCTION void *bdk_phys_to_ptr(uint64_t physical_address);

BDK_FUNCTION void bdk_write64_int64(uint64_t address, int64_t value);
BDK_FUNCTION void bdk_write64_uint64(uint64_t address, uint64_t value);
BDK_FUNCTION void bdk_write64_int32(uint64_t address, int32_t value);
BDK_FUNCTION void bdk_write64_uint32(uint64_t address, uint32_t value);
BDK_FUNCTION void bdk_write64_int16(uint64_t address, int16_t value);
BDK_FUNCTION void bdk_write64_uint16(uint64_t address, uint16_t value);
BDK_FUNCTION void bdk_write64_int8(uint64_t address, int8_t value);
BDK_FUNCTION void bdk_write64_uint8(uint64_t address, uint8_t value);

BDK_FUNCTION int64_t bdk_read64_int64(uint64_t address);
BDK_FUNCTION uint64_t bdk_read64_uint64(uint64_t address);
BDK_FUNCTION int32_t bdk_read64_int32(uint64_t address);
BDK_FUNCTION uint32_t bdk_read64_uint32(uint64_t address);
BDK_FUNCTION int16_t bdk_read64_int16(uint64_t address);
BDK_FUNCTION uint16_t bdk_read64_uint16(uint64_t address);
BDK_FUNCTION int8_t bdk_read64_int8(uint64_t address);
BDK_FUNCTION uint8_t bdk_read64_uint8(uint64_t address);

/**
 * Returns the number of bits set in the provided value.
 * Simple wrapper for POP instruction.
 *
 * @param val    32 bit value to count set bits in
 *
 * @return Number of bits set
 */
BDK_FUNCTION uint32_t bdk_pop(uint32_t val);

/**
 * Returns the number of bits set in the provided value.
 * Simple wrapper for DPOP instruction.
 *
 * @param val    64 bit value to count set bits in
 *
 * @return Number of bits set
 */
BDK_FUNCTION int bdk_dpop(uint64_t val);

/**
 * Wait for the specified number of core clock cycles
 *
 * @param cycles
 */
extern void bdk_wait(uint64_t cycles);

/**
 * Wait for the specified number of micro seconds
 *
 * @param usec   micro seconds to wait
 */
extern void bdk_wait_usec(uint64_t usec);

/**
 * Perform a soft reset of the chip
 *
 * @return
 */
extern void bdk_reset_chip(bdk_node_t node);

#undef BDK_FUNCTION

/** @} */
