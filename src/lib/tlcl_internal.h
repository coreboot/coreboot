/* Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef TPM_LITE_TLCL_INTERNAL_H_
#define TPM_LITE_TLCL_INTERNAL_H_

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
 * Conversion functions.  ToTpmTYPE puts a value of type TYPE into a TPM
 * command buffer.  FromTpmTYPE gets a value of type TYPE from a TPM command
 * buffer into a variable.
 */
__attribute__((unused))
static inline void ToTpmUint32(uint8_t *buffer, uint32_t x) {
  buffer[0] = (uint8_t)(x >> 24);
  buffer[1] = (uint8_t)((x >> 16) & 0xff);
  buffer[2] = (uint8_t)((x >> 8) & 0xff);
  buffer[3] = (uint8_t)(x & 0xff);
}

/*
 * See comment for above function.
 */
__attribute__((unused))
static inline void FromTpmUint32(const uint8_t *buffer, uint32_t *x) {
  *x = ((buffer[0] << 24) |
        (buffer[1] << 16) |
        (buffer[2] << 8) |
        buffer[3]);
}

/*
 * See comment for above function.
 */
__attribute__((unused))
static inline void ToTpmUint16(uint8_t *buffer, uint16_t x) {
  buffer[0] = (uint8_t)(x >> 8);
  buffer[1] = (uint8_t)(x & 0xff);
}

/*
 * See comment for above function.
 */
__attribute__((unused))
static inline void FromTpmUint16(const uint8_t *buffer, uint16_t *x) {
  *x = (buffer[0] << 8) | buffer[1];
}

#endif  /* TPM_LITE_TLCL_INTERNAL_H_ */
