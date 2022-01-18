subdirs-y += ../common

ramstage-y += model_f2x_init.c
ramstage-y += mp_init.c

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/0f-02-*)
