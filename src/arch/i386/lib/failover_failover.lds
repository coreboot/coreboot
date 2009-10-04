	__fallback_image = (CONFIG_ROMBASE & 0xfffffff0) - 8;
	__normal_image = ((CONFIG_ROMBASE - CONFIG_FALLBACK_SIZE) & 0xfffffff0) - 8;
