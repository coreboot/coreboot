# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += variants/$(VARIANT_DIR)/early_gpio.c

ramstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-y += variants/$(VARIANT_DIR)/port_descriptors.c

ifneq ($(CONFIG_MANDOLIN_LPC),y)
ramstage-y += emmc_gpio.c
endif

ifeq ($(CONFIG_BOARD_AMD_MANDOLIN),y)
APCB_SOURCES = $(MAINBOARD_BLOBS_DIR)/APCB_mandolin.bin
else # CONFIG_BOARD_AMD_CEREME
APCB_SOURCES = $(MAINBOARD_BLOBS_DIR)/APCB_cereme.bin
endif

ifeq ($(CONFIG_MANDOLIN_HAVE_MCHP_FW),y)

$(call add_intermediate, add_mchp_fw)
	$(CBFSTOOL) $< write -r EC -f $(CONFIG_MANDOLIN_MCHP_FW_FILE) --fill-upward

else
files_added:: warn_no_mchp
endif # CONFIG_MANDOLIN_HAVE_MCHP_FW

PHONY+=warn_no_mchp
warn_no_mchp:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without the Microchip EC.\n"
	printf "Do not flash this image.  Your Mandolin's power button\n"
	printf "will not respond when you press it.\n\n"
