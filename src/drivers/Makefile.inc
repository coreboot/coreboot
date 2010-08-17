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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

subdirs-y += dec

# should these be cleaned up to behave like all other subdirectories?

subdirs-y += ati/ragexl
subdirs-y += emulation/qemu
subdirs-y += generic/debug
subdirs-y += i2c/adm1026
subdirs-y += i2c/adm1027
subdirs-y += i2c/adt7463
subdirs-y += i2c/i2cmux
subdirs-y += i2c/i2cmux2
subdirs-y += i2c/lm63
subdirs-y += sil/3114
subdirs-y += trident/blade3d

