ramstage-y += model_106cx_init.c
subdirs-y += ../../x86/name
subdirs-y += ../smm/gen1
ramstage-$(CONFIG_PARALLEL_MP) += ../model_1067x/mp_init.c

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-1c-*)
