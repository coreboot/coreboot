ifeq ($(CONFIG_SOC_INTEL_COMMON),y)

romstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c
romstage-$(CONFIG_SOC_INTEL_COMMON_FSP_RAM_INIT) += raminit.c
romstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c
romstage-$(CONFIG_SOC_INTEL_COMMON_FSP_ROMSTAGE) += romstage.c
romstage-$(CONFIG_SOC_INTEL_COMMON_STACK) += stack.c
romstage-$(CONFIG_SOC_INTEL_COMMON_STAGE_CACHE) += stage_cache.c
romstage-$(CONFIG_PLATFORM_USES_FSP1_1) += util.c

ramstage-$(CONFIG_PLATFORM_USES_FSP1_1) += fsp_ramstage.c
ramstage-y += hda_verb.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += nvm.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_STAGE_CACHE) += stage_cache.c
ramstage-$(CONFIG_PLATFORM_USES_FSP1_1) += util.c
ramstage-$(CONFIG_GOP_SUPPORT) += vbt.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_ACPI_WAKE_SOURCE) += acpi_wake_source.c

# Create and add the MRC cache to the cbfs image
ifneq ($(CONFIG_CHROMEOS),y)
$(obj)/mrc.cache: $(obj)/config.h
	dd if=/dev/zero count=1 \
	bs=$(shell printf "%d" $(CONFIG_MRC_SETTINGS_CACHE_SIZE) ) | \
	tr '\000' '\377' > $@

cbfs-files-$(CONFIG_CACHE_MRC_SETTINGS) += mrc.cache
mrc.cache-file := $(obj)/mrc.cache
mrc.cache-position := $(CONFIG_MRC_SETTINGS_CACHE_BASE)
mrc.cache-type := mrc_cache
endif

endif
