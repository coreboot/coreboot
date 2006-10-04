	__fallback_image = (CONFIG_ROM_STREAM_START & 0xfffffff0) - 8;
	__normal_image = ((CONFIG_ROM_STREAM_START - FALLBACK_SIZE) & 0xfffffff0) - 8;
