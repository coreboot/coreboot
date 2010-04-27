//#include <device/smbus_def.h>
#define SMBUS_ERROR -1
#define SMBUS_WAIT_UNTIL_READY_TIMEOUT -2
#define SMBUS_WAIT_UNTIL_DONE_TIMEOUT  -3

#define	SMB_SDA		0x00
#define SMB_STS		0x01
#define SMB_CTRL_STS	0x02
#define	SMB_CTRL1	0x03
#define SMB_ADD		0x04
#define SMB_CTRL2	0x05
#define	SMB_CTRL3	0x06

#define SMB_STS_SLVSTP	(0x01 << 7)
#define SMB_STS_SDAST	(0x01 << 6)
#define	SMB_STS_BER	(0x01 << 5)
#define SMB_STS_NEGACK	(0x01 << 4)
#define	SMB_STS_STASTR	(0x01 << 3)
#define SMB_STS_NMATCH	(0x01 << 2)
#define	SMB_STS_MASTER	(0x01 << 1)
#define SMB_STS_XMIT	(0x01 << 0)

#define	SMB_CSTS_TGSCL	(0x01 << 5)
#define SMB_CSTS_TSDA	(0x01 << 4)
#define	SMB_CSTS_GCMTCH	(0x01 << 3)
#define SMB_CSTS_MATCH	(0x01 << 2)
#define	SMB_CSTS_BB	(0x01 << 1)
#define SMB_CSTS_BUSY	(0x01 << 0)

#define	SMB_CTRL1_STASTRE (0x01 << 7)
#define SMB_CTRL1_NMINTE  (0x01 << 6)
#define	SMB_CTRL1_GCMEN   (0x01 << 5)
#define SMB_CTRL1_ACK     (0x01 << 4)
#define	SMB_CTRL1_RSVD    (0x01 << 3)
#define SMB_CTRL1_INTEN   (0x01 << 2)
#define	SMB_CTRL1_STOP    (0x01 << 1)
#define SMB_CTRL1_START   (0x01 << 0)

#define	SMB_ADD_SAEN	  (0x01 << 7)

#define	SMB_CTRL2_ENABLE  0x01

#define SMBUS_TIMEOUT (100*1000*10)
#define SMBUS_STATUS_MASK 0xfbff

#define SMBUS_IO_BASE 0x6000

static void smbus_delay(void)
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

static int smbus_check_stop_condition(unsigned smbus_io_base)
{
	unsigned char val;
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	/* check for SDA status */
	do {
		smbus_delay();
		val = inw(smbus_io_base + SMB_CTRL1);
		if ((val & SMB_CTRL1_STOP) == 0) {
			break;
		}
	} while(--loops);
	return loops?0:SMBUS_WAIT_UNTIL_READY_TIMEOUT;
}

static int smbus_stop_condition(unsigned smbus_io_base)
{
	unsigned char val;
	val = inb(smbus_io_base + SMB_CTRL1);
	outb(SMB_CTRL1_STOP, smbus_io_base + SMB_CTRL1);
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
	if (((val & SMB_STS_BER)    != 0) ||
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
	if (((val & SMB_STS_BER)    != 0) ||
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

static unsigned char do_smbus_read_byte(unsigned smbus_io_base, unsigned char device, unsigned char address)
{
	unsigned char val, val1;

	smbus_check_stop_condition(smbus_io_base);

	smbus_start_condition(smbus_io_base);

	smbus_send_slave_address(smbus_io_base, device);

	smbus_send_command(smbus_io_base, address);

	smbus_start_condition(smbus_io_base);

	smbus_send_slave_address(smbus_io_base, device | 0x01);

	/* send NACK to slave */
	val = inb(smbus_io_base + SMB_CTRL1);
	outb(val | SMB_CTRL1_ACK, smbus_io_base + SMB_CTRL1);

	val = inb(smbus_io_base + SMB_SDA);

	//smbus_stop_condition(smbus_io_base);

	return val;
}
