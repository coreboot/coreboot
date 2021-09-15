/* SPDX-License-Identifier: GPL-2.0-only */

/* Example file for libpayload. */

#include <libpayload-config.h>
#include <libpayload.h>

int main(void)
{
	printf("Hello world!\n");
	halt();
	return 0;
}
