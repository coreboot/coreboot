## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_COMMON_SKYLAKE_BASE),y)

subdirs-y += nhlt
subdirs-y += romstage
subdirs-y += ../../../cpu/intel/common
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

bootblock-y += bootblock/bootblock.c
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

verstage-y += gpio.c
verstage-y += gspi.c
verstage-y += pmutil.c
verstage-y += i2c.c
verstage-y += spi.c
verstage-y += uart.c

romstage-y += gpio.c
romstage-y += gspi.c
romstage-y += i2c.c
romstage-y += me.c
romstage-y += pmutil.c
romstage-y += reset.c
romstage-y += spi.c
romstage-y += uart.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += elog.c
ramstage-y += fadt.c
ramstage-y += finalize.c
ramstage-y += gpio.c
ramstage-y += gspi.c
ramstage-y += i2c.c
ramstage-y += graphics.c
ramstage-y += irq.c
ramstage-y += lockdown.c
ramstage-y += lpc.c
ramstage-y += me.c
ramstage-y += p2sb.c
ramstage-y += pmc.c
ramstage-y += pmutil.c
ramstage-y += reset.c
ramstage-y += sd.c
ramstage-y += spi.c
ramstage-y += systemagent.c
ramstage-y += uart.c
ramstage-y += vr_config.c
ramstage-y += xhci.c

smm-y += elog.c
smm-y += gpio.c
smm-y += p2sb.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += uart.c
smm-y += xhci.c

postcar-y += gspi.c
postcar-y += spi.c
postcar-y += i2c.c
postcar-y += uart.c

ifeq ($(CONFIG_SKYLAKE_SOC_PCH_H),y)
ifeq ($(CONFIG_MAINBOARD_SUPPORTS_SKYLAKE_CPU),y)
# Skylake H Q0
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-5e-03
endif
ifeq ($(CONFIG_MAINBOARD_SUPPORTS_KABYLAKE_CPU),y)
# Kabylake H B0 S0
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-9e-09
endif
# CoffeeLake
ifeq ($(CONFIG_MAINBOARD_SUPPORTS_COFFEELAKE_CPU),y)
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-9e-0a
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-9e-0b
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-9e-0c
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-9e-0d
endif
else
ifeq ($(CONFIG_MAINBOARD_SUPPORTS_SKYLAKE_CPU),y)
# Skylake D0
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-4e-03
endif
ifeq ($(CONFIG_MAINBOARD_SUPPORTS_KABYLAKE_DUAL),y)
# Kabylake H0, J0, J1
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-8e-09
endif
ifeq ($(CONFIG_MAINBOARD_SUPPORTS_KABYLAKE_QUAD),y)
# Kabylake Y0
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-8e-0a
endif
endif
# Missing for Skylake C0 (0x406e2), Kabylake G0 (0x406e8), Kabylake HA0 (0x506e8)
# since those are probably pre-release samples.

CPPFLAGS_common += -I$(src)/soc/intel/skylake
CPPFLAGS_common += -I$(src)/soc/intel/skylake/include

# Currently used for microcode path.
CPPFLAGS_common += -I3rdparty/blobs/mainboard/$(MAINBOARDDIR)

endif
