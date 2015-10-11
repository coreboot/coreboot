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
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc.
#

ramstage-y += model_406dx_init.c
subdirs-y += ../../x86/name

ramstage-y += acpi.c

CPPFLAGS_romstage += -I$(src)/cpu/intel/fsp_model_406dx
# We don't have microcode for this CPU
# Use CONFIG_CPU_MICROCODE_CBFS_EXTERNAL with a binary microcode file
# cpu_microcode_bins += ???
