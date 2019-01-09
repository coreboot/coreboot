	gdtptr16_offset = gdtptr16 & 0xffff;
	nullidt_offset = nullidt & 0xffff;
	ap_sipi_vector_in_rom = (_start16bit >> 12) & 0xff;
