##
## This file is part of the coreboot project.
##
## Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm

ramstage-y += nano_init.c
ramstage-y += update_ucode.c

# This microcode is included as a separate CBFS file. It is never linked in to
# the rest of coreboot.
cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE) += nano_ucode_blob.c

cpu_incs += $(src)/cpu/via/car/cache_as_ram.inc
