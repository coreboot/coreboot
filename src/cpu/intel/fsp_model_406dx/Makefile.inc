#
# This file is part of the coreboot project.
#
# Copyrignt (C) 2014 Sage Electronic Engineering, LLC.
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

ramstage-y += model_406dx_init.c
subdirs-y += ../../x86/name

ramstage-y += acpi.c

ifeq ($(CONFIG_HAVE_CPU_MICROCODE_FILE), y)
cpu_microcode_bins += $(call strip_quotes,$(CONFIG_CPU_MICROCODE_FILE))
endif

CPPFLAGS_romstage += -I$(src)/cpu/intel/fsp_model_406dx
