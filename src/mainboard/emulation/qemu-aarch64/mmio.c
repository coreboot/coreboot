/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Asami Doi <d0iasm.pub@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <console/uart.h>
#include <mainboard/addressmap.h>

uintptr_t uart_platform_base(int idx)
{
	return VIRT_UART_BASE;
}
