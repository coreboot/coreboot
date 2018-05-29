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
 * Functions for accessing memory and CSRs when we are compiling
 * natively.
 *
 * <hr>$Revision: 38306 $<hr>
*/

/**
 * Convert a memory pointer (void*) into a hardware compatible
 * memory address (uint64_t). Cavium hardware widgets don't
 * understand logical addresses.
 *
 * @param ptr    C style memory pointer
 * @return Hardware physical address
 */
static inline uint64_t bdk_ptr_to_phys(void *ptr) __attribute__ ((pure, always_inline));
static inline uint64_t bdk_ptr_to_phys(void *ptr)
{
    bdk_warn_if(!ptr, "bdk_ptr_to_phys() passed a NULL\n");
    return (long)ptr;
}


/**
 * Convert a hardware physical address (uint64_t) into a
 * memory pointer (void *).
 *
 * @param physical_address
 *               Hardware physical address to memory
 * @return Pointer to memory
 */
static inline void *bdk_phys_to_ptr(uint64_t physical_address) __attribute__ ((pure, always_inline));
static inline void *bdk_phys_to_ptr(uint64_t physical_address)
{
    bdk_warn_if(physical_address==0, "bdk_phys_to_ptr() passed a zero address\n");
    return (void*)(long)physical_address;
}


/* We have a full 64bit ABI. Writing to a 64bit address can be done with
    a simple volatile pointer */
#define BDK_BUILD_WRITE64(TYPE)                                     \
static inline void bdk_write64_##TYPE(uint64_t addr, TYPE##_t val) __attribute__ ((always_inline)); \
static inline void bdk_write64_##TYPE(uint64_t addr, TYPE##_t val)  \
{                                                                   \
    *(volatile TYPE##_t *)bdk_phys_to_ptr(addr) = val;              \
}

/* We have a full 64bit ABI. Writing to a 64bit address can be done with
    a simple volatile pointer */
#define BDK_BUILD_READ64(TYPE)                                      \
static inline TYPE##_t bdk_read64_##TYPE(uint64_t addr) __attribute__ ((always_inline)); \
static inline TYPE##_t bdk_read64_##TYPE(uint64_t addr)             \
{                                                                   \
    return *(volatile TYPE##_t *)bdk_phys_to_ptr(addr);             \
}

/* The following defines 8 functions for writing to a 64bit address. Each
    takes two arguments, the address and the value to write.
    bdk_write64_int64      bdk_write64_uint64
    bdk_write64_int32      bdk_write64_uint32
    bdk_write64_int16      bdk_write64_uint16
    bdk_write64_int8       bdk_write64_uint8 */
BDK_BUILD_WRITE64(int64)
BDK_BUILD_WRITE64(int32)
BDK_BUILD_WRITE64(int16)
BDK_BUILD_WRITE64(int8)
BDK_BUILD_WRITE64(uint64)
BDK_BUILD_WRITE64(uint32)
BDK_BUILD_WRITE64(uint16)
BDK_BUILD_WRITE64(uint8)

/* The following defines 8 functions for reading from a 64bit address. Each
    takes the address as the only argument
    bdk_read64_int64       bdk_read64_uint64
    bdk_read64_int32       bdk_read64_uint32
    bdk_read64_int16       bdk_read64_uint16
    bdk_read64_int8        bdk_read64_uint8 */
BDK_BUILD_READ64(int64)
BDK_BUILD_READ64(int32)
BDK_BUILD_READ64(int16)
BDK_BUILD_READ64(int8)
BDK_BUILD_READ64(uint64)
BDK_BUILD_READ64(uint32)
BDK_BUILD_READ64(uint16)
BDK_BUILD_READ64(uint8)


/**
 * Returns the number of bits set in the provided value.
 * Simple wrapper for POP instruction.
 *
 * @param val    32 bit value to count set bits in
 *
 * @return Number of bits set
 */
static inline uint32_t bdk_pop(uint32_t val)
{
    return __builtin_popcount(val);
}


/**
 * Returns the number of bits set in the provided value.
 * Simple wrapper for DPOP instruction.
 *
 * @param val    64 bit value to count set bits in
 *
 * @return Number of bits set
 */
static inline int bdk_dpop(uint64_t val)
{
    return __builtin_popcountl(val);
}

