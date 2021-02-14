/* bincfg - Compiler/Decompiler for data blobs with specs */
/* SPDX-License-Identifier: GPL-3.0-or-later */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bincfg.tab.h"

extern struct blob binary;

unsigned int parsehex (char *s)
{
	unsigned int i, nib, retval = 0;
	unsigned int nibs = strlen(s) - 2;

	for (i = 2; i < nibs + 2; i++) {
		if (s[i] >= '0' && s[i] <= '9') {
			nib = s[i] - '0';
		} else if (s[i] >= 'a' && s[i] <= 'f') {
			nib = s[i] - 'a' + 10;
		} else if (s[i] >= 'A' && s[i] <= 'F') {
			nib = s[i] - 'A' + 10;
		} else {
			return 0;
		}
		retval |= nib << (((nibs - 1) - (i - 2)) * 4);
	}
	return retval;
}

char* stripquotes (char *string)
{
	char *stripped;
	unsigned int len = strlen(string);
	if (len >= 2 && string[0] == '"' && string[len - 1] == '"') {
		stripped = (char *) malloc (len - 1);
		if (stripped == NULL) {
			printf("Out of memory\n");
			exit(1);
		}
		snprintf (stripped, len - 1, "%s", string + 1);
		return stripped;
	}
	return NULL;
}

%}

%option noyywrap
%option nounput

DIGIT [0-9]
INT [-]?{DIGIT}|[-]?[1-9]{DIGIT}+
FRAC [.]{DIGIT}+
EXP [eE][+-]?{DIGIT}+
NUMBER {INT}|{INT}{FRAC}|{INT}{EXP}|{INT}{FRAC}{EXP}
HEX [0][x][0-9a-fA-F]+
STRING ["][^"]*["]
COMMENT [#][^\n]*$

%%

{STRING} {
    yylval.str = stripquotes(yytext);
    return name;
};

{NUMBER} {
    yylval.u32 = atoi(yytext);
    return val;
};

{HEX} {
    yylval.u32 = parsehex(yytext);
    return val;
};

\{ {
    return '{';
};

\} {
    return '}';
};

\[ {
    return '[';
};

\] {
    return ']';
};

, {
    return ',';
};

: {
    return ':';
};

= {
    return '=';
};

[ \t\n]+ /* ignore whitespace */;

{COMMENT} /* ignore comments */

\% {
    return '%';
};

<<EOF>> { return eof; };

%%

void set_input_string(char* in) {
	yy_scan_string(in);
}
