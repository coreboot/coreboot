ramstage-y += model_106cx_init.c
subdirs-y += ../../x86/name

cpu_incs-y += $(src)/cpu/intel/car/cache_as_ram_ht.inc
cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_106cx/microcode.bin
