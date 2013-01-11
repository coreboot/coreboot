SECTIONS {
	/* FIXME: determine a sensible location... */
	. = (0x2026400);
	.id (.): {
		*(.id)
	}
}
