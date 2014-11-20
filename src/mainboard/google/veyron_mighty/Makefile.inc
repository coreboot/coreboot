##
## This file is part of the coreboot project.
##
## Copyright 2014 Rockchip Inc.
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
bootblock-y += bootblock.c
bootblock-y += boardid.c
bootblock-y += chromeos.c
bootblock-y += reset.c

verstage-y += boardid.c
verstage-y += chromeos.c
verstage-y += reset.c

romstage-y += boardid.c
romstage-y += romstage.c
romstage-y += sdram_configs.c
romstage-y += reset.c

ramstage-y += boardid.c
ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += reset.c

bootblock-y += memlayout.ld
verstage-y += memlayout.ld
romstage-y += memlayout.ld
ramstage-y += memlayout.ld
