/* 32 bit mode code */
#define PCI_COMMAND_PORT	0xcf8
#define PCI_DATA_PORT		0xcfc

#define NORTH_BRIDGE_BASE_ADDR	0x80000000
#define LPC_BRIDGE_BASE_ADDR	0x80000800

#define SMB_BASE_ADDR		0x5080

#define RET_LABEL(label)	\
	jmp label##_done

#define CALL_LABEL(label)	\
	jmp label		;\
label##_done:

#define CALL_SP(func) \
	leal	0f, %esp	; \
	jmp func	; \
0:

#define RET_SP \
	jmpl *%esp

#define CALL_BP(func) \
	leal	0f, %ebp	; \
	jmp func	; \
0:

#define RET_BP \
	jmpl *%ebp
