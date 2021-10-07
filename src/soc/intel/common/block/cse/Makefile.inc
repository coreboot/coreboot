bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CSE) += cse.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CSE) += cse.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CSE) += cse.c
romstage-$(CONFIG_SOC_INTEL_CSE_LITE_SKU) += cse_lite.c
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_HECI_DISABLE_IN_SMM) += disable_heci.c

ramstage-$(CONFIG_SOC_INTEL_CSE_SET_EOP) += cse_eop.c

ifeq ($(CONFIG_SOC_INTEL_CSE_RW_UPDATE),y)

ifeq ($(CONFIG_SOC_INTEL_CSE_RW_FILE),"")
$(error "CSE RW file path is missing and need to be set by mainboard config")
endif

ifeq ($(CONFIG_SOC_INTEL_CSE_RW_VERSION),"")
$(error "CSE RW version is missing and need to be set by mainboard config")
endif

CSE_RW_FILE=$(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_FILE))

CSE_LITE_ME_RW = $(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_CBFS_NAME))
regions-for-file-$(CSE_LITE_ME_RW) = $(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_A_FMAP_NAME)), \
					$(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_B_FMAP_NAME))
cbfs-files-y += $(CSE_LITE_ME_RW)
$(CSE_LITE_ME_RW)-file := $(CSE_RW_FILE)
$(CSE_LITE_ME_RW)-name := $(CSE_LITE_ME_RW)
$(CSE_LITE_ME_RW)-type := raw

$(obj)/cse_rw.version:
	@echo '$(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_VERSION))' > $@

CSE_RW_VERSION = $(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_VERSION_CBFS_NAME))
regions-for-file-$(CSE_RW_VERSION) = FW_MAIN_A,FW_MAIN_B
cbfs-files-y += $(CSE_RW_VERSION)
$(CSE_RW_VERSION)-file := $(obj)/cse_rw.version
$(CSE_RW_VERSION)-name := $(CSE_RW_VERSION)
$(CSE_RW_VERSION)-type := raw

$(obj)/cse_rw.hash: $(CSE_RW_FILE)
	openssl dgst -sha256 -binary $< > $@

CSE_RW_HASH = $(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_HASH_CBFS_NAME))
regions-for-file-$(CSE_RW_HASH) = FW_MAIN_A,FW_MAIN_B
cbfs-files-y += $(CSE_RW_HASH)
$(CSE_RW_HASH)-file := $(obj)/cse_rw.hash
$(CSE_RW_HASH)-name := $(CSE_RW_HASH)
$(CSE_RW_HASH)-type := raw

endif
