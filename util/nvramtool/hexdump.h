/*****************************************************************************\
 * hexdump.h
\*****************************************************************************/

#ifndef _HEXDUMP_H
#define _HEXDUMP_H

/* hexdump.h
 *
 * Copyright (C) 2002
 *     David S. Peterson.  All rights reserved.
 *
 * Author: David S. Peterson <dave_peterson@pobox.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions, and the entire permission notice, including
 *    the following disclaimer of warranties.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions, and the entire permission notice,
 *    including the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 3. The name(s) of the author(s) may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * ALTERNATIVELY, this product may be distributed under the terms of the GNU
 * General Public License, in which case the provisions of the GPL are
 * required INSTEAD OF the above restrictions.  (This clause is necessary due
 * to a potential bad interaction between the GPL and the restrictions
 * contained in a BSD-style copyright.)
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <sys/types.h>
#include <stdio.h>

/*--------------------------------------------------------------------------
 * hexdump_format_t
 *
 * This specifies how the output of the 'hexdump' function should look.
 *
 * fields:
 *     bytes_per_line:  the number of data bytes to display per line of
 *                      output
 *     addrprint_width: Each line of output begins with the address of the
 *                      first data byte displayed on that line.  This
 *                      specifies the number of bytes wide the address
 *                      should be displayed as.  This value must be from 1
 *                      to 8.
 *     indent:          This is a string to display at the start of each
 *                      output line.  Its purpose is to indent the output.
 *     sep1:            This is a string to display between the address and
 *                      the bytes of data displayed in hex.  It serves as a
 *                      separator.
 *     sep2:            This is a string to display between individual hex
 *                      values.  It serves as a separator.
 *     sep3:            This is a string to display between the bytes of
 *                      data in hex and the bytes of data displayed as
 *                      characters.  It serves as a separator.
 *     nonprintable:    This is a substitute character to display in place
 *                      of nonprintable characters.
 *--------------------------------------------------------------------------*/
typedef struct {
	int bytes_per_line;
	int addrprint_width;
	const char *indent;
	const char *sep1;
	const char *sep2;
	const char *sep3;
	unsigned char nonprintable;
} hexdump_format_t;

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
	     FILE * outfile, const hexdump_format_t * format);

#endif				/* _HEXDUMP_H */
