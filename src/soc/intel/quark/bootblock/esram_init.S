/* SPDX-License-Identifier: BSD-3-Clause */

#include <cpu/x86/cr.h>
#include <cpu/x86/post_code.h>
#include <soc/QuarkNcSocId.h>
#include <soc/sd.h>

.macro RET32
	jmp	*%esp
.endm

/* ROM/SPI/MEMORY Definitions */
.equ  QUARK_DDR3_MEM_BASE_ADDRESS, (0x000000000)  /* Memory Base Address = 0 */
.equ  QUARK_MAX_DDR3_MEM_SIZE_BYTES, (0x80000000) /* DDR3 Memory Size = 2GB */
.equ  QUARK_ESRAM_MEM_BASE_ADDRESS, (QUARK_DDR3_MEM_BASE_ADDRESS \
		+ QUARK_MAX_DDR3_MEM_SIZE_BYTES)  /* eSRAM Memory above DDR3 */
.equ  QUARK_ESRAM_MEM_SIZE_BYTES, (0x00080000)	  /* eSRAM Memory Size = 512K */
.equ  QUARK_STACK_SIZE_BYTES, (0x008000)	  /* Quark stack size = 32K */
.equ	QUARK_STACK_BASE_ADDRESS, (QUARK_ESRAM_MEM_BASE_ADDRESS \
		+ QUARK_ESRAM_MEM_SIZE_BYTES \
		- QUARK_STACK_SIZE_BYTES)    /* Top of eSRAM - stack size */
.equ  QUARK_CMH_SIZE_BYTES, (0x0400)	     /* Quark Module Header size */
.equ	QUARK_ESRAM_STAGE1_BASE_ADDRESS, (QUARK_ESRAM_MEM_BASE_ADDRESS \
		+ QUARK_CMH_SIZE_BYTES)	     /* Start of Stage1 code in eSRAM */

/* RTC/CMOS definitions */
.equ  RTC_INDEX, (0x70)
.equ    NMI_DISABLE, (0x80)	/* Bit7=1 disables NMI */
.equ  RTC_DATA, (0x71)

/* PCI Configuration definitions (Datasheet 5.5.1) */
.equ  PCI_CFG, (0x80000000) /* PCI configuration access mechanism */
.equ  PCI_ADDRESS_PORT, (0xCF8)
.equ  PCI_DATA_PORT, (0xCFC)

/* Quark PCI devices */
.equ  HOST_BRIDGE_PFA, (0 << 11)	/* B0:D0:F0 (Host Bridge) */
.equ  ILB_PFA, (0x1F << 11)		/* B0:D31:F0 (Legacy Block) */

/* ILB PCI Config Registers */
.equ  BDE, (0x0D4)                             /* BIOS Decode Enable register */
.equ    DECODE_ALL_REGIONS_ENABLE, (0xFF000000)	  /* Decode all BIOS ranges */

/* iLB Reset Register */
.equ  ILB_RESET_REG, (0x0CF9)
.equ    CF9_WARM_RESET, (0x02)
.equ    CF9_COLD_RESET, (0x08)

/* Memory Arbiter Config Registers */
.equ  AEC_CTRL_OFFSET, (0x00)

/* Host Bridge Config Registers */
.equ  HMBOUND_OFFSET, (0x08)
.equ    HMBOUND_ADDRESS, (QUARK_DDR3_MEM_BASE_ADDRESS \
		+ QUARK_MAX_DDR3_MEM_SIZE_BYTES + QUARK_ESRAM_MEM_SIZE_BYTES)
.equ  HECREG_OFFSET, (0x09)
.equ    EC_BASE, (0xE0000000)
.equ    EC_ENABLE, (0x01)

/* Memory Manager Config Registers */
.equ    ESRAM_ADDRESS_2G, (0x10000080)
.equ  BIMRVCTL_OFFSET, (0x19)
.equ    ENABLE_IMR_INTERRUPT, (0x80000000)

/* SOC UNIT Debug Registers */
.equ  CFGSTICKY_W1_OFFSET, (0x50)
.equ    FORCE_COLD_RESET, (0x00000001)
.equ  CFGSTICKY_RW_OFFSET, (0x51)
.equ    RESET_FOR_ESRAM_LOCK, (0x00000020)
.equ    RESET_FOR_HMBOUND_LOCK, (0x00000040)
.equ  CFGNONSTICKY_W1_OFFSET, (0x52)
.equ    FORCE_WARM_RESET, (0x00000001)

.section .init, "ax", @progbits

	.global bootblock_pre_c_entry

bootblock_pre_c_entry:

	/* Get the timestamp since code in bootblock_crt0.S requires
	 * MMX register support.
	 */
	rdtsc
	movl	%eax, %ebp
	movl	%edx, %edi

	/* Registers:
	*     ebp: Low 32-bits of timestamp
	*     edi: High 32-bits of timestamp
	*/

setup_esram:
	/* Ensure cache is disabled. */
	movl	%cr0, %eax
	orl	$(CR0_CD | CR0_NW), %eax
	invd
	movl	%eax, %cr0

	/*
	 * Disable NMI operation
	 * Good convention suggests you should read back RTC data port after
	 * accessing the RTC index port.
	 */
	movb	$(NMI_DISABLE), %al
	movw	$(RTC_INDEX), %dx
	outb	%al, %dx
	movw	$(RTC_DATA), %dx
	inb	%dx, %al

	/* Disable SMI (Disables SMI wire, not SMI messages) */
	movl	$((QUARK_OPCODE_READ << QNC_MCR_OP_OFFSET) \
		| (QUARK_NC_HOST_BRIDGE_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (QNC_MSG_FSBIC_REG_HMISC << QNC_MCR_REG_OFFSET)), %ecx
	leal	L1, %esp
	jmp	stackless_SideBand_Read
L1:
	andl	$(~SMI_EN), %eax
	movl	$((QUARK_OPCODE_WRITE << QNC_MCR_OP_OFFSET) \
		| (QUARK_NC_HOST_BRIDGE_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (QNC_MSG_FSBIC_REG_HMISC << QNC_MCR_REG_OFFSET)), %ecx
	leal	L2, %esp
	jmp	stackless_SideBand_Write
L2:

	/*
	 * Before we get going, check SOC Unit Registers to see if we are
	 * required to issue a warm/cold reset
	 */
	movl	$((QUARK_ALT_OPCODE_READ << QNC_MCR_OP_OFFSET) \
		| (QUARK_SCSS_SOC_UNIT_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (CFGNONSTICKY_W1_OFFSET << QNC_MCR_REG_OFFSET)), %ecx
	leal	L3, %esp
	jmp	stackless_SideBand_Read
L3:
	andl	$(FORCE_WARM_RESET), %eax
	jz	TestForceColdReset		/* No warm reset - branch */
	jmp	IssueWarmReset

TestForceColdReset:
	movl	$((QUARK_ALT_OPCODE_READ << QNC_MCR_OP_OFFSET) \
		| (QUARK_SCSS_SOC_UNIT_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (CFGNONSTICKY_W1_OFFSET << QNC_MCR_REG_OFFSET)), %ecx
	leal	L4, %esp
	jmp	stackless_SideBand_Read
L4:
	andl	$(FORCE_COLD_RESET), %eax
	jz	TestHmboundLock		/* No cold reset - branch */
	jmp	IssueColdReset

	/* Before setting HMBOUND, check it's not locked */
TestHmboundLock:
	movl	$((QUARK_OPCODE_READ << QNC_MCR_OP_OFFSET) \
		| (QUARK_NC_HOST_BRIDGE_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (HMBOUND_OFFSET << QNC_MCR_REG_OFFSET)), %ecx
	leal	L5, %esp
	jmp	stackless_SideBand_Read
L5:
	andl	$(HMBOUND_LOCK), %eax
	jz	ConfigHmbound	/* Good configuration - branch */

	/* Failed to config - store sticky bit debug */
	movl	$((QUARK_ALT_OPCODE_READ << QNC_MCR_OP_OFFSET) \
		| (QUARK_SCSS_SOC_UNIT_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (CFGSTICKY_RW_OFFSET << QNC_MCR_REG_OFFSET)), %ecx
	leal	L6, %esp
	jmp	stackless_SideBand_Read
L6:
	orl	$(RESET_FOR_HMBOUND_LOCK), %eax
	movl	$((QUARK_ALT_OPCODE_WRITE << QNC_MCR_OP_OFFSET) \
		| (QUARK_SCSS_SOC_UNIT_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (CFGSTICKY_RW_OFFSET << QNC_MCR_REG_OFFSET)), %ecx
	leal	L7, %esp
	jmp	stackless_SideBand_Write
L7:
	jmp	IssueWarmReset

	/* Set up the HMBOUND register */
ConfigHmbound:
	movl	$(HMBOUND_ADDRESS), %eax    /* Data (Set HMBOUND location) */
	movl	$((QUARK_OPCODE_WRITE << QNC_MCR_OP_OFFSET) \
		| (QUARK_NC_HOST_BRIDGE_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (HMBOUND_OFFSET << QNC_MCR_REG_OFFSET)), %ecx
	leal	L8, %esp
	jmp	stackless_SideBand_Write
L8:

	/*
	 * Enable interrupts to Remote Management Unit when a IMR/SMM/HMBOUND
	 * violation occurs.
	 */
	movl	$(ENABLE_IMR_INTERRUPT), %eax    /* Set interrupt enable mask */
	movl	$((QUARK_OPCODE_WRITE << QNC_MCR_OP_OFFSET) \
		| (QUARK_NC_MEMORY_MANAGER_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (BIMRVCTL_OFFSET << QNC_MCR_REG_OFFSET)), %ecx
	leal	L9, %esp
	jmp	stackless_SideBand_Write
L9:

	/* Move eSRAM memory to 2GB */
	movl	$(ESRAM_ADDRESS_2G), %eax      /* Data (Set eSRAM location) */
	movl	$((QUARK_OPCODE_WRITE << QNC_MCR_OP_OFFSET) \
		| (QUARK_NC_MEMORY_MANAGER_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (QUARK_NC_MEMORY_MANAGER_ESRAMPGCTRL_BLOCK \
			<< QNC_MCR_REG_OFFSET)), %ecx
	leal	L10, %esp
	jmp	stackless_SideBand_Write
L10:

	/* Check that we're not blocked from setting the config that we want. */
	movl	$((QUARK_OPCODE_READ << QNC_MCR_OP_OFFSET) \
		| (QUARK_NC_MEMORY_MANAGER_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (QUARK_NC_MEMORY_MANAGER_ESRAMPGCTRL_BLOCK \
			<< QNC_MCR_REG_OFFSET)), %ecx
	leal	L11, %esp
	jmp	stackless_SideBand_Read
L11:
	andl	$(BLOCK_ENABLE_PG), %eax
	jnz	ConfigPci	/* Good configuration - branch */

	/* Failed to config - store sticky bit debug */
	movl	$((QUARK_ALT_OPCODE_READ << QNC_MCR_OP_OFFSET) \
		| (QUARK_SCSS_SOC_UNIT_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (CFGSTICKY_RW_OFFSET << QNC_MCR_REG_OFFSET)), %ecx
	leal	L12, %esp
	jmp	stackless_SideBand_Read
L12:
	orl	$(RESET_FOR_ESRAM_LOCK), %eax
	movl	$((QUARK_ALT_OPCODE_WRITE << QNC_MCR_OP_OFFSET) \
		| (QUARK_SCSS_SOC_UNIT_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (CFGSTICKY_RW_OFFSET << QNC_MCR_REG_OFFSET)), %ecx
	leal	L13, %esp
	jmp	stackless_SideBand_Write
L13:
	jmp IssueWarmReset

	/* Enable PCIEXBAR */
ConfigPci:
	movl	$(EC_BASE + EC_ENABLE), %eax      /* Data */
	movl	$((QUARK_OPCODE_WRITE << QNC_MCR_OP_OFFSET) \
		| (QUARK_NC_MEMORY_ARBITER_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (AEC_CTRL_OFFSET << QNC_MCR_REG_OFFSET)), %ecx
	leal	L14, %esp
	jmp	stackless_SideBand_Write
L14:

	movl	$(EC_BASE + EC_ENABLE), %eax      /* Data */
	movl	$((QUARK_OPCODE_WRITE << QNC_MCR_OP_OFFSET) \
		| (QUARK_NC_HOST_BRIDGE_SB_PORT_ID << QNC_MCR_PORT_OFFSET) \
		| (HECREG_OFFSET << QNC_MCR_REG_OFFSET)), %ecx
	leal	L15, %esp
	jmp	stackless_SideBand_Write
L15:

	/* Open up full 8MB SPI decode */
	movl	$(PCI_CFG | ILB_PFA | BDE), %ebx    /* PCI config address */
	movl	$(DECODE_ALL_REGIONS_ENABLE), %eax
	leal	L16, %esp
	jmp	stackless_PCIConfig_Write
L16:

	jmp	esram_init_done

IssueWarmReset:
	/* Issue Warm Reset request to Remote Management Unit via iLB */
	movw	$(CF9_WARM_RESET), %ax
	movw	$(ILB_RESET_REG), %dx
	outw	%ax, %dx
	jmp	.	/* Stay here until we are reset. */

IssueColdReset:
	/* Issue Cold Reset request to Remote Management Unit via iLB */
	movw	$(CF9_COLD_RESET), %ax
	movw	$(ILB_RESET_REG), %dx
	outw	%ax, %dx
	jmp	.	/* Stay here until we are reset. */

/*
 *----------------------------------------------------------------------------
 *
 * Procedure:    stackless_SideBand_Read
 *
 * Input:        esp - return address
 *               ecx[15:8] - Register offset
 *               ecx[23:16] - Port ID
 *               ecx[31:24] - Opcode
 *
 * Output:       eax - Data read
 *
 * Destroys:     eax
 *               ebx
 *               cl
 *               esi
 *
 * Description:
 *               Perform requested sideband read
 *----------------------------------------------------------------------------
 */

stackless_SideBand_Read:

	movl	%esp, %esi      /* Save the return address */

	/* Load the SideBand Packet Register to generate the transaction */
	movl	$(PCI_CFG | HOST_BRIDGE_PFA | QNC_ACCESS_PORT_MCR), %ebx
	movb	$QNC_MCR_BYTE_ENABLES, %cl	/* Set all Byte Enable bits */
	xchgl	%ecx, %eax
	leal	L17, %esp
	jmp	stackless_PCIConfig_Write
L17:
	xchgl	%ecx, %eax

	/* Read the SideBand Data Register */
	movl	$(PCI_CFG | HOST_BRIDGE_PFA | (QNC_ACCESS_PORT_MDR)), %ebx
	leal	L18, %esp
	jmp	stackless_PCIConfig_Read
L18:

	movl	%esi, %esp      /* Restore the return address */
	RET32

/*
 *----------------------------------------------------------------------------
 *
 * Procedure:   stackless_SideBand_Write
 *
 * Input:       esp - return address
 *              eax - Data
 *              ecx[15:8] - Register offset
 *              ecx[23:16] - Port ID
 *              ecx[31:24] - Opcode
 *
 * Output:      None
 *
 * Destroys:    ebx
 *              cl
 *              esi
 *
 * Description:
 *              Perform requested sideband write
 *
 *----------------------------------------------------------------------------
 */

stackless_SideBand_Write:

	movl	%esp, %esi      /* Save the return address */

	/* Load the SideBand Data Register with the data */
	movl	$(PCI_CFG | HOST_BRIDGE_PFA | QNC_ACCESS_PORT_MDR), %ebx
	leal	L19, %esp
	jmp	stackless_PCIConfig_Write
L19:

	/* Load the SideBand Packet Register to generate the transaction */
	movl	$(PCI_CFG | HOST_BRIDGE_PFA | QNC_ACCESS_PORT_MCR), %ebx
	movb	$QNC_MCR_BYTE_ENABLES, %cl	/* Set all Byte Enable bits */
	xchgl	%ecx, %eax
	leal	L20, %esp
	jmp	stackless_PCIConfig_Write
L20:
	xchgl	%ecx, %eax

	movl	%esi, %esp      /* Restore the return address */
	RET32

/*
 *----------------------------------------------------------------------------
 *
 * Procedure:   stackless_PCIConfig_Write
 *
 * Input:       esp - return address
 *              eax - Data to write
 *              ebx - PCI Config Address
 *
 * Output:      None
 *
 * Destroys:    dx
 *
 * Description:
 *              Perform a DWORD PCI Configuration write
 *
 *----------------------------------------------------------------------------
 */

stackless_PCIConfig_Write:

	/* Write the PCI Config Address to the address port */
	xchgl	%ebx, %eax
	movw	$(PCI_ADDRESS_PORT), %dx
	outl	%eax, %dx
	xchgl	%ebx, %eax

	/* Write the PCI DWORD Data to the data port */
	movw	$(PCI_DATA_PORT), %dx
	outl	%eax, %dx

	RET32

/*
 *----------------------------------------------------------------------------
 *
 * Procedure:   stackless_PCIConfig_Read
 *
 * Input:       esp - return address
 *              ebx - PCI Config Address
 *
 * Output:      eax - Data read
 *
 * Destroys:    eax
 *              dx
 *
 * Description:
 *              Perform a DWORD PCI Configuration read
 *
 *----------------------------------------------------------------------------
 */

stackless_PCIConfig_Read:

	/* Write the PCI Config Address to the address port */
	xchgl	%ebx, %eax
	movw	$(PCI_ADDRESS_PORT), %dx
	outl	%eax, %dx
	xchgl	%ebx, %eax

	/* Read the PCI DWORD Data from the data port */
	movw	$(PCI_DATA_PORT), %dx
	inl	%dx, %eax

	RET32

/*----------------------------------------------------------------------------*/

esram_init_done:

#if CONFIG(ENABLE_DEBUG_LED)
sd_led:

	/* Set the SDIO controller's base address */
	movl	$(SD_BASE_ADDR), %eax
	movl	$(SD_CFG_ADDR), %ebx
	leal	L40, %esp
	jmp	stackless_PCIConfig_Write

L40:
	movl	$(SD_CFG_ADDR), %ebx
	leal	L41, %esp
	jmp	stackless_PCIConfig_Read

L41:
	/*  Enable the SDIO controller */
	movl	$(SD_CFG_CMD), %ebx
	leal	L42, %esp
	jmp	stackless_PCIConfig_Read

L42:
	orl	$2, %eax
	movl	$(SD_CFG_CMD), %ebx
	leal	L43, %esp
	jmp	stackless_PCIConfig_Write

L43:
	movl	$(SD_CFG_CMD), %ebx
	leal	L44, %esp
	jmp	stackless_PCIConfig_Read

L44:
#if CONFIG(ENABLE_DEBUG_LED_ESRAM)
	jmp	light_sd_led
#endif /* CONFIG_ENABLE_DEBUG_LED_ESRAM */
#endif /* CONFIG_ENABLE_DEBUG_LED */

	/* Registers:
	*     ebp: Low 32-bits of timestamp
	*     edi: High 32-bits of timestamp
	*/

	/* Setup bootblock stack */
	movl	$_ecar_stack, %esp

before_carstage:
	post_code(0x2b)

	/* Get the timestamp passed in bootblock_crt0.S */
	push	%edi
	push	%ebp

	/* We can call into C functions now */
	call bootblock_c_entry

	/* Never reached */

	.global	light_sd_led

light_sd_led:
	/* Turn on SD LED to indicate ESRAM successfully initialized */
	movl	$SD_HOST_CTRL, %ebx
	movb	0(%ebx), %al
	orb	$1, %al
	movb	%al, 0(%ebx)

	/* Loop forever */
die:
	hlt
	jmp	die
