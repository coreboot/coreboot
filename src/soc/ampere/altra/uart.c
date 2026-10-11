/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/uart.h>
#include <soc/addressmap.h>

uintptr_t uart_platform_base(unsigned int idx)
{
	return 0x100002600000;
}
