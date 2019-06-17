ifeq ($(CONFIG_SOC_INTEL_COMMON_SKYLAKE_BASE),y)

subdirs-y += nhlt
subdirs-y += romstage
subdirs-y += ../../../cpu/intel/common
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc

bootblock-y += bootblock/bootblock.c
bootblock-$(CONFIG_FSP_CAR) += fspcar.c
bootblock-y += bootblock/cpu.c
bootblock-y += i2c.c
bootblock-y += bootblock/pch.c
bootblock-y += bootblock/report_platform.c
bootblock-y += gpio.c
bootblock-y += gspi.c
bootblock-y += p2sb.c
bootblock-y += pmutil.c
bootblock-y += spi.c
bootblock-y += lpc.c
bootblock-y += uart.c

verstage-y += gspi.c
verstage-y += pmutil.c
verstage-y += i2c.c
verstage-y += spi.c
verstage-y += uart.c

romstage-y += gpio.c
romstage-y += gspi.c
romstage-y += i2c.c
romstage-y += memmap.c
romstage-y += me.c
romstage-y += pmc.c
romstage-y += pmutil.c
romstage-$(CONFIG_PLATFORM_USES_FSP2_0) += reset.c
romstage-y += spi.c
romstage-y += uart.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_PLATFORM_USES_FSP1_1) += chip.c
ramstage-$(CONFIG_PLATFORM_USES_FSP2_0) += chip_fsp20.c
ramstage-y += cpu.c
ramstage-y += elog.c
ramstage-y += finalize.c
ramstage-y += gpio.c
ramstage-y += gspi.c
ramstage-y += i2c.c
ramstage-y += graphics.c
ramstage-y += irq.c
ramstage-y += lockdown.c
ramstage-y += lpc.c
ramstage-y += me.c
ramstage-y += memmap.c
ramstage-y += p2sb.c
ramstage-y += pmc.c
ramstage-y += pmutil.c
ramstage-$(CONFIG_PLATFORM_USES_FSP2_0) += reset.c
ramstage-y += sd.c
ramstage-y += smmrelocate.c
ramstage-y += spi.c
ramstage-y += systemagent.c
ramstage-y += thermal.c
ramstage-y += uart.c
ramstage-y += vr_config.c

smm-y += elog.c
smm-y += gpio.c
smm-y += p2sb.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += uart.c

postcar-y += memmap.c
postcar-y += gspi.c
postcar-y += spi.c
postcar-y += i2c.c
postcar-y += uart.c

ifeq ($(CONFIG_SKYLAKE_SOC_PCH_H),y)
# Skylake H Q0
cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_506ex/microcode.bin
# Kabylake HB0
cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_906ex/microcode.bin
else
# Skylake D0
cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_406ex/microcode.bin
# Kabylake H0, Y0
cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_806ex/microcode.bin
endif
# Missing for Skylake C0 (0x406e2), Kabylake G0 (0x406e8), Kabylake HA0 (0x506e8)
# since those are probably pre-release samples.

CPPFLAGS_common += -I$(src)/soc/intel/skylake
CPPFLAGS_common += -I$(src)/soc/intel/skylake/include

ifeq ($(CONFIG_PLATFORM_USES_FSP1_1),y)
CPPFLAGS_common += -I$(src)/soc/intel/skylake/include/fsp11
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp1_1/skylake
else
CPPFLAGS_common += -I$(src)/soc/intel/skylake/include/fsp20
endif

# Currently used for microcode path.
CPPFLAGS_common += -I3rdparty/blobs/mainboard/$(MAINBOARDDIR)

endif
