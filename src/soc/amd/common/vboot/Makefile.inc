ifeq ($(CONFIG_VBOOT_VBNV_CMOS),y)
bootblock-y += vbnv_cmos.c
verstage-y += vbnv_cmos.c
romstage-y += vbnv_cmos.c
ramstage-y += vbnv_cmos.c
endif

bootblock-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += vboot_bootblock.c
bootblock-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += transfer_buffer.c
smm-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += transfer_buffer.c

# We don't use the early RAM memlayout linker for SMM, so we don't have access
# to the early RAM symbols. We manually generate a file that defines the symbols
# for us.
$(obj)/smm/early_ram_symbols.ld: $(objcbfs)/bootblock.map $(src)/soc/amd/common/vboot/early_ram_symbols.awk
	awk -f $(src)/soc/amd/common/vboot/early_ram_symbols.awk -- "$<" > "$@"

$(obj)/smm/smm.elf: $(obj)/smm/early_ram_symbols.ld
$(obj)/smm/smm.elf-ldflags += -T $(obj)/smm/early_ram_symbols.ld
