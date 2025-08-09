/*
 *
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#include <libpayload.h>

/*
 * Convert a number in BCD format to decimal.
 *
 * @param b The BCD number.
 * @return The given BCD number in decimal format.
 */
int bcd2dec(int b)
{
	return ((b >> 4) & 0x0f) * 10 + (b & 0x0f);
}

/*
 * Convert a number in decimal format into the BCD format.
 *
 * @param d The decimal number.
 * @return The given decimal number in BCD format.
 */
int dec2bcd(int d)
{
	return ((d / 10) << 4) | (d % 10);
}

/**
 * Return the absolute value of the specified integer.
 *
 * @param j The integer of which we want to know the absolute value.
 * @return The absolute value of the specified integer.
 */
int abs(int j)
{
	return (j >= 0 ? j : -j);
}

long int labs(long int j)
{
	return (j >= 0 ? j : -j);
}

long long int llabs(long long int j)
{
	return (j >= 0 ? j : -j);
}

/**
 * Given a 4-bit value, return the ASCII hex representation of it.
 *
 * @param b A 4-bit value which shall be converted to ASCII hex.
 * @return The ASCII hex representation of the specified 4-bit value.
 *         Returned hex-characters will always be lower-case (a-f, not A-F).
 */
u8 bin2hex(u8 b)
{
	return (b < 10) ? '0' + b : 'a' + (b - 10);
}

/**
 * Given an ASCII hex input character, return its integer value.
 *
 * For example, the input value '6' will be converted to 6, 'a'/'A' will
 * be converted to 10, 'f'/'F' will be converted to 15, and so on.
 *
 * The return value for invalid input characters is 0.
 *
 * @param h The input byte in ASCII hex format.
 * @return The integer value of the specified ASCII hex byte.
 */
u8 hex2bin(u8 h)
{
	return (('0' <= h && h <= '9') ? (h - '0') : \
	        ('A' <= h && h <= 'F') ? (h - 'A' + 10) : \
	        ('a' <= h && h <= 'f') ? (h - 'a' + 10) : 0);
}

/**
 * Enters HALT state, after printing msg
 *
 * @param msg message to print
 */
void fatal(const char *msg)
{
	fprintf(stderr, "%s",msg);
	halt();
}

void exit(int status)
{
	printf("exited with status %d\n", status);
	halt();
}

void __noreturn abort(void)
{
	halt();
}

char *getenv(const char *name)
{
	return NULL;
}

/*
 * Reads a transfer buffer from 32-bit FIFO registers. fifo_stride is the
 * distance in bytes between registers (e.g. pass 4 for a normal array of 32-bit
 * registers or 0 to read everything from the same register). fifo_width is
 * the amount of bytes read per register (can be 1 through 4).
 */
void buffer_from_fifo32(void *buffer, size_t size, void *fifo,
			int fifo_stride, int fifo_width)
{
	u8 *p = buffer;
	int i, j;

	assert(fifo_width > 0 && fifo_width <= sizeof(u32) &&
	       fifo_stride % sizeof(u32) == 0);

	for (i = 0; i < size; i += fifo_width, fifo += fifo_stride) {
		u32 val = read32(fifo);
		for (j = 0; j < MIN(size - i, fifo_width); j++)
			*p++ = (u8)(val >> (j * 8));
	}
}

/*
 * Version of buffer_to_fifo32() that can prepend a prefix of up to fifo_width
 * size to the transfer. This is often useful for protocols where a command word
 * precedes the actual payload data. The prefix must be packed in the low-order
 * bytes of the 'prefix' u32 parameter and any high-order bytes exceeding prefsz
 * must be 0. Note that 'size' counts total bytes written, including 'prefsz'.
 */
void buffer_to_fifo32_prefix(const void *buffer, u32 prefix, int prefsz, size_t size,
			     void *fifo, int fifo_stride, int fifo_width)
{
	const u8 *p = buffer;
	int i, j = prefsz;

	assert(fifo_width > 0 && fifo_width <= sizeof(u32) &&
	       fifo_stride % sizeof(u32) == 0 && prefsz <= fifo_width);

	uint32_t val = prefix;
	for (i = 0; i < size; i += fifo_width, fifo += fifo_stride) {
		for (; j < MIN(size - i, fifo_width); j++)
			val |= *p++ << (j * 8);
		write32(fifo, val);
		val = 0;
		j = 0;
	}

}

__weak void reboot(void)
{
	fatal("Reboot requested but not implemented\n");
}
