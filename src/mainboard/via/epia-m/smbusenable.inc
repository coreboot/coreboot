/* Useful macros PCIBUS, and SMBUS functions for getting DRAM going. */
/* courtesy Eric Biederman of linuxnetworx.com */

#define CS_WRITE_BYTE(addr, byte) \
        movl $addr, %eax        ; \
        movl $byte, %edx        ; \
        PCI_WRITE_CONFIG_BYTE

#define CS_WRITE_WORD(addr, word) \
        movl $addr, %eax        ; \
        movl $word, %ecx        ; \
        PCI_WRITE_CONFIG_WORD

#define CS_WRITE_LONG(addr, dword) \
        movl $addr, %eax        ; \
        movl $dword, %ecx       ; \
        PCI_WRITE_CONFIG_DWORD

#define DEVFN(device, function) (((device) << 3) + (function))
#ifndef CONFIG_ADDR
#define CONFIG_ADDR(bus,devfn,where) (((bus) << 16) | ((devfn) << 8) | (where))
#endif

/* generic SMB routines that work for many systems. The only one that might
 * not work is the enable_smbus. 
 * you have to define PM_FUNCTION for this to work. 
 */#define SMBUS_IO_BASE 0xf00
#define SMBHSTSTAT 0
#define SMBHSTCTL  2
#define SMBHSTCMD  3
#define SMBHSTADD  4
#define SMBHSTDAT0 5
#define SMBHSTDAT1 6
#define SMBBLKDAT  7

/* (DA) Lines added to get this to compile */
#define PM_DEVFN CONFIG_ADDR(0,0x11*8,0)
#define DRAM_CONFIG_PORT 0x5a
#define SMBUS_MEM_DEVICE_0 0x50
#define LAST_SMBUS_MEM_DEVICE  SMBUS_MEM_DEVICE_0
#define REGISTERED_DRAM_REGISTER $0x69
#define REGISTERED_DRAM $0x2d
#define NONREGISTERED_DRAM $0


        /* put the SMBUS at port 0xf00 + enable*/
        CS_WRITE_WORD(PM_DEVFN+ 0xd0, SMBUS_IO_BASE|1)   /* iobase addr */
        CS_WRITE_BYTE(PM_DEVFN + 0xd2, (0x4 << 1) | 1)      /* smbus enable */
        CS_WRITE_WORD(PM_DEVFN + 0x4, 1)                    /* iospace enable */

/* The VT1211 serial port needs 48 mhz clock, on power up it is getting
   only 24 mhz, there is some mysterious device on the smbus that can
   fix this...this code below does it. */


#define MYOUTB(val, port) movb val, %al; movw port, %dx ; outb %al, %dx

#define  I2C_TRANS_CMD          0x40
#define  CLOCK_SLAVE_ADDRESS    0x69

        MYOUTB( $0xff, $(SMBUS_IO_BASE+SMBHSTSTAT))
        MYOUTB( $0x7f, $(SMBUS_IO_BASE+SMBHSTDAT0))
        MYOUTB( $0x83, $(SMBUS_IO_BASE+SMBHSTCMD))
        MYOUTB( $(CLOCK_SLAVE_ADDRESS<<1), $(SMBUS_IO_BASE+SMBHSTADD))
        MYOUTB( $(8 | I2C_TRANS_CMD), $(SMBUS_IO_BASE+SMBHSTCTL))
        movw $(SMBUS_IO_BASE+SMBHSTSTAT), %dx
smbwait1:
        inb %dx, %al
        and  $1, %al
        jnz smbwait1

