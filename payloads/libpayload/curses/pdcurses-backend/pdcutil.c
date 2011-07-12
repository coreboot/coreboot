/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2008 Ulf Jordan <jordan@chalmers.se>
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2011 secunet Security Networks AG
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

#include "lppdc.h"
#include <libpayload.h>

int curses_flags = F_ENABLE_SERIAL | F_ENABLE_CONSOLE;

void PDC_beep(void)
{
    PDC_LOG(("PDC_beep() - called\n"));

#ifdef CONFIG_SPEAKER
    speaker_tone(1760, 500); /* 1760 == note A6 */
#endif
}

void PDC_napms(int ms)
{
    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

    mdelay(ms);
}

const char *PDC_sysname(void)
{
    return "LIBPAYLOAD";
}

void curses_enable_serial(int enable)
{
    curses_flags = (curses_flags & ~F_ENABLE_SERIAL) | (enable * F_ENABLE_SERIAL);
}

void curses_enable_vga(int enable)
{
    curses_flags = (curses_flags & ~F_ENABLE_CONSOLE) | (enable * F_ENABLE_CONSOLE);
}

int curses_serial_enabled()
{
    return !!(curses_flags & F_ENABLE_SERIAL);
}

int curses_vga_enabled()
{
    return !!(curses_flags & F_ENABLE_CONSOLE);
}

