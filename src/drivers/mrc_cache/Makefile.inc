
romstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c

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
