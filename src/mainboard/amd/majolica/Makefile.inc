# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_MAJOLICA_HAVE_MCHP_FW),y)
$(call add_intermediate, add_mchp_fw, $(obj)/coreboot.pre)
	$(CBFSTOOL) $(obj)/coreboot.pre write -r EC -f $(CONFIG_MAJOLICA_MCHP_FW_FILE) --fill-upward
else
files_added:: warn_no_mchp
endif # CONFIG_MAJOLICA_HAVE_MCHP_FW

PHONY+=warn_no_mchp
warn_no_mchp:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without an the Microchip EC FW.\n"
	printf "Do not flash this image.  Your Majolica's power button\n"
	printf "will not respond when you press it.\n\n"
