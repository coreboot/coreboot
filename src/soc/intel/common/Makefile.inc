ifeq ($(CONFIG_SOC_INTEL_COMMON),y)

verstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c

romstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c
romstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c
romstage-y += util.c
romstage-$(CONFIG_MMA) += mma.c

ramstage-y += hda_verb.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += mrc_cache.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += nvm.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c
ramstage-y += util.c
ramstage-$(CONFIG_MMA) += mma.c
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

ifeq ($(CONFIG_MMA),y)
MMA_BLOBS_PATH = $(call strip_quotes,$(CONFIG_MMA_BLOBS_PATH))
MMA_TEST_NAMES = $(notdir $(wildcard $(MMA_BLOBS_PATH)/tests/*))
MMA_TEST_CONFIG_NAMES = $(notdir $(wildcard $(MMA_BLOBS_PATH)/configs/*))

#
# MMA_CBFS_template is the template to be expanded by eval
# where $(1) is file name
#	$(2) is file path
#	$(3) is file type, efi for test names (all .EFI files under $(MMA_BLOBS_PATH)/tests )
#			 , mma for test param (all .BIN files under $(MMA_BLOBS_PATH)/configs/<test name>)
#
# $(MMA_BLOBS_PATH)/tests/<testX>.efi has coresponding test params
#  at $(MMA_BLOBS_PATH)/configs/<testX>/<XYZ>.bin
#


define MMA_CBFS_template =
        cbfs-files-y += $(1)
        $(1)-file := $(MMA_BLOBS_PATH)/$(2)/$(1)
        $(1)-type := $(3)
endef

#
# following loop calls MMA_CBFS_template for each .EFI file under $(MMA_BLOBS_PATH)/tests with type = efi
#
$(foreach mma_test,$(MMA_TEST_NAMES),$(eval $(call MMA_CBFS_template,$(mma_test),tests,efi)))


#
# following nested loops calls MMA_CBFS_template for each .BIN file under each MMA_TEST_CONFIG_NAMES
#
#  foreach <testX> do following
#        foreach <XYZ>.bin in <testX> do following
#                call MMA_CBFS_template for each <XYZ>.bin under current <testX> with type = mma
#

$(foreach mma_test, $(MMA_TEST_CONFIG_NAMES),\
	$(eval $(foreach mma_config,$(notdir $(wildcard $(MMA_BLOBS_PATH)/configs/$(mma_test)/*)),\
		$(eval $(call MMA_CBFS_template,$(mma_config),configs/$(mma_test),mma)))))

endif

endif
