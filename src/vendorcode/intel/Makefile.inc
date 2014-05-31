##
## This file is part of the coreboot project.
##
## Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

ifneq ($(CONFIG_FSP_VENDORCODE_HEADER_PATH),)
FSP_PATH := $(call strip_quotes,$(CONFIG_FSP_VENDORCODE_HEADER_PATH))
FSP_SRC_FILES := $(wildcard src/vendorcode/intel/$(FSP_PATH)/srx/*.c)
FSP_C_INPUTS := $(foreach file, $(FSP_SRC_FILES), $(FSP_PATH)/srx/$(notdir $(file)))
ramstage-y += $(FSP_C_INPUTS)

CFLAGS_x86_32 += -Isrc/vendorcode/intel/$(FSP_PATH)/include
endif
