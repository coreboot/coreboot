#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/p6/msr.h>

#include <arch/io.h>
#include <device/chip.h>
#include "../../../northbridge/amd/amdk8/northbridge.h"
#include "chip.h"

#include "pc80/mc146818rtc.h"



unsigned long initial_apicid[CONFIG_MAX_CPUS] =
{
	0, 1,
};

#define SMBGSTATUS 0xe0
#define SMBGCTL    0xe2
#define SMBHSTADDR 0xe4
#define SMBHSTDAT  0xe6
#define SMBHSTCMD  0xe8
#define SMBHSTFIFO 0xe9

#define SMBUS_TIMEOUT (100*1000*10)

static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
}

static int smbus_wait_until_ready(unsigned smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned short val;
		smbus_delay();
		val = inw(smbus_io_base + SMBGSTATUS);
		if ((val & 0x800) == 0) {
			break;
		}
		if(loops == (SMBUS_TIMEOUT / 2)) {
			outw(inw(smbus_io_base + SMBGSTATUS), 
				smbus_io_base + SMBGSTATUS);
		}
	} while(--loops);
	return loops?0:-2;
}

static int smbus_wait_until_done(unsigned smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned short val;
		smbus_delay();
		
		val = inw(smbus_io_base + SMBGSTATUS);
		if (((val & 0x8) == 0) | ((val & 0x437) != 0)) {
			break;
		}
	} while(--loops);
	return loops?0:-3;
}

static int smbus_send_byte(unsigned smbus_io_base, unsigned device, unsigned value)
{
	unsigned char global_status_register;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2;
	}
	
	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 0, smbus_io_base + SMBHSTADDR);
	/* set the command/address... */
	outb(0, smbus_io_base + SMBHSTCMD);
	/* set up for a send byte */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x1), smbus_io_base + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* set the data word...*/
	outw(value, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}
	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	if (global_status_register != (1 << 4)) {
		return -1;
	}
	return 0;
}

static int smbus_recv_byte(unsigned smbus_io_base, unsigned device)
{
	unsigned char global_status_register;
	unsigned char byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2;
	}
	
	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 1, smbus_io_base + SMBHSTADDR);
	/* set the command/address... */
	outb(0, smbus_io_base + SMBHSTCMD);
	/* set up for a send byte */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x1), smbus_io_base + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* set the data word...*/
	outw(0, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}

	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	/* read results of transaction */
	byte = inw(smbus_io_base + SMBHSTDAT) & 0xff;

	if (global_status_register != (1 << 4)) {
		return -1;
	}
	return byte;
}

#if 0
static int smbus_read_byte(unsigned smbus_io_base, unsigned device, unsigned address)
{
	unsigned char global_status_register;
	unsigned char byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2;
	}
	
	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 1, smbus_io_base + SMBHSTADDR);
	/* set the command/address... */
	outb(address & 0xFF, smbus_io_base + SMBHSTCMD);
	/* set up for a byte data read */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x2), smbus_io_base + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* clear the data word...*/
	outw(0, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}

	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	/* read results of transaction */
	byte = inw(smbus_io_base + SMBHSTDAT) & 0xff;

	if (global_status_register != (1 << 4)) {
		return -1;
	}
	return byte;
}

static int smbus_write_byte(unsigned smbus_io_base, unsigned device, unsigned address, unsigned char val)
{
	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2;
	}

	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)),
			smbus_io_base + SMBGCTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 0, smbus_io_base + SMBHSTADDR);
	outb(address & 0xFF, smbus_io_base + SMBHSTCMD);
	/* set up for a byte data write */ /* FIXME */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x1), smbus_io_base + SMBGCTL);
	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* clear the data word...*/
	outw(val, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}
	return 0;
}
#endif

#define SMBUS_MUX 0x70
static void mainboard_init(device_t dev)
{
	/* Set the mux to see the temperature sensors */
	dev = dev_find_device(0x1022, 0x746b, 0);
	if (dev) {
		unsigned smbus_io_base;
		unsigned device;
		int result;
		int mux_setting;
		device = SMBUS_MUX;
		mux_setting = 1;
		smbus_io_base = pci_read_config32(dev, 0x58) & ~1;;
		result = smbus_send_byte(smbus_io_base, device, mux_setting);
		if ((result < 0) || 
			(smbus_recv_byte(smbus_io_base, device) != mux_setting)) {
			printk_err("SMBUS mux would not set to %d\n", mux_setting);
		}
		
	}
	else {
		printk_err("SMBUS_controller not found\n");
	}
}

static struct device_operations mainboard_operations = {
	.read_resources   = root_dev_read_resources,
	.set_resources    = root_dev_set_resources,
	.enable_resources = enable_childrens_resources,
	.init             = mainboard_init,
	.scan_bus         = amdk8_scan_root_bus,
	.enable           = 0,
};

static void enumerate(struct chip *chip)
{
	struct chip *child;
	dev_root.ops = &mainboard_operations;
	chip->dev = &dev_root;
	chip->bus = 0;
	for(child = chip->children; child; child = child->next) {
		child->bus = &dev_root.link[0];
	}
}
struct chip_control mainboard_arima_hdama_control = {
	.enumerate = enumerate, 
	.name      = "Arima HDAMA mainboard ",
};

