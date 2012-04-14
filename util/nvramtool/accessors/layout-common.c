/*****************************************************************************\
 * layout_common.c
 *****************************************************************************
 *  Copyright (C) 2012, Vikram Narayanan
 *  Unified build_opt_tbl and nvramtool
 *  build_opt_tbl.c
 *  	Copyright (C) 2003 Eric Biederman (ebiederm@xmission.com)
 *  	Copyright (C) 2007-2010 coresystems GmbH
 *
 *  This file is part of nvramtool, a utility for reading/writing coreboot
 *  parameters and displaying information from the coreboot table.
 *  For details, see http://coreboot.org/nvramtool.
 *
 *  Please also read the file DISCLAIMER which is included in this software
 *  distribution.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License (as published by the
 *  Free Software Foundation) version 2, dated June 1991.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the terms and
 *  conditions of the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
\*****************************************************************************/

#include <ctype.h>

static int is_ident_nondigit(int c)
{
	int result;
	switch(c) {
	case 'A':	case 'B':	case 'C':	case 'D':
	case 'E':	case 'F':	case 'G':	case 'H':
	case 'I':	case 'J':	case 'K':	case 'L':
	case 'M':	case 'N':	case 'O':	case 'P':
	case 'Q':	case 'R':	case 'S':	case 'T':
	case 'U':	case 'V':	case 'W':	case 'X':
	case 'Y':	case 'Z':
	case 'a':	case 'b':	case 'c':	case 'd':
	case 'e':	case 'f':	case 'g':	case 'h':
	case 'i':	case 'j':	case 'k':	case 'l':
	case 'm':	case 'n':	case 'o':	case 'p':
	case 'q':	case 'r':	case 's':	case 't':
	case 'u':	case 'v':	case 'w':	case 'x':
	case 'y':	case 'z':
	case '_':
		result = 1;
		break;
	default:
		result = 0;
		break;
	}
	return result;
}

int is_ident(char *str)
{
	int result;
	int ch;
	ch = *str;
	result = 0;
	if (is_ident_nondigit(ch)) {
		do {
			str++;
			ch = *str;
		} while(ch && (is_ident_nondigit(ch) || (isdigit(ch))));
		result = (ch == '\0');
	}
	return result;
}
