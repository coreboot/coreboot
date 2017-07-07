#
# This file is part of the coreboot project.
#
# Copyright (C) 2011 - 2016 Advanced Micro Devices, Inc.
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

subdirs-$(CONFIG_CPU_AMD_PI_00630F01) += 00630F01
subdirs-$(CONFIG_CPU_AMD_PI_00730F01) += 00730F01
subdirs-$(CONFIG_CPU_AMD_PI_00660F01) += 00660F01

ramstage-$(CONFIG_SPI_FLASH) += spi.c

cpu_incs-y += $(src)/cpu/amd/pi/cache_as_ram.inc

romstage-y += heapmanager.c
ramstage-y += heapmanager.c
ramstage-y += amd_late_init.c
