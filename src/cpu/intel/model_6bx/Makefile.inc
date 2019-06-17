ramstage-y += model_6bx_init.c
subdirs-y += ../../x86/name

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-0b-*)
