# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += early_gpio.c
bootblock-y += ec.c

romstage-y += ec.c
romstage-y += port_descriptors_v2000a.c

ramstage-y += chromeos.c
ramstage-y += gpio.c
ramstage-y += ec.c
ramstage-y += port_descriptors_v2000a.c

# APCB_SOURCES is empty, since we don't need to update APCB settings from the running system
APCB_SOURCES_RECOVERY = $(src)/mainboard/$(MAINBOARDDIR)/APCB_RN_D4_DefaultRecovery_32GB_DRAM_512MB_VRAM.apcb

ifeq ($(CONFIG_CRATER_HAVE_MCHP_FW),y)
subdirs-y += ../../../../util/mec152x

$(call add_intermediate, add_mchp_fw, $(objutil)/mec152x/mec152xtool)
	$(CBFSTOOL) $(obj)/coreboot.pre write -r EC_BODY -f $(CONFIG_CRATER_MCHP_FW_FILE) --fill-upward
	$(objutil)/mec152x/mec152xtool $(obj)/coreboot.pre GEN_ECFW_PTR -f EC_BODY

ifeq ($(CONFIG_CRATER_HAVE_PD_FW),y)
subdirs-y += ../../../../util/amdtools/ec_usb_pd_fw

$(call add_intermediate, add_pd_fw, $(objutil)/ec_usb_pd_fw/ec_usb_pd_fw)
	$(CBFSTOOL) $(obj)/coreboot.pre write -r PD_FW -f $(CONFIG_CRATER_PD_FW_FILE) --fill-upward
	$(objutil)/ec_usb_pd_fw/ec_usb_pd_fw $(obj)/coreboot.pre GEN_PDFW_PTR -n -f PD_FW
endif

else # CONFIG_CRATER_HAVE_MCHP_FW
show_notices:: warn_no_mchp
endif

PHONY+=warn_no_mchp
warn_no_mchp:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without the EC FW.\n"
	printf "Do not flash this image.  Your Crater's power button\n"
	printf "will not respond when you press it.\n\n"
