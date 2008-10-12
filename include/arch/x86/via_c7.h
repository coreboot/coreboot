/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Corey Osgood <corey.osgood@gmail.com>
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

#ifndef CPU_VIA_C7_H
#define CPU_VIA_C7_H


#ifndef __ASSEMBLER__

/* This is new. 
 * We're not using it yet. 
 * K8 requires it and, for future ports, we are going to require it. 
 * it's a useful placeholder for platform info that usually ends up 
 * scattered everywhere. On K8, it is stored near the base of stack
 * in the global variable struct.
 */
struct sys_info {
	int empty;
};

#endif

#endif
