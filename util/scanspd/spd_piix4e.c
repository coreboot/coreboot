#ifndef __KERNEL__
#include <stdio.h>
#endif

#include <asm/io.h>

#ifndef __KERNEL__
typedef unsigned short u16;
typedef unsigned long u32;

#define CONFIG_CMD(bus,devfn, where)   (0x80000000 | (bus << 16) | (devfn << 8) | (where & ~3))

static int pci_conf1_write_config_byte(unsigned char bus, int devfn, int where, unsigned char value)
{
	outl(CONFIG_CMD(bus,devfn,where), 0xCF8);    
	outb(value, (0xCFC + (where&2)));
	return 0;    
}

static int pci_conf1_write_config_word(unsigned char bus, int devfn, int where, u16 value)
{
	outl(CONFIG_CMD(bus,devfn,where), 0xCF8);    
	outw(value, (0xCFC + (where&2)));
	return 0;    
}

static int pci_conf1_write_config_dword(unsigned char bus, int devfn, int where, u32 value)
{
	outl(CONFIG_CMD(bus,devfn,where), 0xCF8);
	outl(value, 0xcfc);
	return 0;    
}

static int pci_conf1_read_config_word(unsigned char bus, int devfn, int where, u16 *value)
{
	outl(CONFIG_CMD(bus,devfn,where), 0xCF8);    
	*value = inw(0xCFC + (where&2));
	return 0;    
}

static int pci_conf1_read_config_dword(unsigned char bus, int devfn, int where, u32 *value)
{
	u32 retval;
	int i, j;
	outl(CONFIG_CMD(bus,devfn,where), 0xCF8);
	retval = inl(0xCFC);
	*value = retval;
	return 0;    
}

#endif


#include <linux/pci.h>

/* Define register offsets */
#define SMBHSTSTS  0x00
#define SMBHSTCTL  0x02
#define SMBHSTCMD  0x03       // Register tells what data to get
#define SMBHSTADD  0x04
#define SMBHSTDAT0 0x05
#define SMBHSTDAT1 0x06
#define SMBBLKDAT  0x07

/* Define register settings */
#define HOST_RESET 0x8e
#define DIMM_BASE 0xa0        // 1010000 is base for DIMM in SMBus
#define READ_CMD  0x01        // 1 in the 0 bit of SMBHSTADD states to READ

/* Define SPD Data locations */
#define MEM_TYPE  2           // Memory Type - EDO, FPM, SDRAM
#define NUM_ROWS  3           // Number of Row Addresses
#define NUM_COLS  4           // Number of Column Addresses
#define NUM_MOD_ROWS 5        // Number of Module Rows
#define CAS_LAT   18          // CAS Latencies Supported
#define MOD_ATTR  21          // SDRAM Module Attributes
#define BANK_DENSITY 31       // Module Bank Density
#define SPD_REV   62          // SPD Revision
#define SPEC_FREQ 126         // Specification Frequency - 66, 100, ...

inline void waitbusy(unsigned short baseaddr);
inline int waitstatus(unsigned short baseaddr);
int error_check(unsigned short baseaddr);

int main()
{
  int devfn;
  unsigned short baseaddr;
  int dimm;
  int status;
  int data;
  int rows, density; // For RAM size calcs
      u32 val;
  int i;

  iopl(3);
  // Find the PIIX4 Power Management function
  for (devfn = 0; devfn < 256; devfn++)
    {
      pci_conf1_read_config_dword(0, devfn, 0, &val);
      if (val == 0x71138086)
	break;
    }

  if (devfn == 256)
    return(1);

  // Get SMBus IO Base register
  pci_conf1_read_config_word(0, devfn, 0x90, &baseaddr);
  baseaddr &= 0xfff0;

	printf("got devfn 0x%x baseaddr 0x%x\n", devfn, baseaddr);
  if (!baseaddr) // Still in default
    return(2);
  pci_conf1_write_config_word(0, devfn, 0xd2, (4<<1)|1);

  /* Communicate on the SMBus through the SMBus IO Space Registers
   * See the System Management Bus Specification [1] for general
   * information about the bus.  See the PIIX4 datasheet [2] for 
   * information about the SMBus IO Space Registers (p. 148) */

  waitbusy(baseaddr);


  for(i = 0x1; i < 0x100; i += 2) {
	if (i == 0xd3)
		continue;
      outb(HOST_RESET, baseaddr + SMBHSTSTS);
      outb(i, baseaddr + SMBHSTADD);
      outb(0x3, baseaddr + SMBHSTCMD);
      outb(0, baseaddr + SMBHSTDAT0);
      outb((2<<2)|(1<<6), baseaddr + SMBHSTCTL);

      /* Check DIMM status once */
      status =  waitstatus(baseaddr);
      printf("status 0x%x ", status);
      if (status == 4) {printf("no device at 0x%x\n", i);}
	else printf("device at 0x%x\n", i);
	}
#if 0
#endif

  /* Select the DIMM chip to read SPD data (cycle through all of them) */

  for (dimm = 0; dimm < 8; dimm++)
    {

      /* Reset the controller status before using */
      outb(HOST_RESET, baseaddr + SMBHSTSTS);
      printf("DIMM #%d:\n", dimm);

      /* Set up SPD for READ's */
      outb(DIMM_BASE | (dimm << 1) | READ_CMD, baseaddr + SMBHSTADD);


      /* For each piece of Data below, we select the data number in the
	 Command Register (SBDHSTCMD) and tell the Control Register 
	 (SMBHSTCTL) we are reading a byte of data. Then grab the data from
	 data register 0 (SMBHSTDAT0).
       */

#if 0
      /* Specification Revision */
      outb(SPD_REV, baseaddr + SMBHSTCMD);
      outb(0x48, baseaddr + SMBHSTCTL);
#endif

      outb(0x3, baseaddr + SMBHSTCMD);
      outb(0, baseaddr + SMBHSTDAT0);
      outb((2<<2)|(1<<6), baseaddr + SMBHSTCTL);

      /* Check DIMM status once */
      status =  waitstatus(baseaddr);
      if (status & 0x04)
	{
	  printf("No SPD present at DIMM\n\n");
	  continue;
	}
      else if (error_check(baseaddr)) return (3);

      data = inb(baseaddr + SMBHSTDAT0);
      printf("SPD Revision: %d.%d\n", (data & 0xf0) >> 4, data & 0x0f);

      /* Memory Type */
      outb(HOST_RESET, baseaddr + SMBHSTSTS);
      outb(MEM_TYPE, baseaddr + SMBHSTCMD);
      outb(0x48, baseaddr + SMBHSTCTL);

      if (error_check(baseaddr)) return(4);

      data = inb(baseaddr + SMBHSTDAT0);
      switch(data)
	{
	case 0x02:
	  {
	    printf("DIMM #%d contains EDO\n", dimm);
	    break;
	  }
	case 0x04:
	  {
	    printf("DIMM #%d contains SDRAM\n", dimm);
	    break;
	  }
	default:
	  {
	    printf("DIMM %d contains unknown (0x%02X) type\n", dimm, data);
	    break;
	  }
	}

      /* Memory Size */

      /* Start w/ number of module rows */
      outb(HOST_RESET, baseaddr + SMBHSTSTS);
      outb(NUM_MOD_ROWS, baseaddr + SMBHSTCMD);
      outb(0x48, baseaddr + SMBHSTCTL);

      if (error_check(baseaddr)) return(5);
      
      rows = inb(baseaddr + SMBHSTDAT0);
      if (rows == 0)
	{
	  printf("Undefined number of rows");
	}
      else
	{
	  printf("%d module rows on DIMM\n", rows);
	}

      /* Now do size of row */
      outb(HOST_RESET, baseaddr + SMBHSTSTS);
      outb(BANK_DENSITY, baseaddr + SMBHSTCMD);
      outb(0x48, baseaddr + SMBHSTCTL);

      if (error_check(baseaddr)) return(6);

      density = inb(baseaddr + SMBHSTDAT0);
      switch (density)
	{
	case 0x01:
        case 0x02:
	case 0x04:
	case 0x08:
	case 0x10:
	case 0x20:
	case 0x40:
        case 0x80:
	  {
	    printf("DIMM has %dMByte per row.\n", density << 2);
	    printf("DIMM has %dMByte total.\n", (density << 2) * rows);
	    break;
	  }

	default:
	  {
	    printf("DIMM has mixed memory per row.\n");
	    printf("Unknown total memory.\n");
	    break;
	  }
	}
    }
  return(0);
}

/* Waitbusy... could possibly hang CPU, so insert timeout */
inline void waitbusy (unsigned short baseaddr)
{
  while(1)
    if ( !(inb(baseaddr + SMBHSTSTS) & 0x01) )
      return;
}

/* waitstatus... could also hang CPU */
inline int waitstatus (unsigned short baseaddr)
{
  int status;
  
  do
    status = inb(baseaddr + SMBHSTSTS);
  while ( (status & 0x01) != 0 || status == 0);

  return (status);
}

/* Error check for transaction errors */
int error_check (unsigned short baseaddr)
{
  int status;

  status = waitstatus(baseaddr);
  if ( !(status & 0x02) )
    {
      printf("Transaction error 0x%02X\n", status);
      return(1);
    }
  return(0);
}

