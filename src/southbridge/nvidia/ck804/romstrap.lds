SECTIONS {
	. = (_ROMBASE + ROM_IMAGE_SIZE - 0x10) - (__romstrap_end - __romstrap_start);
	.romstrap (.): {
		*(.romstrap)
	}
}
