/* 
   v1.0: 02/15/2001: Program procedure to make workable and stable register settings:
         (1) Initial registers: 0x40:0x48, 0x41:0x4d, 0x43:0x44, 0x44:0x04, 0x83:0x02, 0x3e:0x0c
   v1.1: 02/15/2001: 
	(1) Initial registers: To add 0x3e:0x0c 
	(2) After initial: To add bus0, devfn0 0x84:0xc0
   v1.2: 02/15/2001:
	To set 0x13 register to 0xd8 value, thus it won't hang the system if enable graphic aperture(0x88 bit 2). But 
	I am not sure if it would take effect for graphic device. According to the result, the system won't hang and check
	the register setting, that's right, they do set those values except the aperture base(0x13) has been changed to 0x84
	which means the pci configure does the resource allocation. But why it won't start up the monitor??? 
*/

jmp   ptp_afteram_set_registers

ptp_afteram_table: 
	.byte 0x40, 0x48
	.byte 0x41, 0x4d
	.byte 0x43, 0x44
	.byte 0x44, 0x04
	.byte 0x83, 0x02
	.byte 0x3e, 0x0c
	.byte 0x0 /* end of table */

ptp_afteram_set_registers:
	/* standard x86 loop on table until done code */
	/* assumes that: devfn is 8 (safe on anything we've seen) */
	/* which means addresses are a byte */
	/* address is first, then data */
	movl	$ptp_afteram_table, %esi
1: 
	/* zero out eax. This is a must right here. */
	xor	%eax, %eax
	movb	(%esi), %al
	testb 	%al, %al
	jz	done_ptp_afteram_set_registers
	inc	%esi
	orl     $0x0800, %eax
	movb	(%esi), %dl
	PCI_WRITE_CONFIG_BYTE
	inc %esi
	jmp 1b

done_ptp_afteram_set_registers:	
	CS_WRITE($0x13, $0xd8)
	CS_WRITE($0x84, $0xc0)
	CS_WRITE($0x80, $0x0f)
	CS_WRITE($0x88, $0x02)
	intel_chip_post_macro(0x99)

