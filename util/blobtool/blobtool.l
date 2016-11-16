/*
 * blobtool - Compiler/Decompiler for data blobs with specs
 * Copyright (C) 2017 Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blobtool.tab.h"

extern struct blob binary;

unsigned int parsehex (char *s)
{
	unsigned int i, nib, val = 0;
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
		val |= nib << (((nibs - 1) - (i - 2)) * 4);
	}
	return val;
}

char* stripquotes (char *string)
{
	char *stripped;
	unsigned int len = strlen(string);
	if (len >= 2 && string[0] == '\"' && string[len-1] == '\"') {
		stripped = (char *) malloc (len - 2 + 1);
		snprintf (stripped, len - 2 + 1, "%s", string+1);
		stripped[len-2] = '\0';
		return stripped;
	} else {
		return 0;
	}
}

%}

%option noyywrap
%option nounput

DIGIT1to9 [1-9]
DIGIT [0-9]
DIGITS {DIGIT}+
INT {DIGIT}|{DIGIT1to9}{DIGITS}|-{DIGIT}|-{DIGIT1to9}{DIGITS}
FRAC [.]{DIGITS}
E [eE][+-]?
EXP {E}{DIGITS}
HEX_DIGIT [0-9a-fA-F]
HEX_DIGITS {HEX_DIGIT}+
NUMBER {INT}|{INT}{FRAC}|{INT}{EXP}|{INT}{FRAC}{EXP}
UNICODECHAR \\u{HEX_DIGIT}{HEX_DIGIT}{HEX_DIGIT}{HEX_DIGIT}
ALPHA [a-zA-Z]
SPECIAL [()\[\]"'@_\-+:;/\\.,<> 	]
VARCHAR {ALPHA}|{DIGIT}|{SPECIAL}
CHAR {VARCHAR}|{UNICODECHAR}
CHARS {CHAR}+
QUOTE ["]
HEX_PREFIX [0][x]
HEX {HEX_PREFIX}{HEX_DIGITS}
STRING {QUOTE}{QUOTE}|{QUOTE}{CHARS}{QUOTE}
COMMENT [#]{CHARS}[\n]|[#]\n

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

