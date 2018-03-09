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
 * Utility functions handling signed nad possibly encrypted files
 *
 * @defgroup signed Signed File IO
 * @{
 */

/**
 * Enumeration representing the possible data types in a signed file
 */
typedef enum
{
    BDK_SIGNED_IMAGE,   /* BDK code image */
    BDK_SIGNED_DTS,     /* Device tree file */
    BDK_SIGNED_PUB_KEY, /* Chain of trust public key, BDK proprietary format */
} bdk_signed_data_t;

/**
 * Flags to pass to bdk_signed functions
 */
typedef enum
{
    BDK_SIGNED_FLAG_NONE = 0,               /* Good for most files. Verfies as needed for trusted boot */
    BDK_SIGNED_FLAG_NOT_ENCRYPTED = 1 << 1, /* The file is not encrypted, even with trusted boot */
    BDK_SIGNED_FLAG_ALLOW_UNSIGNED = 1 << 2,/* File is not signed, even with trusted boot */
} bdk_signed_flags_t;

/**
 * Load a file and verify its signature. If the file is encrypted, it is
 * decrypted. If the file is compressed, it is decompressed.
 *
 * @param filename  File to load
 * @param loc       Offset into file for image. This is normally zero for normal files. Device
 *                  files, such as /dev/mem, will use this to locate the image.
 * @param data_type Type of data in the file, enumerated by bdk_signed_data_t. This is required
 *                  so the code can determine the file size before loading the whole file.
 * @param flags     Flags for controlling file loading
 * @param filesize  Set the size of the file if the file is loaded properly. If load fails, set to
 *                  zero.
 *
 * @return Pointer to the data from the file, or NULL on failure
 */
extern void *bdk_signed_load(const char *filename, uint64_t loc,
    bdk_signed_data_t data_type, bdk_signed_flags_t flags, uint64_t *filesize);

/**
 * Load the BDK's public signing key, which is signed by the Root of Trust
 *
 * @return Zero on success, negative on failure
 */
extern int bdk_signed_load_public(void);

/** @} */
