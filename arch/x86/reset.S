# reset vector
#
# (C) 2007 coresystems GmbH

# RVECTOR: size of reset vector, default is 0x10
# RESRVED: size of vpd code, default is 0xf0
#
# BOOTBLK: size of bootblock code, default is 0x1f00 (8k-256b)
#

SEGMENT_SIZE = 0x10000
RVECTOR      = 0x00010

	.code16
	.globl _start
_start:
	jmp SEGMENT_SIZE-(BOOTBLK+RESRVED+RVECTOR)
	
.byte	0

# date? idstring? we might want to put something else in here.
.ascii DATE

# checksum
.word  0

