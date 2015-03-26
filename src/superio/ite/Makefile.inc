##
## This file is part of the coreboot project.
##
## Copyright (C) 2009 Ronald G. Minnich
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
## Foundation, Inc.
##

## include generic ite pre-ram stage driver
romstage-$(CONFIG_SUPERIO_ITE_COMMON_ROMSTAGE) += common/early_serial.c

subdirs-y += it8661f
subdirs-y += it8671f
subdirs-y += it8712f
subdirs-y += it8716f
subdirs-y += it8718f
subdirs-y += it8721f
subdirs-y += it8728f
subdirs-y += it8772f
