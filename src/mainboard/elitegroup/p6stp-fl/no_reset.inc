/*
 * turns auto-reset and software-watchdog
 * of the sis 630 off.
 * Author: Felix Kloeckner, root@hamburg.de
*/

#define PCI_COMMAND_PORT	0xcf8
#define PCI_DATA_PORT		0xcfc

#define LPC_BRIDGE_BASE_ADDR	0x80000800

  // Enable ACPI by set B7 on Reg 0x40, LPC
  movl $LPC_BRIDGE_BASE_ADDR, %eax
  addb $0x40, %al
  movw $PCI_COMMAND_PORT, %dx
  outl %eax, %dx
  movw $PCI_DATA_PORT, %dx
  inb %dx, %al
  orb $0x80, %al
  
  outb %al, %dx
  
  
  // set acpi base address to 0x5000
  movl $LPC_BRIDGE_BASE_ADDR, %eax
  addb $0x74, %al
  movw $PCI_COMMAND_PORT, %dx
  outl %eax, %dx
  movw $PCI_DATA_PORT, %dx
  movw $0x5000, %ax
  outw %ax, %dx
  
  movw %ax, %bx                         # acpi_base now in bx 
  
  
  // disable auto-reset
  movw %bx, %dx
  addw $0x56, %dx
  inb %dx, %al
  orb $0x40, %al
  xorb %ah, %ah
  outw %ax, %dx
  
  
  // disable software watchdog
  movw %bx, %dx
  addw $0x4b, %dx                       # dx = acpi_base + 0x4b
  xorb %al, %al
  outb %al, %dx
  
  CONSOLE_DEBUG_TX_STRING($str_auto_reset_off)
     
  jmp auto_reset_off_end

str_auto_reset_off:
  .string "auto-reset switched off\r\n"
auto_reset_off_end:
