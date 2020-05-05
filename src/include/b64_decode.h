/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __INCLUDE_B64_DECODE_H__
#define __INCLUDE_B64_DECODE_H__

#include <stddef.h>
#include <stdint.h>

/*
 * A function to convert a buffer of base64 format data into its source.
 *
 * The user provides output buffer of the size guaranteed to fit the result.
 *
 * Returns the size of the decoded data or zero if invalid characters were
 * encountered in the input buffer.
 */
size_t b64_decode(const uint8_t *input_data,
		  size_t input_length,
		  uint8_t *output_data);

/* A macro to derive decoded size of a base64 encoded blob. */
#define B64_DECODED_SIZE(encoded_size) (((encoded_size) * 3)/4)

#endif
