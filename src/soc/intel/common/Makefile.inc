ifeq ($(CONFIG_SOC_INTEL_COMMON),y)

subdirs-y += basecode/
subdirs-y += block/
subdirs-y += pch/

verstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c

bootblock-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c

romstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c
romstage-$(CONFIG_MMA) += mma.c
romstage-y += smbios.c

postcar-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c

ramstage-y += hda_verb.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_RESET) += reset.c
ramstage-$(CONFIG_MMA) += mma.c
ramstage-y += vbt.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_NHLT) += nhlt.c

all-$(CONFIG_TPM_GOOGLE) += tpm_tis.c

romstage-$(CONFIG_SOC_INTEL_COMMON_FSP_RESET) += fsp_reset.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_FSP_RESET) += fsp_reset.c

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
# $(MMA_BLOBS_PATH)/tests/<testX>.efi has corresponding test params
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

# SI_DESC contains soft straps that may modify security-relevant behavior, so it should be
# verified by GSCVD.
vboot-gscvd-ranges += $(shell ( \
	offset=$$($(call fmap-section-offset-cmd,SI_DESC)) ;\
	if [ -n "$$offset" ]; then \
		printf "%x:%x" $$offset $$($(call fmap-section-size-cmd,SI_DESC)) ;\
	fi ;\
))

endif
