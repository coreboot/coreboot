SECTIONS {
	. = (CONFIG_ROMBASE + CONFIG_ROM_IMAGE_SIZE - 0x10) - (__id_end - __id_start);
	.id (.): {
		*(.id)
	}
}
