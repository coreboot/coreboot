bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CSE) += cse.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CSE) += cse.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CSE) += cse.c
ramstage-$(CONFIG_SOC_INTEL_CSE_LITE_SKU) += cse_lite.c
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_HECI_DISABLE_IN_SMM) += disable_heci.c

ifneq ($(CONFIG_SOC_INTEL_CSE_RW_FILE),"")
CSE_LITE_ME_RW = $(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_CBFS_NAME))
regions-for-file-$(CSE_LITE_ME_RW) = FW_MAIN_A,FW_MAIN_B
cbfs-files-y += $(CSE_LITE_ME_RW)
$(CSE_LITE_ME_RW)-file := $(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_FILE))
$(CSE_LITE_ME_RW)-name := $(CSE_LITE_ME_RW)
$(CSE_LITE_ME_RW)-type := raw
endif
