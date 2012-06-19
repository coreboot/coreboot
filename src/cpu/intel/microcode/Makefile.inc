ramstage-y += microcode.c


ifeq ($(CONFIG_MICROCODE_IN_CBFS),y)

SRC_PATH = src/cpu/intel/microcode
FLAGS = -I $(CONFIG_MICROCODE_INCLUDE_PATH) -include $(obj)/config.h
$(obj)/microcode_blob.o: $(SRC_PATH)/microcode_blob.c
	$(CC) $(FLAGS) -MMD -c -o $@ $<

$(obj)/microcode_blob.bin: $(obj)/microcode_blob.o
	objcopy -j .data -O binary $< $@

-include $(obj)/microcode_blob.d
endif
