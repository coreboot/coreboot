##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 coresystems GmbH
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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

subdirs-y += ati
subdirs-y += dec
subdirs-y += emulation
subdirs-y += generic
subdirs-y += i2c
subdirs-y += maxim
subdirs-y += oxford
subdirs-y += parade
subdirs-y += realtek
subdirs-y += sil
subdirs-y += trident
subdirs-y += ics
subdirs-y += spi
subdirs-y += ti
subdirs-y += ipmi
subdirs-y += elog
subdirs-$(CONFIG_ARCH_X86) += pc80
