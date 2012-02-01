driver-y += model_f2x_init.c

cpu_incs-$(CONFIG_CACHE_AS_RAM) += $(src)/cpu/intel/model_f2x/cache_as_ram.inc

