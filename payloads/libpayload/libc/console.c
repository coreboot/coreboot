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
static console_input_type last_getchar_input_type;

static int output_driver_exists(struct console_output_driver *out)
{
	struct console_output_driver *head = console_out;

	while (head) {
		if (head == out)
			return 1;
		head = head->next;
	}

	return 0;
}

static int input_driver_exists(struct console_input_driver *in)
{
	struct console_input_driver *head = console_in;

	while (head) {
		if (head == in)
			return 1;
		head = head->next;
	}

	return 0;
}

void console_add_output_driver(struct console_output_driver *out)
{
	die_if(!out->putchar && !out->write, "Need at least one output func\n");
	/* Check if this driver was already added to the console list */
	if (output_driver_exists(out))
		return;
	out->next = console_out;
	console_out = out;
}

void console_add_input_driver(struct console_input_driver *in)
{
	/* Check if this driver was already added to the console list */
	if (input_driver_exists(in))
		return;
	/* Flush out the driver input buffer. */
	while (in->havekey())
		in->getchar();
	in->next = console_in;
	console_in = in;
}

/*
 * For when you really need to silence an output driver (e.g. to avoid ugly
 * recursions). Takes the pointer of either of the two output functions, since
 * the struct console_output_driver itself is often static and inaccessible.
 */
int console_remove_output_driver(void *function)
{
	struct console_output_driver **out;
	for (out = &console_out; *out; out = &(*out)->next)
		if ((*out)->putchar == function || (*out)->write == function) {
			*out = (*out)->next;
			return 1;
		}

	return 0;
}

void console_init(void)
{
#if IS_ENABLED(CONFIG_LP_VIDEO_CONSOLE)
	video_console_init();
#endif
#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
	serial_console_init();
#endif
#if IS_ENABLED(CONFIG_LP_PC_KEYBOARD)
	keyboard_init();
#endif
#if IS_ENABLED(CONFIG_LP_CBMEM_CONSOLE)
	cbmem_console_init();
#endif
}

void console_write(const void *buffer, size_t count)
{
	const char *ptr;
	struct console_output_driver *out;
	for (out = console_out; out != 0; out = out->next)
		if (out->write)
			out->write(buffer, count);
		else
			for (ptr = buffer; (void *)ptr < buffer + count; ptr++)
				out->putchar(*ptr);
}

int putchar(unsigned int i)
{
	unsigned char c = (unsigned char)i;
	console_write(&c, 1);
	return (int)c;
}

int puts(const char *s)
{
	size_t size = strlen(s);

	console_write(s, size);

	putchar('\n');
	return size + 1;
}

int havekey(void)
{
#if IS_ENABLED(CONFIG_LP_USB)
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
#if IS_ENABLED(CONFIG_LP_USB)
		usb_poll();
#endif
		struct console_input_driver *in;
		for (in = console_in; in != 0; in = in->next)
			if (in->havechar()) {
				last_getchar_input_type = in->input_type;
				return in->getchar();
			}
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

console_input_type last_key_input_type(void)
{
	return last_getchar_input_type;
}
