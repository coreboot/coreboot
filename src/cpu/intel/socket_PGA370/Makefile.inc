##
## This file is part of the coreboot project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
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

ifeq ($(CONFIG_CPU_INTEL_SOCKET_PGA370),y)
	obj-y += socket_PGA370.o
	subdirs-y += ../model_6xx
	subdirs-y += ../../x86/tsc
	subdirs-y += ../../x86/mtrr
	subdirs-y += ../../x86/fpu
	subdirs-y += ../../x86/mmx
	subdirs-y += ../../x86/sse
	subdirs-y += ../../x86/lapic
	subdirs-y += ../../x86/cache
	subdirs-y += ../../x86/smm
	subdirs-y += ../microcode
endif

