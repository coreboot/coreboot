/*
 * Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef __LIB_VPD__
#define __LIB_VPD__

#include <inttypes.h>

enum {
	VPD_OK = 0,
	VPD_FAIL,
};

enum {
	VPD_TYPE_TERMINATOR = 0,
	VPD_TYPE_STRING,
	VPD_TYPE_INFO                = 0xfe,
	VPD_TYPE_IMPLICIT_TERMINATOR = 0xff,
};

enum {
	VPD_AS_LONG_AS = -1,
};

enum {  /* export_type */
	VPD_EXPORT_KEY_VALUE = 1,
	VPD_EXPORT_VALUE,
	VPD_EXPORT_AS_PARAMETER,
};

/* Callback for decodeVpdString to invoke. */
typedef int VpdDecodeCallback(const uint8_t *key, int32_t key_len,
                              const uint8_t *value, int32_t value_len,
                              void *arg);

/* Container data types */
struct StringPair {
	uint8_t *key;
	uint8_t *value;
	int pad_len;
	int filter_out;  /* TRUE means not exported. */
	struct StringPair *next;
};

struct PairContainer {
	struct StringPair *first;
};


/***********************************************************************
 * Encode and decode VPD entries
 ***********************************************************************/

/* Encodes the len into multiple bytes with the following format.
 *
 *    7   6 ............ 0
 *  +----+------------------+
 *  |More|      Length      |  ...
 *  +----+------------------+
 *
 * The encode_buf points to the actual position we are going to store.
 * encoded_len will return the exact bytes we encoded in this function.
 * Returns fail if the buffer is not long enough.
 */
int encodeLen(
    const int32_t len,
    uint8_t *encode_buf,
    const int32_t max_len,
    int32_t *encoded_len);

/* Given an encoded string, this functions decodes the length field which varies
 * from 1 byte to many bytes.
 *
 * The in points the actual byte going to be decoded. The *length returns
 * the decoded length field. The number of consumed bytes will be stroed in
 * decoded_len.
 *
 * Returns VPD_FAIL if more bit is 1, but actually reaches the end of string.
 */
int decodeLen(
    const int32_t max_len,
    const uint8_t *in,
    int32_t *length,
    int32_t *decoded_len);


/* Encodes the terminator.
 * When calling, the output_buf should point to the start of buffer while
 * *generated_len should contain how many bytes exist in buffer now.
 * After return, *generated_len would be plused the number of bytes generated
 * in this function.
 */
int encodeVpdTerminator(
    const int max_buffer_len,
    uint8_t *output_buf,
    int *generated_len);

/* Encodes the given type/key/value pair into buffer.
 *
 * The pad_value_len is used to control the output value length.
 * When pad_value_len > 0, the value is outputted into fixed length (pad \0
 *                         if the value is shorter). Truncated if too long.
 *      pad_value_len == VPD_NO_LIMIT, output the value as long as possible.
 * This is useful when we want to fix the structure layout at beginning.
 *
 * The encoded string will be stored in output_buf. If it is longer than
 * max_buffer_len, this function returns fail. If the buffer is long enough,
 * the generated_len will be updated.
 *
 * When calling, the output_buf should point to the start of buffer while
 * *generated_len should contain how many bytes exist in buffer now.
 * After return, *generated_len would be plused the number of bytes generated
 * in this function.
 *
 * The initial value of *generated_len can be non-zero, so that this value
 * can be used between multiple calls to encodeVpd2Pair().
 */
int encodeVpdString(
    const uint8_t *key,
    const uint8_t *value,
    const int pad_value_len,
    const int max_buffer_len,
    uint8_t *output_buf,
    int *generated_len);


/* Given the encoded string, this function invokes callback with extracted
 * (key, value). The *consumed will be plused the number of bytes consumed in
 * this function.
 *
 * The input_buf points to the first byte of the input buffer.
 *
 * The *consumed starts from 0, which is actually the next byte to be decoded.
 * It can be non-zero to be used in multiple calls.
 *
 * If one entry is successfully decoded, sends it to callback and returns the
 * result.
 */
int decodeVpdString(
    const int32_t max_len,
    const uint8_t *input_buf,
    int32_t *consumed,
    VpdDecodeCallback callback,
    void *callback_arg);

/***********************************************************************
 * Container helpers
 ***********************************************************************/
void initContainer(struct PairContainer *container);

struct StringPair *findString(struct PairContainer *container,
                              const uint8_t *key,
                              struct StringPair ***prev_next);

/* If key is already existed in container, its value will be replaced.
 * If not existed, creates new entry in container.
 */
void setString(struct PairContainer *container,
               const uint8_t *key,
               const uint8_t *value,
               const int pad_len);

/* merge all entries in src into dst. If key is duplicate, overwrite it.
 */
void mergeContainer(struct PairContainer *dst,
                    const struct PairContainer *src);

/* subtract src from dst.
*/
int subtractContainer(struct PairContainer *dst,
                      const struct PairContainer *src);

/* Given a container, encode its all entries into the buffer.
 */
int encodeContainer(const struct PairContainer *container,
                    const int max_buf_len,
                    uint8_t *buf,
                    int *generated);

/* Given a VPD blob, decode its entries and push into container.
 */
int decodeToContainer(struct PairContainer *container,
                      const int32_t max_len,
                      const uint8_t *input_buf,
                      int32_t *consumed);

/* Set filter for exporting functions.
 * If filter is NULL, resets the filter so that everything can be exported.
 */
int setContainerFilter(struct PairContainer *container,
                       const uint8_t *filter);

/*
 * Remove a key.
 * Returns VPD_OK if deleted successfully. Otherwise, VPD_FAIL.
 */
int deleteKey(struct PairContainer *container,
              const uint8_t *key);

/*
 * Returns number of pairs in container.
 */
int lenOfContainer(const struct PairContainer *container);

/*
 * Export the container content with human-readable text.
 *
 * The buf points to the first byte of buffer and *generated contains the number
 * of bytes already existed in buffer.
 *
 * Afterward, the *generated will be plused on exact bytes this function has
 * generated.
 */
int exportContainer(const int export_type,
                    const struct PairContainer *container,
                    const int max_buf_len,
                    uint8_t *buf,
                    int *generated);

void destroyContainer(struct PairContainer *container);

#endif  /* __LIB_VPD__ */
