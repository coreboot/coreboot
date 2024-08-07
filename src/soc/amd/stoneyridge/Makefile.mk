# SPDX-License-Identifier: BSD-3-Clause

ifeq ($(CONFIG_SOC_AMD_STONEYRIDGE),y)

subdirs-y += ../../../cpu/amd/mtrr/

bootblock-y += aoac.c
bootblock-y += BiosCallOuts.c
bootblock-y += bootblock.c
bootblock-y += early_fch.c
bootblock-y += gpio.c
bootblock-y += i2c.c
bootblock-y += enable_usbdebug.c

romstage-y += BiosCallOuts.c
romstage-y += i2c.c
romstage-y += romstage.c
romstage-y += enable_usbdebug.c
romstage-y += fch_agesa.c
romstage-y += gpio.c
romstage-y += smbus_spd.c
romstage-y += memmap.c
romstage-y += psp.c

verstage-y += gpio.c
verstage-y += i2c.c

postcar-y += memmap.c
postcar-$(CONFIG_TPM_MEASURED_BOOT) += i2c.c

ramstage-y += aoac.c
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
ramstage-y += graphics.c
ramstage-y += northbridge.c
ramstage-y += sata.c
ramstage-y += memmap.c
ramstage-y += usb.c
ramstage-y += psp.c

all-y += monotonic_timer.c
all-y += pstate_util.c
all-y += reset.c
all-y += tsc_freq.c
all-y += uart.c

smm-y += pstate_util.c
smm-y += monotonic_timer.c
smm-y += smihandler.c
smm-y += tsc_freq.c
smm-$(CONFIG_DEBUG_SMI) += uart.c
smm-y += gpio.c
smm-y += psp.c

CPPFLAGS_common += -I$(src)/soc/amd/stoneyridge/include
CPPFLAGS_common += -I$(src)/soc/amd/stoneyridge/acpi

# ROMSIG Normally At ROMBASE + 0x20000
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# |PSPDIR ADDR|
# +-----------+
#
# EC ROM should be 64K aligned.
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

# type = 0x04
# The flashmap section used for this is expected to be named PSP_NVRAM
PSP_NVRAM_BASE=$(call get_fmap_value,FMAP_SECTION_PSP_NVRAM_START)
PSP_NVRAM_SIZE=$(call get_fmap_value,FMAP_SECTION_PSP_NVRAM_SIZE)

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_PSP_NVRAM_BASE=$(call add_opt_prefix, $(PSP_NVRAM_BASE), --nvram-base)
OPT_PSP_NVRAM_SIZE=$(call add_opt_prefix, $(PSP_NVRAM_SIZE), --nvram-size)

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
		$(OPT_PSP_NVRAM_BASE) \
		$(OPT_PSP_NVRAM_SIZE) \
		$(OPT_STONEYRIDGE_XHCI_FWM_FILE) \
		$(OPT_STONEYRIDGE_GEC_FWM_FILE) \
		$(OPT_PSP_USE_PSPSECUREOS) \
		$(OPT_EFS_SPI_READ_MODE) \
		$(OPT_EFS_SPI_SPEED) \
		$(OPT_DEBUG_AMDFWTOOL) \
		--config $(CONFIG_AMDFW_CONFIG_FILE) \
		--flashsize $(CONFIG_ROM_SIZE) \
		--location $(CONFIG_AMD_FWM_POSITION) \
		--output	$@

ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)

$(call add_intermediate, add_amdfw, $(obj)/amdfw.rom)
	printf "    DD         Adding AMD Firmware at ROM offset 0x%x\n" \
		"$(CONFIG_AMD_FWM_POSITION)"
	dd if=$(obj)/amdfw.rom \
		of=$< conv=notrunc bs=1 \
		seek=$(CONFIG_AMD_FWM_POSITION) >/dev/null 2>&1

else # ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)

STONEYRIDGE_FWM_POSITION=$(call int-add, \
	$(call int-subtract, 0xffffffff $(CONFIG_ROM_SIZE)) \
	1 \
	$(CONFIG_AMD_FWM_POSITION))
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

show_notices:: warn_no_amdfw

endif # ifneq ($(FIRMWARE_LOCATION),)

endif # ($(CONFIG_SOC_AMD_STONEYRIDGE),y)
