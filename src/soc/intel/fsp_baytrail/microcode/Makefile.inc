#
# This file is part of the coreboot project.
#
# Copyright (C) 2014 Sage Electronic Engineering, LLC.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#
cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE) += microcode_blob.c
CPPFLAGS_romstage += -I$(src)/soc/intel/fsp_baytrail/microcode

ifneq ($(CONFIG_MICROCODE_INCLUDE_PATH),)
ifneq ($(wildcard $(shell readlink -f "$(top)/$(CONFIG_MICROCODE_INCLUDE_PATH)")),)
CPPFLAGS_common += -I$(CONFIG_MICROCODE_INCLUDE_PATH)
endif
endif
