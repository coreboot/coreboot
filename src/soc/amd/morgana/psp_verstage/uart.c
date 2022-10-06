/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Can this be made common?  Kconfig option? */

#include <bl_uapp/bl_syscall_public.h>
#include <amdblocks/uart.h>
#include <types.h>

uintptr_t get_uart_base(unsigned int idx)
{
	/* Mapping the UART is not supported. */
	return 0;
}
