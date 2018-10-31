ifeq ($(CONFIG_SOC_INTEL_ICELAKE),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cpu.c
bootblock-y += bootblock/pch.c
bootblock-y += pmutil.c
bootblock-y += bootblock/report_platform.c
bootblock-y += espi.c
bootblock-y += gpio.c
bootblock-y += gspi.c
bootblock-y += i2c.c
bootblock-y += memmap.c
bootblock-y += spi.c
bootblock-y += p2sb.c
bootblock-y += uart.c

romstage-y += espi.c
romstage-y += gpio.c
romstage-y += gspi.c
romstage-y += i2c.c
romstage-y += memmap.c
romstage-y += pmutil.c
romstage-y += reset.c
romstage-y += spi.c
romstage-y += uart.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += elog.c
ramstage-y += espi.c
ramstage-y += finalize.c
ramstage-y += fsp_params.c
ramstage-y += gpio.c
ramstage-y += graphics.c
ramstage-y += gspi.c
ramstage-y += gpio.c
ramstage-y += i2c.c
ramstage-y += lockdown.c
ramstage-y += memmap.c
ramstage-y += p2sb.c
ramstage-y += pmc.c
ramstage-y += pmutil.c
ramstage-$(CONFIG_PLATFORM_USES_FSP2_0) += reset.c
ramstage-y += smmrelocate.c
ramstage-y += spi.c
ramstage-y += systemagent.c
ramstage-y += uart.c
ramstage-y += sd.c

smm-y += gpio.c
smm-y += p2sb.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += uart.c

postcar-y += memmap.c
postcar-y += pmutil.c
postcar-y += i2c.c
postcar-y += gspi.c
postcar-y += spi.c
postcar-y += uart.c

verstage-y += gspi.c
verstage-y += i2c.c
verstage-y += pmutil.c
verstage-y += spi.c
verstage-y += uart.c

CPPFLAGS_common += -I$(src)/soc/intel/icelake
CPPFLAGS_common += -I$(src)/soc/intel/icelake/include

endif
