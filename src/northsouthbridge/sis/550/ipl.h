#define PCI_COMMAND_PORT	0xcf8
#define PCI_DATA_PORT		0xcfc

#define NORTH_BRIDGE_BASE_ADDR	0x80000000
#define LPC_BRIDGE_BASE_ADDR	0x80000800

#define SMB_BASE_ADDR		0x5080

#define DOC_WIN_SEG   		0xfe00
#define DOC_STACK_SEG		0x0400
#define SPL_RAM_SEG		0x8000

#if (USE_DOC_2000_TSOP == 1)
#define DOC_SPL_START_PAGE	4	/* 0-3 for IPL (each of 1KB size) */
#else /* defined (USE_DOC_MIL) */
#define DOC_SPL_START_PAGE	2	/* 0,1 for IPL (each of 512B size) */
#endif

#define DOC_SPL_SIZE_IN_PAGE	128 - DOC_SPL_START_PAGE    /* 1 page = 512 bytes, total 63kB */

#define RET_LABEL(label)	\
	jmp label##_done

#define CALL_LABEL(label)	\
	jmp label		;\
label##_done:

#define CALL_SP(func) \
	lea	0f, %sp	; \
	jmp func	; \
0:

#define RET_SP \
	jmp *%sp

#define CALL_BP(func) \
	lea	0f, %bp	; \
	jmp func	; \
0:

#define RET_BP \
	jmp *%bp
