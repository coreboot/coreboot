/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef SUPERIOTOOL_H
#define SUPERIOTOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>

unsigned char regval(unsigned short port, unsigned char reg);
void regwrite(unsigned short port, unsigned char reg, unsigned char val);
void probe_superio(unsigned short port);

/* fintek.c */
void dump_fintek(unsigned short port, unsigned int did);
void probe_idregs_fintek(unsigned short port);

/* ite.c */
void dump_ite(unsigned short port, unsigned short id);
void probe_idregs_ite(unsigned short port);

/* nsc.c */
void dump_ns8374(unsigned short port);
void probe_idregs_simple(unsigned short port);

#endif
