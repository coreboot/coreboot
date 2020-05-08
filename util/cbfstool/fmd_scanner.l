/* scanner generator for flashmap descriptor language */
/* SPDX-License-Identifier: GPL-2.0-only */

%{
#include "fmd_parser.h"

#include <assert.h>
#include <string.h>

int parse_integer(char *src, int base);
int copy_string(const char *src);
%}

%option noyywrap
%s FLAGS

MULTIPLIER [KMG]

%%
[[:space:]]+                /* Eat whitespace. */
#.*$                        /* Eat comments. */
\(                          BEGIN(FLAGS); return *yytext;
<FLAGS>\)                   BEGIN(INITIAL); return *yytext;
<FLAGS>CBFS                 return FLAG_CBFS;
<FLAGS>PRESERVE             return FLAG_PRESERVE;
0{MULTIPLIER}?              |
[1-9][0-9]*{MULTIPLIER}?    return parse_integer(yytext, 10);
0[0-9]+{MULTIPLIER}?        return OCTAL;
0[xX][0-9a-fA-F]+{MULTIPLIER}? return parse_integer(yytext + 2, 16);
[^#@{}()[:space:]]*         return copy_string(yytext);
.                           return *yytext;

%%

int parse_integer(char *src, int base)
{
	char *multiplier = NULL;
	unsigned val = strtoul(src, &multiplier, base);

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

int copy_string(const char *src)
{
	yylval.strval = strdup(src);
	return STRING;
}
