# SPDX-License-Identifier: BSD-3-Clause

ifeq ($(CONFIG_SOC_AMD_STONEYRIDGE),y)

subdirs-y += ../../../cpu/amd/mtrr/

bootblock-y += aoac.c
bootblock-y += uart.c
bootblock-y += BiosCallOuts.c
bootblock-y += bootblock.c
bootblock-y += early_fch.c
bootblock-y += gpio.c
bootblock-y += i2c.c
bootblock-y += enable_usbdebug.c
bootblock-y += monotonic_timer.c
bootblock-y += tsc_freq.c

romstage-y += BiosCallOuts.c
romstage-y += i2c.c
romstage-y += romstage.c
romstage-y += enable_usbdebug.c
romstage-y += fch_agesa.c
romstage-y += gpio.c
romstage-y += monotonic_timer.c
romstage-y += smbus_spd.c
romstage-y += memmap.c
romstage-y += uart.c
romstage-y += tsc_freq.c
romstage-y += psp.c

verstage-y += gpio.c
verstage-y += i2c.c
verstage-y += monotonic_timer.c
verstage-y += uart.c
verstage-y += tsc_freq.c

postcar-y += monotonic_timer.c
postcar-y += uart.c
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
ramstage-y += fch.c
ramstage-y += fch_agesa.c
ramstage-y += gpio.c
ramstage-y += monotonic_timer.c
ramstage-y += northbridge.c
ramstage-y += sata.c
ramstage-y += memmap.c
ramstage-y += uart.c
ramstage-y += usb.c
ramstage-y += tsc_freq.c
ramstage-y += psp.c

all-y += reset.c

smm-y += monotonic_timer.c
smm-y += smihandler.c
smm-y += tsc_freq.c
smm-$(CONFIG_DEBUG_SMI) += uart.c
smm-y += gpio.c
smm-y += psp.c

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

FIRMWARE_LOCATION=$(shell grep -e FIRMWARE_LOCATION $(CONFIG_AMDFW_CONFIG_FILE) | awk '{print $$2}')

ifneq ($(FIRMWARE_LOCATION),)

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

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_STONEYRIDGE_XHCI_FWM_FILE=$(call add_opt_prefix, $(CONFIG_STONEYRIDGE_XHCI_FWM_FILE), --xhci)
OPT_STONEYRIDGE_GEC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_STONEYRIDGE_GEC_FWM_FILE), --gec)

SMUFWM_FILE=$(top)/$(FIRMWARE_LOCATION)/$(shell awk '($$1=="PSP_SMUFW1_SUB0_FILE") {print $$2}' $(CONFIG_AMDFW_CONFIG_FILE))
SMUFWM_FN_FILE=$(top)/$(FIRMWARE_LOCATION)/$(shell awk '($$1=="PSP_SMUFW1_SUB1_FILE") {print $$2}' $(CONFIG_AMDFW_CONFIG_FILE))

SMUFIRMWARE2_FILE=$(top)/$(FIRMWARE_LOCATION)/$(shell awk '($$1=="PSP_SMUFW2_SUB0_FILE") {print $$2}' $(CONFIG_AMDFW_CONFIG_FILE))
SMUFIRMWARE2_FN_FILE=$(top)/$(FIRMWARE_LOCATION)/$(shell awk '($$1=="PSP_SMUFW2_SUB1_FILE") {print $$2}' $(CONFIG_AMDFW_CONFIG_FILE))

ifeq ("$(wildcard $(SMUFWM_FN_FILE))","")
SMUFWM_FN_FILE=
SMUFIRMWARE2_FN_FILE=
endif

ifeq ($(FIRMWARE_TYPE),ST)
OPT_COMBOCAPABLE=--combo-capable
OPT_SOCNAME=--soc-name "Stoneyridge"
endif

ifeq ($(CONFIG_USE_PSPSECUREOS),y)
PSP_USE_PSPSECUREOS="--use-pspsecureos"
endif

OPT_PSP_USE_PSPSECUREOS=$(call strip_quotes, $(PSP_USE_PSPSECUREOS))

OPT_EFS_SPI_READ_MODE=$(call add_opt_prefix, $(CONFIG_EFS_SPI_READ_MODE), --spi-read-mode)
OPT_EFS_SPI_SPEED=$(call add_opt_prefix, $(CONFIG_EFS_SPI_SPEED), --spi-speed)

$(obj)/amdfw.rom:	$(call strip_quotes, $(CONFIG_STONEYRIDGE_XHCI_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_STONEYRIDGE_GEC_FWM_FILE)) \
			$(DEP_FILES) \
			$(AMDFWTOOL)
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(OPT_STONEYRIDGE_XHCI_FWM_FILE) \
		$(OPT_STONEYRIDGE_GEC_FWM_FILE) \
		$(OPT_COMBOCAPABLE)\
		$(OPT_PSP_USE_PSPSECUREOS) \
		$(OPT_EFS_SPI_READ_MODE) \
		$(OPT_EFS_SPI_SPEED) \
		--config $(CONFIG_AMDFW_CONFIG_FILE) \
		$(OPT_SOCNAME) \
		--flashsize $(CONFIG_ROM_SIZE) \
		--location $(shell printf "0x%x" $(STONEYRIDGE_FWM_POSITION)) \
		--output	$@

ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)

# Calculate firmware position inside the ROM
STONEYRIDGE_FWM_ROM_POSITION=$(call int-add, \
	$(call int-subtract, $(CONFIG_ROM_SIZE) \
	$(call int-shift-left, \
	0x80000 $(CONFIG_AMD_FWM_POSITION_INDEX))) 0x20000)

$(call add_intermediate, add_amdfw, $(obj)/amdfw.rom)
	printf "    DD         Adding AMD Firmware at ROM offset 0x%x\n" \
		"$(STONEYRIDGE_FWM_ROM_POSITION)"
	dd if=$(obj)/amdfw.rom \
		of=$< conv=notrunc bs=1 \
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

else # ifneq ($(FIRMWARE_LOCATION),)

warn_no_amdfw:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built with no PSP firmware and "
	printf "a non-booting image has been generated.\n\n"

PHONY+=warn_no_amdfw

files_added:: warn_no_amdfw

endif # ifneq ($(FIRMWARE_LOCATION),)

endif # ($(CONFIG_SOC_AMD_STONEYRIDGE),y)
