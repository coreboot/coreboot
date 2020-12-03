bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CSE) += cse.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CSE) += cse.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_CSE) += cse.c
romstage-$(CONFIG_SOC_INTEL_CSE_LITE_SKU) += cse_lite.c
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_HECI_DISABLE_IN_SMM) += disable_heci.c

ifeq ($(CONFIG_SOC_INTEL_CSE_RW_UPDATE),y)
ifneq ($(CONFIG_SOC_INTEL_CSE_RW_FILE),"")
CSE_LITE_ME_RW = $(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_CBFS_NAME))
regions-for-file-$(CSE_LITE_ME_RW) = $(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_A_FMAP_NAME)), \
					$(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_B_FMAP_NAME))
cbfs-files-y += $(CSE_LITE_ME_RW)
$(CSE_LITE_ME_RW)-file := $(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_FILE))
$(CSE_LITE_ME_RW)-name := $(CSE_LITE_ME_RW)
$(CSE_LITE_ME_RW)-type := raw
else
$(error "CSE RW file path is missing and need to be set by mainboard config")
endif

# Extract the CSE RW firmware version and update the cse_rw_metadata structure
ifneq ($(CONFIG_SOC_INTEL_CSE_RW_VERSION),"")
CSE_RW_VERSION:=$(subst ., ,$(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_VERSION)))
MAJOR := $(word 1, $(CSE_RW_VERSION))
MINOR := $(word 2, $(CSE_RW_VERSION))
HOTFIX := $(word 3, $(CSE_RW_VERSION))
BUILD := $(word 4, $(CSE_RW_VERSION))
CPPFLAGS_common += -DCSE_RW_MAJOR=$(MAJOR) -DCSE_RW_MINOR=$(MINOR) -DCSE_RW_HOTFIX=$(HOTFIX) -DCSE_RW_BUILD=$(BUILD)
else
$(error "CSE RW version is missing and need to be set by mainboard config")
endif

# Compute the hash of the CSE RW binary and update the cse_rw_metadata structure
CSE_RW_PATH := $(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_FILE))
HASH := $(shell openssl dgst -sha256 -hex $(CSE_RW_PATH) | cut -d " " -f2 | fold -w2 | paste -sd',' -)
CPPFLAGS_common += -DCSE_RW_SHA256=$(HASH)

# Add the CSE RW metadata file to FW_MAIN_A/B
CSE_RW_METADATA = $(call strip_quotes,$(CONFIG_SOC_INTEL_CSE_RW_METADATA_CBFS_NAME))
regions-for-file-$(CSE_RW_METADATA) = FW_MAIN_A,FW_MAIN_B
cbfs-files-y += $(CSE_RW_METADATA)
$(CSE_RW_METADATA)-file := cse_rw_metadata.c:struct
$(CSE_RW_METADATA)-name := $(CSE_RW_METADATA)
$(CSE_RW_METADATA)-type := raw
endif
