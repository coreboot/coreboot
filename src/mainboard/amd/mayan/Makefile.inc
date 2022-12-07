# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += early_gpio.c
bootblock-y += ec.c

romstage-y += port_descriptors.c

ramstage-y += chromeos.c
ramstage-y += gpio.c
ramstage-y += port_descriptors.c

ifneq ($(wildcard $(MAINBOARD_BLOBS_DIR)/APCB_Updatable.bin),)
APCB_SOURCES = $(MAINBOARD_BLOBS_DIR)/APCB_Updatable.bin
APCB_SOURCES_RECOVERY = $(MAINBOARD_BLOBS_DIR)/APCB_DefaultRecovery.bin
else
$(info APCB sources not found. Skipping APCB. The resulting image won't boot.)
endif

ifeq ($(CONFIG_MAYAN_HAVE_MCHP_FW),y)
$(call add_intermediate, add_mchp_fw)
	$(CBFSTOOL) $(obj)/coreboot.pre write -r EC -f $(CONFIG_MAYAN_MCHP_SIG_FILE) --fill-upward

# calculate the absolute position from the config offset
MAYAN_EC_POSITION=$(call int-add, \
	$(call int-subtract, 0xffffffff \
	$(CONFIG_ROM_SIZE)) $(CONFIG_MAYAN_MCHP_FW_OFFSET) 1)

cbfs-files-y += ec/ecfw
ec/ecfw-file := $(CONFIG_MAYAN_MCHP_FW_FILE)
ec/ecfw-position := $(MAYAN_EC_POSITION)
ec/ecfw-type := raw

else
files_added:: warn_no_mchp
endif # CONFIG_MAYAN_HAVE_MCHP_FW

PHONY+=warn_no_mchp
warn_no_mchp:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without the EC FW.\n"
	printf "Do not flash this image.  Your Mayan's power button\n"
	printf "will not respond when you press it.\n\n"
