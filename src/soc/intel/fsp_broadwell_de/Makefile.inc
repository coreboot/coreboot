ifeq ($(CONFIG_SOC_INTEL_FSP_BROADWELL_DE),y)

subdirs-y += fsp
subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/cache
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../lib/fsp

romstage-y += gpio.c
romstage-y += memmap.c
romstage-y += tsc_freq.c
romstage-y += smbus-imc.c
romstage-y += ubox.c
romstage-y += vtd.c

postcar-y += tsc_freq.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += gpio.c
ramstage-y += iou_complto.c
ramstage-y += memmap.c
ramstage-y += northcluster.c
ramstage-y += ramstage.c
ramstage-y += smbus.c
ramstage-y += smbus_common.c
ramstage-y += smi.c
ramstage-y += southcluster.c
ramstage-y += tsc_freq.c
ramstage-y += vtd.c
ramstage-y += ubox.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += pmutil.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.c

smm-y += pmutil.c
smm-y += smihandler.c
smm-y += tsc_freq.c

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-56-*)

CPPFLAGS_common += -I$(src)/soc/intel/fsp_broadwell_de/include
CPPFLAGS_common += -I$(src)/soc/intel/fsp_broadwell_de/fsp
CPPFLAGS_common += -I$(src)/soc/intel/fsp_broadwell_de/

endif # ifeq ($(CONFIG_SOC_INTEL_FSP_BROADWELL_DE),y)
