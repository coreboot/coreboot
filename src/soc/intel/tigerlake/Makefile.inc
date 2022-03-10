ifeq ($(CONFIG_SOC_INTEL_TIGERLAKE),y)

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

romstage-y += espi.c
romstage-y += meminit.c
romstage-y += reset.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += dptf.c
ramstage-y += elog.c
ramstage-y += espi.c
ramstage-y += finalize.c
ramstage-y += fsp_params.c
ramstage-y += graphics.c
ramstage-y += lockdown.c
ramstage-y += lpm.c
ramstage-y += me.c
ramstage-y += p2sb.c
ramstage-y += pcie_rp.c
ramstage-y += pmc.c
ramstage-y += reset.c
ramstage-y += retimer.c
ramstage-y += soundwire.c
ramstage-y += systemagent.c
ramstage-y += tcss.c
ramstage-y += xhci.c
ramstage-$(CONFIG_SOC_INTEL_CRASHLOG) += crashlog_lib.c

smm-y += p2sb.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += uart.c
smm-y += elog.c
smm-y += xhci.c

ifeq ($(CONFIG_SOC_INTEL_TIGERLAKE_PCH_H),y)
bootblock-y += gpio_pch_h.c
romstage-y += gpio_pch_h.c
ramstage-y += gpio_pch_h.c
smm-y += gpio_pch_h.c
verstage-y += gpio_pch_h.c
else
bootblock-y += gpio.c
romstage-y += gpio.c
ramstage-y += gpio.c
smm-y += gpio.c
verstage-y += gpio.c
endif

CPPFLAGS_common += -I$(src)/soc/intel/tigerlake
CPPFLAGS_common += -I$(src)/soc/intel/tigerlake/include

ifeq ($(CONFIG_SOC_INTEL_TIGERLAKE_PCH_H),y)
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-8d-01
else
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-8c-01
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-8c-02
endif

endif
