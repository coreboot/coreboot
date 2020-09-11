# SPDX-License-Identifier: BSD-3-Clause

ifeq ($(CONFIG_SOC_AMD_STONEYRIDGE),y)

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
bootblock-y += enable_usbdebug.c
bootblock-y += monotonic_timer.c
bootblock-y += pmutil.c
bootblock-y += tsc_freq.c
bootblock-y += southbridge.c
bootblock-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c

romstage-y += BiosCallOuts.c
romstage-y += i2c.c
romstage-y += romstage.c
romstage-y += enable_usbdebug.c
romstage-y += gpio.c
romstage-y += monotonic_timer.c
romstage-y += pmutil.c
romstage-y += smbus_spd.c
romstage-y += memmap.c
romstage-$(CONFIG_STONEYRIDGE_UART) += uart.c
romstage-y += tsc_freq.c
romstage-y += southbridge.c
romstage-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c
romstage-y += psp.c

verstage-y += gpio.c
verstage-y += i2c.c
verstage-y += monotonic_timer.c
verstage-y += pmutil.c
verstage-$(CONFIG_STONEYRIDGE_UART) += uart.c
verstage-y += tsc_freq.c

postcar-y += monotonic_timer.c
postcar-$(CONFIG_STONEYRIDGE_UART) += uart.c
postcar-y += memmap.c
postcar-$(CONFIG_VBOOT_MEASURED_BOOT) += i2c.c
postcar-y += tsc_freq.c

ramstage-y += BiosCallOuts.c
ramstage-y += i2c.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += mca.c
ramstage-y += enable_usbdebug.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += gpio.c
ramstage-y += monotonic_timer.c
ramstage-y += southbridge.c
ramstage-y += northbridge.c
ramstage-y += pmutil.c
ramstage-y += sata.c
ramstage-y += memmap.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c
ramstage-$(CONFIG_STONEYRIDGE_UART) += uart.c
ramstage-y += usb.c
ramstage-y += tsc_freq.c
ramstage-y += finalize.c
ramstage-y += psp.c

all-y += reset.c

smm-y += monotonic_timer.c
smm-y += smihandler.c
smm-y += smi_util.c
smm-y += tsc_freq.c
smm-$(CONFIG_DEBUG_SMI) += uart.c
smm-y += gpio.c
smm-y += psp.c

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
ifneq ($(FIRMWARE_LOCATE),)

ifeq ($(CONFIG_AMD_APU_STONEYRIDGE),y)
FIRMWARE_TYPE=ST
else
ifeq ($(CONFIG_AMD_APU_MERLINFALCON),y)
FIRMWARE_TYPE=CZ
else
ifeq ($(CONFIG_AMD_APU_PRAIRIEFALCON),y)
FIRMWARE_TYPE=ST
else
$(error soc/amd/stoneyridge: Unusable FIRMWARE_TYPE)

endif # CONFIG_AMD_APU_PRAIRIEFALCON
endif # CONFIG_AMD_APU_MERLINFALCON
endif # CONFIG_AMD_APU_STONEYRIDGE

###5
PUBSIGNEDKEY_FILE=$(top)/$(FIRMWARE_LOCATE)/RtmPubSigned$(FIRMWARE_TYPE).key

###1
PSPBTLDR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspBootLoader_prod_$(FIRMWARE_TYPE).sbin

###3
PSPRCVR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspRecoveryBootLoader_prod_$(FIRMWARE_TYPE).sbin

###4
PSPNVRAM_FILE=$(top)/$(FIRMWARE_LOCATE)/PspNvram$(FIRMWARE_TYPE).bin

###8 - Check for SMU firmware named either *.sbin or *.csbin.  Both "signed" and
###    "compressed signed" are used by generations supported by this file.
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

ifeq ($(CONFIG_USE_PSPSECUREOS),y)
###2
PSPSECUREOS_FILE=$(top)/$(FIRMWARE_LOCATE)/PspSecureOs_prod_$(FIRMWARE_TYPE).csbin

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

ifeq ("$(wildcard $(SMUFWM_FN_FILE))","")
SMUFWM_FN_FILE=
SMUFIRMWARE2_FN_FILE=
endif

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_STONEYRIDGE_XHCI_FWM_FILE=$(call add_opt_prefix, $(CONFIG_STONEYRIDGE_XHCI_FWM_FILE), --xhci)
OPT_STONEYRIDGE_GEC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_STONEYRIDGE_GEC_FWM_FILEddd), --gec)

OPT_AMD_PUBKEY_FILE=$(call add_opt_prefix, $(CONFIG_AMD_PUBKEY_FILE), --pubkey)
OPT_PSPBTLDR_FILE=$(call add_opt_prefix, $(PSPBTLDR_FILE), --bootloader)
OPT_SMUFWM_FILE=$(call add_opt_prefix, $(SMUFWM_FILE), --smufirmware)
OPT_PSPRCVR_FILE=$(call add_opt_prefix, $(PSPRCVR_FILE), --recovery)
OPT_PUBSIGNEDKEY_FILE=$(call add_opt_prefix, $(PUBSIGNEDKEY_FILE), --rtmpubkey)
OPT_PSPNVRAM_FILE=$(call add_opt_prefix, $(PSPNVRAM_FILE), --nvram)
OPT_PSPSECUREDEBUG_FILE=$(call add_opt_prefix, $(PSPSECUREDEBUG_FILE), --securedebug)
ifeq ($(CONFIG_USE_PSPSECUREOS),y)
OPT_PSPSECUREOS_FILE=$(call add_opt_prefix, $(PSPSECUREOS_FILE), --secureos)
OPT_PSPTRUSTLETS_FILE=$(call add_opt_prefix, $(PSPTRUSTLETS_FILE), --trustlets)
OPT_TRUSTLETKEY_FILE=$(call add_opt_prefix, $(TRUSTLETKEY_FILE), --trustletkey)
endif
OPT_SMUFIRMWARE2_FILE=$(call add_opt_prefix, $(SMUFIRMWARE2_FILE), --smufirmware2)
OPT_SMUSCS_FILE=$(call add_opt_prefix, $(SMUSCS_FILE), --smuscs)
SUBPROG_FN_SMU_FW=1
OPT_SMUFWM_FN_FILE=$(call add_opt_prefix, $(SMUFWM_FN_FILE), --subprogram $(SUBPROG_FN_SMU_FW) --smufirmware)
OPT_SMUFIRMWARE2_FN_FILE=$(call add_opt_prefix, $(SMUFIRMWARE2_FN_FILE), --subprogram $(SUBPROG_FN_SMU_FW) --smufirmware2)

ifeq ($(FIRMWARE_TYPE),ST)
OPT_COMBOCAPABLE=--combo-capable
endif

$(obj)/amdfw.rom:	$(call strip_quotes, $(CONFIG_STONEYRIDGE_XHCI_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_STONEYRIDGE_GEC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)) \
			$(call strip_quotes, $(PUBSIGNEDKEY_FILE)) \
			$(call strip_quotes, $(PSPBTLDR_FILE)) \
			$(call strip_quotes, $(PSPRCVR_FILE)) \
			$(call strip_quotes, $(PSPSECUREOS_FILE)) \
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
		$(OPT_STONEYRIDGE_GEC_FWM_FILE) \
		$(OPT_AMD_PUBKEY_FILE) \
		$(OPT_PSPBTLDR_FILE) \
		$(OPT_SMUFWM_FILE) \
		$(OPT_PSPRCVR_FILE) \
		$(OPT_PUBSIGNEDKEY_FILE) \
		$(OPT_PSPSECUREOS_FILE) \
		$(OPT_PSPNVRAM_FILE) \
		$(OPT_PSPSECUREDEBUG_FILE) \
		$(OPT_PSPTRUSTLETS_FILE) \
		$(OPT_TRUSTLETKEY_FILE) \
		$(OPT_SMUFIRMWARE2_FILE) \
		$(OPT_SMUSCS_FILE) \
		$(OPT_AMD_PUBKEY_FILE) \
		$(OPT_PSPBTLDR_FILE) \
		$(OPT_SMUFWM_FILE) \
		$(OPT_SMUFWM_FN_FILE) \
		$(OPT_PSPRCVR_FILE) \
		$(OPT_PUBSIGNEDKEY_FILE) \
		$(OPT_PSPSECUREOS_FILE) \
		$(OPT_PSPNVRAM_FILE) \
		$(OPT_PSPSECUREDEBUG_FILE) \
		$(OPT_PSPTRUSTLETS_FILE) \
		$(OPT_TRUSTLETKEY_FILE) \
		$(OPT_SMUFIRMWARE2_FILE) \
		$(OPT_SMUFIRMWARE2_FN_FILE) \
		$(OPT_SMUSCS_FILE) \
		$(OPT_COMBOCAPABLE)\
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

else # ifneq ($(FIRMWARE_LOCATE),)

warn_no_amdfw:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built with no PSP firmware and "
	printf "a non-booting image has been generated.\n\n"

PHONY+=warn_no_amdfw

files_added:: warn_no_amdfw

endif # ifneq ($(FIRMWARE_LOCATE),)

endif # ($(CONFIG_SOC_AMD_STONEYRIDGE),y)
