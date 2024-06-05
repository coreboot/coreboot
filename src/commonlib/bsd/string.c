/* SPDX-License-Identifier: BSD-3-Clause */

#include <commonlib/bsd/string.h>
#include <ctype.h>

unsigned int skip_atoi(char **ptr)
{
	unsigned int result = 0;
	char *str;

	for (str = *ptr; isdigit(str[0]); str++)
		result = result * 10 + (str[0] - '0');
	*ptr = str;
	return result;
}
