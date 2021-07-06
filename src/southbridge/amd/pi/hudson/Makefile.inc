#*****************************************************************************
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

# ROMSIG At ROMBASE + 0x20000:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# |PSPDIR ADDR|
# +-----------+
#
# EC ROM should be 64K aligned.

ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)
HUDSON_FWM_POSITION=$(call int-add, $(call int-subtract, 0xffffffff $(CONFIG_ROM_SIZE)) 0x20000 1)
else
HUDSON_FWM_POSITION=0xfff20000
endif

ifeq ($(CONFIG_HUDSON_PSP), y)

ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
FIRMWARE_LOCATION=$(shell grep -e FIRMWARE_LOCATION $(CONFIG_AMDFW_CONFIG_FILE) | awk '{print $$2}')
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

# Add all the files listed in the config file
POUND_SIGN=$(call strip_quotes, "\#")
DEP_FILES= $(patsubst %,$(FIRMWARE_LOCATION)/%, $(shell sed -e /^$(POUND_SIGN)/d -e /^FIRMWARE_LOCATION/d $(CONFIG_AMDFW_CONFIG_FILE) | awk '{print $$2}' ))

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
