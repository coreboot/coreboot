# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += early_gpio.c
bootblock-y += ec.c

romstage-$(CONFIG_BOARD_AMD_BIRMAN_PHOENIX_FSP) += port_descriptors_phoenix.c
romstage-$(CONFIG_BOARD_AMD_BIRMAN_GLINDA) += port_descriptors_glinda.c

ramstage-y += chromeos.c
ramstage-y += gpio.c
ramstage-$(CONFIG_BOARD_AMD_BIRMAN_PHOENIX_OPENSIL) += update_devicetree_phoenix_opensil.c
ramstage-$(CONFIG_BOARD_AMD_BIRMAN_PHOENIX_FSP) += port_descriptors_phoenix.c
ramstage-$(CONFIG_BOARD_AMD_BIRMAN_GLINDA) += port_descriptors_glinda.c

ifneq ($(wildcard $(MAINBOARD_BLOBS_DIR)/APCB_FP8_LPDDR5.bin),)
APCB_SOURCES = $(MAINBOARD_BLOBS_DIR)/APCB_FP8_LPDDR5.bin
APCB_SOURCES_RECOVERY = $(MAINBOARD_BLOBS_DIR)/APCB_FP8_LPDDR5_DefaultRecovery.bin
else
show_notices:: warn_no_apcb
endif

ifeq ($(CONFIG_BIRMAN_HAVE_MCHP_FW),y)
$(call add_intermediate, add_mchp_fw)
	$(CBFSTOOL) $(obj)/coreboot.pre write -r EC_SIG -f $(CONFIG_BIRMAN_MCHP_SIG_FILE) --fill-upward
	$(CBFSTOOL) $(obj)/coreboot.pre write -r EC_BODY -f $(CONFIG_BIRMAN_MCHP_FW_FILE) --fill-upward

else
show_notices:: warn_no_mchp
endif # CONFIG_BIRMAN_HAVE_MCHP_FW

PHONY+=warn_no_mchp
warn_no_mchp:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without the EC FW.\n"
	printf "Do not flash this image.  Your Birman's power button\n"
	printf "will not respond when you press it.\n\n"
