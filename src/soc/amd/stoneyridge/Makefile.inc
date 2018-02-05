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
bootblock-y += BiosCallOuts.c
bootblock-y += bootblock/bootblock.c
bootblock-y += gpio.c
bootblock-y += i2c.c
bootblock-y += monotonic_timer.c
bootblock-y += pmutil.c
bootblock-y += reset.c
bootblock-y += sb_util.c
bootblock-y += tsc_freq.c
bootblock-y += southbridge.c
bootblock-y += sb_util.c

romstage-y += BiosCallOuts.c
romstage-y += i2c.c
romstage-y += romstage.c
romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += enable_usbdebug.c
romstage-y += gpio.c
romstage-$(CONFIG_STONEYRIDGE_IMC_FWM) += imc.c
romstage-y += monotonic_timer.c
romstage-y += pmutil.c
romstage-y += reset.c
romstage-y += sb_util.c
romstage-y += smbus.c
romstage-y += smbus_spd.c
romstage-y += ramtop.c
romstage-$(CONFIG_STONEYRIDGE_UART) += uart.c
romstage-y += tsc_freq.c
romstage-y += southbridge.c

verstage-y += gpio.c
verstage-y += i2c.c
verstage-y += monotonic_timer.c
verstage-y += sb_util.c
verstage-y += pmutil.c
verstage-y += reset.c
verstage-$(CONFIG_STONEYRIDGE_UART) += uart.c
verstage-y += tsc_freq.c

postcar-y += monotonic_timer.c
postcar-$(CONFIG_STONEYRIDGE_UART) += uart.c
postcar-y += ramtop.c
postcar-y += sb_util.c

ramstage-y += BiosCallOuts.c
ramstage-y += i2c.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += gpio.c
ramstage-y += hda.c
ramstage-y += monotonic_timer.c
ramstage-y += southbridge.c
ramstage-y += sb_util.c
ramstage-$(CONFIG_STONEYRIDGE_IMC_FWM) += imc.c
ramstage-y += lpc.c
ramstage-y += northbridge.c
ramstage-y += pmutil.c
ramstage-y += reset.c
ramstage-y += sata.c
ramstage-y += sm.c
ramstage-y += smbus.c
ramstage-y += ramtop.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c
ramstage-$(CONFIG_STONEYRIDGE_UART) += uart.c
ramstage-y += usb.c
ramstage-y += tsc_freq.c
ramstage-$(CONFIG_SPI_FLASH) += spi.c
ramstage-y += finalize.c

smm-y += monotonic_timer.c
smm-y += smihandler.c
smm-y += smi_util.c
smm-y += sb_util.c
smm-y += tsc_freq.c
smm-$(CONFIG_DEBUG_SMI) += uart.c
smm-$(CONFIG_SPI_FLASH) += spi.c

CPPFLAGS_common += -I$(src)/soc/amd/stoneyridge
CPPFLAGS_common += -I$(src)/soc/amd/stoneyridge/include
CPPFLAGS_common += -I$(src)/soc/amd/stoneyridge/acpi

# ROMSIG Normally At ROMBASE + 0x20000
# Overridden by CONFIG_AMD_FWM_POSITION_INDEX
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# |PSPDIR ADDR|
# +-----------+
#
# EC ROM should be 64K aligned.
STONEYRIDGE_FWM_POSITION=$(call int-add, \
	$(call int-subtract, 0xffffffff \
	$(call int-shift-left, \
	0x80000 $(CONFIG_AMD_FWM_POSITION_INDEX))) 0x20000 1)

### 0
FIRMWARE_LOCATE=$(dir $(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)))
FIRMWARE_TYPE=ST

###5
PUBSIGNEDKEY_FILE=$(top)/$(FIRMWARE_LOCATE)/RtmPubSigned$(FIRMWARE_TYPE).key

###1
PSPBTLDR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspBootLoader_prod_$(FIRMWARE_TYPE).sbin

###3
PSPRCVR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspRecoveryBootLoader_prod_$(FIRMWARE_TYPE).sbin

###4
PSPNVRAM_FILE=$(top)/$(FIRMWARE_LOCATE)/PspNvram$(FIRMWARE_TYPE).bin

###8 - Check for SMU firmware named either *.sbin or *.csbin
###    TODO: Remove *.sbin section after the blobs repo is updated.
SMUFWM_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware_$(FIRMWARE_TYPE).csbin
SMUFWM_FN_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware_$(FIRMWARE_TYPE)_FN.csbin
ifeq ("$(wildcard $(SMUFWM_FILE))","")
SMUFWM_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware$(FIRMWARE_TYPE).sbin
SMUFWM_FN_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware$(FIRMWARE_TYPE)_FN.sbin
endif

###95
SMUSCS_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuScs$(FIRMWARE_TYPE).bin

###9
PSPSECUREDEBUG_FILE=$(top)/$(FIRMWARE_LOCATE)/PspSecureDebug$(FIRMWARE_TYPE).Key

ifeq ($(CONFIG_USE_PSPSCUREOS),y)
###2
PSPSCUREOS_FILE=$(top)/$(FIRMWARE_LOCATE)/PspSecureOs_prod_$(FIRMWARE_TYPE).csbin

###12
PSPTRUSTLETS_FILE=$(wildcard $(top)/$(FIRMWARE_LOCATE)/PspTrustlets*_prod_$(FIRMWARE_TYPE).cbin)

###13
TRUSTLETKEY_FILE=$(top)/$(FIRMWARE_LOCATE)/TrustletKey_prod_$(FIRMWARE_TYPE).sbin
endif

###18- Check for SMU firmware2 named either *.sbin or *.csbin
###    TODO: Remove *.sbin section after the blobs repo is updated.
SMUFIRMWARE2_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware2_prod_$(FIRMWARE_TYPE).csbin
SMUFIRMWARE2_FN_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware2_prod_$(FIRMWARE_TYPE)_FN.csbin
ifeq ("$(wildcard $(SMUFIRMWARE2_FILE))","")
SMUFIRMWARE2_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware2_prod_$(FIRMWARE_TYPE).sbin
SMUFIRMWARE2_FN_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware2_prod_$(FIRMWARE_TYPE)_FN.sbin
endif

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_STONEYRIDGE_XHCI_FWM_FILE=$(call add_opt_prefix, $(CONFIG_STONEYRIDGE_XHCI_FWM_FILE), --xhci)
OPT_STONEYRIDGE_IMC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_STONEYRIDGE_IMC_FWM_FILE), --imc)
OPT_STONEYRIDGE_GEC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_STONEYRIDGE_GEC_FWM_FILEddd), --gec)

OPT_2AMD_PUBKEY_FILE=$(call add_opt_prefix, $(CONFIG_AMD_PUBKEY_FILE), --pubkey2)
OPT_2PSPBTLDR_FILE=$(call add_opt_prefix, $(PSPBTLDR_FILE), --bootloader2)
OPT_2SMUFWM_FILE=$(call add_opt_prefix, $(SMUFWM_FILE), --smufirmware_2)
OPT_2PSPRCVR_FILE=$(call add_opt_prefix, $(PSPRCVR_FILE), --recovery2)
OPT_2PUBSIGNEDKEY_FILE=$(call add_opt_prefix, $(PUBSIGNEDKEY_FILE), --rtmpubkey2)
OPT_2PSPNVRAM_FILE=$(call add_opt_prefix, $(PSPNVRAM_FILE), --nvram2)
OPT_2PSPSECUREDEBUG_FILE=$(call add_opt_prefix, $(PSPSECUREDEBUG_FILE), --securedebug2)
ifeq ($(CONFIG_USE_PSPSCUREOS),y)
OPT_2PSPSCUREOS_FILE=$(call add_opt_prefix, $(PSPSCUREOS_FILE), --secureos2)
OPT_2PSPTRUSTLETS_FILE=$(call add_opt_prefix, $(PSPTRUSTLETS_FILE), --trustlets2)
OPT_2TRUSTLETKEY_FILE=$(call add_opt_prefix, $(TRUSTLETKEY_FILE), --trustletkey2)
endif
OPT_2SMUFIRMWARE2_FILE=$(call add_opt_prefix, $(SMUFIRMWARE2_FILE), --smufirmware2_2)
OPT_2SMUSCS_FILE=$(call add_opt_prefix, $(SMUSCS_FILE), --smuscs2)
OPT_2SMUFWM_FN_FILE=$(call add_opt_prefix, $(SMUFWM_FN_FILE), --smufnfirmware_2)
OPT_2SMUFIRMWARE2_FN_FILE=$(call add_opt_prefix, $(SMUFIRMWARE2_FN_FILE), --smufnfirmware2_2)


$(obj)/amdfw.rom:	$(call strip_quotes, $(CONFIG_STONEYRIDGE_XHCI_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_STONEYRIDGE_IMC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_STONEYRIDGE_GEC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)) \
			$(call strip_quotes, $(PUBSIGNEDKEY_FILE)) \
			$(call strip_quotes, $(PSPBTLDR_FILE)) \
			$(call strip_quotes, $(PSPRCVR_FILE)) \
			$(call strip_quotes, $(PSPSCUREOS_FILE)) \
			$(call strip_quotes, $(PSPNVRAM_FILE)) \
			$(call strip_quotes, $(SMUFWM_FILE)) \
			$(call strip_quotes, $(SMUFWM_FN_FILE)) \
			$(call strip_quotes, $(SMUSCS_FILE)) \
			$(call strip_quotes, $(PSPSECUREDEBUG_FILE)) \
			$(call strip_quotes, $(PSPTRUSTLETS_FILE)) \
			$(call strip_quotes, $(TRUSTLETKEY_FILE)) \
			$(call strip_quotes, $(SMUFIRMWARE2_FILE)) \
			$(call strip_quotes, $(SMUFIRMWARE2_FN_FILE)) \
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
		--location $(shell printf "0x%x" $(STONEYRIDGE_FWM_POSITION)) \
		--output	$@

ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)
PHONY+=add_amdfw
INTERMEDIATE+=add_amdfw

# Calculate firmware position inside the ROM
STONEYRIDGE_FWM_ROM_POSITION=$(call int-add, \
	$(call int-subtract, $(CONFIG_ROM_SIZE) \
	$(call int-shift-left, \
	0x80000 $(CONFIG_AMD_FWM_POSITION_INDEX))) 0x20000)

add_amdfw: $(obj)/coreboot.pre $(obj)/amdfw.rom
	printf "    DD         Adding AMD Firmware at ROM offset 0x%x\n" \
		"$(STONEYRIDGE_FWM_ROM_POSITION)"
	dd if=$(obj)/amdfw.rom \
		of=$(obj)/coreboot.pre conv=notrunc bs=1 \
		seek=$(STONEYRIDGE_FWM_ROM_POSITION) >/dev/null 2>&1

else # ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)

cbfs-files-y += apu/amdfw
apu/amdfw-file := $(obj)/amdfw.rom
apu/amdfw-position := $(STONEYRIDGE_FWM_POSITION)
apu/amdfw-type := raw

endif # ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)

ifeq ($(CONFIG_SOC_AMD_PSP_SELECTABLE_SMU_FW),y)

cbfs-files-y += smu_fw
cbfs-files-y += smu_fw2
smu_fw-type := raw
smu_fw2-type := raw

ifeq ($(CONFIG_SOC_AMD_SMU_FANLESS),y)
smu_fw-file := $(SMUFWM_FN_FILE)
smu_fw2-file := $(SMUFIRMWARE2_FN_FILE)
else ifeq ($(CONFIG_SOC_AMD_SMU_FANNED),y)
smu_fw-file := $(SMUFWM_FILE)
smu_fw2-file := $(SMUFIRMWARE2_FILE)
else
$(error "Proper SMU Firmware not selected")
endif

endif # ifeq ($(CONFIG_SOC_AMD_PSP_SELECTABLE_SMU_FW),y)

endif # ($(CONFIG_SOC_AMD_STONEYRIDGE_FP4)$(CONFIG_SOC_AMD_STONEYRIDGE_FT4),y)
