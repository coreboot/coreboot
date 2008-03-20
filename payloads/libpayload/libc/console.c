/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

void console_init(void)
{
#ifdef CONFIG_VGA_CONSOLE
	vga_init();
#endif
#ifdef CONFIG_SERIAL_CONSOLE
	serial_init();
#endif
}

static void device_putchar(unsigned char c)
{
#ifdef CONFIG_VGA_CONSOLE
	vga_putchar(0x700 | c);
#endif
#ifdef CONFIG_SERIAL_CONSOLE
	serial_putchar(c);
#endif
}

int putchar(int c)
{
	c &= 0xff;
	if (c == '\n')
		device_putchar('\r');
	device_putchar(c);
	return c;
}

int puts(const char *s)
{
	int n = 0;

	while (*s) {
		putchar(*s++);
		n++;
	}

	putchar('\n');
	return n + 1;
}

int havekey(void)
{
#ifdef CONFIG_SERIAL_CONSOLE
	if (serial_havechar())
		return 1;
#endif
#ifdef CONFIG_PC_KEYBOARD
	if (keyboard_havechar())
		return 1;
#endif
	return 0;
}

/**
 * This returns an ASCII value - the two getchar functions
 * cook the respective input from the device.
 */
int getchar(void)
{
	while (1) {
#ifdef CONFIG_SERIAL_CONSOLE
		if (serial_havechar())
			return serial_getchar();
#endif
#ifdef CONFIG_PC_KEYBOARD
		if (keyboard_havechar())
			return keyboard_getchar();
#endif
	}
}
