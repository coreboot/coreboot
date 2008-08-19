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

#include <libpayload.h>

int isalpha(int c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

int isascii(int c)
{
	return (c >= 0 && c <= 127);
}

int isblank(int c)
{
	return (c == ' ' || c == '\t');
}

int iscntrl(int c)
{
	return (c <= 31 || c == 127);
}

int isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

int isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}

int isgraph(int c)
{
	return (c >= 33 && c <= 126);
}

int islower(int c)
{
	return (c >= 'a' && c <= 'z');
}

int isprint(int c)
{
	return (c >= 32 && c <= 126);
}

int ispunct(int c)
{
	return isprint(c) && !isspace(c) && !isalnum(c);
}

int isspace(int c)
{
	return (c == ' ' || (c >= '\t' && c <= '\r'));
}

int isupper(int c)
{
	return (c >= 'A' && c <= 'Z');
}

int isxdigit(int c)
{
	return isdigit(c) || (tolower(c) >= 'a' && tolower(c) <= 'f');
}

int tolower(int c)
{
	return (c >= 'A' && c <= 'Z') ? (c + 32) : c;
}

int toupper(int c)
{
	return (c >= 'a' && c <= 'z') ? (c - 32) : c;
}

