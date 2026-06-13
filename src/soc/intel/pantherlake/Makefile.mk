## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_INTEL_PANTHERLAKE_BASE),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

ifeq ($(CONFIG_SOC_INTEL_PANTHERLAKE_U_H),y)
cpu_microcode_bins += \
       3rdparty/intel-microcode/intel-ucode/06-cc-02 \
       3rdparty/intel-microcode/intel-ucode/06-cc-03
endif

# all (bootblock, verstage, romstage, postcar, ramstage)
all-y += gpio.c
all-y += isclk.c

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/pcd.c
bootblock-y += bootblock/report_platform.c

romstage-$(CONFIG_SOC_INTEL_CSE_PRE_CPU_RESET_TELEMETRY) += cse_telemetry.c
romstage-y += meminit.c
romstage-y += pcie_rp.c
romstage-y += tdp.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-$(CONFIG_SOC_INTEL_CRASHLOG) += crashlog.c
ramstage-y += elog.c
ramstage-y += fsp_params.c
ramstage-y += p2sb.c
ramstage-y += pcie_rp.c
ramstage-y += pmc.c
ramstage-y += retimer.c
ramstage-y += systemagent.c
ramstage-y += tcss.c
ramstage-y += tdp.c
ramstage-y += xhci.c
ramstage-$(CONFIG_DRIVERS_INTEL_TOUCH) += touch.c

smm-y += elog.c
smm-y += gpio.c
smm-y += p2sb.c
smm-y += xhci.c
CPPFLAGS_common += -I$(src)/soc/intel/pantherlake
CPPFLAGS_common += -I$(src)/soc/intel/pantherlake/include

# FSP repo is missing some PTL headers, so add the vendorcode headers as a
# fallback. See: https://github.com/intel/FSP/issues/129
ifeq ($(CONFIG_FSP_TYPE_IOT),y)
CPPFLAGS_common += -idirafter $(top)/src/vendorcode/intel/fsp/fsp2_0/pantherlake
endif

endif
