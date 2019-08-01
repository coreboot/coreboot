romstage-y += romstage.c
romstage-y += memory.c

$(obj)/soc/intel/fsp_broadwell_de/romstage/romstage.romstage.o : $(obj)/build.h
