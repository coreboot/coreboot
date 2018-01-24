ramstage-y += model_6fx_init.c
subdirs-y += ../../x86/name
subdirs-y += ../common
subdirs-$(CONFIG_SMM_TSEG) += ../smm/gen1

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_6fx/microcode.bin
