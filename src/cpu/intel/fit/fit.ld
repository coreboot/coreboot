SECTIONS {
	. = 0xffffffc0;
	.fit_pointer (.): {
		KEEP(*(.fit_pointer))
	}
}
