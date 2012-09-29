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

#include <libpayload-config.h>
#include <libpayload.h>
#include <usb/usb.h>

struct console_output_driver *console_out;
struct console_input_driver *console_in;

void console_add_output_driver(struct console_output_driver *out)
{
	out->next = console_out;
	console_out = out;
}

void console_add_input_driver(struct console_input_driver *in)
{
	in->next = console_in;
	console_in = in;
}

void console_init(void)
{
#ifdef CONFIG_VIDEO_CONSOLE
	video_console_init();
#endif
#ifdef CONFIG_SERIAL_CONSOLE
	serial_init();
#endif
#ifdef CONFIG_PC_KEYBOARD
	keyboard_init();
#endif
#ifdef CONFIG_CBMEM_CONSOLE
	cbmem_console_init();
#endif
}

static void device_putchar(unsigned char c)
{
	struct console_output_driver *out;
	for (out = console_out; out != 0; out = out->next)
		out->putchar(c);
}

int putchar(unsigned int c)
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
#ifdef CONFIG_USB
	usb_poll();
#endif
	struct console_input_driver *in;
	for (in = console_in; in != 0; in = in->next)
		if (in->havekey())
			return 1;
	return 0;
}

/**
 * This returns an ASCII value - the two getchar functions
 * cook the respective input from the device.
 */
int getchar(void)
{
	while (1) {
#ifdef CONFIG_USB
		usb_poll();
#endif
		struct console_input_driver *in;
		for (in = console_in; in != 0; in = in->next)
			if (in->havechar())
				return in->getchar();
	}
}

int getchar_timeout(int *ms)
{
	while (*ms > 0) {
		if (havekey())
			return getchar();

		mdelay(100);
		*ms -= 100;
	}

	if (*ms < 0)
		*ms = 0;

	return 0;
}
