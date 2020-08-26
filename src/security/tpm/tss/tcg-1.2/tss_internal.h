/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef TCG_TSS_INTERNAL_H_
#define TCG_TSS_INTERNAL_H_

#include <stdint.h>

/*
 * These numbers derive from adding the sizes of command fields as shown in the
 * TPM commands manual.
 */
#define kTpmRequestHeaderLength 10
#define kTpmResponseHeaderLength 10
#define kTpmReadInfoLength 12
#define kEncAuthLength 20
#define kPcrDigestLength 20

/*
 * Conversion functions.  to_tpm_TYPE puts a value of type TYPE into a TPM
 * command buffer. from_tpm_TYPE gets a value of type TYPE from a TPM command
 * buffer into a variable.
 */
__attribute__((unused))
static inline void to_tpm_uint32(uint8_t *buffer, uint32_t x)
{
	buffer[0] = (uint8_t)(x >> 24);
	buffer[1] = (uint8_t)((x >> 16) & 0xff);
	buffer[2] = (uint8_t)((x >> 8) & 0xff);
	buffer[3] = (uint8_t)(x & 0xff);
}

/*
 * See comment for above function.
 */
__attribute__((unused))
static inline void from_tpm_uint32(const uint8_t *buffer, uint32_t *x)
{
	*x = ((buffer[0] << 24) |
	      (buffer[1] << 16) |
	      (buffer[2] << 8) |
	      buffer[3]);
}

/*
 * See comment for above function.
 */
__attribute__((unused))
static inline void to_tpm_uint16(uint8_t *buffer, uint16_t x)
{
	buffer[0] = (uint8_t)(x >> 8);
	buffer[1] = (uint8_t)(x & 0xff);
}

/*
 * See comment for above function.
 */
__attribute__((unused))
static inline void from_tpm_uint16(const uint8_t *buffer, uint16_t *x)
{
	*x = (buffer[0] << 8) | buffer[1];
}

#endif  /* TCG_TSS_INTERNAL_H_ */
