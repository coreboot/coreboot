	.section ".id", "a", @progbits

#define __STR(X) #X
#define STR(X) __STR(X)

	.globl __id_start
__id_start:
vendor:	
	.asciz STR(MAINBOARD_VENDOR)
part:		
	.asciz STR(MAINBOARD_PART_NUMBER)
.long __id_end + 0x10 - vendor  /* Reverse offset to the vendor id */
.long __id_end + 0x10 - part    /* Reverse offset to the part number */
.long PAYLOAD_SIZE + ROM_IMAGE_SIZE  /* Size of this romimage */
	.globl __id_end

#undef __STR
#undef STR

__id_end:
.previous
