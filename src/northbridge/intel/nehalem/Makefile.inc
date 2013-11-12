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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#

ramstage-y += northbridge.c
ramstage-y += gma.c

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.c
ramstage-y += ../sandybridge/mrccache.c

romstage-y += raminit.c
romstage-y += early_init.c
romstage-y += ../sandybridge/mrccache.c
romstage-y += ../../../arch/x86/lib/walkcbfs.S

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

$(obj)/mrc.cache:
	dd if=/dev/zero count=1 \
	bs=$(shell printf "%d" $(CONFIG_MRC_CACHE_SIZE) ) | \
	tr '\000' '\377' > $@

cbfs-files-$(CONFIG_HAVE_MRC) += mrc.cache
mrc.cache-file := $(obj)/mrc.cache
mrc.cache-position := 0xfff80000
mrc.cache-type := 0xac

$(obj)/northbridge/intel/nehalem/acpi.ramstage.o : $(obj)/build.h
