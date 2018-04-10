ramstage-y += model_6ex_init.c
subdirs-y += ../../x86/name
subdirs-y += ../common
subdirs-$(CONFIG_SMM_TSEG) += ../smm/gen1
ramstage-y += ../model_1067x/mp_init.c

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_6ex/microcode.bin
