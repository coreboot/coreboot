ramstage-y += model_6fx_init.c
subdirs-y += ../../x86/name
subdirs-y += ../common
ramstage-$(CONFIG_PARALLEL_MP) += ../model_1067x/mp_init.c
subdirs-y += ../smm/gen1

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-0f-*) \
	3rdparty/intel-microcode/intel-ucode/06-16-01
