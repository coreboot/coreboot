## 
## This file is part of the LinuxBIOS project.
## 
## Copyright (C) 2007 Stefan Reinauer <stepan@coresystems.de>
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
## 

# Reset vector.

# RVECTOR: size of reset vector, default is 0x10
# RESRVED: size of vpd code, default is 0xf0
#
# BOOTBLK: size of bootblock code, default is 0x1f00 (8k-256b)
#

SEGMENT_SIZE = 0x10000
RVECTOR      = 0x00010

	.code16
	.globl _start
_start:
	jmp SEGMENT_SIZE-(BOOTBLK+RESRVED+RVECTOR)
	
.byte	0

# date? idstring? we might want to put something else in here.
.ascii DATE

# checksum
.word  0

