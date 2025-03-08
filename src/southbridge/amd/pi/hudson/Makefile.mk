## SPDX-License-Identifier: BSD-3-Clause

# Copyright (c) 2012,        Advanced Micro Devices, Inc.
#               2013 - 2014, Sage Electronic Engineering, LLC

bootblock-y += bootblock.c
bootblock-y += early_setup.c
bootblock-$(CONFIG_USBDEBUG) += enable_usbdebug.c

romstage-y += early_setup.c
romstage-y += enable_usbdebug.c
romstage-$(CONFIG_HUDSON_IMC_FWM) += imc.c
romstage-y += smbus.c
romstage-y += smbus_spd.c

verstage-y += early_setup.c

ramstage-y += enable_usbdebug.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c
ramstage-y += hda.c
ramstage-y += hudson.c
ramstage-y += ide.c
ramstage-$(CONFIG_HUDSON_IMC_FWM) += imc.c
ramstage-y += lpc.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sata.c
ramstage-y += sd.c
ramstage-y += sm.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c
ramstage-y += usb.c

all-y += reset.c

smm-y += smihandler.c
smm-y += smi_util.c

CPPFLAGS_common += -I$(src)/southbridge/amd/pi/hudson/include

# ROMSIG At ROMBASE + 0x20000:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# |PSPDIR ADDR|
# +-----------+
#
# EC ROM should be 64K aligned.

ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)
HUDSON_FWM_POSITION=0x20000
else
HUDSON_FWM_POSITION=0x720000
endif

ifeq ($(CONFIG_HUDSON_PSP), y)

ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
FIRMWARE_TYPE=

endif

#PUBSIGNEDKEY_FILE=$(top)/$(FIRMWARE_LOCATION)/RtmPubSigned$(FIRMWARE_TYPE).key
#PSPNVRAM_FILE=$(top)/$(FIRMWARE_LOCATION)/PspNvram$(FIRMWARE_TYPE).bin
#PSPSECUREDEBUG_FILE=$(top)/$(FIRMWARE_LOCATION)/PspSecureDebug$(FIRMWARE_TYPE).Key

endif

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_HUDSON_XHCI_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_XHCI_FWM_FILE), --xhci)
OPT_HUDSON_IMC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_IMC_FWM_FILE), --imc)
OPT_HUDSON_GEC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_GEC_FWM_FILEddd), --gec)

OPT_AMD_PUBKEY_FILE=$(call add_opt_prefix, $(CONFIG_AMD_PUBKEY_FILE), --pubkey)
OPT_PSPBTLDR_FILE=$(call add_opt_prefix, $(PSPBTLDR_FILE), --bootloader)
OPT_SMUFWM_FILE=$(call add_opt_prefix, $(SMUFWM_FILE), --smufirmware)
OPT_PSPRCVR_FILE=$(call add_opt_prefix, $(PSPRCVR_FILE), --recovery)
OPT_PUBSIGNEDKEY_FILE=$(call add_opt_prefix, $(PUBSIGNEDKEY_FILE), --rtmpubkey)
OPT_PSPSECUREOS_FILE=$(call add_opt_prefix, $(PSPSECUREOS_FILE), --secureos)
OPT_PSPNVRAM_FILE=$(call add_opt_prefix, $(PSPNVRAM_FILE), --nvram)
OPT_PSPSECUREDEBUG_FILE=$(call add_opt_prefix, $(PSPSECUREDEBUG_FILE), --securedebug)
OPT_PSPTRUSTLETS_FILE=$(call add_opt_prefix, $(PSPTRUSTLETS_FILE), --trustlets)
OPT_TRUSTLETKEY_FILE=$(call add_opt_prefix, $(TRUSTLETKEY_FILE), --trustletkey)
OPT_SMUFIRMWARE2_FILE=$(call add_opt_prefix, $(SMUFIRMWARE2_FILE), --smufirmware2)
OPT_SMUSCS_FILE=$(call add_opt_prefix, $(SMUSCS_FILE), --smuscs)

$(obj)/amdfw.rom:	$(call strip_quotes, $(CONFIG_HUDSON_XHCI_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_IMC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_GEC_FWM_FILE)) \
			$(call strip_quotes, $(AMD_PUBKEY2_FILE)) \
			$(call strip_quotes, $(PUBSIGNEDKEY2_FILE)) \
			$(call strip_quotes, $(PSPBTLDR2_FILE)) \
			$(call strip_quotes, $(SMUFWM2_FILE)) \
			$(call strip_quotes, $(SMUFWM2_FN_FILE)) \
			$(call strip_quotes, $(PSPRCVR2_FILE)) \
			$(call strip_quotes, $(PSPSECUREOS2_FILE)) \
			$(call strip_quotes, $(PSPNVRAM2_FILE)) \
			$(call strip_quotes, $(SMUSCS2_FILE)) \
			$(call strip_quotes, $(PSPSECUREDEBUG2_FILE)) \
			$(call strip_quotes, $(PSPTRUSTLETS2_FILE)) \
			$(call strip_quotes, $(TRUSTLETKEY2_FILE)) \
			$(call strip_quotes, $(SMUFIRMWARE2_2_FILE)) \
			$(call strip_quotes, $(SMUFIRMWARE2_2_FN_FILE)) \
			$(DEP_FILES) \
			$(AMDFWTOOL)
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(OPT_HUDSON_XHCI_FWM_FILE) \
		$(OPT_HUDSON_IMC_FWM_FILE) \
		$(OPT_HUDSON_GEC_FWM_FILE) \
		$(OPT_2AMD_PUBKEY_FILE) \
		$(OPT_2PSPBTLDR_FILE) \
		$(OPT_2SMUFWM_FILE) \
		$(OPT_2SMUFWM_FN_FILE) \
		$(OPT_2PSPRCVR_FILE) \
		$(OPT_2PUBSIGNEDKEY_FILE) \
		$(OPT_2PSPSECUREOS_FILE) \
		$(OPT_2PSPNVRAM_FILE) \
		$(OPT_2PSPSECUREDEBUG_FILE) \
		$(OPT_2PSPTRUSTLETS_FILE) \
		$(OPT_2TRUSTLETKEY_FILE) \
		$(OPT_2SMUFIRMWARE2_FILE) \
		$(OPT_2SMUFIRMWARE2_FN_FILE) \
		$(OPT_2SMUSCS_FILE) \
		--flashsize $(CONFIG_ROM_SIZE) \
		--location $(HUDSON_FWM_POSITION) \
		--config $(CONFIG_AMDFW_CONFIG_FILE) \
		--output	$@

ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)
$(call add_intermediate, add_amdfw, $(obj)/amdfw.rom)
	printf "    DD         Adding AMD Firmware\n"
	dd if=$(obj)/amdfw.rom \
		of=$< conv=notrunc bs=1 seek=131072 >/dev/null 2>&1

else # ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)

cbfs-files-y += apu/amdfw
apu/amdfw-file := $(obj)/amdfw.rom
apu/amdfw-position := $(HUDSON_FWM_POSITION)
apu/amdfw-type := raw

endif # ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)
