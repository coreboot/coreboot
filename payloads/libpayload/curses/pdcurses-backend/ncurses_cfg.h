/*
 * This file is part of the libpayload project.
 *
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

#define USE_RCS_IDS 0
#define USE_WIDEC_SUPPORT 0
#define NCURSES_SP_FUNCS 0
#define DECL_ERRNO 0
#define NCURSES_INTEROP_FUNCS 0

#define NCURSES_API
#define NCURSES_IMPEXP
#define NCURSES_INLINE
#define NCURSES_SP_DCLx
#define NCURSES_SP_NAME(x) x
#define NCURSES_EXPORT_VAR(x) x
#define NCURSES_EXPORT(x) x

#define SP_PARM SP
#define CURRENT_SCREEN SP
#define StdScreen(x) stdscr

#define T(x)
#define T_CALLED(x...)

#define GCC_UNUSED __attribute__((unused))

#define BLANK           (' '|A_NORMAL)
#define ZEROS           ('\0'|A_NORMAL)
#define ISBLANK	isblank

#define ChCharOf(c)	((c) & (chtype)A_CHARTEXT)
#define CharOf(c)	ChCharOf(c)

#define WINDOW_ATTRS(x) ((x)->_attrs)
#define IsValidScreen(x) 1

#define returnCode return
#define returnWin return
#define returnCPtr return
#define returnVoidPtr return
#define returnPtr return
#define returnAttr return
#define returnBool return
