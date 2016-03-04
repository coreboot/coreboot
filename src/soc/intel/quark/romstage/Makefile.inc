#
# This file is part of the coreboot project.
#
# Copyright (C) 2015-2016 Intel Corporation
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

cpu_incs-y += $(src)/soc/intel/quark/romstage/esram_init.inc
cpu_incs-y += $(src)/soc/intel/quark/romstage/cache_as_ram.inc

romstage-y += mtrr.c
romstage-y += report_platform.c
romstage-y += romstage.c
romstage-$(CONFIG_ENABLE_BUILTIN_HSUART1) += uart.c
