ramstage-y += model_1067x_init.c
subdirs-y += ../../x86/name
subdirs-y += ../common
subdirs-$(CONFIG_SMM_TSEG) += ../smm/gen1

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_1067x/microcode.bin
