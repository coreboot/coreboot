SECTIONS {
	. = (CONFIG_ROMBASE + CONFIG_ROM_IMAGE_SIZE - CONFIG_ID_SECTION_OFFSET) - (__id_end - __id_start);
	.id (.): {
		*(.id)
	}
}
