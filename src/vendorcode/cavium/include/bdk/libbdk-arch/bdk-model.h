#ifndef __CB_BDK_MODEL_H__
#define __CB_BDK_MODEL_H__

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
 * Functions for determining which Cavium chip you are running
 * on.
 *
 * <hr>$Revision: 49448 $<hr>
 * @addtogroup chips
 * @{
 */


/* Flag bits in top byte. The top byte of MIDR_EL1 is defined
   as ox43, the Cavium implementer code. In this number, bits
   7,5,4 are defiend as zero. We use these bits to signal
   that revision numbers should be ignored. It isn't ideal
   that these are in the middle of an already defined field,
   but this keeps the model numbers as 32 bits */
#define __OM_IGNORE_REVISION        0x80000000
#define __OM_IGNORE_MINOR_REVISION  0x20000000
#define __OM_IGNORE_MODEL           0x10000000

#define CAVIUM_CN88XX_PASS1_0   0x430f0a10
#define CAVIUM_CN88XX_PASS1_1   0x430f0a11
#define CAVIUM_CN88XX_PASS2_0   0x431f0a10
#define CAVIUM_CN88XX_PASS2_1   0x431f0a11
#define CAVIUM_CN88XX_PASS2_2   0x431f0a12
#define CAVIUM_CN88XX           (CAVIUM_CN88XX_PASS1_0 | __OM_IGNORE_REVISION)
#define CAVIUM_CN88XX_PASS1_X   (CAVIUM_CN88XX_PASS1_0 | __OM_IGNORE_MINOR_REVISION)
#define CAVIUM_CN88XX_PASS2_X   (CAVIUM_CN88XX_PASS2_0 | __OM_IGNORE_MINOR_REVISION)
/* Note CN86XX will also match the CN88XX macros above. See comment in
   CAVIUM_IS_MODEL() about MIO_FUS_FUSE_NUM_E::CHIP_IDX bits 6-7 */

#define CAVIUM_CN83XX_PASS1_0   0x430f0a30
#define CAVIUM_CN83XX           (CAVIUM_CN83XX_PASS1_0 | __OM_IGNORE_REVISION)
#define CAVIUM_CN83XX_PASS1_X   (CAVIUM_CN83XX_PASS1_0 | __OM_IGNORE_MINOR_REVISION)

#define CAVIUM_CN81XX_PASS1_0   0x430f0a20
#define CAVIUM_CN81XX           (CAVIUM_CN81XX_PASS1_0 | __OM_IGNORE_REVISION)
#define CAVIUM_CN81XX_PASS1_X   (CAVIUM_CN81XX_PASS1_0 | __OM_IGNORE_MINOR_REVISION)
/* Note CN80XX will also match the CN81XX macros above. See comment in
   CAVIUM_IS_MODEL() about MIO_FUS_FUSE_NUM_E::CHIP_IDX bits 6-7 */

#define CAVIUM_CN93XX_PASS1_0   0x430f0b20
#define CAVIUM_CN93XX           (CAVIUM_CN93XX_PASS1_0 | __OM_IGNORE_REVISION)
#define CAVIUM_CN93XX_PASS1_X   (CAVIUM_CN93XX_PASS1_0 | __OM_IGNORE_MINOR_REVISION)

/* These match entire families of chips */
#define CAVIUM_CN8XXX           (CAVIUM_CN88XX_PASS1_0 | __OM_IGNORE_MODEL)
#define CAVIUM_CN9XXX           (CAVIUM_CN93XX_PASS1_0 | __OM_IGNORE_MODEL)

static inline uint64_t cavium_get_model(void) __attribute__ ((pure, always_inline));
static inline uint64_t cavium_get_model(void)
{
#ifdef BDK_BUILD_HOST
    extern uint32_t thunder_remote_get_model(void) __attribute__ ((pure));
    return thunder_remote_get_model();
#else
    uint64_t result;
    asm ("mrs %[rd],MIDR_EL1" : [rd] "=r" (result));
    return result;
#endif
}

/**
 * Return non-zero if the chip matech the passed model.
 *
 * @param arg_model One of the CAVIUM_* constants for chip models and passes
 *
 * @return Non-zero if match
 */
static inline int CAVIUM_IS_MODEL(uint32_t arg_model) __attribute__ ((pure, always_inline));
static inline int CAVIUM_IS_MODEL(uint32_t arg_model)
{
    const uint32_t FAMILY = 0xff00;     /* Bits 15:8, generation t8x=0xa, t9x=0xb */
    const uint32_t PARTNUM = 0xfff0;    /* Bits 15:4, chip t88=0x81, t81=0xa2, t83=0xa3, etc */
    const uint32_t VARIANT = 0xf00000;  /* Bits 23:20, major pass */
    const uint32_t REVISION = 0xf;      /* Bits 3:0, minor pass */

    /* Note that the model matching here is unaffected by
       MIO_FUS_FUSE_NUM_E::CHIP_IDX bits 6-7, which are the alternate package
       fuses. These bits don't affect MIDR_EL1, so:
            CN80XX will match CN81XX (CHIP_IDX 6 is set for 676 ball package)
            CN80XX will match CN81XX (CHIP_IDX 7 is set for 555 ball package)
            CN86XX will match CN88XX (CHIP_IDX 6 is set for 676 ball package)
       Alternate package parts are detected using MIO_FUS_DAT2[chip_id],
       specifically the upper two bits */

    uint32_t my_model = cavium_get_model();
    uint32_t mask;

    if (arg_model & __OM_IGNORE_MODEL)
        mask = FAMILY; /* Matches chip generation (CN8XXX, CN9XXX) */
    else if (arg_model & __OM_IGNORE_REVISION)
        mask = PARTNUM; /* Matches chip model (CN88XX, CN81XX, CN83XX) */
    else if (arg_model & __OM_IGNORE_MINOR_REVISION)
        mask = PARTNUM | VARIANT; /* Matches chip model and major version */
    else
        mask = PARTNUM | VARIANT | REVISION; /* Matches chip model, major version, and minor version */
    return ((arg_model & mask) == (my_model & mask));
}

/**
 * Return non-zero if the die is in an alternate package. The
 * normal is_model() checks will treat alternate package parts
 * as all the same, where this function can be used to detect
 * them. The return value is the upper two bits of
 * MIO_FUS_DAT2[chip_id]. Most alternate packages use bit 6,
 * which will return 1 here. Parts with a second alternative
 * will use bit 7, which will return 2.
 *
 * @param arg_model One of the CAVIUM_* constants for chip models and passes
 *
 * @return Non-zero if an alternate package
 *         0 = Normal package
 *         1 = Alternate package 1 (CN86XX, CN80XX with 555 balls)
 *         2 = Alternate package 2 (CN80XX with 676 balls)
 *         3 = Alternate package 3 (Currently unused)
 */
extern int cavium_is_altpkg(uint32_t arg_model);

/** @} */
#endif
