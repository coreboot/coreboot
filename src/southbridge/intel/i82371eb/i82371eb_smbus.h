#include <device/smbus_def.h>

#define SMBHST_STATUS 	0x0
#define SMBHST_CTL    	0x2
#define SMBHST_CMD  	0x3
#define SMBHST_ADDR 	0x4
#define SMBHST_DAT  	0x5

#define SMBUS_TIMEOUT (100*1000*10)
#define SMBUS_STATUS_MASK 0x1e
#define SMBUS_ERROR_FLAG (1<<2)

static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
	outb(0x80, 0x80);
	outb(0x80, 0x80);
	outb(0x80, 0x80);
	outb(0x80, 0x80);
	outb(0x80, 0x80);
}

static int smbus_wait_until_ready(unsigned smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();
		val = inb(smbus_io_base + SMBHST_STATUS);
		if ((val & 0x1) == 0) {
			break;
		}
#if 0		
		if(loops == (SMBUS_TIMEOUT / 2)) {
			outw(inw(smbus_io_base + SMBHST_STATUS), 
				smbus_io_base + SMBHST_STATUS);
		}
#endif
	} while(--loops);
	return loops?0:SMBUS_WAIT_UNTIL_READY_TIMEOUT;
}

static int smbus_wait_until_done(unsigned smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned short val;
		smbus_delay();
		
		val = inb(smbus_io_base + SMBHST_STATUS);
		// Make sure the command is done
		if ((val & 0x1) != 0) { 
			continue;
		}
		// Don't break out until one of the interrupt
		// flags is set.
		if (val & 0xfe) {
			break;
		}
	} while(--loops);
	return loops?0:SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
}

static int do_smbus_recv_byte(unsigned smbus_io_base, unsigned device)
{
	unsigned global_status_register;
	unsigned byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}
	
	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBHST_CTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBHST_CTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 1, smbus_io_base + SMBHST_ADDR);
	/* set the command/address... */
	outb(0, smbus_io_base + SMBHST_CMD);
	/* set up for a send byte */
	outw((inw(smbus_io_base + SMBHST_CTL) & ~7) | (0x1), smbus_io_base + SMBHST_CTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBHST_STATUS), smbus_io_base + SMBHST_STATUS);

	/* set the data word...*/
	outw(0, smbus_io_base + SMBHST_DAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBHST_CTL) | (1 << 3)), smbus_io_base + SMBHST_CTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}

	global_status_register = inw(smbus_io_base + SMBHST_STATUS);

	/* read results of transaction */
	byte = inb(smbus_io_base + SMBHST_DAT) & 0xff;

	// Check for any result other than a command completion
	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 1)) {
		return SMBUS_ERROR;
	}
	return byte;
}

static int do_smbus_send_byte(unsigned smbus_io_base, unsigned device, unsigned value)
{
	unsigned global_status_register;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}
	
	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBHST_CTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBHST_CTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 0, smbus_io_base + SMBHST_ADDR);
	/* set the command/address... */
	outb(0, smbus_io_base + SMBHST_CMD);
	/* set up for a send byte */
	outw((inw(smbus_io_base + SMBHST_CTL) & ~7) | (0x1), smbus_io_base + SMBHST_CTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBHST_STATUS), smbus_io_base + SMBHST_STATUS);

	/* set the data word...*/
	outw(value, smbus_io_base + SMBHST_DAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBHST_CTL) | (1 << 3)), smbus_io_base + SMBHST_CTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}
	global_status_register = inw(smbus_io_base + SMBHST_STATUS);

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 4)) {
		return SMBUS_ERROR;
	}
	return 0;
}


static int do_smbus_read_byte(unsigned smbus_io_base, unsigned device, unsigned address)
{
	unsigned status_register;
	unsigned byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}
	
	/* setup transaction */

	/* clear any lingering errors, so the transaction will run */
	outb(0x1e, smbus_io_base + SMBHST_STATUS);

	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, smbus_io_base + SMBHST_ADDR);

	/* set the command/address... */
	outb(address & 0xff, smbus_io_base + SMBHST_CMD);

	/* clear the data word...*/
	outb(0, smbus_io_base + SMBHST_DAT);

	/* start a byte read with interrupts disabled */
	outb( (0x02 << 2)|(1<<6), smbus_io_base + SMBHST_CTL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}

	status_register = inw(smbus_io_base + SMBHST_STATUS);

	/* read results of transaction */
	byte = inw(smbus_io_base + SMBHST_DAT) & 0xff;

	if (status_register & 0x04) {
#if 0
 		print_debug("Read fail ");
		print_debug_hex16(status_register);
		print_debug("\n");
#endif
		return SMBUS_ERROR;
	}
	return byte;
}

static int do_smbus_write_byte(unsigned smbus_io_base, unsigned device, unsigned address, unsigned char val)
{
	unsigned global_status_register;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}

	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBHST_CTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBHST_CTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 0, smbus_io_base + SMBHST_ADDR);
	outb(address & 0xFF, smbus_io_base + SMBHST_CMD);
	/* set up for a byte data write */ /* FIXME */
	outw((inw(smbus_io_base + SMBHST_CTL) & ~7) | (0x2), smbus_io_base + SMBHST_CTL);
	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBHST_STATUS), smbus_io_base + SMBHST_STATUS);

	/* write the data word...*/
	outw(val, smbus_io_base + SMBHST_DAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBHST_CTL) | (1 << 3)), smbus_io_base + SMBHST_CTL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}
	global_status_register = inw(smbus_io_base + SMBHST_STATUS);

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 1)) {
		return SMBUS_ERROR;
	}
	return 0;
}

