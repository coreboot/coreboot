/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
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

/*
 * Datasheet:
 *  - Name: MC146818: Real-time Clock Plus RAM (RTC)
 *  - PDF: http://www.freescale.com/files/microcontrollers/doc/data_sheet/MC146818.pdf
 *  - Order number: MC146818/D
 */

/*
 * See also:
 * http://bochs.sourceforge.net/techspec/CMOS-reference.txt
 * http://www.bioscentral.com/misc/cmosmap.htm
 */

#include <libpayload.h>

#define RTC_PORT 0x70

/**
 * Read a byte from the specified CMOS address.
 * 
 * @param addr The CMOS address to read a byte from.
 * @return The byte at the given CMOS address.
 */
u8 cmos_read(u8 addr)
{
	outb(addr, RTC_PORT);
	return inb(RTC_PORT + 1);
}

/**
 * Write a byte to the specified CMOS address.
 * 
 * @param val The byte to write to CMOS.
 * @param addr The CMOS address to write to.
 */
void cmos_write(u8 val, u8 addr)
{
	outb(addr, RTC_PORT);
	outb(val, RTC_PORT + 1);
}
