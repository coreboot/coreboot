#define DOC_WIN_SEG		0xfe00
#define DOC_STACK_SEG		0x0400
#define SPL_RAM_SEG		0x8000

#define DOC_SPL_START_BLK	2	/* 0,1 for IPL */
#define DOC_SPL_SIZE_IN_PAGE	126     /* 1 page = 512 bytes, total 63kB */


#define DOC_STACK		0x04000
#define SPL_RAM		0x80000


#define CALL_SP(func) lea 0f, %sp ; jmp func ; 0:

#define RET_SP 	jmp *%sp


