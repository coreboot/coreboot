ifeq ($(CONFIG_SOC_INTEL_TIGERLAKE_BASE),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc

# all (bootblock, verstage, romstage, postcar, ramstage)
all-y += gspi.c
all-y += i2c.c
all-y += pmutil.c
all-y += spi.c
all-y += uart.c

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cpu.c
bootblock-y += bootblock/pch.c
bootblock-y += bootblock/report_platform.c
bootblock-y += espi.c
bootblock-$(CONFIG_SOC_INTEL_TIGERLAKE) += gpio_tgl.c
bootblock-$(CONFIG_SOC_INTEL_JASPERLAKE) += gpio_jsl.c
bootblock-y += p2sb.c

romstage-y += espi.c
romstage-$(CONFIG_SOC_INTEL_TIGERLAKE) += meminit_tgl.c
romstage-$(CONFIG_SOC_INTEL_JASPERLAKE) += meminit_jsl.c
romstage-$(CONFIG_SOC_INTEL_TIGERLAKE) += gpio_tgl.c
romstage-$(CONFIG_SOC_INTEL_JASPERLAKE) += gpio_jsl.c
romstage-y += reset.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += elog.c
ramstage-y += espi.c
ramstage-y += finalize.c
ramstage-$(CONFIG_SOC_INTEL_TIGERLAKE) += fsp_params_tgl.c
ramstage-$(CONFIG_SOC_INTEL_JASPERLAKE) += fsp_params_jsl.c
ramstage-$(CONFIG_SOC_INTEL_TIGERLAKE) += gpio_tgl.c
ramstage-$(CONFIG_SOC_INTEL_JASPERLAKE) += gpio_jsl.c
ramstage-y += graphics.c
ramstage-y += lockdown.c
ramstage-y += p2sb.c
ramstage-y += pmc.c
ramstage-y += reset.c
ramstage-y += smmrelocate.c
ramstage-y += systemagent.c
ramstage-y += sd.c

smm-$(CONFIG_SOC_INTEL_TIGERLAKE) += gpio_tgl.c
smm-$(CONFIG_SOC_INTEL_JASPERLAKE) += gpio_jsl.c
smm-y += p2sb.c
smm-y += pmc.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += uart.c

verstage-$(CONFIG_SOC_INTEL_TIGERLAKE) += gpio_tgl.c

CPPFLAGS_common += -I$(src)/soc/intel/tigerlake
CPPFLAGS_common += -I$(src)/soc/intel/tigerlake/include

endif
