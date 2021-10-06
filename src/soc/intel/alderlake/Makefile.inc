ifeq ($(CONFIG_SOC_INTEL_ALDERLAKE),y)
subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr

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

romstage-y += espi.c
romstage-y += gpio.c
romstage-y += meminit.c
romstage-y += pcie_rp.c
romstage-y += reset.c

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
ramstage-y += soundwire.c
ramstage-y += systemagent.c
ramstage-y += vr_config.c
ramstage-y += xhci.c
ramstage-$(CONFIG_SOC_INTEL_CRASHLOG) += crashlog.c

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

$(call cse_add_dummy_to_bp1_bp2,DLMP)
$(call cse_add_dummy_to_bp1_bp2,IFPP)
$(call cse_add_dummy_to_bp1_bp2,SBDT)
$(call cse_add_decomp_to_bp1_bp2,RBEP)
$(call cse_add_dummy_to_bp1_bp2,UFSP)
$(call cse_add_dummy_to_bp1_bp2,UFSG)
$(call cse_add_dummy_to_bp1_bp2,OEMP)
$(call cse_add_input_to_bp1_bp2,PMCP)
$(call cse_add_decomp,bp1,MFTP)
$(call cse_add_decomp,bp2,FTPR)
$(call cse_add_input_to_bp1_bp2,IOMP)
$(call cse_add_input_to_bp1_bp2,NPHY)
$(call cse_add_input_to_bp1_bp2,TBTP)
$(call cse_add_input_to_bp1_bp2,PCHC)
$(call cse_add_decomp,bp2,NFTP)
$(call cse_add_dummy,bp2,ISHP)
$(call cse_add_input,bp2,IUNP)

endif

endif
