#
# This file is part of the coreboot project.
#
# Copyright (C) 2010 Google Inc.
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

driver-y += northbridge.c
driver-y += gma.c

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.c

romstage-y += udelay.c
romstage-y += raminit.c
romstage-y += early_init.c
romstage-y += ../../../arch/x86/lib/walkcbfs.S

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

# We don't ship that, but booting without it is bound to fail
cbfs-files-$(CONFIG_HAVE_MRC) += mrc.bin
mrc.bin-file := $(CONFIG_MRC_FILE)
ifeq ($(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE),y)
mrc.bin-position := 0xfffa0000
endif
ifeq ($(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE),y)
mrc.bin-position := 0xfffe0000
endif
mrc.bin-type := 0xab
