################################################################################
## This file is part of the coreboot project.
##
## Copyright (C) 2014 Google Inc
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License as
## published by the Free Software Foundation; version 2 of
## the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
## MA 02110-1301 USA
##
################################################################################

lib_access = pstate.c sysctrl.c cache.c tlb.c clock.c

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARMV8_64),y)
bootblock-y += $(lib_access)
endif

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV8_64),y)
romstage-y += $(lib_access)
endif

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARMV8_64),y)
ramstage-y += $(lib_access)

ifeq ($(CONFIG_ARCH_USE_SECURE_MONITOR),y)
secmon-y += $(lib_access)
endif

endif
