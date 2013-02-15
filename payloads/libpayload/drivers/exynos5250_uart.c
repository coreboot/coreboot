/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2013 Google, Inc.
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

/*
 * this is a seriously cut-down UART implementation, the assumption
 * being that you should let coreboot set it up. It's quite
 * messy to keep doing UART setup everywhere on these ARM SOCs.
 */

/* word offset of registers from MEMBASE */
enum {
	/* RX and TX data. 
	 * these are the lsb of the word
	 */
	RXDATA = 9,
	TXDATA = 8,

	ERR = 5,
	FIFOSTAT = 6,
	RXCOUNT = 0xff,
	RXFULL = 0x100,
	RXREADY = 0x1ff,
	TXFULL = 1<<24,
};

#define MEMBASE (u32 *)(phys_to_virt(lib_sysinfo.serial->baseaddr))

static inline u8 regreadb(u32 reg)
{
	return readb(MEMBASE + reg);
}

static inline u32 regreadl(u32 reg)
{
	return readl(MEMBASE + reg);
}

static inline void regwritel(u32 val, u32 reg)
{
	writel(val, MEMBASE + reg);
}

static inline void regwriteb(u8 val, u32 reg)
{
	writeb(val, MEMBASE + reg);
}

/*
 * Initialise the serial port.
 * This hardware is really complex, and we're not going to pretend
 * it's a good idea to mess with it here. So, take what coreboot did
 * and leave it at that.
 */
void serial_init(void)
{
}

int serial_havechar(void)
{
	return (regreadl(ERR)&RXREADY);
}

int serial_getchar(void)
{
	/* wait for character to arrive */
	while (! serial_havechar())
		;
	return regreadl(RXDATA);
}

static int serial_cansend(void)
{
	return (! (regreadl(FIFOSTAT)&TXFULL));
}

/*
 * Output a single byte to the serial port.
 * The function is defined as taking an int; unfortunate.
 */
void serial_putchar(unsigned int c)
{

	/* wait for room in the tx FIFO */
	while (! serial_cansend())
		;

	regwriteb(c, TXDATA);
}
