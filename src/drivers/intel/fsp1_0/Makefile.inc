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
# Foundation, Inc.
#

ramstage-y += fsp_util.c hob.c
romstage-y += fsp_util.c hob.c

ramstage-$(CONFIG_ENABLE_MRC_CACHE) += fastboot_cache.c
romstage-$(CONFIG_ENABLE_MRC_CACHE) += fastboot_cache.c

CPPFLAGS_common += -Isrc/drivers/intel/fsp1_0 -I$(objgenerated)

ifneq ($(cpu_microcode_bins),)
$(objgenerated)/microcode_size.h: $(obj)/cpu_microcode_blob.bin
	printf "#define MICROCODE_REGION_LENGTH $(call file-size,$<)" > $@.tmp \
	&& cmp $@.tmp $@ 2>/dev/null || mv $@.tmp $@
else
$(objgenerated)/microcode_size.h:
	printf "#define MICROCODE_REGION_LENGTH 0" > $@.tmp \
	&& cmp $@.tmp $@ 2>/dev/null || mv $@.tmp $@
endif

cpu_incs-$(CONFIG_PLATFORM_USES_FSP1_0) += $(objgenerated)/microcode_size.h
cpu_incs-$(CONFIG_USE_GENERIC_FSP_CAR_INC) += $(src)/drivers/intel/fsp1_0/cache_as_ram.inc

ifeq ($(CONFIG_HAVE_FSP_BIN),y)
cbfs-files-y += fsp.bin
fsp.bin-file := $(call strip_quotes,$(CONFIG_FSP_FILE))
fsp.bin-position := $(CONFIG_FSP_LOC)
fsp.bin-type := fsp
endif

ifeq ($(CONFIG_ENABLE_MRC_CACHE),y)
$(obj)/mrc.cache:
	dd if=/dev/zero count=1 \
	bs=$(shell printf "%d" $(CONFIG_MRC_CACHE_SIZE) ) | \
	tr '\000' '\377' > $@

cbfs-files-y += mrc.cache
mrc.cache-file := $(obj)/mrc.cache
mrc.cache-align := 0x10000
mrc.cache-type := mrc_cache
endif

