ramstage-y += model_1067x_init.c
ramstage-$(CONFIG_PARALLEL_MP) += mp_init.c
subdirs-y += ../../x86/name
subdirs-y += ../smm/gen1

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-17-*)
