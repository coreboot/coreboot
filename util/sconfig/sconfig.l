%{
/*
 * sconfig, coreboot device tree compiler
 *
 * Copyright (C) 2010 coresystems GmbH
 *   written by Patrick Georgi <patrick@georgi-clan.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "sconfig.tab.h"

int linenum = 0;
%}
%option nodebug
%%
[ \t]+		{}
#.*\n		{linenum++;}
\r?\n		{linenum++;}
chip		{return(CHIP);}
device		{return(DEVICE);}
register	{return(REGISTER);}
on		{yylval.number=1; return(BOOL);}
off		{yylval.number=0; return(BOOL);}
pci		{yylval.number=PCI; return(BUS);}
ioapic          {yylval.number=IOAPIC; return(BUS);}
pnp		{yylval.number=PNP; return(BUS);}
i2c		{yylval.number=I2C; return(BUS);}
lapic		{yylval.number=APIC; return(BUS);}
cpu_cluster	{yylval.number=CPU_CLUSTER; return(BUS);}
cpu		{yylval.number=CPU; return(BUS);}
domain		{yylval.number=DOMAIN; return(BUS);}
generic		{yylval.number=GENERIC; return(BUS);}
mmio		{yylval.number=MMIO; return(BUS);}
spi		{yylval.number=SPI; return(BUS);}
usb		{yylval.number=USB; return(BUS);}
irq		{yylval.number=IRQ; return(RESOURCE);}
drq		{yylval.number=DRQ; return(RESOURCE);}
io		{yylval.number=IO; return(RESOURCE);}
ioapic_irq      {return(IOAPIC_IRQ);}
inherit		{return(INHERIT);}
subsystemid	{return(SUBSYSTEMID);}
end		{return(END);}
=		{return(EQUALS);}
0x[0-9a-fA-F.]+	{yylval.string = malloc(yyleng+1); strncpy(yylval.string, yytext, yyleng); yylval.string[yyleng]='\0'; return(NUMBER);}
[0-9.]+		{yylval.string = malloc(yyleng+1); strncpy(yylval.string, yytext, yyleng); yylval.string[yyleng]='\0'; return(NUMBER);}
[0-9a-fA-F.]+	{yylval.string = malloc(yyleng+1); strncpy(yylval.string, yytext, yyleng); yylval.string[yyleng]='\0'; return(NUMBER);}
INT[A-D]        {yylval.string = malloc(yyleng+1); strncpy(yylval.string, yytext, yyleng); yylval.string[yyleng]='\0'; return(PCIINT);}
\"\"[^\"]+\"\"	{yylval.string = malloc(yyleng-1); strncpy(yylval.string, yytext+1, yyleng-2); yylval.string[yyleng-2]='\0'; return(STRING);}
\"[^\"]+\"	{yylval.string = malloc(yyleng-1); strncpy(yylval.string, yytext+1, yyleng-2); yylval.string[yyleng-2]='\0'; return(STRING);}
[^ \n\t]+	{yylval.string = malloc(yyleng+1); strncpy(yylval.string, yytext, yyleng); yylval.string[yyleng]='\0'; return(STRING);}
%%
