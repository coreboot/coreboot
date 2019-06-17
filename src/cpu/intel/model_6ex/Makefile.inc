ramstage-y += model_6ex_init.c
subdirs-y += ../../x86/name
subdirs-y += ../common
subdirs-$(CONFIG_SMM_TSEG) += ../smm/gen1
ramstage-y += ../model_1067x/mp_init.c

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-0e-*)
