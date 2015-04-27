ifeq ($(CONFIG_SOC_INTEL_BAYTRAIL)$(CONFIG_SOC_INTEL_BROADWELL),y)

ramstage-y += hda_verb.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += nvm.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c
romstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c
romstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c

endif
