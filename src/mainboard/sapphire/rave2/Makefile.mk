# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += early_gpio.c

romstage-y += port_descriptors.c

ramstage-y += gpio.c
ramstage-y += mainboard.c
ramstage-y += port_descriptors.c

ifneq ($(wildcard $(MAINBOARD_BLOBS_DIR)/APCB_FP8_LPDDR5.bin),)
APCB_SOURCES = $(MAINBOARD_BLOBS_DIR)/APCB_FP8_LPDDR5.bin
APCB_SOURCES_RECOVERY = $(MAINBOARD_BLOBS_DIR)/APCB_FP8_LPDDR5_DefaultRecovery.bin
else
show_notices:: warn_no_apcb
endif

PHONY+=warn_no_apcb
warn_no_apcb:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without APCB binary.\n"
	printf "Do not flash this image.\n\n"
