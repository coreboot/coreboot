ramstage-y += model_f4x_init.c
subdirs-$(CONFIG_SMM_TSEG) += ../smm/gen1

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_f4x/microcode.bin
