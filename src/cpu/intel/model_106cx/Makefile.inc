ramstage-y += model_106cx_init.c
subdirs-y += ../../x86/name
subdirs-y += ../common
subdirs-$(CONFIG_SMM_TSEG) += ../smm/gen1
ramstage-$(CONFIG_PARALLEL_MP) += ../model_1067x/mp_init.c

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_106cx/microcode.bin
