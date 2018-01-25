ramstage-y += model_6fx_init.c
subdirs-y += ../../x86/name
subdirs-y += ../common
ramstage-$(CONFIG_PARALLEL_MP) += ../model_1067x/mp_init.c
subdirs-$(CONFIG_SMM_TSEG) += ../smm/gen1

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_6fx/microcode.bin
