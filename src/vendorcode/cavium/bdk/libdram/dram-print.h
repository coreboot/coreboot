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
 * Functions for diplaying output in libdram. Internal use only.
 */

typedef enum {
    // low 4 bits are verbosity level
    VBL_OFF       =   0, // use this only to init dram_verbosity
    VBL_ALL       =   0, // use this only in VBL_PR() to get printf equiv
    VBL_NORM      =   1,
    VBL_FAE       =   2,
    VBL_TME       =   3,
    VBL_DEV       =   4,
    VBL_DEV2      =   5,
    VBL_DEV3      =   6,
    VBL_DEV4      =   7,
    VBL_NONE      =  15, // use this only in VBL_PR() to get no printing
    // upper 4 bits are special verbosities
    VBL_SEQ       =  16,
    VBL_CSRS      =  32,
    VBL_SPECIAL   =  48,
    // force at least 8 bits for enum
    VBL_LAST      = 255
} dram_verbosity_t;

extern dram_verbosity_t dram_verbosity;

// "level" should be 1-7, or only one of the special bits
// let the compiler optimize the test for verbosity
#define is_verbosity_level(level)   ((int)(dram_verbosity & 0x0f) >= (level))
#define is_verbosity_special(level) (((int)(dram_verbosity & 0xf0) & (level)) != 0)
#define dram_is_verbose(level)      (((level) & VBL_SPECIAL) ? is_verbosity_special(level) : is_verbosity_level(level))

#define VB_PRT(level, format, ...)         \
    do {                                    \
        if (dram_is_verbose(level))         \
            printf(format, ##__VA_ARGS__);  \
    } while (0)

#define ddr_print(format, ...) VB_PRT(VBL_NORM, format, ##__VA_ARGS__)

#define error_print(format, ...) printf(format, ##__VA_ARGS__)

#ifdef DEBUG_DEBUG_PRINT
    #define debug_print(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define debug_print(format, ...) do {} while (0)
#endif
