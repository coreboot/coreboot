#define SMBUS_IO_BASE 0x5000

#define SMBHSTSTAT 0x0
#define SMBSLVSTAT 0x1
#define SMBHSTCTL  0x2
#define SMBHSTCMD  0x3
#define SMBXMITADD 0x4
#define SMBHSTDAT0 0x5
#define SMBHSTDAT1 0x6
#define SMBBLKDAT  0x7
#define SMBSLVCTL  0x8
#define SMBTRNSADD 0x9
#define SMBSLVDATA 0xa
#define SMLINK_PIN_CTL 0xe
#define SMBUS_PIN_CTL  0xf 

/* Define register settings */
#define HOST_RESET 0xff
#define DIMM_BASE 0xa0        // 1010000 is base for DIMM in SMBus
#define READ_CMD  0x01        // 1 in the 0 bit of SMBHSTADD states to READ


#define SMBUS_TIMEOUT (100*1000*10)

static void enable_smbus(void)
{
  device_t dev;
  unsigned char c;
  /* Power management controller */
  dev = pci_locate_device(PCI_ID(0x1106,0x8235), 0);

  if (dev == PCI_DEV_INVALID) {
    die("SMBUS controller not found\r\n");
  }

  // set IO base address to SMBUS_IO_BASE
  pci_write_config32(dev, 0x90, SMBUS_IO_BASE|1);

  // Enable SMBus 
  c = pci_read_config8(dev, 0xd2);
  c |= 5;
  pci_write_config8(dev, 0xd2, c);

  c = pci_read_config8(dev, 0x54);
  c &= ~0x80;
  pci_write_config8(dev, 0xd2, c);


  print_debug("SMBus controller enabled\r\n");
}


static inline void smbus_delay(void)
{
  outb(0x80, 0x80);
}

static int smbus_wait_until_ready(void)
{
  unsigned long loops;
  loops = SMBUS_TIMEOUT;
  do {
    unsigned char val;
    smbus_delay();
    c = my_inb(smbus_io_base + SMBHSTSTAT);
    while((c & 1) == 1) {
      printk_err("c is 0x%x\n", c);
      c = my_inb(smbus_io_base + SMBHSTSTAT);
      /* nop */ 
    }

  } while(--loops);
  return loops?0:-1;
}

void smbus_reset(void)
{
  my_outb(HOST_RESET, smbus_io_base + SMBHSTSTAT);
  my_outb(HOST_RESET, smbus_io_base + SMBHSTSTAT);
  my_outb(HOST_RESET, smbus_io_base + SMBHSTSTAT);
  my_outb(HOST_RESET, smbus_io_base + SMBHSTSTAT);

  smbus_wait_until_ready();
  printk_err("After reset status %#x\n", my_inb(smbus_io_base + SMBHSTSTAT));
}
  


static int smbus_wait_until_done(void)
{
  unsigned long loops;
  loops = SMBUS_TIMEOUT;
  do {
    unsigned char val;
    smbus_delay();
		
    byte = my_inb(smbus_io_base + SMBHSTSTAT);
    if (byte & 1)
      break;

  } while(--loops);
  return loops?0:-1;
}

static void smbus_print_error(unsigned char host_status_register)
{

  printk_err("smbus_error: 0x%02x\n", host_status_register);
  if (host_status_register & (1 << 4)) {
    printk_err("Interrup/SMI# was Failed Bus Transaction\n");
  }
  if (host_status_register & (1 << 3)) {
    printk_err("Bus Error\n");
  }
  if (host_status_register & (1 << 2)) {
    printk_err("Device Error\n");
  }
  if (host_status_register & (1 << 1)) {
    printk_err("Interrupt/SMI# was Successful Completion\n");
  }
  if (host_status_register & (1 << 0)) {
    printk_err("Host Busy\n");
  }
}


/* SMBus routines borrowed from VIA's Trident Driver */
/* this works, so I am not going to touch it for now -- rgm */
static unsigned char smbus_read(unsigned char devAdr, unsigned char bIndex, 
				unsigned char *result)
{
  unsigned short i;
  unsigned char  bData;
  unsigned char  sts;
		        
  /* clear host status */
  my_outb(0xff, smbus_io_base);
			    
  /* check SMBUS ready */
  for ( i = 0; i < 0xFFFF; i++ )
    if ( (my_inb(smbus_io_base) & 0x01) == 0 )
      break;

  /* set host command */
  my_outb(bIndex, smbus_io_base+3);

  /* set slave address */
  my_outb(devAdr | 0x01, smbus_io_base+4);

  /* start */
  my_outb(0x48, smbus_io_base+2);

  /* SMBUS Wait Ready */
  for ( i = 0; i < 0xFFFF; i++ )
    if ( ((sts = my_inb(smbus_io_base)) & 0x01) == 0 )
      break;
  if ((sts & ~3) != 0) {
    smbus_print_error(sts);
    return 0;
  }
  bData=my_inb(smbus_io_base+5);

  *result = bData;

  /* return 1 if ok */
  return 1;

}

/* for reference, here is the fancier version which we will use at some 
 * point
 */
# if 0
int smbus_read_byte(unsigned device, unsigned address, unsigned char *result)
{
  unsigned char host_status_register;
  unsigned char byte;

  reset();

  smbus_wait_until_ready();

  /* setup transaction */
  /* disable interrupts */
  my_outb(my_inb(smbus_io_base + SMBHSTCTL) & (~1), smbus_io_base + SMBHSTCTL);
  /* set the device I'm talking too */
  my_outb(((device & 0x7f) << 1) | 1, smbus_io_base + SMBXMITADD);
  /* set the command/address... */
  my_outb(address & 0xFF, smbus_io_base + SMBHSTCMD);
  /* set up for a byte data read */
  my_outb((my_inb(smbus_io_base + SMBHSTCTL) & 0xE3) | (0x2 << 2),
	  smbus_io_base + SMBHSTCTL);

  /* clear any lingering errors, so the transaction will run */
  my_outb(my_inb(smbus_io_base + SMBHSTSTAT), smbus_io_base + SMBHSTSTAT);

  /* clear the data byte...*/
  my_outb(0, smbus_io_base + SMBHSTDAT0);

  /* start the command */
  my_outb((my_inb(smbus_io_base + SMBHSTCTL) | 0x40),
	  smbus_io_base + SMBHSTCTL);

  /* poll for transaction completion */
  smbus_wait_until_done();

  host_status_register = my_inb(smbus_io_base + SMBHSTSTAT);

  /* Ignore the In Use Status... */
  host_status_register &= ~(1 << 6);

  /* read results of transaction */
  byte = my_inb(smbus_io_base + SMBHSTDAT0);
  smbus_print_error(byte);

  *result = byte;
  return host_status_register != 0x02;
}


#endif

