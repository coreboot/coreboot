	.section ".id", "a", @progbits

	.globl __id_start
__id_start:
vendor:	
	.asciz CONFIG_MAINBOARD_VENDOR
part:		
	.asciz CONFIG_MAINBOARD_PART_NUMBER
.long __id_end + 0x10 - vendor  /* Reverse offset to the vendor id */
.long __id_end + 0x10 - part    /* Reverse offset to the part number */
.long CONFIG_PAYLOAD_SIZE + CONFIG_ROM_IMAGE_SIZE  /* Size of this romimage */
	.globl __id_end

__id_end:
.previous
