## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_ALDERLAKE),y)
subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

# all (bootblock, verstage, romstage, postcar, ramstage)
all-y += gspi.c
all-y += i2c.c
all-y += pmutil.c
all-y += spi.c
all-y += uart.c

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/pch.c
bootblock-y += bootblock/report_platform.c
bootblock-y += espi.c
bootblock-y += p2sb.c
bootblock-$(CONFIG_ALDERLAKE_CONFIGURE_DESCRIPTOR) += bootblock/update_descriptor.c

romstage-$(CONFIG_SOC_INTEL_CSE_PRE_CPU_RESET_TELEMETRY) += cse_telemetry.c
romstage-y += espi.c
romstage-y += meminit.c
romstage-y += pcie_rp.c
romstage-y += reset.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += elog.c
ramstage-y += espi.c
ramstage-y += finalize.c
ramstage-y += fsp_params.c
ramstage-y += graphics.c
ramstage-y += hsphy.c
ramstage-y += lockdown.c
ramstage-y += p2sb.c
ramstage-y += pcie_rp.c
ramstage-y += pmc.c
ramstage-y += reset.c
ramstage-$(CONFIG_SOC_INTEL_ALDERLAKE_TCSS_USB4_SUPPORT) += retimer.c
ramstage-y += soundwire.c
ramstage-y += systemagent.c
ramstage-y += tcss.c
ramstage-y += vr_config.c
ramstage-y += xhci.c
ramstage-$(CONFIG_SOC_INTEL_CRASHLOG) += crashlog.c

smm-y += elog.c
smm-y += p2sb.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += uart.c
smm-y += xhci.c

ifeq ($(CONFIG_SOC_INTEL_ALDERLAKE_PCH_S),y)
bootblock-y += gpio_pch_s.c
romstage-y += gpio_pch_s.c
ramstage-y += gpio_pch_s.c
smm-y += gpio_pch_s.c
verstage-y += gpio_pch_s.c
else
bootblock-y += gpio.c
romstage-y += gpio.c
ramstage-y += gpio.c
smm-y += gpio.c
verstage-y += gpio.c
endif

CPPFLAGS_common += -I$(src)/soc/intel/alderlake
CPPFLAGS_common += -I$(src)/soc/intel/alderlake/include

# Include the missing MemInfoHob.h from vendorcode
ifeq ($(CONFIG_SOC_INTEL_RAPTORLAKE_PCH_S)$(CONFIG_FSP_TYPE_IOT),yy)
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp2_0/iot/raptorlake_s
endif

ifeq ($(CONFIG_SOC_INTEL_ALDERLAKE_PCH_S),y)
# 06-97-00, 06-97-01, 06-97-04 are ADL-S Engineering Samples
# 06-97-02 are ADL-S/HX Quality Samples but also ADL-HX Engineering Samples
# 06-b7-00 are RPL-S Engineering Samples
# ADL-S/HX C0/H0 and RPL-S C0/H0
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-97-05
# RPL-S/HX B0
cpu_microcode_bins += 3rdparty/blobs/soc/intel/raptorlake/06-b7-01
else ifeq ($(CONFIG_SOC_INTEL_ALDERLAKE_PCH_N),y)
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-be-00
else
# 06-9a-00, 06-9a-01 are ADL-P/ADL-M Engineering Samples
# Missing 06-9a-02 ADL-P K0
# ADL-P L0, ADL-P R0 and ADL-M R0
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-9a-04
# RPL-P/H J0, RPL-U Q0
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-ba-02
endif

ifeq ($(CONFIG_STITCH_ME_BIN),y)

$(eval $(call cse_add_dummy_to_bp1_bp2,DLMP))
$(eval $(call cse_add_dummy_to_bp1_bp2,IFPP))
$(eval $(call cse_add_dummy_to_bp1_bp2,SBDT))
$(eval $(call cse_add_decomp_to_bp1_bp2,RBEP))
$(eval $(call cse_add_dummy_to_bp1_bp2,UFSP))
$(eval $(call cse_add_dummy_to_bp1_bp2,UFSG))
$(eval $(call cse_add_input_to_bp1_bp2,OEMP))
$(eval $(call cse_add_input_to_bp1_bp2,PMCP))
$(eval $(call cse_add_decomp,bp1,MFTP))
$(eval $(call cse_add_decomp,bp2,FTPR))
$(eval $(call cse_add_input_to_bp1_bp2,IOMP))
$(eval $(call cse_add_input_to_bp1_bp2,NPHY))
$(eval $(call cse_add_input_to_bp1_bp2,TBTP))
$(eval $(call cse_add_input_to_bp1_bp2,PCHC))
$(eval $(call cse_add_decomp,bp2,NFTP))
$(eval $(call cse_add_dummy,bp2,ISHP))
$(eval $(call cse_add_input,bp2,IUNP))

endif

ifeq ($(CONFIG_INCLUDE_HSPHY_IN_FMAP),y)
ifneq ($(call strip_quotes,$(CONFIG_HSPHY_FW_FILE)),)

# Create the target HSPHY file that will be put into flashmap region.
# First goes the HSPHY size, then hash algorithm (3 - SHA384, default for now),
# the hash digest, padding to max digest size (SHA512 - 64 bytes) and at last the
# HSPHY firmware itself
$(obj)/hsphy_fw.bin: $(call strip_quotes,$(top)/$(CONFIG_HSPHY_FW_FILE))
	printf "    HSPHY      $(obj)/hsphy_fw.bin\n"
	$(shell wc -c $< | awk '{print $$1}' | tr -d '\n' | xargs -0 printf '%08X' | \
			 tac -rs .. | xxd -r -p > $@)
	$(shell printf '%02X' 3 | xxd -r -p >> $@)
	$(shell sha384sum $< | awk '{print $$1}' | tac -rs .. | xxd -r -p >> $@)
	$(shell dd if=/dev/zero bs=1 count=16 2> /dev/null >> $@)
	$(shell cat $< >> $@)

add_hsphy_firmware: $(obj)/hsphy_fw.bin $(obj)/fmap.fmap $(obj)/coreboot.pre $(CBFSTOOL)
	$(CBFSTOOL) $(obj)/coreboot.pre write -u -r HSPHY_FW -f $(obj)/hsphy_fw.bin

$(call add_intermediate, add_hsphy_firmware)

endif
endif

endif
