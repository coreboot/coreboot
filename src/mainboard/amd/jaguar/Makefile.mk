# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += early_gpio.c
bootblock-y += ec.c

romstage-y += port_descriptors.c

ramstage-y += gpio.c
ramstage-y += port_descriptors.c

APCB_SOURCES_RECOVERY = $(src)/mainboard/$(MAINBOARDDIR)/APCB_FP8_LPDDR5_DefaultRecovery.apcb

ifeq ($(CONFIG_JAGUAR_HAVE_MCHP_FW),y)
subdirs-y += ../../../../util/mec152x

$(call add_intermediate, add_mchp_fw, $(objutil)/mec152x/mec152xtool)
	$(CBFSTOOL) $(obj)/coreboot.pre write -r EC_BODY -f $(CONFIG_JAGUAR_MCHP_FW_FILE) --fill-upward
	$(objutil)/mec152x/mec152xtool $(obj)/coreboot.pre GEN_ECFW_PTR -f EC_BODY
else
files_added:: warn_no_mchp
endif # CONFIG_JAGUAR_HAVE_MCHP_FW

PHONY+=warn_no_mchp
warn_no_mchp:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without the EC FW.\n"
	printf "Do not flash this image.  Your Jaguar's power button\n"
	printf "will not respond when you press it.\n\n"
