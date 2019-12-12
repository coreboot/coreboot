#*****************************************************************************
#
# Copyright (c) 2012, 2016-2019 Advanced Micro Devices, Inc.
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
ifeq ($(CONFIG_SOC_AMD_PICASSO),y)

subdirs-y += ../../../cpu/amd/mtrr/
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/cache
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/pae
subdirs-y += ../../../cpu/x86/smm

romstage-y += i2c.c
romstage-y += romstage.c
romstage-y += gpio.c
romstage-y += pmutil.c
romstage-y += smbus.c
romstage-y += memmap.c
romstage-$(CONFIG_PICASSO_UART) += uart.c
romstage-y += tsc_freq.c
romstage-y += southbridge.c
romstage-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c

verstage-y += gpio.c
verstage-y += i2c.c
verstage-y += pmutil.c
verstage-$(CONFIG_PICASSO_UART) += uart.c
verstage-y += tsc_freq.c

postcar-y += monotonic_timer.c
postcar-$(CONFIG_PICASSO_UART) += uart.c
postcar-y += memmap.c
postcar-$(CONFIG_VBOOT_MEASURED_BOOT) += i2c.c
postcar-y += tsc_freq.c

ramstage-y += i2c.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += mca.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += gpio.c
ramstage-y += southbridge.c
ramstage-y += northbridge.c
ramstage-y += pmutil.c
ramstage-y += acp.c
ramstage-y += sata.c
ramstage-y += sm.c
ramstage-y += smbus.c
ramstage-y += memmap.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c
ramstage-$(CONFIG_PICASSO_UART) += uart.c
ramstage-y += usb.c
ramstage-y += tsc_freq.c
ramstage-y += finalize.c

all-y += reset.c

smm-y += smihandler.c
smm-y += smi_util.c
smm-y += tsc_freq.c
smm-$(CONFIG_DEBUG_SMI) += uart.c
smm-y += gpio.c

CPPFLAGS_common += -I$(src)/soc/amd/picasso
CPPFLAGS_common += -I$(src)/soc/amd/picasso/include
CPPFLAGS_common += -I$(src)/soc/amd/picasso/acpi

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

# type = 0x5
PUBSIGNEDKEY_FILE=$(top)/$(FIRMWARE_LOCATE)/RtmPubSignedRV.key

# types = 0x8 and 0x18
PSP_SMUFW1_SUB1_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmwareRV2.csbin
PSP_SMUFW1_SUB2_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmwarePCO.csbin
PSP_SMUFW2_SUB1_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware2RV2.csbin
PSP_SMUFW2_SUB2_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware2PCO.csbin

# type = 0x9
PSP_SEC_DBG_KEY_FILE=$(top)/$(FIRMWARE_LOCATE)/RavenSecureDebug_PublicKey.bin

# type = 0xb - See #55758 (NDA) for bit definitions.
PSP_SOFTFUSE="0x0000000010000001"

ifeq ($(CONFIG_USE_PSPSCUREOS),y)
# types = 0x2, 0xc
PSPSCUREOS_FILE=$(top)/$(FIRMWARE_LOCATE)/psp_os_combined_prod_RV.sbin
PSPTRUSTLETS_FILE=$(top)/$(FIRMWARE_LOCATE)/dr_ftpm_prod_RV.csbin
endif

# type = 0x13
PSP_SEC_DEBUG_FILE=$(top)/$(FIRMWARE_LOCATE)/secure_unlock_prod_RV.sbin

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
PSP_SOFTFUSE="0x0000000030000001"
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
PSP_APCB0_FILE=$(call strip_quotes, $(CONFIG_PSP_APCB_FILE))
PSP_APCB1_FILE=$(call strip_quotes, $(CONFIG_PSP_APCB1_FILE))
PSP_APCB2_FILE=$(call strip_quotes, $(CONFIG_PSP_APCB2_FILE))
PSP_APCB3_FILE=$(call strip_quotes, $(CONFIG_PSP_APCB3_FILE))
PSP_APCB4_FILE=$(call strip_quotes, $(CONFIG_PSP_APCB4_FILE))

# type = 0x61
PSP_APOB_BASE=$(CONFIG_PSP_APOB_DESTINATION)

# type = 0x62
PSP_BIOSBIN_FILE=$(obj)/amd_biospsp.img
PSP_BIOSBIN_DEST=$(CONFIG_ROMSTAGE_ADDR)
PSP_BIOSBIN_SIZE=$(CONFIG_RAM_RESET_VECTOR_STAGE_SIZE)

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

#
# Build the arguments to amdfwtool (order is unimportant).  Missing file names
# result in empty OPT_ variables, i.e. the argument is not passed to amdfwtool.
#

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_AMD_PUBKEY_FILE=$(call add_opt_prefix, $(CONFIG_AMD_PUBKEY_FILE), --pubkey)
OPT_PSPBTLDR_FILE=$(call add_opt_prefix, $(PSPBTLDR_FILE), --bootloader)
OPT_PSPNVRAM_FILE=$(call add_opt_prefix, $(PSPNVRAM_FILE), --nvram)
OPT_PUBSIGNEDKEY_FILE=$(call add_opt_prefix, $(PUBSIGNEDKEY_FILE), --rtmpubkey)
OPT_SMUFW1_SUB1_FILE=$(call add_opt_prefix, $(PSP_SMUFW1_SUB1_FILE), --subprogram 1 --smufirmware)
OPT_SMUFW1_SUB2_FILE=$(call add_opt_prefix, $(PSP_SMUFW1_SUB2_FILE), --subprogram 2 --smufirmware)
OPT_SMUFW2_SUB1_FILE=$(call add_opt_prefix, $(PSP_SMUFW2_SUB1_FILE), --subprogram 1 --smufirmware2)
OPT_SMUFW2_SUB2_FILE=$(call add_opt_prefix, $(PSP_SMUFW2_SUB2_FILE), --subprogram 2 --smufirmware2)
OPT_PSP_SEC_DBG_KEY_FILE=$(call add_opt_prefix, $(PSP_SEC_DBG_KEY_FILE), --securedebug)
OPT_PSP_SOFTFUSE=$(call add_opt_prefix, $(PSP_SOFTFUSE), --soft-fuse)
OPT_PSPSCUREOS_FILE=$(call add_opt_prefix, $(PSPSCUREOS_FILE), --secureos)
OPT_PSPTRUSTLETS_FILE=$(call add_opt_prefix, $(PSPTRUSTLETS_FILE), --trustlets)
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

OPT_PSP_APCB0_FILE=$(call add_opt_prefix, $(PSP_APCB0_FILE), --instance 0 --apcb)
OPT_PSP_APCB1_FILE=$(call add_opt_prefix, $(PSP_APCB1_FILE), --instance 1 --apcb)
OPT_PSP_APCB2_FILE=$(call add_opt_prefix, $(PSP_APCB2_FILE), --instance 2 --apcb)
OPT_PSP_APCB3_FILE=$(call add_opt_prefix, $(PSP_APCB3_FILE), --instance 3 --apcb)
OPT_PSP_APCB4_FILE=$(call add_opt_prefix, $(PSP_APCB4_FILE), --instance 4 --apcb)
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

$(obj)/amdfw.rom:	$(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)) \
			$(call strip_quotes, $(PUBSIGNEDKEY_FILE)) \
			$(call strip_quotes, $(PSPBTLDR_FILE)) \
			$(call strip_quotes, $(PSPSCUREOS_FILE)) \
			$(call strip_quotes, $(PSP_SEC_DBG_KEY_FILE)) \
			$(call strip_quotes, $(PSPTRUSTLETS_FILE)) \
			$(call strip_quotes, $(PSP_APCB0_FILE)) \
			$(call strip_quotes, $(PSP_APCB1_FILE)) \
			$(call strip_quotes, $(PSP_APCB2_FILE)) \
			$(call strip_quotes, $(PSP_APCB3_FILE)) \
			$(call strip_quotes, $(PSP_APCB4_FILE)) \
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
			$(AMDFWTOOL)
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(OPT_AMD_PUBKEY_FILE) \
		$(OPT_PSPBTLDR_FILE) \
		$(OPT_PUBSIGNEDKEY_FILE) \
		$(OPT_PSPSCUREOS_FILE) \
		$(OPT_PSP_SEC_DBG_KEY_FILE) \
		$(OPT_PSPTRUSTLETS_FILE) \
		$(OPT_SMUFW1_SUB2_FILE) \
		$(OPT_SMUFW2_SUB2_FILE) \
		$(OPT_SMUFW1_SUB1_FILE) \
		$(OPT_SMUFW2_SUB1_FILE) \
		$(OPT_PSP_APCB0_FILE) \
		$(OPT_PSP_APCB1_FILE) \
		$(OPT_PSP_APCB2_FILE) \
		$(OPT_PSP_APCB3_FILE) \
		$(OPT_PSP_APCB4_FILE) \
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
		--token-unlock \
		--flashsize $(CONFIG_ROM_SIZE) \
		--location $(shell printf "0x%x" $(PICASSO_FWM_POSITION)) \
		--output	$@

USE_BIOS_FILE=$(obj)/cbfs/fallback/romstage.elf
$(PSP_BIOSBIN_FILE): $(obj)/cbfs/fallback/romstage.elf $(AMDCOMPRESS)
	rm -f $@
	@printf "    AMDCOMPRS  $(subst $(obj)/,,$(@))\n"
	$(AMDCOMPRESS) --infile $(USE_BIOS_FILE) --outfile $@ --compress \
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
