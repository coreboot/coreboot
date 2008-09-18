
	.section ".id", "a", @progbits

	.globl __id_start
__id_start:
vendor:
	.asciz MAINBOARD_VENDOR
part:
	.asciz MAINBOARD_PART_NUMBER
.long __id_end + 0x80 - vendor  /* Reverse offset to the vendor id */
.long __id_end + 0x80 - part    /* Reverse offset to the part number */
.long PAYLOAD_SIZE + ROM_IMAGE_SIZE  /* Size of this romimage */
	.globl __id_end

__id_end:
.previous
