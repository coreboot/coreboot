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
bootblock-y += gpio.c
bootblock-y += p2sb.c
bootblock-$(CONFIG_ALDERLAKE_A0_CONFIGURE_PMC_DESCRIPTOR) += bootblock/pmc_descriptor.c

romstage-y += espi.c
romstage-y += gpio.c
romstage-y += meminit.c
romstage-y += pcie_rp.c
romstage-y += reset.c
romstage-y += cpu.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += dptf.c
ramstage-y += elog.c
ramstage-y += espi.c
ramstage-y += finalize.c
ramstage-y += fsp_params.c
ramstage-y += gpio.c
ramstage-y += lockdown.c
ramstage-y += me.c
ramstage-y += p2sb.c
ramstage-y += pcie_rp.c
ramstage-y += pmc.c
ramstage-y += reset.c
ramstage-y += retimer.c
ramstage-y += soundwire.c
ramstage-y += systemagent.c
ramstage-y += tcss.c
ramstage-y += vr_config.c
ramstage-y += xhci.c
ramstage-$(CONFIG_SOC_INTEL_CRASHLOG) += crashlog.c

verstage-y += gpio.c

smm-y += elog.c
smm-y += gpio.c
smm-y += p2sb.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += uart.c
smm-y += xhci.c

CPPFLAGS_common += -I$(src)/soc/intel/alderlake
CPPFLAGS_common += -I$(src)/soc/intel/alderlake/include

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

endif
