/* SPDX-License-Identifier: GPL-2.0-only */

#include "common.h"
#include "cmos_ops.h"
#include "cmos_lowlevel.h"

static int prepare_cmos_op_common(const cmos_entry_t * e);

/****************************************************************************
 * prepare_cmos_op_common
 *
 * Perform a few checks common to both reads and writes.
 ****************************************************************************/
static int prepare_cmos_op_common(const cmos_entry_t * e)
{
	int result;

	if (e->config == CMOS_ENTRY_RESERVED)
		/* Access to reserved parameters is not permitted. */
		return CMOS_OP_RESERVED;

	if ((result = verify_cmos_op(e->bit, e->length, e->config)) != OK)
		return result;

	assert(e->length > 0);
	return OK;
}

/****************************************************************************
 * prepare_cmos_read
 *
 * The caller wishes to read a CMOS parameter represented by 'e'.  Perform
 * sanity checking on 'e'.  If a problem was found with e, return an error
 * code.  Else return OK.
 ****************************************************************************/
int prepare_cmos_read(const cmos_entry_t * e)
{
	int result;

	if ((result = prepare_cmos_op_common(e)) != OK)
		return result;

	switch (e->config) {
	case CMOS_ENTRY_ENUM:
	case CMOS_ENTRY_HEX:
	case CMOS_ENTRY_STRING:
		break;

	default:
		BUG();
	}

	return OK;
}

/****************************************************************************
 * prepare_cmos_write
 *
 * The caller wishes to set a CMOS parameter represented by 'e' to a value
 * whose string representation is stored in 'value_str'.  Perform sanity
 * checking on 'value_str'.  On error, return an error code.  Else store the
 * numeric equivalent of 'value_str' in '*value' and return OK.
 ****************************************************************************/
int prepare_cmos_write(const cmos_entry_t * e, const char value_str[],
		       unsigned long long *value)
{
	const cmos_enum_t *q;
	unsigned long long out;
	const char *p;
	char *memory = NULL;
	int negative, result, found_one;

	if ((result = prepare_cmos_op_common(e)) != OK)
		return result;

	switch (e->config) {
	case CMOS_ENTRY_ENUM:
		/* Make sure the user's input corresponds to a valid option. */
		for (q = first_cmos_enum_id(e->config_id), found_one = 0;
		     q != NULL; q = next_cmos_enum_id(q)) {
			found_one = 1;

			if (!strncmp(q->text, value_str, CMOS_MAX_TEXT_LENGTH))
				break;
		}

		if (!found_one)
			return CMOS_OP_NO_MATCHING_ENUM;

		if (q == NULL)
			return CMOS_OP_BAD_ENUM_VALUE;

		out = q->value;
		break;

	case CMOS_ENTRY_HEX:
		/* See if the first character of 'value_str' (excluding
		 * any initial whitespace) is a minus sign.
		 */
		for (p = value_str; isspace((int)(unsigned char)*p); p++) ;
		negative = (*p == '-');

		out = strtoull(value_str, (char **)&p, 0);

		if (*p)
			return CMOS_OP_INVALID_INT;

		/* If we get this far, the user specified a valid integer.
		 * However we do not currently support the use of negative
		 * numbers as CMOS parameter values.
		 */
		if (negative)
			return CMOS_OP_NEGATIVE_INT;

		break;

	case CMOS_ENTRY_STRING:
		if (e->length < (8 * strlen(value_str)))
			return CMOS_OP_VALUE_TOO_WIDE;
		memory = malloc(e->length / 8);
		memset(memory, 0, e->length / 8);
		strcpy(memory, value_str);
		out = (unsigned long)memory;
		break;

	default:
		BUG();
	}

	if ((e->length < (8 * sizeof(*value))) && (out >= (1ull << e->length))) {
		if (memory) free(memory);
		return CMOS_OP_VALUE_TOO_WIDE;
	}

	*value = out;
	return OK;
}

/****************************************************************************
 * cmos_checksum_read
 *
 * Read the checksum for the coreboot parameters stored in CMOS and return
 * this value.
 ****************************************************************************/
uint16_t cmos_checksum_read(void)
{
	uint16_t lo, hi;

	/* The checksum is stored in a big-endian format. */
	hi = cmos_read_byte(cmos_checksum_index);
	lo = cmos_read_byte(cmos_checksum_index + 1);
	return (hi << 8) + lo;
}

/****************************************************************************
 * cmos_checksum_write
 *
 * Set the checksum for the coreboot parameters stored in CMOS to
 * 'checksum'.
 ****************************************************************************/
void cmos_checksum_write(uint16_t checksum)
{
	unsigned char lo, hi;

	/* The checksum is stored in a big-endian format. */
	hi = (unsigned char)(checksum >> 8);
	lo = (unsigned char)(checksum & 0x00ff);
	cmos_write_byte(cmos_checksum_index, hi);
	cmos_write_byte(cmos_checksum_index + 1, lo);
}

/****************************************************************************
 * cmos_checksum_compute
 *
 * Compute a checksum for the coreboot parameter values currently stored in
 * CMOS and return this checksum.
 ****************************************************************************/
uint16_t cmos_checksum_compute(void)
{
	unsigned i, sum;

	sum = 0;

	for (i = cmos_checksum_start; i <= cmos_checksum_end; i++)
		sum += cmos_read_byte(i);

	return (uint16_t)(sum & 0xffff);
}

/****************************************************************************
 * cmos_checksum_verify
 *
 * Verify that the coreboot CMOS checksum is valid.  If checksum is not
 * valid then print warning message and exit.
 ****************************************************************************/
void cmos_checksum_verify(void)
{
	uint16_t computed, actual;

	set_iopl(3);
	computed = cmos_checksum_compute();
	actual = cmos_checksum_read();
	set_iopl(0);

	if (computed != actual) {
		fprintf(stderr, "%s: Warning: coreboot CMOS checksum is bad.\n",
			prog_name);
		fprintf(stderr, "Computed checksum: 0x%x. Stored checksum: 0x%x\n",
			computed, actual);
		exit(1);
	}
}
