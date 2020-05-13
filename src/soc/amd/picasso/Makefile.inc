# SPDX-License-Identifier: BSD-3-Clause

ifeq ($(CONFIG_SOC_AMD_PICASSO),y)

subdirs-y += ../../../cpu/amd/mtrr/
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/cache
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/pae
subdirs-y += ../../../cpu/x86/smm

bootblock-y += bootblock/pre_c.S
bootblock-y += bootblock/bootblock.c
bootblock-y += southbridge.c
bootblock-y += i2c.c
bootblock-$(CONFIG_PICASSO_UART) += uart.c
bootblock-y += tsc_freq.c
bootblock-y += gpio.c
bootblock-y += smi_util.c
bootblock-y += config.c

romstage-y += i2c.c
romstage-y += romstage.c
romstage-y += gpio.c
romstage-y += pmutil.c
romstage-y += memmap.c
romstage-$(CONFIG_PICASSO_UART) += uart.c
romstage-y += tsc_freq.c
romstage-y += southbridge.c
romstage-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c
romstage-y += psp.c
romstage-y += mtrr.c
romstage-y += config.c

verstage-y += gpio.c
verstage-y += i2c.c
verstage-y += pmutil.c
verstage-y += config.c
verstage-$(CONFIG_PICASSO_UART) += uart.c
verstage-y += tsc_freq.c

ramstage-y += i2c.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += data_fabric_util.c
ramstage-y += mca.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += gpio.c
ramstage-y += southbridge.c
ramstage-y += pmutil.c
ramstage-y += acp.c
ramstage-y += sata.c
ramstage-y += memmap.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c
ramstage-$(CONFIG_PICASSO_UART) += uart.c
ramstage-y += usb.c
ramstage-y += tsc_freq.c
ramstage-y += finalize.c
ramstage-y += soc_util.c
ramstage-y += psp.c
ramstage-y += fsp_params.c
ramstage-y += config.c

all-y += reset.c

smm-y += smihandler.c
smm-y += smi_util.c
smm-y += tsc_freq.c
smm-$(CONFIG_DEBUG_SMI) += uart.c
smm-y += gpio.c
smm-y += psp.c
smm-y += config.c

CPPFLAGS_common += -I$(src)/soc/amd/picasso
CPPFLAGS_common += -I$(src)/soc/amd/picasso/include
CPPFLAGS_common += -I$(src)/soc/amd/picasso/acpi
CPPFLAGS_common += -I$(src)/vendorcode/amd/fsp/picasso

MAINBOARD_BLOBS_DIR:=$(top)/3rdparty/blobs/mainboard/$(MAINBOARDDIR)

# ROMSIG Normally At ROMBASE + 0x20000
# Overridden by CONFIG_AMD_FWM_POSITION_INDEX
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |               |                |            |
# +-----------+---------------+----------------+------------+
# |           |  PSPDIR ADDR  |  BIOSDIR ADDR  |
# +-----------+---------------+----------------+

PICASSO_FWM_POSITION=$(call int-add, \
	$(call int-subtract, 0xffffffff \
	$(call int-shift-left, \
	0x80000 $(CONFIG_AMD_FWM_POSITION_INDEX))) 0x20000 1)

#
# PSP Directory Table items
#
# Certain ordering requirements apply, however these are ensured by amdfwtool.
# For more information see "AMD Platform Security Processor BIOS Architecture
# Design Guide for AMD Family 17h Processors" (PID #55758, NDA only).
#

# type = 0x0
FIRMWARE_LOCATE=$(dir $(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)))

# type = 0x1
ifeq ($(CONFIG_HAVE_PSP_WHITELIST_FILE),y)
PSPBTLDR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspBootLoader_WL_RV.sbin
else
PSPBTLDR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspBootLoader_prod_RV.sbin
endif

# types = 0x8 and 0x12
PSP_SMUFW1_SUB1_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmwareRV2.csbin
PSP_SMUFW1_SUB2_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmwarePCO.csbin
PSP_SMUFW2_SUB1_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware2RV2.csbin
PSP_SMUFW2_SUB2_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware2PCO.csbin

ifeq ($(CONFIG_PSP_UNLOCK_SECURE_DEBUG),y)
# type = 0x9
PSP_SEC_DBG_KEY_FILE=$(top)/$(FIRMWARE_LOCATE)/RavenSecureDebug_PublicKey.bin
# type = 0x13
PSP_SEC_DEBUG_FILE=$(top)/$(FIRMWARE_LOCATE)/secure_unlock_prod_RV.sbin
# Enable secure debug unlock
PSP_SOFTFUSE_BITS += 0
PSP_TOKEN_UNLOCK="--token-unlock"
endif

ifeq ($(CONFIG_USE_PSPSCUREOS),y)
# types = 0x2
PSPSCUREOS_FILE=$(top)/$(FIRMWARE_LOCATE)/psp_os_combined_prod_RV.sbin
endif

# type = 0x21
PSP_IKEK_FILE=$(top)/$(FIRMWARE_LOCATE)/PspIkekRV.bin

# type = 0x24
PSP_SECG1_FILE=$(top)/$(FIRMWARE_LOCATE)/security_policy_RV2_FP5_AM4.sbin
PSP_SECG2_FILE=$(top)/$(FIRMWARE_LOCATE)/security_policy_PCO_FP5_AM4.sbin

ifeq ($(CONFIG_PSP_LOAD_MP2_FW),y)
# type = 0x25
PSP_MP2FW1_FILE=$(top)/$(FIRMWARE_LOCATE)/MP2I2CFWRV2.sbin
PSP_MP2FW2_FILE=$(top)/$(FIRMWARE_LOCATE)/MP2I2CFWPCO.sbin
# BIOS type = 0x6a
PSP_MP2CFG_FILE=$(top)/$(FIRMWARE_LOCATE)/MP2FWConfig.sbin
else
# Disable MP2 firmware loading
PSP_SOFTFUSE_BITS += 29
endif

# type = 0x28
PSP_DRIVERS_FILE=$(top)/$(FIRMWARE_LOCATE)/drv_sys_prod_RV.sbin

ifeq ($(CONFIG_PSP_LOAD_S0I3_FW),y)
PSP_S0I3_FILE=$(top)/$(FIRMWARE_LOCATE)/dr_agesa_prod_RV.sbin
endif

# types = 0x30 - 0x37
PSP_ABL0_FILE=$(top)/$(FIRMWARE_LOCATE)/AgesaBootloader0_prod_RV.csbin
PSP_ABL1_FILE=$(top)/$(FIRMWARE_LOCATE)/AgesaBootloader1_prod_RV.csbin
PSP_ABL2_FILE=$(top)/$(FIRMWARE_LOCATE)/AgesaBootloader2_prod_RV.csbin
PSP_ABL3_FILE=$(top)/$(FIRMWARE_LOCATE)/AgesaBootloader3_prod_RV.csbin
PSP_ABL4_FILE=$(top)/$(FIRMWARE_LOCATE)/AgesaBootloader4_prod_RV.csbin
PSP_ABL5_FILE=$(top)/$(FIRMWARE_LOCATE)/AgesaBootloader5_prod_RV.csbin
PSP_ABL6_FILE=$(top)/$(FIRMWARE_LOCATE)/AgesaBootloader6_prod_RV.csbin
PSP_ABL7_FILE=$(top)/$(FIRMWARE_LOCATE)/AgesaBootloader7_prod_RV.csbin

# type = 0x3a
ifeq ($(CONFIG_HAVE_PSP_WHITELIST_FILE),y)
PSP_WHITELIST_FILE=$(CONFIG_PSP_WHITELIST_FILE)
endif

#
# BIOS Directory Table items - proper ordering is managed by amdfwtool
#

# type = 0x60
PSP_APCB_FILES=$(foreach f, $(APCB_SOURCES), $(obj)/APCB_$(f).bin)

# type = 0x61
PSP_APOB_BASE=$(CONFIG_PSP_APOB_DESTINATION)

# type = 0x62
PSP_BIOSBIN_FILE=$(obj)/amd_biospsp.img
PSP_ELF_FILE=$(objcbfs)/bootblock.elf
# TODO(b/154957411): Refactor amdfwtool to extract the address and size from
# the elf file.
PSP_BIOSBIN_SIZE=$(CONFIG_C_ENV_BOOTBLOCK_SIZE)
# This address must match the BOOTBLOCK logic in arch/x86/memlayout.ld.
PSP_BIOSBIN_DEST=$(shell printf "%x" $(call int-subtract, $(call int-add, $(CONFIG_X86_RESET_VECTOR) 0x10) $(PSP_BIOSBIN_SIZE)))

# type = 0x63
ifeq ($(CONFIG_HAVE_ACPI_RESUME),y)
PSP_APOBNV_BASE=$(CONFIG_PSP_APOB_NV_ADDRESS)
PSP_APOBNV_SIZE=$(CONFIG_PSP_APOB_NV_SIZE)
endif

# type2 = 0x64, 0x65
PSP_PMUI_FILE1=$(top)/$(FIRMWARE_LOCATE)/Appb_Rv_1D_Ddr4_Udimm_Imem.csbin
PSP_PMUI_FILE2=$(top)/$(FIRMWARE_LOCATE)/Appb_Rv_2D_Ddr4_Imem.csbin
PSP_PMUI_FILE3=$(top)/$(FIRMWARE_LOCATE)/Appb_Rv2_1D_ddr4_Udimm_Imem.csbin
PSP_PMUI_FILE4=$(top)/$(FIRMWARE_LOCATE)/Appb_Rv2_2D_ddr4_Udimm_Imem.csbin
PSP_PMUD_FILE1=$(top)/$(FIRMWARE_LOCATE)/Appb_Rv_1D_Ddr4_Udimm_Dmem.csbin
PSP_PMUD_FILE2=$(top)/$(FIRMWARE_LOCATE)/Appb_Rv_2D_Ddr4_Dmem.csbin
PSP_PMUD_FILE3=$(top)/$(FIRMWARE_LOCATE)/Appb_Rv2_1D_ddr4_Udimm_Dmem.csbin
PSP_PMUD_FILE4=$(top)/$(FIRMWARE_LOCATE)/Appb_Rv2_2D_ddr4_Udimm_Dmem.csbin

# type = 0x66
PSP_UCODE_FILE1=$(top)/$(FIRMWARE_LOCATE)/UcodePatch_PCO_B1.bin
PSP_UCODE_FILE2=$(top)/$(FIRMWARE_LOCATE)/UcodePatch_PCO_B0.bin
PSP_UCODE_FILE3=$(top)/$(FIRMWARE_LOCATE)/UcodePatch_RV2_A0.bin

# type = 0xb - See #55758 (NDA) for bit definitions.
PSP_SOFTFUSE_BITS += 28

# Helper function to return a value with given bit set
set-bit=$(call int-shift-left, 1 $(call _toint,$1))
PSP_SOFTFUSE=$(shell A=$(call int-add, \
		$(foreach bit,$(PSP_SOFTFUSE_BITS),$(call set-bit,$(bit)))); printf "0x%x" $$A)

#
# Build the arguments to amdfwtool (order is unimportant).  Missing file names
# result in empty OPT_ variables, i.e. the argument is not passed to amdfwtool.
#

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_AMD_PUBKEY_FILE=$(call add_opt_prefix, $(CONFIG_AMD_PUBKEY_FILE), --pubkey)
OPT_PSPBTLDR_FILE=$(call add_opt_prefix, $(PSPBTLDR_FILE), --bootloader)
OPT_SMUFW1_SUB1_FILE=$(call add_opt_prefix, $(PSP_SMUFW1_SUB1_FILE), --subprogram 1 --smufirmware)
OPT_SMUFW1_SUB2_FILE=$(call add_opt_prefix, $(PSP_SMUFW1_SUB2_FILE), --subprogram 2 --smufirmware)
OPT_SMUFW2_SUB1_FILE=$(call add_opt_prefix, $(PSP_SMUFW2_SUB1_FILE), --subprogram 1 --smufirmware2)
OPT_SMUFW2_SUB2_FILE=$(call add_opt_prefix, $(PSP_SMUFW2_SUB2_FILE), --subprogram 2 --smufirmware2)
OPT_PSP_SEC_DBG_KEY_FILE=$(call add_opt_prefix, $(PSP_SEC_DBG_KEY_FILE), --securedebug)
OPT_TOKEN_UNLOCK=$(call add_opt_prefix, $(PSP_TOKEN_UNLOCK), "")
OPT_PSP_SOFTFUSE=$(call add_opt_prefix, $(PSP_SOFTFUSE), --soft-fuse)
OPT_PSPSCUREOS_FILE=$(call add_opt_prefix, $(PSPSCUREOS_FILE), --secureos)
OPT_SEC_DEBUG_FILE=$(call add_opt_prefix, $(PSP_SEC_DEBUG_FILE), --secdebug)
OPT_IKEK_FILE=$(call add_opt_prefix, $(PSP_IKEK_FILE), --ikek)
OPT_SECG1_FILE=$(call add_opt_prefix, $(PSP_SECG1_FILE), --subprog 1 --sec-gasket)
OPT_SECG2_FILE=$(call add_opt_prefix, $(PSP_SECG2_FILE), --subprog 2 --sec-gasket)
OPT_MP2FW1_FILE=$(call add_opt_prefix, $(PSP_MP2FW1_FILE), --subprog 1 --mp2-fw)
OPT_MP2FW2_FILE=$(call add_opt_prefix, $(PSP_MP2FW2_FILE), --subprog 2 --mp2-fw)
OPT_DRIVERS_FILE=$(call add_opt_prefix, $(PSP_DRIVERS_FILE), --drv-entry-pts)
OPT_PSP_S0I3_FILE=$(call add_opt_prefix, $(PSP_S0I3_FILE), --s0i3drv)
OPT_ABL0_FILE=$(call add_opt_prefix,  $(PSP_ABL0_FILE), --abl-image)
OPT_ABL1_FILE=$(call add_opt_prefix,  $(PSP_ABL1_FILE), --abl-image)
OPT_ABL2_FILE=$(call add_opt_prefix,  $(PSP_ABL2_FILE), --abl-image)
OPT_ABL3_FILE=$(call add_opt_prefix,  $(PSP_ABL3_FILE), --abl-image)
OPT_ABL4_FILE=$(call add_opt_prefix,  $(PSP_ABL4_FILE), --abl-image)
OPT_ABL5_FILE=$(call add_opt_prefix,  $(PSP_ABL5_FILE), --abl-image)
OPT_ABL6_FILE=$(call add_opt_prefix,  $(PSP_ABL6_FILE), --abl-image)
OPT_ABL7_FILE=$(call add_opt_prefix,  $(PSP_ABL7_FILE), --abl-image)
OPT_WHITELIST_FILE=$(call add_opt_prefix, $(PSP_WHITELIST_FILE), --whitelist)

OPT_PSP_APCB_FILES=$(foreach i, $(shell seq $(words $(PSP_APCB_FILES))), \
	$(call add_opt_prefix, $(word $(i), $(PSP_APCB_FILES)), \
	--instance $(shell printf "%x" $$(($(i)-1))) --apcb ))

OPT_APOB_ADDR=$(call add_opt_prefix, $(PSP_APOB_BASE), --apob-base)
OPT_PSP_BIOSBIN_FILE=$(call add_opt_prefix, $(PSP_BIOSBIN_FILE), --bios-bin)
OPT_PSP_BIOSBIN_DEST=$(call add_opt_prefix, $(PSP_BIOSBIN_DEST), --bios-bin-dest)
OPT_PSP_BIOSBIN_SIZE=$(call add_opt_prefix, $(PSP_BIOSBIN_SIZE), --bios-uncomp-size)
OPT_APOBNV_ADDR=$(call add_opt_prefix, $(PSP_APOBNV_BASE), --apob-nv-base)
OPT_APOBNV_SIZE=$(call add_opt_prefix, $(PSP_APOBNV_SIZE), --apob-nv-size)
OPT_PSP_PMUI_FILE1=$(call add_opt_prefix, $(PSP_PMUI_FILE1), --subprogram 0 --instance 1 --pmu-inst)
OPT_PSP_PMUI_FILE2=$(call add_opt_prefix, $(PSP_PMUI_FILE2), --subprogram 0 --instance 4 --pmu-inst)
OPT_PSP_PMUI_FILE3=$(call add_opt_prefix, $(PSP_PMUI_FILE3), --subprogram 1 --instance 1 --pmu-inst)
OPT_PSP_PMUI_FILE4=$(call add_opt_prefix, $(PSP_PMUI_FILE4), --subprogram 1 --instance 4 --pmu-inst)
OPT_PSP_PMUD_FILE1=$(call add_opt_prefix, $(PSP_PMUD_FILE1), --subprogram 0 --instance 1 --pmu-data)
OPT_PSP_PMUD_FILE2=$(call add_opt_prefix, $(PSP_PMUD_FILE2), --subprogram 0 --instance 4 --pmu-data)
OPT_PSP_PMUD_FILE3=$(call add_opt_prefix, $(PSP_PMUD_FILE3), --subprogram 1 --instance 1 --pmu-data)
OPT_PSP_PMUD_FILE4=$(call add_opt_prefix, $(PSP_PMUD_FILE4), --subprogram 1 --instance 4 --pmu-data)
OPT_PSP_UCODE_FILE1=$(call add_opt_prefix, $(PSP_UCODE_FILE1), --instance 0 --ucode)
OPT_PSP_UCODE_FILE2=$(call add_opt_prefix, $(PSP_UCODE_FILE2), --instance 1 --ucode)
OPT_PSP_UCODE_FILE3=$(call add_opt_prefix, $(PSP_UCODE_FILE3), --instance 2 --ucode)
OPT_MP2CFG_FILE=$(call add_opt_prefix, $(PSP_MP2CFG_FILE), --mp2-config)

# Copy prebuild APCBs if they exist
$(obj)/APCB_%.bin: $(MAINBOARD_BLOBS_DIR)/APCB_%.bin
	cp $< $@

# APCB binary with magic numbers to be replaced by apcb_edit tool
APCB_MAGIC_BLOB:=$(MAINBOARD_BLOBS_DIR)/APCB_magic.bin

$(obj)/APCB_empty.bin: $(APCB_MAGIC_BLOB) $(APCB_EDIT_TOOL)
	$(APCB_EDIT_TOOL) \
	$(APCB_MAGIC_BLOB) \
	$@ \
	--board_id_gpio0 $(APCB_BOARD_ID_GPIO0) \
	--board_id_gpio1 $(APCB_BOARD_ID_GPIO1) \
	--board_id_gpio2 $(APCB_BOARD_ID_GPIO2) \
	--board_id_gpio3 $(APCB_BOARD_ID_GPIO3)

$(obj)/APCB_%_x1.bin:	$$(SPD_SOURCES_DIR)/%.spd.hex \
			$(APCB_EDIT_TOOL) \
			$(APCB_MAGIC_BLOB)
	$(APCB_EDIT_TOOL) \
	$(APCB_MAGIC_BLOB) \
	$@ \
	--hex \
	--spd_0_0 $< \
	--board_id_gpio0 $(APCB_BOARD_ID_GPIO0) \
	--board_id_gpio1 $(APCB_BOARD_ID_GPIO1) \
	--board_id_gpio2 $(APCB_BOARD_ID_GPIO2) \
	--board_id_gpio3 $(APCB_BOARD_ID_GPIO3)

$(obj)/APCB_%_x2.bin:	$$(SPD_SOURCES_DIR)/%.spd.hex \
			$(APCB_EDIT_TOOL) \
			$(APCB_MAGIC_BLOB)
	$(APCB_EDIT_TOOL) \
	$(APCB_MAGIC_BLOB) \
	$@ \
	--hex \
	--spd_0_0 $< \
	--spd_1_0 $< \
	--board_id_gpio0 $(APCB_BOARD_ID_GPIO0) \
	--board_id_gpio1 $(APCB_BOARD_ID_GPIO1) \
	--board_id_gpio2 $(APCB_BOARD_ID_GPIO2) \
	--board_id_gpio3 $(APCB_BOARD_ID_GPIO3)

$(obj)/amdfw.rom:	$(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)) \
			$(call strip_quotes, $(PSPBTLDR_FILE)) \
			$(call strip_quotes, $(PSPSCUREOS_FILE)) \
			$(call strip_quotes, $(PSP_SEC_DBG_KEY_FILE)) \
			$(call strip_quotes, $(PSP_BIOSBIN_FILE)) \
			$(call strip_quotes, $(PSP_PMUI_FILE1)) \
			$(call strip_quotes, $(PSP_PMUI_FILE2)) \
			$(call strip_quotes, $(PSP_PMUI_FILE3)) \
			$(call strip_quotes, $(PSP_PMUI_FILE4)) \
			$(call strip_quotes, $(PSP_PMUD_FILE1)) \
			$(call strip_quotes, $(PSP_PMUD_FILE2)) \
			$(call strip_quotes, $(PSP_PMUD_FILE3)) \
			$(call strip_quotes, $(PSP_PMUD_FILE4)) \
			$(call strip_quotes, $(PSP_UCODE_FILE1)) \
			$(call strip_quotes, $(PSP_UCODE_FILE2)) \
			$(call strip_quotes, $(PSP_UCODE_FILE3)) \
			$(call strip_quotes, $(PSP_MP2CFG_FILE)) \
			$(call strip_quotes, $(PSP_SMUFW1_SUB1_FILE)) \
			$(call strip_quotes, $(PSP_SMUFW1_SUB2_FILE)) \
			$(call strip_quotes, $(PSP_SMUFW2_SUB1_FILE)) \
			$(call strip_quotes, $(PSP_SMUFW2_SUB2_FILE)) \
			$(call strip_quotes, $(PSP_ABL0_FILE)) \
			$(call strip_quotes, $(PSP_ABL1_FILE)) \
			$(call strip_quotes, $(PSP_ABL2_FILE)) \
			$(call strip_quotes, $(PSP_ABL3_FILE)) \
			$(call strip_quotes, $(PSP_ABL4_FILE)) \
			$(call strip_quotes, $(PSP_ABL5_FILE)) \
			$(call strip_quotes, $(PSP_ABL6_FILE)) \
			$(call strip_quotes, $(PSP_ABL7_FILE)) \
			$(call strip_quotes, $(PSP_WHITELIST_FILE)) \
			$(call strip_quotes, $(PSP_SECG1_FILE)) \
			$(call strip_quotes, $(PSP_SECG2_FILE)) \
			$(call_strip_quotes, $(PSP_DRIVERS_FILE)) \
			$(call_strip_quotes, $(PSP_S0I3_FILE)) \
			$(call_strip_quotes, $(PSP_IKEK_FILE)) \
			$(call_strip_quotes, $(PSP_SEC_DEBUG_FILE)) \
			$$(PSP_APCB_FILES) \
			$(AMDFWTOOL)
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(OPT_AMD_PUBKEY_FILE) \
		$(OPT_PSPBTLDR_FILE) \
		$(OPT_PSPSCUREOS_FILE) \
		$(OPT_PSP_SEC_DBG_KEY_FILE) \
		$(OPT_SMUFW1_SUB2_FILE) \
		$(OPT_SMUFW2_SUB2_FILE) \
		$(OPT_SMUFW1_SUB1_FILE) \
		$(OPT_SMUFW2_SUB1_FILE) \
		$(OPT_PSP_APCB_FILES) \
		$(OPT_APOB_ADDR) \
		$(OPT_APOBNV_ADDR) \
		$(OPT_APOBNV_SIZE) \
		$(OPT_PSP_BIOSBIN_FILE) \
		$(OPT_PSP_BIOSBIN_DEST) \
		$(OPT_PSP_BIOSBIN_SIZE) \
		$(OPT_PSP_SOFTFUSE) \
		$(OPT_PSP_PMUI_FILE1) \
		$(OPT_PSP_PMUI_FILE2) \
		$(OPT_PSP_PMUI_FILE3) \
		$(OPT_PSP_PMUI_FILE4) \
		$(OPT_PSP_PMUD_FILE1) \
		$(OPT_PSP_PMUD_FILE2) \
		$(OPT_PSP_PMUD_FILE3) \
		$(OPT_PSP_PMUD_FILE4) \
		$(OPT_PSP_UCODE_FILE1) \
		$(OPT_PSP_UCODE_FILE2) \
		$(OPT_PSP_UCODE_FILE3) \
		$(OPT_MP2CFG_FILE) \
		$(OPT_ABL0_FILE) \
		$(OPT_ABL1_FILE) \
		$(OPT_ABL2_FILE) \
		$(OPT_ABL3_FILE) \
		$(OPT_ABL4_FILE) \
		$(OPT_ABL5_FILE) \
		$(OPT_ABL6_FILE) \
		$(OPT_ABL7_FILE) \
		$(OPT_WHITELIST_FILE) \
		$(OPT_SECG1_FILE) \
		$(OPT_SECG2_FILE) \
		$(OPT_MP2FW1_FILE) \
		$(OPT_MP2FW2_FILE) \
		$(OPT_DRIVERS_FILE) \
		$(OPT_PSP_S0I3_FILE) \
		$(OPT_IKEK_FILE) \
		$(OPT_SEC_DEBUG_FILE) \
		--combo-capable \
		$(OPT_TOKEN_UNLOCK) \
		--flashsize $(CONFIG_ROM_SIZE) \
		--location $(shell printf "0x%x" $(PICASSO_FWM_POSITION)) \
		--output	$@

$(PSP_BIOSBIN_FILE): $(PSP_ELF_FILE) $(AMDCOMPRESS)
	rm -f $@
	@printf "    AMDCOMPRS  $(subst $(obj)/,,$(@))\n"
	$(AMDCOMPRESS) --infile $(PSP_ELF_FILE) --outfile $@ --compress \
		--maxsize $(PSP_BIOSBIN_SIZE)

ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)
PHONY+=add_amdfw
INTERMEDIATE+=add_amdfw

# Calculate firmware position inside the ROM
PICASSO_FWM_ROM_POSITION=$(call int-add, \
	$(call int-subtract, $(CONFIG_ROM_SIZE) \
	$(call int-shift-left, \
	0x80000 $(CONFIG_AMD_FWM_POSITION_INDEX))) 0x20000)

add_amdfw: $(obj)/coreboot.pre $(obj)/amdfw.rom
	printf "    DD         Adding AMD Firmware at ROM offset 0x%x\n" \
		"$(PICASSO_FWM_ROM_POSITION)"
	dd oflag=seek_bytes \
		if=$(obj)/amdfw.rom \
		of=$(obj)/coreboot.pre conv=notrunc \
		seek=$(PICASSO_FWM_ROM_POSITION) >/dev/null 2>&1

else # ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)

cbfs-files-y += apu/amdfw
apu/amdfw-file := $(obj)/amdfw.rom
apu/amdfw-position := $(PICASSO_FWM_POSITION)
apu/amdfw-type := raw

endif # ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)

endif # ($(CONFIG_SOC_AMD_PICASSO),y)
