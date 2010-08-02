##
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
##

obj-y += cx700_reset.o
obj-y += northbridge.o

driver-y += cx700_agp.o
driver-y += cx700_lpc.o
driver-y += cx700_sata.o
driver-$(CONFIG_VGA_ROM_RUN) += cx700_vga.o
