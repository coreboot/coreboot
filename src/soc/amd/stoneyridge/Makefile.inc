#*****************************************************************************
#
# Copyright (c) 2012, 2016-2017 Advanced Micro Devices, Inc.
#               2013 - 2014 Sage Electronic Engineering, LLC
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Advanced Micro Devices, Inc. nor the names of
#       its contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************
ifeq ($(CONFIG_SOC_AMD_STONEYRIDGE_FP4)$(CONFIG_SOC_AMD_STONEYRIDGE_FT4),y)

subdirs-y += ../../../cpu/amd/mtrr/
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/cache
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/pae
subdirs-y += ../../../cpu/x86/smm

bootblock-$(CONFIG_STONEYRIDGE_UART) += uart.c
bootblock-y += fixme.c
bootblock-y += bootblock/bootblock.c
bootblock-y += early_setup.c
bootblock-y += tsc_freq.c

romstage-y += romstage.c
romstage-y += early_setup.c
romstage-y += dimmSpd.c
romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += enable_usbdebug.c
romstage-y += fixme.c
romstage-$(CONFIG_STONEYRIDGE_IMC_FWM) += imc.c
romstage-y += smbus.c
romstage-y += smbus_spd.c
romstage-y += ramtop.c
romstage-$(CONFIG_STONEYRIDGE_UART) += uart.c
romstage-y += tsc_freq.c

ramstage-y += chip.c
ramstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c
ramstage-y += fixme.c
ramstage-y += gpio.c
ramstage-y += hda.c
ramstage-y += hudson.c
ramstage-$(CONFIG_STONEYRIDGE_IMC_FWM) += imc.c
ramstage-y += lpc.c
ramstage-y += model_15_init.c
ramstage-y += northbridge.c
ramstage-y += reset.c
ramstage-y += sata.c
ramstage-y += sd.c
ramstage-y += sm.c
ramstage-y += smbus.c
ramstage-y += ramtop.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c
ramstage-$(CONFIG_STONEYRIDGE_UART) += uart.c
ramstage-y += usb.c
ramstage-y += tsc_freq.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += tsc_freq.c

CPPFLAGS_common += -I$(src)/soc/amd/stoneyridge
CPPFLAGS_common += -I$(src)/soc/amd/stoneyridge/include
CPPFLAGS_common += -I$(src)/soc/amd/stoneyridge/acpi

# ROMSIG At ROMBASE + 0x20000:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# |PSPDIR ADDR|
# +-----------+
#
# EC ROM should be 64K aligned.
STONEYRIDGE_FWM_POSITION=$(call int-add, $(call int-subtract, 0xffffffff $(CONFIG_ROM_SIZE)) 0x20000 1)

### 0
FIRMWARE_LOCATE=$(dir $(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)))
FIRMWARE_TYPE=ST

###5
CONFIG_PUBSIGNEDKEY_FILE=$(top)/$(FIRMWARE_LOCATE)/RtmPubSigned$(FIRMWARE_TYPE).key

###1
CONFIG_PSPBTLDR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspBootLoader_prod_$(FIRMWARE_TYPE).sbin

###3
CONFIG_PSPRCVR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspRecoveryBootLoader_prod_$(FIRMWARE_TYPE).sbin

###2
CONFIG_PSPSCUREOS_FILE=$(top)/$(FIRMWARE_LOCATE)/PspSecureOs_prod_$(FIRMWARE_TYPE).csbin

###4
CONFIG_PSPNVRAM_FILE=$(top)/$(FIRMWARE_LOCATE)/PspNvram$(FIRMWARE_TYPE).bin

###8
CONFIG_SMUFWM_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware$(FIRMWARE_TYPE).sbin
CONFIG_SMUFWM_FN_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware$(FIRMWARE_TYPE)_FN.sbin

###95
CONFIG_SMUSCS_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuScs$(FIRMWARE_TYPE).bin

###9
CONFIG_PSPSECUREDEBUG_FILE=$(top)/$(FIRMWARE_LOCATE)/PspSecureDebug$(FIRMWARE_TYPE).Key

###12
CONFIG_PSPTRUSTLETS_FILE=$(top)/$(FIRMWARE_LOCATE)/PspTrustlets_prod_$(FIRMWARE_TYPE).cbin

###13
CONFIG_TRUSTLETKEY_FILE=$(top)/$(FIRMWARE_LOCATE)/TrustletKey_prod_$(FIRMWARE_TYPE).sbin

###18
CONFIG_SMUFIRMWARE2_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware2_prod_$(FIRMWARE_TYPE).sbin
CONFIG_SMUFIRMWARE2_FN_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware2_prod_$(FIRMWARE_TYPE)_FN.sbin

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_STONEYRIDGE_XHCI_FWM_FILE=$(call add_opt_prefix, $(CONFIG_STONEYRIDGE_XHCI_FWM_FILE), --xhci)
OPT_STONEYRIDGE_IMC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_STONEYRIDGE_IMC_FWM_FILE), --imc)
OPT_STONEYRIDGE_GEC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_STONEYRIDGE_GEC_FWM_FILEddd), --gec)

CONFIG_2AMD_PUBKEY_FILE      =$(CONFIG_AMD_PUBKEY_FILE)
CONFIG_2PSPBTLDR_FILE        =$(CONFIG_PSPBTLDR_FILE)
CONFIG_2SMUFWM_FILE          =$(CONFIG_SMUFWM_FILE)
CONFIG_2SMUFWM_FN_FILE       =$(CONFIG_SMUFWM_FN_FILE)
CONFIG_2PSPRCVR_FILE         =$(CONFIG_PSPRCVR_FILE)
CONFIG_2PUBSIGNEDKEY_FILE    =$(CONFIG_PUBSIGNEDKEY_FILE)
CONFIG_2PSPSCUREOS_FILE      =$(CONFIG_PSPSCUREOS_FILE)
CONFIG_2PSPNVRAM_FILE        =$(CONFIG_PSPNVRAM_FILE)
CONFIG_2PSPSECUREDEBUG_FILE  =$(CONFIG_PSPSECUREDEBUG_FILE)
CONFIG_2PSPTRUSTLETS_FILE    =$(CONFIG_PSPTRUSTLETS_FILE)
CONFIG_2TRUSTLETKEY_FILE     =$(CONFIG_TRUSTLETKEY_FILE)
CONFIG_2SMUFIRMWARE2_FILE    =$(CONFIG_SMUFIRMWARE2_FILE)
CONFIG_2SMUFIRMWARE2_FN_FILE =$(CONFIG_SMUFIRMWARE2_FN_FILE)
CONFIG_2SMUSCS_FILE          =$(CONFIG_SMUSCS_FILE)

OPT_2AMD_PUBKEY_FILE=$(call add_opt_prefix, $(CONFIG_2AMD_PUBKEY_FILE), --pubkey2)
OPT_2PSPBTLDR_FILE=$(call add_opt_prefix, $(CONFIG_2PSPBTLDR_FILE), --bootloader2)
OPT_2SMUFWM_FILE=$(call add_opt_prefix, $(CONFIG_2SMUFWM_FILE), --smufirmware_2)
OPT_2PSPRCVR_FILE=$(call add_opt_prefix, $(CONFIG_2PSPRCVR_FILE), --recovery2)
OPT_2PUBSIGNEDKEY_FILE=$(call add_opt_prefix, $(CONFIG_2PUBSIGNEDKEY_FILE), --rtmpubkey2)
OPT_2PSPSCUREOS_FILE=$(call add_opt_prefix, $(CONFIG_2PSPSCUREOS_FILE), --secureos2)
OPT_2PSPNVRAM_FILE=$(call add_opt_prefix, $(CONFIG_2PSPNVRAM_FILE), --nvram2)
OPT_2PSPSECUREDEBUG_FILE=$(call add_opt_prefix, $(CONFIG_2PSPSECUREDEBUG_FILE), --securedebug2)
OPT_2PSPTRUSTLETS_FILE=$(call add_opt_prefix, $(CONFIG_2PSPTRUSTLETS_FILE), --trustlets2)
OPT_2TRUSTLETKEY_FILE=$(call add_opt_prefix, $(CONFIG_2TRUSTLETKEY_FILE), --trustletkey2)
OPT_2SMUFIRMWARE2_FILE=$(call add_opt_prefix, $(CONFIG_2SMUFIRMWARE2_FILE), --smufirmware2_2)
OPT_2SMUSCS_FILE=$(call add_opt_prefix, $(CONFIG_2SMUSCS_FILE), --smuscs2)
OPT_2SMUFWM_FN_FILE=$(call add_opt_prefix, $(CONFIG_2SMUFWM_FN_FILE), --smufnfirmware_2)
OPT_2SMUFIRMWARE2_FN_FILE=$(call add_opt_prefix, $(CONFIG_2SMUFIRMWARE2_FN_FILE), --smufnfirmware2_2)


$(obj)/amdfw.rom:	$(call strip_quotes, $(CONFIG_STONEYRIDGE_XHCI_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_STONEYRIDGE_IMC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_STONEYRIDGE_GEC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)) \
			$(call strip_quotes, $(CONFIG_PUBSIGNEDKEY_FILE)) \
			$(call strip_quotes, $(CONFIG_PSPBTLDR_FILE)) \
			$(call strip_quotes, $(CONFIG_PSPRCVR_FILE)) \
			$(call strip_quotes, $(CONFIG_PSPSCUREOS_FILE)) \
			$(call strip_quotes, $(CONFIG_PSPNVRAM_FILE)) \
			$(call strip_quotes, $(CONFIG_SMUFWM_FILE)) \
			$(call strip_quotes, $(CONFIG_SMUSCS_FILE)) \
			$(call strip_quotes, $(CONFIG_PSPSECUREDEBUG_FILE)) \
			$(call strip_quotes, $(CONFIG_PSPTRUSTLETS_FILE)) \
			$(call strip_quotes, $(CONFIG_TRUSTLETKEY_FILE)) \
			$(call strip_quotes, $(CONFIG_SMUFIRMWARE2_FILE)) \
			$(call strip_quotes, $(CONFIG_2AMD_PUBKEY_FILE)) \
			$(call strip_quotes, $(CONFIG_2PUBSIGNEDKEY_FILE)) \
			$(call strip_quotes, $(CONFIG_2PSPBTLDR_FILE)) \
			$(call strip_quotes, $(CONFIG_2SMUFWM_FILE)) \
			$(call strip_quotes, $(CONFIG_2SMUFWM_FN_FILE)) \
			$(call strip_quotes, $(CONFIG_2PSPRCVR_FILE)) \
			$(call strip_quotes, $(CONFIG_2PSPSCUREOS_FILE)) \
			$(call strip_quotes, $(CONFIG_2PSPNVRAM_FILE)) \
			$(call strip_quotes, $(CONFIG_2SMUSCS_FILE)) \
			$(call strip_quotes, $(CONFIG_2PSPSECUREDEBUG_FILE)) \
			$(call strip_quotes, $(CONFIG_2PSPTRUSTLETS_FILE)) \
			$(call strip_quotes, $(CONFIG_2TRUSTLETKEY_FILE)) \
			$(call strip_quotes, $(CONFIG_2SMUFIRMWARE2_FILE)) \
			$(call strip_quotes, $(CONFIG_2SMUFIRMWARE2_FN_FILE)) \
			$(AMDFWTOOL)
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(OPT_STONEYRIDGE_XHCI_FWM_FILE) \
		$(OPT_STONEYRIDGE_IMC_FWM_FILE) \
		$(OPT_STONEYRIDGE_GEC_FWM_FILE) \
		$(OPT_AMD_PUBKEY_FILE) \
		$(OPT_PSPBTLDR_FILE) \
		$(OPT_SMUFWM_FILE) \
		$(OPT_PSPRCVR_FILE) \
		$(OPT_PUBSIGNEDKEY_FILE) \
		$(OPT_PSPSCUREOS_FILE) \
		$(OPT_PSPNVRAM_FILE) \
		$(OPT_PSPSECUREDEBUG_FILE) \
		$(OPT_PSPTRUSTLETS_FILE) \
		$(OPT_TRUSTLETKEY_FILE) \
		$(OPT_SMUFIRMWARE2_FILE) \
		$(OPT_SMUSCS_FILE) \
		$(OPT_2AMD_PUBKEY_FILE) \
		$(OPT_2PSPBTLDR_FILE) \
		$(OPT_2SMUFWM_FILE) \
		$(OPT_2SMUFWM_FN_FILE) \
		$(OPT_2PSPRCVR_FILE) \
		$(OPT_2PUBSIGNEDKEY_FILE) \
		$(OPT_2PSPSCUREOS_FILE) \
		$(OPT_2PSPNVRAM_FILE) \
		$(OPT_2PSPSECUREDEBUG_FILE) \
		$(OPT_2PSPTRUSTLETS_FILE) \
		$(OPT_2TRUSTLETKEY_FILE) \
		$(OPT_2SMUFIRMWARE2_FILE) \
		$(OPT_2SMUFIRMWARE2_FN_FILE) \
		$(OPT_2SMUSCS_FILE) \
		--flashsize $(CONFIG_ROM_SIZE) \
		--output	$@

ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)
PHONY+=add_amdfw
INTERMEDIATE+=add_amdfw

add_amdfw: $(obj)/coreboot.pre $(obj)/amdfw.rom
	printf "    DD         Adding AMD Firmware\n"
	dd if=$(obj)/amdfw.rom \
		of=$(obj)/coreboot.pre conv=notrunc bs=1 seek=131072 >/dev/null 2>&1

else # ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)

cbfs-files-y += apu/amdfw
apu/amdfw-file := $(obj)/amdfw.rom
apu/amdfw-position := $(STONEYRIDGE_FWM_POSITION)
apu/amdfw-type := raw

endif # ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)

endif # ($(CONFIG_SOC_AMD_STONEYRIDGE_FP4)$(CONFIG_SOC_AMD_STONEYRIDGE_FT4),y)
