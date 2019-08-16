cpu_incs-y += $(src)/cpu/intel/car/non-evict/cache_as_ram.S
cpu_incs-y += $(obj)/fmap_config.h

romstage-y += ../../../../cpu/intel/car/romstage.c
romstage-y += romstage.c
romstage-y += raminit.c
romstage-$(CONFIG_ENABLE_BUILTIN_COM1) += uart.c
romstage-y += gfx.c
romstage-y += pmc.c
