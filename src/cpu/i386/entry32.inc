	/* For starting linuxBIOS in protected mode */
	.text
	.align	4
	.code32
	intel_chip_post_macro(0x10)	/* post 10 */
