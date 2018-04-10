ramstage-y += model_f3x_init.c
subdirs-$(CONFIG_SMM_TSEG) += ../smm/gen1
ramstage-$(CONFIG_PARALLEL_MP) += ../model_1067x/mp_init.c

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_f3x/microcode.bin
