##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 Google Inc.
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

romstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += i915.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += graphics.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += intel_dp.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += mainboard_smi.c
