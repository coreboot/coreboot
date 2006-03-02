//#include <device/smbus_def.h>
#define SMBUS_ERROR -1
#define SMBUS_WAIT_UNTIL_READY_TIMEOUT -2
#define SMBUS_WAIT_UNTIL_DONE_TIMEOUT  -3

enum smb_native_registers {    
	SMB_SDA		= 0x00, SMB_STS		= 0x01, SMB_CTRL_STS	= 0x02,
	SMB_CTRL1	= 0x03, SMB_ADD		= 0x04, SMB_CTRL2	= 0x05,
	SMB_CTRL3	= 0x06
};

enum smb_sts_bits {
	SMB_STS_SLVSTP	= (0x01 << 7), SMB_STS_SDAST	= (0x01 << 6),
	SMB_STS_BER	= (0x01 << 5), SMB_STS_NEGACK	= (0x01 << 4),
	SMB_STS_STASTR	= (0x01 << 3), SMB_STS_NMATCH	= (0x01 << 2),
	SMB_STS_MASTER	= (0x01 << 1), SMB_STS_XMIT	= (0x01 << 0)
};

enum smb_ctrl_sts_bits {
	SMB_CSTS_TGSCL	= (0x01 << 5), SMB_CSTS_TSDA	= (0x01 << 4),
	SMB_CSTS_GCMTCH	= (0x01 << 3), SMB_CSTS_MATCH   = (0x01 << 2),
	SMB_CSTS_BB	= (0x01 << 1), SMB_CSTS_BUSY	= (0x01 << 0)
};

enum smb_ctrl1_bits {
	SMB_CTRL1_STASTRE = (0x01 << 7), SMB_CTRL1_NMINTE  = (0x01 << 6),
	SMB_CTRL1_GCMEN   = (0x01 << 5), SMB_CTRL1_ACK     = (0x01 << 4),
	SMB_CTRL1_RSVD    = (0x01 << 3), SMB_CTRL1_INTEN   = (0x01 << 2),
	SMB_CTRL1_STOP    = (0x01 << 1), SMB_CTRL1_START   = (0x01 << 0)
};

enum smb_add_bits {
	SMB_ADD_SAEN	= (0x01 << 7)
};

enum smb_ctrl2_bits {
	SMB_CTRL2_ENABLE  = 0x01,
};

#define SMBUS_TIMEOUT (100*1000*10)
#define SMBUS_STATUS_MASK 0xfbff

static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
}

/* generate a smbus start condition */
static int smbus_start_condition(unsigned smbus_io_base)
{
	unsigned char val;
	unsigned long loops;
	loops = SMBUS_TIMEOUT;

	/* issue a START condition */
	val = inb(smbus_io_base + SMB_CTRL1);
	outb(val | SMB_CTRL1_START, smbus_io_base + SMB_CTRL1);

	/* check for bus conflict */
	val = inb(smbus_io_base + SMB_STS);
	if ((val & SMB_STS_BER) != 0)
		return SMBUS_ERROR;

	/* check for SDA status */
	do {
		smbus_delay();
		val = inw(smbus_io_base + SMB_STS);
		if ((val & SMB_STS_SDAST) != 0) {
			break;
		}
	} while(--loops);
	return loops?0:SMBUS_WAIT_UNTIL_READY_TIMEOUT;
}

static int smbus_send_slave_address(unsigned smbus_io_base, unsigned char device)
{
	unsigned char val;
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
                        
	/* send the slave address */
	outb(device, smbus_io_base + SMB_SDA);

	/* check for bus conflict and NACK */
	val = inb(smbus_io_base + SMB_STS);
	if ( ((val & SMB_STS_BER)    != 0) ||
	     ((val & SMB_STS_NEGACK) != 0))
		return SMBUS_ERROR;

	/* check for SDA status */
	do {
		smbus_delay();
		val = inw(smbus_io_base + SMB_STS);
		if ((val & SMB_STS_SDAST) != 0) {
			break;
		}
	} while(--loops);
	return loops?0:SMBUS_WAIT_UNTIL_READY_TIMEOUT;	
}

static int smbus_send_command(unsigned smbus_io_base, unsigned char command)
{
        unsigned char val;
	unsigned long loops;
	loops = SMBUS_TIMEOUT;

	/* send the command */
	outb(command, smbus_io_base + SMB_SDA);

	/* check for bus conflict and NACK */
	val = inb(smbus_io_base + SMB_STS);
	if ( ((val & SMB_STS_BER)    != 0) ||
	     ((val & SMB_STS_NEGACK) != 0))
		return SMBUS_ERROR;

	/* check for SDA status */
	do {
		smbus_delay();
		val = inw(smbus_io_base + SMB_STS);
		if ((val & SMB_STS_SDAST) != 0) {
			break;
		}
	} while(--loops);
	return loops?0:SMBUS_WAIT_UNTIL_READY_TIMEOUT;	
}

static int do_smbus_read_byte(unsigned smbus_io_base, unsigned char device, unsigned char address)
{
	unsigned char val;

	smbus_start_condition(smbus_io_base);

	smbus_send_slave_address(smbus_io_base, device);

	smbus_send_command(smbus_io_base, address);

	smbus_start_condition(smbus_io_base);

	smbus_send_slave_address(smbus_io_base, device | 0x01);

	val = inb(smbus_io_base + SMB_CTRL1);
	outb(val | SMB_CTRL1_ACK, smbus_io_base + SMB_CTRL1);

	return inb(smbus_io_base + SMB_SDA);
}
