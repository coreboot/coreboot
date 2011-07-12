/*
 * This file is part of the libpayload project.
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

int errno;

char *getenv(const char *name)
{
	return NULL;
}

