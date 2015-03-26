##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 Google Inc.
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

subdirs-y += spd

romstage-y += chromeos.c
ramstage-y += chromeos.c
ramstage-y += gpio.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c
ramstage-y += irqroute.c
ramstage-y += w25q64.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += mainboard_smi.c
