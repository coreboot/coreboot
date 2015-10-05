##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 DMP Electronics Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm

bootblock-y += biosdata.ld
bootblock-y += biosdata_ex.ld
bootblock-y += biosdata.S
bootblock-y += biosdata_ex.S

ROMCCFLAGS := -mcpu=i386 -O2
