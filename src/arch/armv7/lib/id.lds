SECTIONS {
/*	. = (0x2024000) - (__id_end - __id_start); */
	. = (0x2026400);
	.id (.): {
		*(.id)
	}
}
