/* SPDX-License-Identifier: BSD-3-Clause */

#include <commonlib/bsd/string.h>
#include <ctype.h>
#include <stddef.h>

size_t strlen(const char *str)
{
	const char *ptr = str;

	while (*ptr++)
		;
	return ptr - str - 1;
}

size_t strnlen(const char *str, size_t maxlen)
{
	const char *ptr = str;
	const char *end = str + maxlen + 1;

	while (*ptr++ && ptr < end)
		;
	return ptr - str - 1;
}

char *strcat(char *dst, const char *src)
{
	char *ptr = dst + strlen(dst);

	while (*src)
		*ptr++ = *src++;

	*ptr = '\0';
	return dst;
}

char *strncat(char *dst, const char *src, size_t n)
{
	char *ptr = dst + strlen(dst);

	/* Not using strncpy() because '\0' may not be appended. */
	while (n-- > 0 && *src)
		*ptr++ = *src++;

	*ptr = '\0';
	return dst;
}

unsigned int skip_atoi(char **ptr)
{
	unsigned int result = 0;
	char *str;

	for (str = *ptr; isdigit(str[0]); str++)
		result = result * 10 + (str[0] - '0');
	*ptr = str;
	return result;
}
