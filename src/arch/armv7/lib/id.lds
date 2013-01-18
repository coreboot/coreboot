SECTIONS {
	. = CONFIG_ID_SECTION_BASE;
	.id (.): {
		*(.id)
	}
}
