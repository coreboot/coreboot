################################################################################
## Subdirectories
################################################################################
subdirs-y += amd
subdirs-y += intel
subdirs-y += via


################################################################################
## Rules for building the microcode blob in CBFS
################################################################################

ifeq ($(CONFIG_CPU_MICROCODE_CBFS_GENERATE),y)
###########################
# FIXME FIXME FIXME FIXME #
###########################
SRC_PATH = src/cpu/intel/

FLAGS = -I $(CONFIG_MICROCODE_INCLUDE_PATH) -include $(obj)/config.h
$(obj)/cpu_microcode_blob.o: $(SRC_PATH)/microcode_blob.c
	$(CC) $(FLAGS) -MMD -c -o $@ $<

$(obj)/cpu_microcode_blob.bin: $(obj)/cpu_microcode_blob.o
	$(OBJCOPY) -j .data -O binary $< $@

-include $(obj)/microcode_blob.d
endif