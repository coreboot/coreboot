cpu_incs += $(src)/soc/intel/baytrail/romstage/cache_as_ram.inc
romstage-y += romstage.c
romstage-y += raminit.c
romstage-y += gfx.c
romstage-y += pmc.c
romstage-$(CONFIG_ENABLE_BUILTIN_COM1) += uart.c
