ifeq ($(CONFIG_SOC_INTEL_ELKHARTLAKE),y)

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
ramstage-y += smmrelocate.c
ramstage-y += systemagent.c
ramstage-y += sd.c
ramstage-y += me.c

smm-y += gpio.c
smm-y += p2sb.c
smm-y += pmc.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += uart.c

verstage-y += gpio.c

CPPFLAGS_common += -I$(src)/soc/intel/elkhartlake
CPPFLAGS_common += -I$(src)/soc/intel/elkhartlake/include

endif
