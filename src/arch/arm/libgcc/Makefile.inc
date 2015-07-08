################################################################################
##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 The ChromiumOS Authors
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
################################################################################

libgcc_files = ashldi3.S lib1funcs.S lshrdi3.S muldi3.S ucmpdi2.S uldivmod.S
libgcc_files += udivmoddi4.c umoddi3.c

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARM),y)
bootblock-y += $(libgcc_files)
endif

ifeq ($(CONFIG_ARCH_VERSTAGE_ARM),y)
verstage-y += $(libgcc_files)
endif

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM),y)
romstage-y += $(libgcc_files)
endif

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM),y)
ramstage-y += $(libgcc_files)
endif
