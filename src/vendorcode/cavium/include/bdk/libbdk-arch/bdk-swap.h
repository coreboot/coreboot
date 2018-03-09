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
 * Utility functions for endian swapping
 *
 * <hr>$Revision: 32636 $<hr>
 *
 * @addtogroup hal
 * @{
 */

/**
 * Byte swap a 16 bit number
 *
 * @param x      16 bit number
 * @return Byte swapped result
 */
static inline uint16_t bdk_swap16(uint16_t x)
{
    return ((uint16_t)((((uint16_t)(x) & (uint16_t)0x00ffU) << 8) |
                       (((uint16_t)(x) & (uint16_t)0xff00U) >> 8) ));
}


/**
 * Byte swap a 32 bit number
 *
 * @param x      32 bit number
 * @return Byte swapped result
 */
static inline uint32_t bdk_swap32(uint32_t x)
{
#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3))
    return __builtin_bswap32(x);
#else
     x = ((x<<8)&0xFF00FF00) | ((x>>8)&0x00FF00FF);
     x = (x>>16) | (x<<16);
     return x;
#endif
}


/**
 * Byte swap a 64 bit number
 *
 * @param x      64 bit number
 * @return Byte swapped result
 */
static inline uint64_t bdk_swap64(uint64_t x)
{
#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3))
    return __builtin_bswap64(x);
#else
     x = ((x<< 8)&0xFF00FF00FF00FF00ULL) | ((x>> 8)&0x00FF00FF00FF00FFULL);
     x = ((x<<16)&0xFFFF0000FFFF0000ULL) | ((x>>16)&0x0000FFFF0000FFFFULL);
     return (x>>32) | (x<<32);
#endif
}


#if __BYTE_ORDER == __BIG_ENDIAN

#define bdk_cpu_to_le16(x) bdk_swap16(x)
#define bdk_cpu_to_le32(x) bdk_swap32(x)
#define bdk_cpu_to_le64(x) bdk_swap64(x)

#define bdk_cpu_to_be16(x) (x)
#define bdk_cpu_to_be32(x) (x)
#define bdk_cpu_to_be64(x) (x)

#else

#define bdk_cpu_to_le16(x) (x)
#define bdk_cpu_to_le32(x) (x)
#define bdk_cpu_to_le64(x) (x)

#define bdk_cpu_to_be16(x) bdk_swap16(x)
#define bdk_cpu_to_be32(x) bdk_swap32(x)
#define bdk_cpu_to_be64(x) bdk_swap64(x)

#endif

#define bdk_le16_to_cpu(x) bdk_cpu_to_le16(x)
#define bdk_le32_to_cpu(x) bdk_cpu_to_le32(x)
#define bdk_le64_to_cpu(x) bdk_cpu_to_le64(x)

#define bdk_be16_to_cpu(x) bdk_cpu_to_be16(x)
#define bdk_be32_to_cpu(x) bdk_cpu_to_be32(x)
#define bdk_be64_to_cpu(x) bdk_cpu_to_be64(x)

/** @} */
