SECTIONS {
	. = (0x100000000 - CONFIG_ID_SECTION_OFFSET) - (__id_end - __id_start);
	.id (.): {
		*(.id)
	}
}
