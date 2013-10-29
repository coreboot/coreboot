#
# This file is part of the coreboot project.
#
# Copyright (C) 2010 Google Inc.
# Copyright (C) 2013 Sage Electronic Engineering, LLC.
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

ramstage-y += northbridge.c
ramstage-y += gma.c

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.c
ramstage-y += fsp_util.c
ramstage-$(CONFIG_ENABLE_FAST_BOOT) += mrccache.c

romstage-y += raminit.c
romstage-y += fsp_util.c
romstage-y += early_init.c
romstage-y += report_platform.c
romstage-y += ../../../arch/x86/lib/walkcbfs.S
romstage-$(CONFIG_ENABLE_FAST_BOOT) += mrccache.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += udelay.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

ifeq ($(CONFIG_HAVE_FSP_BIN),y)
# We don't ship an FSP, but booting without it is bound to fail
cbfs-files-y += fsp.bin
fsp.bin-file := $(call strip_quotes,$(CONFIG_FSP_FILE))
fsp.bin-position := $(CONFIG_FSP_LOC)
fsp.bin-type := 0xab
endif

ifeq ($(CONFIG_ENABLE_FAST_BOOT),y)
$(obj)/mrc.cache:
	dd if=/dev/zero count=1 \
	bs=$(shell printf "%d" $(CONFIG_MRC_CACHE_SIZE) ) | \
	tr '\000' '\377' > $@

cbfs-files-y += mrc.cache
mrc.cache-file := $(obj)/mrc.cache
mrc.cache-position := 0xfff50000
mrc.cache-type := 0xac
endif

$(obj)/northbridge/intel/fsp_sandybridge/acpi.ramstage.o : $(obj)/build.h
