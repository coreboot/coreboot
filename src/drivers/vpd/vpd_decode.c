/*
 * Copyright 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * This is a copy from upstream:
 * https://chromium.googlesource.com/chromiumos/platform/vpd/+/master/lib/vpd_decode.c
 */
#include "vpd_decode.h"

int vpd_decode_len(
		const u32 max_len, const u8 *in, u32 *length, u32 *decoded_len)
{
	u8 more;
	int i = 0;

	if (!length || !decoded_len)
		return VPD_DECODE_FAIL;

	*length = 0;
	do {
		if (i >= max_len)
			return VPD_DECODE_FAIL;

		more = in[i] & 0x80;
		*length <<= 7;
		*length |= in[i] & 0x7f;
		++i;
	} while (more);

	*decoded_len = i;
	return VPD_DECODE_OK;
}

int vpd_decode_string(
		const u32 max_len, const u8 *input_buf, u32 *consumed,
		vpd_decode_callback callback, void *callback_arg)
{
	int type;
	int res;
	u32 key_len;
	u32 value_len;
	u32 decoded_len;
	const u8 *key;
	const u8 *value;

	/* type */
	if (*consumed >= max_len)
		return VPD_DECODE_FAIL;

	type = input_buf[*consumed];

	switch (type) {
	case VPD_TYPE_INFO:
	case VPD_TYPE_STRING:
		(*consumed)++;

		/* key */
		res = vpd_decode_len(max_len - *consumed, &input_buf[*consumed],
				     &key_len, &decoded_len);
		/* key name cannot be empty, and must be followed by value. */
		if (res != VPD_DECODE_OK || key_len < 1 ||
		    *consumed + decoded_len + key_len >= max_len)
			return VPD_DECODE_FAIL;

		*consumed += decoded_len;
		key = &input_buf[*consumed];
		*consumed += key_len;

		/* value */
		res = vpd_decode_len(max_len - *consumed, &input_buf[*consumed],
				     &value_len, &decoded_len);
		/* value can be empty (value_len = 0). */
		if (res != VPD_DECODE_OK ||
		    *consumed + decoded_len + value_len > max_len)
			return VPD_DECODE_FAIL;

		*consumed += decoded_len;
		value = &input_buf[*consumed];
		*consumed += value_len;

		if (type == VPD_TYPE_STRING)
			return callback(key, key_len, value, value_len,
					callback_arg);
		break;

	default:
		return VPD_DECODE_FAIL;
	}

	return VPD_DECODE_OK;
}
