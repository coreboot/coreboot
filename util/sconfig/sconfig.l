%{
/* sconfig, coreboot device tree compiler */
/* SPDX-License-Identifier: GPL-2.0-only */

#include "sconfig.tab.h"

int linenum = 0;
%}
%option nodebug
%%
[ \t]+			{}
#.*\n			{linenum++;}
\r?\n			{linenum++;}
chip			{return(CHIP);}
device			{return(DEVICE);}
alias			{return(ALIAS);}
ops			{return(OPS);}
use			{return(REFERENCE);}
ref			{return(REFERENCE);}
as			{return(ASSOCIATION);}
register		{return(REGISTER);}
fw_config		{return(FW_CONFIG_TABLE);}
field			{return(FW_CONFIG_FIELD);}
option			{return(FW_CONFIG_OPTION);}
probe			{return(FW_CONFIG_PROBE);}
on			{yylval.number=1; return(BOOL);}
off			{yylval.number=0; return(BOOL);}
hidden			{yylval.number=3; return(STATUS);}
mandatory		{yylval.number=5; return(STATUS);}
pci			{yylval.number=PCI; return(BUS);}
ioapic			{yylval.number=IOAPIC; return(BUS);}
pnp			{yylval.number=PNP; return(BUS);}
i2c			{yylval.number=I2C; return(BUS);}
cpu_cluster		{yylval.number=CPU_CLUSTER; return(BUS);}
cpu			{yylval.number=CPU; return(BUS);}
domain			{yylval.number=DOMAIN; return(BUS);}
generic			{yylval.number=GENERIC; return(BUS);}
mmio			{yylval.number=MMIO; return(BUS);}
spi			{yylval.number=SPI; return(BUS);}
usb			{yylval.number=USB; return(BUS);}
gpio			{yylval.number=GPIO; return(BUS);}
mdio			{yylval.number=MDIO; return(BUS);}
irq			{yylval.number=IRQ; return(RESOURCE);}
drq			{yylval.number=DRQ; return(RESOURCE);}
io			{yylval.number=IO; return(RESOURCE);}
ioapic_irq		{return(IOAPIC_IRQ);}
inherit			{return(INHERIT);}
subsystemid		{return(SUBSYSTEMID);}
end			{return(END);}
smbios_slot_desc	{return(SLOT_DESC);}
smbios_dev_info		{return(SMBIOS_DEV_INFO);}
=			{return(EQUALS);}
\|			{return(PIPE);}
0x[0-9a-fA-F.]+		{yylval.string = malloc(yyleng+1); strncpy(yylval.string, yytext, yyleng); yylval.string[yyleng]='\0'; return(NUMBER);}
[0-9.]+			{yylval.string = malloc(yyleng+1); strncpy(yylval.string, yytext, yyleng); yylval.string[yyleng]='\0'; return(NUMBER);}
[0-9a-fA-F.]+		{yylval.string = malloc(yyleng+1); strncpy(yylval.string, yytext, yyleng); yylval.string[yyleng]='\0'; return(NUMBER);}
INT[A-D]		{yylval.string = malloc(yyleng+1); strncpy(yylval.string, yytext, yyleng); yylval.string[yyleng]='\0'; return(PCIINT);}
\"\"[^\"]+\"\"		{yylval.string = malloc(yyleng-1); strncpy(yylval.string, yytext+1, yyleng-2); yylval.string[yyleng-2]='\0'; return(STRING);}
\"[^\"]+\"		{yylval.string = malloc(yyleng-1); strncpy(yylval.string, yytext+1, yyleng-2); yylval.string[yyleng-2]='\0'; return(STRING);}
[^ \n\t]+		{yylval.string = malloc(yyleng+1); strncpy(yylval.string, yytext, yyleng); yylval.string[yyleng]='\0'; return(STRING);}
%%
