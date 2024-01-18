## SPDX-License-Identifier: GPL-2.0-only

ifneq ($(wildcard $(MAINBOARD_BLOBS_DIR)/Typex60_0_0_0_Apcb.bin),)
APCB_SOURCES = $(MAINBOARD_BLOBS_DIR)/Typex60_0_0_0_Apcb.bin
APCB_SOURCES1 = $(MAINBOARD_BLOBS_DIR)/Typex60_0_1_0_Apcb.bin
APCB_SOURCES_RECOVERY = $(MAINBOARD_BLOBS_DIR)/Typex68_0_0_0_ApcbRec.bin
APCB_SOURCES_RECOVERY1 = $(MAINBOARD_BLOBS_DIR)/Typex68_0_8_0_ApcbRec.bin
APCB_SOURCES_RECOVERY2 = $(MAINBOARD_BLOBS_DIR)/Typex68_0_9_0_ApcbRec.bin
else
show_notices:: warn_no_apcb
endif
