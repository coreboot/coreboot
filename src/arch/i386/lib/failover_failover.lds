	__fallback_image = (CONFIG_ROM_PAYLOAD_START & 0xfffffff0) - 8;
	__normal_image = ((CONFIG_ROM_PAYLOAD_START - CONFIG_FALLBACK_SIZE) & 0xfffffff0) - 8;
