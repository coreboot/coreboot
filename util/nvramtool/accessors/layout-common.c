/* SPDX-License-Identifier: GPL-2.0-only */

#include <ctype.h>

#include "layout-text.h"

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
