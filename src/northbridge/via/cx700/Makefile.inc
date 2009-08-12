## This file is part of the coreboot project.
##
## Copyright (C) 2007-2009 coresystems GmbH
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

obj-$(CONFIG_NORTHBRIDGE_VIA_CX700) += cx700_reset.o
obj-$(CONFIG_NORTHBRIDGE_VIA_CX700) += northbridge.o
obj-$(CONFIG_NORTHBRIDGE_VIA_CX700) += vgabios.o

driver-$(CONFIG_NORTHBRIDGE_VIA_CX700) += cx700_agp.o
driver-$(CONFIG_NORTHBRIDGE_VIA_CX700) += cx700_lpc.o
driver-$(CONFIG_NORTHBRIDGE_VIA_CX700) += cx700_sata.o
driver-$(CONFIG_NORTHBRIDGE_VIA_CX700) += cx700_vga.o
