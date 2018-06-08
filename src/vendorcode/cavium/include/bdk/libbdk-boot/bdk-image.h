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
 * Utility functions for handling binary images
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @defgroup image Binary image utility functions
 * @{
 */

/**
 * Structure present at the beginning of BDK images
 */
typedef struct
{
    uint32_t    instruction;    /* Raw instruction for skipping header */
    uint32_t    length;         /* Length of the image, includes header */
    uint64_t    magic;          /* Magic string "THUNDERX" */
    uint32_t    crc32;          /* CRC32 of image + header. These bytes are zero when calculating the CRC */
    uint32_t    reserved1;      /* Zero, reserved for future use */
    char        name[64];       /* ASCII Image name. Must always end in zero */
    char        version[32];    /* ASCII Version. Must always end in zero */
    uint64_t    reserved[17];   /* Zero, reserved for future use */
} BDK_LITTLE_ENDIAN_STRUCT bdk_image_header_t;

/**
 * Validate image header
 *
 * @param header Header to validate
 *
 * @return 1 if header is valid, zero if invalid
 */
extern int bdk_image_header_is_valid(const bdk_image_header_t *header);

/**
 * Verify image at the given address is good
 *
 * @param image  Pointer to the image
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_image_verify(const void *image);

/**
 * Read a image header from a file
 *
 * @param handle File handel to read from
 * @param header Pointer to header structure to fill
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_image_read_header(FILE *handle, bdk_image_header_t *header);

/**
 * Read a image from a file and boot it, replacing the current running BDK image
 *
 * @param filename   File to read the image from
 * @param loc        Offset into file for image. This is normally zero for normal files. Device
 *                   files, such as /dev/mem, will use this to locate the image.
 *
 * @return Negative on failure. On success this function never returns.
 */
extern int bdk_image_boot(const char *filename, uint64_t loc);

/** @} */
