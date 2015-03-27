SECTIONS {
	. = 0xffffffc0;
	.fit_pointer (.): {
		*(.fit_pointer)
	}
}
