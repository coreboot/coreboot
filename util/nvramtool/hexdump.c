/* SPDX-License-Identifier: BSD-3-Clause or GPL-2.0 */

#include "hexdump.h"
#include <ctype.h>

static void addrprint(FILE * outfile, uint64_t address, int width);

/*--------------------------------------------------------------------------
 * hexdump
 *
 * Write a hex dump of 'mem' to 'outfile'.
 *
 * parameters:
 *     mem:             a pointer to the memory to display
 *     bytes:           the number of bytes of data to display
 *     addrprint_start: The address to associate with the first byte of
 *                      data.  For instance, a value of 0 indicates that the
 *                      first byte displayed should be labeled as byte 0.
 *     outfile:         The place where the hex dump should be written.
 *                      For instance, stdout or stderr may be passed here.
 *     format:          A structure specifying how the hex dump should be
 *                      formatted.
 *--------------------------------------------------------------------------*/
void hexdump(const void *mem, int bytes, uint64_t addrprint_start,
	     FILE * outfile, const hexdump_format_t * format)
{
	int bytes_left, index, i;
	const unsigned char *p;

	/* Quietly return if the caller asks us to do something unreasonable. */
	if ((format->bytes_per_line <= 0) || (bytes < 0))
		return;

	p = (const unsigned char *)mem;
	index = 0;

	/* Each iteration handles one full line of output.  When loop
	 * terminates, the number of remaining bytes to display (if any)
	 * will not be enough to fill an entire line.
	 */
	for (bytes_left = bytes;
			bytes_left >= format->bytes_per_line;
			bytes_left -= format->bytes_per_line) {
		/* print start address for current line */
		fprintf(outfile, "%s", format->indent);
		addrprint(outfile, addrprint_start + index,
			  format->addrprint_width);
		fprintf(outfile, "%s", format->sep1);

		/* display the bytes in hex */
		for (i = 0;;) {
			fprintf(outfile, "%02x", p[index++]);

			if (++i >= format->bytes_per_line)
				break;

			fprintf(outfile, "%s", format->sep2);
		}

		index -= format->bytes_per_line;
		fprintf(outfile, "%s", format->sep3);

		/* display the bytes as characters */
		for (i = 0; i < format->bytes_per_line; i++, index++)
			fputc(isprint(p[index])?p[index]:format->nonprintable, outfile);

		fprintf(outfile, "\n");
	}

	if (bytes_left == 0)
		return;

	/* print start address for last line */
	fprintf(outfile, "%s", format->indent);
	addrprint(outfile, addrprint_start + index, format->addrprint_width);
	fprintf(outfile, "%s", format->sep1);

	/* display bytes for last line in hex */
	for (i = 0; i < bytes_left; i++) {
		fprintf(outfile, "%02x", p[index++]);
		fprintf(outfile, "%s", format->sep2);
	}

	index -= bytes_left;

	/* pad the rest of the hex byte area with spaces */
	for (;;) {
		fprintf(outfile, "  ");

		if (++i >= format->bytes_per_line)
			break;

		fprintf(outfile, "%s", format->sep2);
	}

	fprintf(outfile, "%s", format->sep3);

	/* display bytes for last line as characters */
	for (i = 0; i < bytes_left; i++)
		fputc(isprint(p[index])?p[index++]:format->nonprintable, outfile);

	/* pad the rest of the character area with spaces */
	for (; i < format->bytes_per_line; i++)
		fprintf(outfile, " ");

	fprintf(outfile, "\n");
}

/*--------------------------------------------------------------------------
 * addrprint
 *
 * Display an address as a hexadecimal number.
 *
 * parameters:
 *     outfile: the place where the output should be written
 *     address: the address to display
 *     width:   The number of bytes wide the address should be displayed as.
 *              Must be a value from 1 to 8.
 *--------------------------------------------------------------------------*/
static void addrprint(FILE * outfile, uint64_t address, int width)
{
	char s[17];
	int i;

	/* force the user's input to be valid */
	if (width < 1)
		width = 1;
	else if (width > 8)
		width = 8;

	/* convert address to string */
	sprintf(s, "%016llx", (unsigned long long)address);

	/* write it out, with colons separating consecutive 16-bit
	 * chunks of the address
	 */
	for (i = 16 - (2 * width);;) {
		fprintf(outfile, "%c", s[i]);

		if (++i >= 16)
			break;

		if ((i % 4) == 0)
			fprintf(outfile, ":");
	}
}
