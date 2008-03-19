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
#include <sysinfo.h>

#define IOBASE lib_sysinfo.ser_ioport

#ifdef CONFIG_SERIAL_SET_SPEED
#define DIVISOR (115200 / CONFIG_SERIAL_BAUD_RATE)
#endif

void serial_init(void)
{	
#ifdef CONFIG_SERIAL_SET_SPEED
	unsigned char reg;

	/* Disable interrupts */
	outb(0, IOBASE + 0x01);

	/* Assert RTS and DTR */
	outb(3, IOBASE + 0x04);

	/* Set the divisor latch */
	reg = inb(IOBASE + 0x03);
	outb(reg | 0x80, IOBASE + 0x03);

	/* Write the divisor */
	outb(DIVISOR & 0xFF, IOBASE);
	outb(DIVISOR >> 8 & 0xFF, IOBASE + 1);

	/* Restore the previous value of the divisor */
	outb(reg &= ~0x80, IOBASE + 0x03);
#endif
}
			
void serial_putchar(unsigned char c)
{
	while((inb(IOBASE + 0x05) & 0x20) == 0);
	outb(c, IOBASE);
}

int serial_havechar(void)
{
	return inb(IOBASE + 0x05) & 0x01;
}

int serial_getchar(void)
{
	while (!serial_havechar());
	return (int) inb(IOBASE);
}
