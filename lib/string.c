/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>

/**
 * Calculate the length of a fixed-size string.
 *
 * @param str The input string.
 * @param maxlen Return at most maxlen characters as length of the string.
 * @return The length of the string, not including the final NUL character.
 * 	   The maximum length returned is maxlen.
 */
size_t strnlen(const char *str, size_t maxlen)
{
	size_t len = 0;

	/* NULL and empty strings have length 0. */
	if (!str)
		return 0;

	/* Loop until we find a NUL character, or maxlen is reached. */
	while ((*str++ != '\0') && (len < maxlen))
		len++;

	return len;
}

/**
 * Calculate the length of a string.
 *
 * @param str The input string.
 * @return The length of the string, not including the final NUL character.
 */
size_t strlen(const char *str)
{
	size_t len = 0;

	/* NULL and empty strings have length 0. */
	if (!str)
		return 0;

	/* Loop until we find a NUL character. */
	while (*str++ != '\0')
		len++;

	return len;
}

/**
 * Compare two strings.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @return Returns a value less than zero, if s1 is shorter than s2. Returns
 * 	   zero, if s1 equals s2. Returns a value greater than zero, if
 * 	   s1 is longer than s2.
 */
int strcmp(const char *s1, const char *s2)
{
	char c1, c2;

	/* Set c1 == c2, so that we can enter the while loop. */
	c1 = 0;
	c2 = 0;

	/* Compare characters until they differ, or one of the strings ends. */
	while (c1 == c2) {
		/* Read the next character from each string. */
		c1 = *s1++;
		c2 = *s2++;

		/* Return something negative (if s1 is shorter than s2), or
		   zero (if s1 equals s2). */
		if (c1 == '\0')
			return c1 - c2;
	}

	/* Return someting positive (if s1 is longer than s2), or zero (if s1
	   and s2 are equal). */
	return c1 - c2;
}

/**
 * Compare two strings with fixed length.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @param maxlen Return at most maxlen characters as length of the string.
 * @return A non-zero value if s1 and s2 differ, or zero if s1 equals s2.
 */
int strncmp(const char *s1, const char *s2, int maxlen)
{
	int i;

	for (i = 0; i < maxlen; i++) {
		if (s1[i] != s2[i])
			return s1[i] - s2[i];
	}

	return 0;
}
