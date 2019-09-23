#
# This file is part of the coreboot project.
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

ifeq ($(CONFIG_PLATFORM_USES_FSP1_0),y)

ramstage-y += fsp_util.c hob.c
romstage-y += fsp_util.c hob.c

ramstage-$(CONFIG_ENABLE_MRC_CACHE) += fastboot_cache.c
romstage-$(CONFIG_ENABLE_MRC_CACHE) += fastboot_cache.c

CPPFLAGS_common += -Isrc/drivers/intel/fsp1_0 -I$(objgenerated)

cpu_incs-$(CONFIG_USE_GENERIC_FSP_CAR_INC) += $(src)/drivers/intel/fsp1_0/cache_as_ram.inc

ifeq ($(CONFIG_HAVE_FSP_BIN),y)
cbfs-files-y += fsp.bin
fsp.bin-file := $(call strip_quotes,$(CONFIG_FSP_FILE))
fsp.bin-position := $(CONFIG_FSP_LOC)
fsp.bin-type := fsp
fsp.bin-options := --xip $(TXTIBB)
endif

ifeq ($(CONFIG_ENABLE_MRC_CACHE),y)
ifneq ($(CONFIG_MRC_CACHE_FMAP),y)
$(obj)/mrc.cache:
	dd if=/dev/zero count=1 \
	bs=$(shell printf "%d" $(CONFIG_MRC_CACHE_SIZE) ) | \
	tr '\000' '\377' > $@

cbfs-files-y += mrc.cache
mrc.cache-file := $(obj)/mrc.cache
mrc.cache-align := 0x10000
mrc.cache-type := mrc_cache
endif
endif

ifneq ($(call strip_quotes,$(CONFIG_FSP_SRC_PATH)),)
ramstage-y += $(call strip_quotes,$(CONFIG_FSP_SRC_PATH))
endif

ifneq ($(call strip_quotes,$(CONFIG_FSP_HEADER_PATH)),)
CPPFLAGS_common += -I$(CONFIG_FSP_HEADER_PATH)
endif

endif
