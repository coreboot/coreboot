/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * This is a copy from upstream:
 * https://chromium.googlesource.com/chromiumos/platform/vpd/+/master/lib/vpd_decode.c
 */
#include "vpd_decode.h"

static int vpd_decode_len(
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

static int vpd_decode_entry(
		const u32 max_len, const u8 *input_buf, u32 *consumed,
		const u8 **entry, u32 *entry_len)
{
	u32 decoded_len;

	if (vpd_decode_len(max_len - *consumed, &input_buf[*consumed],
			   entry_len, &decoded_len) != VPD_DECODE_OK)
		return VPD_DECODE_FAIL;
	if (max_len - *consumed < decoded_len)
		return VPD_DECODE_FAIL;

	*consumed += decoded_len;
	*entry = input_buf + *consumed;

	/* entry_len is untrusted data and must be checked again. */
	if (max_len - *consumed < *entry_len)
		return VPD_DECODE_FAIL;

	*consumed += *entry_len;
	return VPD_DECODE_OK;
}

int vpd_decode_string(
		const u32 max_len, const u8 *input_buf, u32 *consumed,
		vpd_decode_callback callback, void *callback_arg)
{
	int type;
	u32 key_len;
	u32 value_len;
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

		if (vpd_decode_entry(max_len, input_buf, consumed, &key,
				     &key_len) != VPD_DECODE_OK)
			return VPD_DECODE_FAIL;

		if (vpd_decode_entry(max_len, input_buf, consumed, &value,
				     &value_len) != VPD_DECODE_OK)
			return VPD_DECODE_FAIL;

		if (type == VPD_TYPE_STRING)
			return callback(key, key_len, value, value_len,
					callback_arg);
		break;

	default:
		return VPD_DECODE_FAIL;
	}

	return VPD_DECODE_OK;
}
