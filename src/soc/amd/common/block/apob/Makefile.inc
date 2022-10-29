## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_APOB),y)
romstage-y += apob_cache.c
ramstage-y += apob_cache.c

$(call src-to-obj,romstage,$(dir)/apob_cache.c) : $(obj)/fmap_config.h
$(call src-to-obj,ramstage,$(dir)/apob_cache.c) : $(obj)/fmap_config.h

endif # CONFIG_SOC_AMD_COMMON_BLOCK_APOB
