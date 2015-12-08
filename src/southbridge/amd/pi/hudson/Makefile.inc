#*****************************************************************************
#
# Copyright (c) 2012,        Advanced Micro Devices, Inc.
#               2013 - 2014, Sage Electronic Engineering, LLC
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

romstage-y += smbus.c smbus_spd.c
ramstage-y += hudson.c
ramstage-y += usb.c
ramstage-y += lpc.c
ramstage-y += sm.c
ramstage-y += ide.c
ramstage-y += sata.c
ramstage-y += hda.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sd.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c
ramstage-y += reset.c
romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += enable_usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c
romstage-y += early_setup.c
ifeq ($(CONFIG_HUDSON_IMC_FWM), y)
romstage-y += imc.c
ramstage-y += imc.c
endif

ifeq ($(CONFIG_HUDSON_UART), y)
romstage-y += uart.c
ramstage-y += uart.c
endif

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c smi_util.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c smi_util.c

# ROMSIG At ROMBASE + 0x20000:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# |PSPDIR ADDR|
# +-----------+
#
# EC ROM should be 64K aligned.

HUDSON_FWM_POS_CALC=$(call int-add, $(call int-subtract, 0xffffffff $(call int-multiply, $(CONFIG_COREBOOT_ROMSIZE_KB) 1024)) 0x20000 1)
HUDSON_FWM_POSITION=$(shell printf %u $(CONFIG_HUDSON_FWM_POSITION))
HUDSON_FWM_INSIDE_CBFS=$(call int-lt, $(CBFS_BASE_ADDRESS) $(HUDSON_FWM_POSITION))

##
ifeq ($(CONFIG_HUDSON_PSP), y)
##
### 0

##
ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
FIRMWARE_LOCATE=$(dir $(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)))
FIRMWARE_TYPE=
endif
##
ifeq ($(CONFIG_CPU_AMD_PI_00660F01), y)
FIRMWARE_LOCATE=$(dir $(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)))
FIRMWARE_TYPE=CZ
endif

###5
CONFIG_PUBSIGNEDKEY_FILE=$(top)/$(FIRMWARE_LOCATE)/RtmPubSigned$(FIRMWARE_TYPE).key
###1
ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
CONFIG_PSPBTLDR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspBootLoader$(FIRMWARE_TYPE).Bypass.sbin
else ifeq ($(CONFIG_CPU_AMD_PI_00660F01), y)
CONFIG_PSPBTLDR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspBootLoader_prod_$(FIRMWARE_TYPE).sbin
endif

###3
ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
CONFIG_PSPRCVR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspRecovery$(FIRMWARE_TYPE).sbin
else ifeq ($(CONFIG_CPU_AMD_PI_00660F01), y)
CONFIG_PSPRCVR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspRecoveryBootLoader_prod_$(FIRMWARE_TYPE).sbin
endif

###2
ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
CONFIG_PSPSCUREOS_FILE=$(top)/$(FIRMWARE_LOCATE)/PspSecureOs$(FIRMWARE_TYPE).sbin
else ifeq ($(CONFIG_CPU_AMD_PI_00660F01), y)
CONFIG_PSPSCUREOS_FILE=$(top)/$(FIRMWARE_LOCATE)/PspSecureOs_prod_$(FIRMWARE_TYPE).csbin
endif

###4
CONFIG_PSPNVRAM_FILE=$(top)/$(FIRMWARE_LOCATE)/PspNvram$(FIRMWARE_TYPE).bin

###8
CONFIG_SMUFWM_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware$(FIRMWARE_TYPE).sbin

###95
CONFIG_SMUSCS_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuScs$(FIRMWARE_TYPE).bin

###9
CONFIG_PSPSECUREDEBUG_FILE=$(top)/$(FIRMWARE_LOCATE)/PspSecureDebug$(FIRMWARE_TYPE).Key

###12
ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
CONFIG_PSPTRUSTLETS_FILE=$(top)/$(FIRMWARE_LOCATE)/trustlets.bin
else ifeq ($(CONFIG_CPU_AMD_PI_00660F01), y)
CONFIG_PSPTRUSTLETS_FILE=$(top)/$(FIRMWARE_LOCATE)/PspTrustlets_prod_$(FIRMWARE_TYPE).cbin
endif

###13
ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
CONFIG_TRUSTLETKEY_FILE=$(top)/$(FIRMWARE_LOCATE)/Trustlet.tkn.cert
else ifeq ($(CONFIG_CPU_AMD_PI_00660F01), y)
CONFIG_TRUSTLETKEY_FILE=$(top)/$(FIRMWARE_LOCATE)/TrustletKey_prod_$(FIRMWARE_TYPE).sbin
endif

###18
ifeq ($(CONFIG_CPU_AMD_PI_00660F01), y)
CONFIG_SMUFIRMWARE2_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware2_prod_$(FIRMWARE_TYPE).sbin
endif

endif

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_HUDSON_XHCI_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_XHCI_FWM_FILE), --xhci)
OPT_HUDSON_IMC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_IMC_FWM_FILE), --imc)
OPT_HUDSON_GEC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_GEC_FWM_FILEddd), --gec)

OPT_AMD_PUBKEY_FILE=$(call add_opt_prefix, $(CONFIG_AMD_PUBKEY_FILE), --pubkey)
OPT_PSPBTLDR_FILE=$(call add_opt_prefix, $(CONFIG_PSPBTLDR_FILE), --bootloader)
OPT_SMUFWM_FILE=$(call add_opt_prefix, $(CONFIG_SMUFWM_FILE), --smufirmware)
OPT_PSPRCVR_FILE=$(call add_opt_prefix, $(CONFIG_PSPRCVR_FILE), --recovery)
OPT_PUBSIGNEDKEY_FILE=$(call add_opt_prefix, $(CONFIG_PUBSIGNEDKEY_FILE), --rtmpubkey)
OPT_PSPSCUREOS_FILE=$(call add_opt_prefix, $(CONFIG_PSPSCUREOS_FILE), --secureos)
OPT_PSPNVRAM_FILE=$(call add_opt_prefix, $(CONFIG_PSPNVRAM_FILE), --nvram)
OPT_PSPSECUREDEBUG_FILE=$(call add_opt_prefix, $(CONFIG_PSPSECUREDEBUG_FILE), --securedebug)
OPT_PSPTRUSTLETS_FILE=$(call add_opt_prefix, $(CONFIG_PSPTRUSTLETS_FILE), --trustlets)
OPT_TRUSTLETKEY_FILE=$(call add_opt_prefix, $(CONFIG_TRUSTLETKEY_FILE), --trustletkey)
OPT_SMUFIRMWARE2_FILE=$(call add_opt_prefix, $(CONFIG_SMUFIRMWARE2_FILE), --smufirmware2)
OPT_SMUSCS_FILE=$(call add_opt_prefix, $(CONFIG_SMUSCS_FILE), --smuscs)

#TODO: Set PSP2 Firmware. The setting below is just for testing.
#CONFIG_2AMD_PUBKEY_FILE     =$(CONFIG_AMD_PUBKEY_FILE)
#CONFIG_2PSPBTLDR_FILE	    =$(CONFIG_PSPBTLDR_FILE)
#CONFIG_2SMUFWM_FILE	    =$(CONFIG_SMUFWM_FILE)
#CONFIG_2PSPRCVR_FILE	    =$(CONFIG_PSPRCVR_FILE)
#CONFIG_2PUBSIGNEDKEY_FILE   =$(CONFIG_PUBSIGNEDKEY_FILE)
#CONFIG_2PSPSCUREOS_FILE	    =$(CONFIG_PSPSCUREOS_FILE)
#CONFIG_2PSPNVRAM_FILE	    =$(CONFIG_PSPNVRAM_FILE)
#CONFIG_2PSPSECUREDEBUG_FILE =$(CONFIG_PSPSECUREDEBUG_FILE)
#CONFIG_2PSPTRUSTLETS_FILE   =$(CONFIG_PSPTRUSTLETS_FILE)
#CONFIG_2TRUSTLETKEY_FILE    =$(CONFIG_TRUSTLETKEY_FILE)
#CONFIG_2SMUFIRMWARE2_FILE   =$(CONFIG_SMUFIRMWARE2_FILE)
#CONFIG_2SMUSCS_FILE	    =$(CONFIG_SMUSCS_FILE)

OPT_2AMD_PUBKEY_FILE=$(call add_opt_prefix, $(CONFIG_2AMD_PUBKEY_FILE), --pubkey2)
OPT_2PSPBTLDR_FILE=$(call add_opt_prefix, $(CONFIG_2PSPBTLDR_FILE), --bootloader2)
OPT_2SMUFWM_FILE=$(call add_opt_prefix, $(CONFIG_2SMUFWM_FILE), --smufirmware2)
OPT_2PSPRCVR_FILE=$(call add_opt_prefix, $(CONFIG_2PSPRCVR_FILE), --recovery2)
OPT_2PUBSIGNEDKEY_FILE=$(call add_opt_prefix, $(CONFIG_2PUBSIGNEDKEY_FILE), --rtmpubkey2)
OPT_2PSPSCUREOS_FILE=$(call add_opt_prefix, $(CONFIG_2PSPSCUREOS_FILE), --secureos2)
OPT_2PSPNVRAM_FILE=$(call add_opt_prefix, $(CONFIG_2PSPNVRAM_FILE), --nvram2)
OPT_2PSPSECUREDEBUG_FILE=$(call add_opt_prefix, $(CONFIG_2PSPSECUREDEBUG_FILE), --securedebug2)
OPT_2PSPTRUSTLETS_FILE=$(call add_opt_prefix, $(CONFIG_2PSPTRUSTLETS_FILE), --trustlets2)
OPT_2TRUSTLETKEY_FILE=$(call add_opt_prefix, $(CONFIG_2TRUSTLETKEY_FILE), --trustletkey2)
OPT_2SMUFIRMWARE2_FILE=$(call add_opt_prefix, $(CONFIG_2SMUFIRMWARE2_FILE), --smufirmware2_2)
OPT_2SMUSCS_FILE=$(call add_opt_prefix, $(CONFIG_2SMUSCS_FILE), --smuscs2)

$(obj)/amdfw.rom:	$(call strip_quotes, $(CONFIG_HUDSON_XHCI_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_IMC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_GEC_FWM_FILE)) \
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
			$(AMDFWTOOL)
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(OPT_HUDSON_XHCI_FWM_FILE) \
		$(OPT_HUDSON_IMC_FWM_FILE) \
		$(OPT_HUDSON_GEC_FWM_FILE) \
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
		$(OPT_2PSPRCVR_FILE) \
		$(OPT_2PUBSIGNEDKEY_FILE) \
		$(OPT_2PSPSCUREOS_FILE) \
		$(OPT_2PSPNVRAM_FILE) \
		$(OPT_2PSPSECUREDEBUG_FILE) \
		$(OPT_2PSPTRUSTLETS_FILE) \
		$(OPT_2TRUSTLETKEY_FILE) \
		$(OPT_2SMUFIRMWARE2_FILE) \
		$(OPT_2SMUSCS_FILE) \
		--output	$@

cbfs-files-y += apu/amdfw
apu/amdfw-file := $(obj)/amdfw.rom
apu/amdfw-position := $(HUDSON_FWM_POSITION)
apu/amdfw-type := raw
