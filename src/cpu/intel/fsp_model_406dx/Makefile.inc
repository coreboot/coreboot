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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#

ramstage-y += model_406dx_init.c
subdirs-y += ../../x86/name

ramstage-y += acpi.c

cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE)  += microcode_blob.c
CPPFLAGS_romstage += -I$(src)/cpu/intel/fsp_model_406dx

ifneq ($(CONFIG_MICROCODE_INCLUDE_PATH),)
ifneq ($(wildcard $(shell readlink -f "$(top)/$(CONFIG_MICROCODE_INCLUDE_PATH)")),)
CPPFLAGS_common += -I$(CONFIG_MICROCODE_INCLUDE_PATH)
endif
endif
