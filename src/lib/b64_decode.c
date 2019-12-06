/*
 * Copyright (C) 2015 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <b64_decode.h>
#include <console/console.h>

/*
 * Translation Table to decode base64 ASCII stream into binary. Borrowed from
 *
 * http://base64.sourceforge.net/b64.c.
 *
 */
static const char cd64[] = "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMN"
	"OPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

struct buffer_descriptor {
	const uint8_t *input_buffer;
	size_t data_size;
	size_t input_index;
};

#define isalnum(c) ((((c) >= 'a') && ((c) <= 'z')) || \
		    (((c) >= 'A') && ((c) <= 'Z')) || \
		    (((c) >= '0') && ((c) <= '9')))

/*
 * On each invocation this function returns the next valid base64 character
 * from the encoded message, ignoring padding and line breaks.
 *
 * Once all input is consumed, 0 is returned on all following invocations. In
 * case any other than expected characters is found in the encoded message, -1
 * is returned for error.
 */
static int get_next_char(struct buffer_descriptor *bd)
{
	uint8_t c;

	/*
	 * The canonical base64 encoded messages include the following
	 * characters:
	 * - '0..9A..Za..z+/' to represent 64 values
	 * - '=' for padding
	 * - '<CR><LF>' to split the message into lines.
	 */
	while (bd->input_index < bd->data_size) {
		c = bd->input_buffer[bd->input_index++];

		switch (c) {
		case '=':
		case 0xa:
		case 0xd:
			continue;

		default:
			break;
		}

		if (!isalnum(c) && (c != '+') && (c != '/'))
			return -1;

		return c;
	}

	return 0;
}

/*
** decode
**
** decode a base64 encoded stream discarding padding and line breaks.
*/
size_t b64_decode(const uint8_t *input_data,
		  size_t input_length,
		  uint8_t *output_data)
{
	struct buffer_descriptor bd;
	unsigned int interim = 0;
	size_t output_size = 0;
	/* count of processed input bits, modulo log2(64) */
	unsigned int bit_count = 0;

	/*
	 * Keep the context on the stack to make things easier if this needs
	 * to run with CAR.
	 */
	bd.input_buffer = input_data;
	bd.data_size = input_length;
	bd.input_index = 0;

	while (1) { /* Until input is exhausted. */
		int v = get_next_char(&bd);

		if (v < 0) {
			printk(BIOS_ERR,
			       "Incompatible character at offset %zd.\n",
			       bd.input_index);
			return 0;
		}

		if (!v)
			break;

		/*
		 * v is guaranteed to be in the proper range for cd64, the
		 * result is a 6 bit number.
		 */
		v = cd64[v - 43] - 62;

		if (bit_count >= 2) {
			/*
			 * Once 6 more bits are added to the output, there is
			 * going to be at least a full byte.
			 *
			 * 'remaining_bits' is the exact number of bits which
			 * need to be added to the output to have another full
			 * byte ready.
			 */
			int remaining_bits = 8 - bit_count;

			interim <<= remaining_bits;
			interim |= v >> (6 - remaining_bits);

			/* Pass the new full byte to the output. */
			output_data[output_size++] = interim & 0xff;

			interim = v;
			bit_count -= 2;
		} else {
			interim <<= 6;
			interim |= v;
			bit_count += 6;
		}
	}

	return output_size;
}
