/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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

#ifndef _CURSES_LOCAL_H
#define _CURSES_LOCAL_H

/* For curses.priv.h: */
#define USE_RCS_IDS 0
#define DECL_ERRNO 0
#define HAVE_LIBGPM 0
#define NCURSES_EXT_FUNCS 0
#define USE_OK_BCOPY 0
#define USE_MY_MEMMOVE 0
#define USE_SCROLL_HINTS 0
#define USE_HASHMAP 0
#define USE_WIDEC_SUPPORT 0	/* We do _not_ want wide character support. */
// #define NCURSES_EXT_COLORS 1
#define NCURSES_EXT_COLORS 0
#define USE_SYSMOUSE 0
#define NCURSES_NO_PADDING 0
#define USE_HARD_TABS 0
#define HAVE_FCNTL_H 0
#define HAVE_LIMITS_H 1
#define HAVE_UNISTD_H 1
#define USE_XMC_SUPPORT 0
#define NCURSES_EXPANDED 0
#define HAVE_GETCWD 0
#define USE_XMC_SUPPORT 0
#define HAVE_STRSTR 0
#define NO_LEAKS 0
#define HAVE_RESIZETERM 0
#define HAVE_VSSCANF 0
#define BROKEN_LINKER 0

#undef USE_TERMLIB

#include <libpayload-config.h>
#include <libpayload.h>
#include <curses.h>
#include <curses.priv.h>

#define SCREEN_X 80
#define SCREEN_Y 25

/* Flags used to determine what output methods are available */

#ifdef CONFIG_VIDEO_CONSOLE
#define F_ENABLE_CONSOLE 0x01
#else
#define F_ENABLE_CONSOLE 0x00
#endif

#ifdef CONFIG_SERIAL_CONSOLE
#define F_ENABLE_SERIAL  0x02
#else
#define F_ENABLE_SERIAL  0x00
#endif

extern int curses_flags;

/* Share the color table for easy lookup */
extern unsigned char color_pairs[256];

#endif
