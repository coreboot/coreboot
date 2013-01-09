SECTIONS {
	/* FIXME: determine a sensible location... */
	. = (0x2024000) - (__id_end - __id_start);
	.id (.): {
		*(.id)
	}
}
