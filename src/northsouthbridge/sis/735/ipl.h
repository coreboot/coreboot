#define PCI_COMMAND_PORT	0xcf8
#define PCI_DATA_PORT		0xcfc

#define NORTH_BRIDGE_BASE_ADDR	0x80000000	/* Bus 0, Device 0, Function 0 */
#define LPC_BRIDGE_BASE_ADDR	0x80001000	/* Bus 0, Device 2, Function 0 */
#define SMBUS_HOST_BASE_ADDR	0x80001100	/* Bus 0, Device 2, Function 1 */

#define ACPI_BASE_ADDR		0x5000
#define SMB_BASE_ADDR		0x5080

/* DDR Initialization Register and Command */
#define DRAM_INIT_REG		0x5c
#define DQS_ENABLE		0x02
#define DDR_CMD_ISSUE		0x08
#define NO_OP			0x00
#define PALL			0x10
#define MRS			0x20
#define REF			0x30
#define DLL_EN_N		0x40
#define DLL_EN_W		0x50
#define DLL_RESET		0x60
#define DLL_DISABLE		0x70

#define DOC_WIN_SEG		0xfe00
#define DOC_STACK_SEG		0x0400
#define SPL_RAM_SEG		0x8000

#if defined(USE_DOC_2000_TSOP)
#define DOC_SPL_START_PAGE	4	/* 0-3 for IPL (each of 1KB size) */
#else /* defined (USE_DOC_MIL) */
#define DOC_SPL_START_PAGE	2	/* 0,1 for IPL (each of 512B size) */
#endif

#define DOC_SPL_SIZE_IN_PAGE	128 - DOC_SPL_START_PAGE    /* 1 page = 512 bytes, total 63kB */

/* Pseudo Call and Return macros */
#define RET_LABEL(label)	\
	jmp	label##_done

#define CALL_LABEL(label)	\
	jmp	label	;\
label##_done:

#define CALL_SP(func) \
	lea	0f, %sp	; \
	jmp	func	; \
0:

#define RET_SP \
	jmp	*%sp

#define CALL_BP(func) \
	lea	0f, %bp	; \
	jmp	func	; \
0:

#define RET_BP \
	jmp	*%bp
