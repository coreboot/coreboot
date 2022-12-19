## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_JASPERLAKE),y)

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

romstage-y += espi.c
romstage-y += gpio.c
romstage-y += meminit.c
romstage-y += reset.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += elog.c
ramstage-y += espi.c
ramstage-y += finalize.c
ramstage-y += fsp_params.c
ramstage-y += gpio.c
ramstage-y += lockdown.c
ramstage-y += p2sb.c
ramstage-y += pmc.c
ramstage-y += reset.c
ramstage-y += systemagent.c
ramstage-y += sd.c
ramstage-y += me.c
ramstage-y += xhci.c

smm-y += gpio.c
smm-y += p2sb.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += uart.c
smm-y += elog.c
smm-y += xhci.c

verstage-y += gpio.c

CPPFLAGS_common += -I$(src)/soc/intel/jasperlake
CPPFLAGS_common += -I$(src)/soc/intel/jasperlake/include

cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-9c-00

endif
