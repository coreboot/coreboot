/*
 * fmd_scanner.l, scanner generator for flashmap descriptor language
 *
 * Copyright (C) 2015 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

%{
#include "fmd_parser.h"

#include <assert.h>
#include <string.h>

int parse_integer(char *input, int base);
int copy_string(const char *input);
%}

%option noyywrap

MULTIPLIER [KMG]

%%

[[:space:]]+                /* Eat whitespace. */
#.*$                        /* Eat comments. */
0{MULTIPLIER}?              |
[1-9][0-9]*{MULTIPLIER}?    return parse_integer(yytext, 10);
0[0-9]+{MULTIPLIER}?        return OCTAL;
0[xX][0-9a-f]+{MULTIPLIER}? return parse_integer(yytext + 2, 16);
[^#@{}()[:space:]]*         return copy_string(yytext);
.                           return *yytext;

%%

int parse_integer(char *input, int base)
{
	char *multiplier = NULL;
	unsigned val = strtoul(input, &multiplier, base);

	if (*multiplier) {
		switch(*multiplier) {
		case 'K':
			val *= 1024;
			break;
		case 'M':
			val *= 1024*1024;
			break;
		case 'G':
			val *= 1024*1024*1024;
			break;
		default:
			// If we ever get here, the MULTIPLIER regex is allowing
			// multiplier suffixes not handled by this code.
			assert(false);
		}
	}

	yylval.intval = val;
	return INTEGER;
}

int copy_string(const char *input)
{
	yylval.strval = strdup(input);
	return STRING;
}
