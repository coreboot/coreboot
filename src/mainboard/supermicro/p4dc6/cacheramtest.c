#include <printk.h>
#include <stdarg.h>
#include <types.h>
#include <string.h>
#include <subr.h>
#include <serial_subr.h>
#include <pci.h>
#include <arch/io.h>
#include <cpu/p6/msr.h>
#include <cpu/p6/mtrr.h>

#define RAM(type, addr) (*((type *)((unsigned char*)((addr) - CACHE_RAM_BASE))))

#define SMBUS_BUS 0
#define SMBUS_DEVFN  ((0x1f << 3) + 3)

#define LPC_BUS 0
#define LPC_DEVFN ((0x1f << 3) + 0)

#define ICH2HUB_BUS 0
#define ICH2HUB_DEVFN ((0x1e << 3) + 0)

#define I860MCH_BUS 0
#define I860MCH_DEVFN ((0x00 << 3) + 0)

#define I860HUBA_BUS 0
#define I860HUBA_DEVFN ((0x01 << 3) + 0)

#define I860HUBB_BUS 0
#define I860HUBB_DEVFN ((0x02 << 3) + 0)

#define SMBUS_MEM_DEVICE_0 (0xa << 3)

static struct smbus_info{
	u32 base;
} smbus; 


#define SMBHSTSTAT 0x0
#define SMBHSTCTL  0x2
#define SMBHSTCMD  0x3
#define SMBXMITADD 0x4
#define SMBHSTDAT0 0x5
#define SMBHSTDAT1 0x6
#define SMBBLKDAT  0x7
#define SMBTRNSADD 0x9
#define SMBSLVDATA 0xa
#define SMLINK_PIN_CTL 0xe
#define SMBUS_PIN_CTL  0xf 

void smbus_setup(void)
{
	u8  smbus_enable;
	u16 smbus_func_enable;

	pcibios_write_config_dword(SMBUS_BUS, SMBUS_DEVFN, 0x20, 0x1000 | 1);
	pcibios_write_config_byte(SMBUS_BUS, SMBUS_DEVFN, 0x40, 1);
	pcibios_write_config_word(SMBUS_BUS, SMBUS_DEVFN, 0x4, 1);

	pcibios_read_config_dword(SMBUS_BUS, SMBUS_DEVFN, 0x20, &smbus.base);
	pcibios_read_config_byte(SMBUS_BUS, SMBUS_DEVFN, 0x40, &smbus_enable);
	pcibios_read_config_word(SMBUS_BUS, SMBUS_DEVFN, 0x4, &smbus_func_enable);
	
	printk_debug("smbus.base = %08x\n", smbus.base);
	printk_debug("smbus.enable = %02x\n", smbus_enable);
	printk_debug("smbus.func_enable = %04x\n", smbus_func_enable);

	smbus.base &= ~1;
	printk_debug("\n");
	printk_debug("smbus.base = %08x\n", smbus.base);
	printk_debug("smbus.enable = %02x\n", smbus_enable);
	printk_debug("smbus.func_enable = %04x\n", smbus_func_enable);

	/* Disable interrupt generation */
	outb(0, smbus.base + SMBHSTCTL);
}

static void smbus_wait_until_ready(void)
{
	while((inb(smbus.base + SMBHSTSTAT) & 1) == 1) {
		/* nop */
	}
}
static void smbus_wait_until_done(void)
{
	unsigned char byte;
	do {
		byte = inb(smbus.base + SMBHSTSTAT);
	}
	while((byte &1) == 1);
	while( (byte & ~(1|(1<<6))) == 0) {
		byte = inb(smbus.base + SMBHSTSTAT);
	}
}


#if 0
static void smbus_print_error(unsigned char host_status_register)
{

	printk_debug("smbus_error: 0x%02x\n", host_status_register);
	if (host_status_register & (1 << 7)) {
		printk_debug("Byte Done Status\n");
	}
	if (host_status_register & (1 << 6)) {
		printk_debug("In Use Status\n");
	}
	if (host_status_register & (1 << 5)) {
		printk_debug("SMBus Alert Status\n");
	}
	if (host_status_register & (1 << 4)) {
		printk_debug("Interrup/SMI# was Failed Bus Transaction\n");
	}
	if (host_status_register & (1 << 3)) {
		printk_debug("Bus Error\n");
	}
	if (host_status_register & (1 << 2)) {
		printk_debug("Device Error\n");
	}
	if (host_status_register & (1 << 1)) {
		printk_debug("Interrupt/SMI# was Successful Completion\n");
	}
	if (host_status_register & (1 << 0)) {
		printk_debug("Host Busy\n");
	}
}
#endif

static int smbus_read_byte(unsigned device, unsigned address, unsigned char *result)
{
	unsigned char host_status_register;
	unsigned char byte;

	smbus_wait_until_ready();

	/* setup transaction */
	/* disable interrupts */
	outb(inb(smbus.base + SMBHSTCTL) & (~1), smbus.base + SMBHSTCTL);
	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, smbus.base + SMBXMITADD);
	/* set the command/address... */
	outb(address & 0xFF, smbus.base + SMBHSTCMD);
	/* set up for a byte data read */
	outb((inb(smbus.base + SMBHSTCTL) & 0xE3) | (0x2 << 2), smbus.base + SMBHSTCTL);

	/* clear any lingering errors, so the transaction will run */
	outb(inb(smbus.base + SMBHSTSTAT), smbus.base + SMBHSTSTAT);

	/* clear the data byte...*/
	outb(0, smbus.base + SMBHSTDAT0);

	/* start the command */
	outb((inb(smbus.base + SMBHSTCTL) | 0x40), smbus.base + SMBHSTCTL);

	/* poll for transaction completion */
	smbus_wait_until_done();

	host_status_register = inb(smbus.base + SMBHSTSTAT);
#if 1
	/* Ignore the In Use Status... */
	host_status_register &= ~(1 << 6);
#endif	

	/* read results of transaction */
	byte = inb(smbus.base + SMBHSTDAT0);

#if 0
	if (host_status_register != 0x02) {
		smbus_print_error(host_status_register);
	}
#endif

	*result = byte;
	return host_status_register != 0x02;
}

#define FLOPPY_DEVICE 0
#define PARALLEL_DEVICE 1
#define COM1_DEVICE 2
#define COM2_DEVICE 3
#define KBC_DEVICE  5
#define CIR_DEVICE  6
#define GAME_PORT_DEVICE 7
#define GPIO_PORT2_DEVICE 8
#define GPIO_PORT3_DEVICE 9
#define ACPI_DEVICE 0xa
#define HW_MONITOR_DEVICE 0xb

#define SIO_PORT 0x2e

static void enter_pnp(void)
{
	outb(0x87, SIO_PORT);
	outb(0x87, SIO_PORT);
}

static void exit_pnp(void)
{
	outb(0xaa, SIO_PORT);
}

static void write_config(unsigned char value, unsigned char reg)
{
	outb(reg, SIO_PORT);
	outb(value, SIO_PORT +1);
}

static unsigned char read_config(unsigned char reg)
{
	outb(reg, SIO_PORT);
	return inb(SIO_PORT +1);
}
static void set_logical_device(int device)
{
	write_config(device, 0x07);
}

static void set_enable(int enable)
{
	write_config(enable?0x1:0x0, 0x30);
#if 0
	if (enable) {
		printk_debug("enabled superio device: %d\n", 
			read_config(0x07));
	}
#endif
}

#if 0
static void set_iobase0(unsigned iobase)
{
	write_config((iobase >> 8) & 0xff, 0x60);
	write_config(iobase & 0xff, 0x61);
}

static void set_iobase1(unsigned iobase)
{
	write_config((iobase >> 8) & 0xff, 0x62);
	write_config(iobase & 0xff, 0x63);
}

static void set_irq0(unsigned irq)
{
	write_config(irq, 0x70);
}

static void set_irq1(unsigned irq)
{
	write_config(irq, 0x72);
}

static void set_drq(unsigned drq)
{
	write_config(drq & 0xff, 0x74);
}
#endif

static void select_rdram_i2c(void)
{
	unsigned char byte;
	enter_pnp();
	byte = read_config(0x2b);
	byte |= 0x30;
	write_config(byte, 0x2b);
	set_logical_device(GPIO_PORT2_DEVICE);
	set_enable(1);
	byte = read_config(0xf0);
	byte &= ~(1 << 3);
	write_config(byte, 0xf0);
	exit_pnp();
}

#define I860_MCH_BUS 0
#define I860_MCH_DEVFN 0

#define MCH_GAR0	0x40
#define MCH_GAR1	0x41
#define MCH_GAR2	0x42
#define MCH_GAR3	0x43
#define MCH_GAR4	0x44
#define MCH_GAR5	0x45
#define MCH_GAR6	0x46
#define MCH_GAR7	0x47
#define MCH_GAR8	0x48
#define MCH_GAR9	0x49
#define MCH_GAR10	0x4a
#define MCH_GAR11	0x4b
#define MCH_GAR12	0x4c
#define MCH_GAR13	0x4d
#define MCH_GAR14	0x4e
#define MCH_GAR15	0x4f
#define MCH_MCHCFG	0x50
#define MCH_FDHC	0x58
#define MCH_PAM0	0x59
#define MCH_PAM1	0x5a
#define MCH_PAM2	0x5b
#define MCH_PAM3	0x5c
#define MCH_PAM4	0x5d
#define MCH_PAM5	0x5e
#define MCH_PAM6	0x5f
#define MCH_GBA0	0x60
#define MCH_GBA1	0x62
#define MCH_GBA2	0x64
#define MCH_GBA3	0x66
#define MCH_GBA4	0x68
#define MCH_GBA5	0x6a
#define MCH_GBA6	0x6c
#define MCH_GBA7	0x6e
#define MCH_GBA8	0x70
#define MCH_GBA9	0x72
#define MCH_GBA10	0x74
#define MCH_GBA11	0x76
#define MCH_GBA12	0x78
#define MCH_GBA13	0x7a
#define MCH_GBA14	0x7c
#define MCH_GBA15	0x7e
#define MCH_RDPS	0x88
#define MCH_DRD		0x90
#define MCH_RICM	0x94
#define MCH_SMRAM	0x9d
#define MCH_ESMRAM	0x9e
#define MCH_RDT		0xbe
#define MCH_TOM		0xc4
#define MCH_ERRSTS	0xc8
#define MCH_ERRCMD	0xca
#define MCH_SMICMD	0xcc
#define MCH_SCICMD	0xce
#define MCH_DRAMRC	0xdc
#define MCH_DERRCTL	0xe2
#define MCH_EAP		0xe4

#define RICM_BUSY (1 << 23)
#define RICM_DONE (1 << 27)


#define __S(NAME) NAME##_SHIFT
#define __B(NAME) NAME##_BITS
#define __BIT_MASK(SHIFT, BITS) ((1 << (BITS + SHIFT)) - (1 << (SHIFT)))
#define MASK(NAME) __BIT_MASK(__S(NAME), __B(NAME))
#define NVAL(NAME, VAL) (MASK(NAME) & ((VAL) << __S(NAME)))
#define VAL(NAME, VALUE) ((MASK(NAME) & VALUE) >> __S(NAME))

#define RICM_CMD_SHIFT		0
#define RICM_CMD_BITS		5
#define RICM_ADDR_SHIFT		5
#define RICM_ADDR_BITS		5
#define RICM_REG_SHIFT		10
#define RICM_REG_BITS		9
#define RICM_BROADCAST_SHIFT	19
#define RICM_BROADCAST_BITS	1
#define RICM_CHANNEL_SHIFT	20
#define RICM_CHANNEL_BITS	2

#define CMD_RDRAM_READ_REG		0
#define CMD_RDRAM_WRITE_REG		1
#define CMD_RDRAM_SET_RESET		2
#define CMD_SET_FAST_CLK		4
#define CMD_TEMP_CALIBRATE_ENABLE	5
#define CMD_TEMP_CALIBRATE		6
#define CMD_MRH_REDIRECT_NEXT_SIO	8
#define CMD_MRH_STICK_SIO_RESET		9
#define CMD_RDRAM_CLEAR_RESET		11
#define CMD_RDRAM_CURRENT_CALIBRATION	16
#define CMD_RDRAM_SIO_RESET		17
#define CMD_RDRAM_POWERDOWN_EXIT	18
#define CMD_RDRAM_POWERDOWN_ENTRY	19
#define CMD_RDRAM_NAP_ENTRY		20
#define CMD_RDRAM_NAP_EXIT		21
#define CMD_RDRAM_REFRESH		22
#define CMD_RDRAM_PRECHARGE		23
#define CMD_MANUAL_CURRENT_CALIBRATION	24
#define CMD_MCH_RAC_LOAD_RACA_CONFIG	25
#define CMD_MCH_RAC_LOAD_RACB_CONFIG	26
#define CMD_MCH_INITIALIZE_RAC		27
#define CMD_MCH_RAC_CURRENT_CALIBRATION	28
#define CMD_MCH_RAC_TEMP_CALIBRATE	29
#define CMD_POWERUP_ALL_SEQUENCE	31

#define REG_INIT	0x21
#define REG_TEST34	0x22
#define REG_CNFGA	0x23
#define REG_CNFGB	0x24
#define REG_DEVID	0x40
#define REG_REFB	0x41
#define REG_REFR	0x42
#define REG_CCA		0x43
#define REG_CCB		0x44
#define REG_NAPX	0x45
#define REG_PDNXA	0x46
#define REG_PDNX	0x47
#define REG_TPARM	0x48
#define REG_TFRM	0x49
#define REG_TCDLY1	0x4a
#define REG_SKIP	0x4b
#define REG_TCYCLE	0x4c
#define REG_TEST77	0x4d
#define REG_TEST78	0x4e
#define REG_TEST79	0x4f

#define BCAST 0xffff

static void rdram_wait_until_ready(void)
{
	u32 ricm;
	do {
		pcibios_read_config_dword(I860_MCH_BUS, I860_MCH_DEVFN, MCH_RICM, &ricm);
	} while(ricm & RICM_BUSY);
}

struct rdram_reg_values {
	u16 channel_a;
	u16 channel_b;
};

static void __rdram_run_command(u8 channel, u16 sdevice_id, u16 reg, u16 command)
{
	u32 ricm;
	int broadcast;

	broadcast = 0;
	if (sdevice_id == BCAST) {
		broadcast = 1;
		sdevice_id = 0;
	}

	/* Read the old register value and modify it */
	pcibios_read_config_dword(I860_MCH_BUS, I860_MCH_DEVFN, MCH_RICM, &ricm);
	ricm &= ~(MASK(RICM_CHANNEL) | MASK(RICM_BROADCAST) | MASK(RICM_ADDR) | 
		MASK(RICM_REG) | MASK(RICM_CMD));
	ricm |= NVAL(RICM_CHANNEL, channel);
	ricm |= NVAL(RICM_BROADCAST, broadcast);
	ricm |= NVAL(RICM_ADDR, sdevice_id);
	ricm |= NVAL(RICM_REG, reg);
	ricm |= NVAL(RICM_CMD, command);

	/* Write the command */
	pcibios_write_config_dword(I860_MCH_BUS, I860_MCH_DEVFN, MCH_RICM, ricm);

	/* Start the command running */
	ricm |= RICM_BUSY;
	pcibios_write_config_dword(I860_MCH_BUS, I860_MCH_DEVFN, MCH_RICM, ricm);

	/* Wait until the command completes */
	rdram_wait_until_ready();

}

static void rdram_run_command(u8 channel, u16 sdevice_id, u16 command)
{
	/* Wait until the previous cmd completes */
	rdram_wait_until_ready();

	/* Run the command */
	__rdram_run_command(channel, sdevice_id, 0, command);
}

static void rdram_read_reg(u8 channel, u16 sdevice_id, u16 reg, struct rdram_reg_values *res)
{
	u32 drd;
	
	/* Wait until the previous cmd completes */
	rdram_wait_until_ready();
	
	__rdram_run_command(channel, sdevice_id, reg, CMD_RDRAM_READ_REG);
	
	/* Read back the register value */
	pcibios_read_config_dword(I860_MCH_BUS, I860_MCH_DEVFN, MCH_DRD, &drd);

	res->channel_a = (drd >> 16) &0xffff;
	res->channel_b = drd & 0xffff;
	return;
}


static void rdram_write_reg(u8 channel, u16 sdevice_id, u16 reg, 
	u16 channel_a, u16 channel_b)
{
	u32 drd;
	
	/* Wait until the previous cmd completes */
	rdram_wait_until_ready();


	/* Write the data values */
	drd = (((u32)channel_a) << 16) | channel_b;
	pcibios_write_config_dword(I860_MCH_BUS, I860_MCH_DEVFN, MCH_DRD, drd);

	__rdram_run_command(channel, sdevice_id, reg, CMD_RDRAM_WRITE_REG);

	/* Wait until the command completes */
	rdram_wait_until_ready();

	return;
}

u16 rdram_regs[] = {
	REG_INIT,
	REG_TEST34,
	REG_CNFGA,
	REG_CNFGB,
	REG_DEVID,
	REG_REFB,
	REG_REFR,
	REG_CCA,
	REG_CCB,
	REG_NAPX,
	REG_PDNXA,
	REG_PDNX,
	REG_TPARM,
	REG_TFRM,
	REG_TCDLY1,
	REG_SKIP,
	REG_TCYCLE,
	REG_TEST77,
	REG_TEST78,
	REG_TEST79,
};
char *rdram_reg_names[] = {
	"INIT",
        "TEST34",
	"CNFGA",
	"CNFGB",
	"DEVID",
	"REFB",
	"REFR",
	"CCA",
	"CCB",
	"NAPX",
	"PDNXA",
	"PDNX",
	"TPARM",
	"TFRM",
	"TCDLY1",
	"SKIP",
	"TCYCLE",
	"TEST77",
	"TEST78",
	"TEST79",
};

static void set_sdevid(int rdram_devices)
{
	int i;
	/* First disable the repeater and set every RDRAM to the
	 * maximum address I can address 
	 */
	rdram_write_reg(0, BCAST, REG_INIT, 0x1f, 0x1f);
	for(i = 0; i < rdram_devices; i++) {
		/* Set SDEVID and reenable the repeater */
		rdram_write_reg(0, 0x1f, REG_INIT, 
			(1 << 7) | (i & 0x1f), (1 << 7) | (i & 0x1f));

	}
}

static void set_devid(int rdram_devices)
{
	int i;
	for(i = 0; i < rdram_devices; i++) {
		/* FIXME make this smarter */
		/* Initially set DEVID == SDEVID */
		if ((i % 4) != 5) {
			rdram_write_reg(0, i, REG_DEVID, i, i );
		} else {
			rdram_write_reg(0, i, REG_DEVID, 0x1f, i);
		}
	}
}

static void set_init_bits(int rdram_devices)
{
	int i;
	for(i = 0; i < rdram_devices; i++) {
		/* Now that the chip is up and running setup the
		 * power management modes.
		 */
		rdram_write_reg(0, i, REG_INIT, 
			(1 << 9) | (1 << 7) | (1 << 6) | (i & 0x1f),
			(1 << 9) | (1 << 7) | (1 << 6) | (i & 0x1f)
			);
	}
}

static void rdram_read_domain_initialization(int rdram_devices)
{
	/* FIXME Figure the rest of this out... */
	int i;
	for(i = 0; i < rdram_devices; i++) {
		struct rdram_reg_values values;
		u16 tcdly1_a, tcdly1_b;
		unsigned long addr, value;
		addr = i*32*1024*1024;
#if 0
		RAM(unsigned long, addr) = addr;
		value = RAM(unsigned long, addr);
#endif
		/* Decrement TCDLY1 for every chip that doesn't have auto-skip=1 */
		rdram_read_reg(0, i, REG_SKIP, &values);
		tcdly1_a = 2;
		tcdly1_b = 2;
		if (i >= 7) {
			tcdly1_b = 1;
		}
		if (i >= 10) {
			tcdly1_a = 1;
		}
		if (!(values.channel_a & (1 << 12))) {
			tcdly1_a--;
		}
		if (!(values.channel_b & (1 << 12))) {
			tcdly1_b--;
		}
		rdram_write_reg(0, i, REG_TCDLY1, tcdly1_a, tcdly1_b);
#if 0
		value = RAM(unsigned long, addr);
#endif
	}
#if 1
	/* RDRAM Device Timing */
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN,	MCH_RDT, 0x8a);
#endif

#if 0
	rdram_run_command(0, 0, CMD_MCH_RAC_LOAD_RACA_CONFIG);
	rdram_run_command(0, 0, CMD_MCH_RAC_LOAD_RACB_CONFIG);
#endif
	
}

static void ndelay(unsigned long ns)
{
	unsigned long long count;
	unsigned long long stop;
	unsigned long long ticks;

	/* FIXME calibrate this... don't just estimage 2Ghz */
	ticks = ns << 1;
	rdtscll(count);
	stop = ticks + count;
	while(stop > count) {
		rdtscll(count);
	}
}

static void rdram_set_clear_reset(void)
{
	rdram_write_reg(0, BCAST, REG_TEST78, 0x04, 0x04);
	rdram_write_reg(0, BCAST, REG_TEST34, 0x40, 0x40);
	rdram_run_command(0, BCAST, CMD_RDRAM_SET_RESET);
	/* FIXME Compute max of 16 * Tscycle or 2816 * Tcycle */
	ndelay(7040);
	rdram_run_command(0, BCAST, CMD_RDRAM_CLEAR_RESET);
	rdram_write_reg(0, BCAST, REG_TEST34, 0, 0);
	rdram_write_reg(0, BCAST, REG_TEST78, 0, 0);
}

/*
 - ro CNFGA, CNFGB,
 - ok REFB, REFR
 -  SKIP, TEST79
  TEST34, TEST78,
  TEST77,
  TCYCLE,
  INIT,
  DEVID,
  PDNXA, PDNX, NAPX, TPARM, TCDLY1, TFRM, REG_CCA, REG, CCB
*/
static void rdram_init(int rdram_devices)
{
	/* 3.1/3.2 RDRAM SIO reset */
	rdram_run_command(0, 0, CMD_RDRAM_SIO_RESET);

	/* 3.3 Clear TEST77 */
	rdram_write_reg(0, BCAST, REG_TEST77, 0, 0);
	
	/* 3.4 Write Tcycle */
	/* FIXME Hard coded as 400Mhz for now */
	rdram_write_reg(0, BCAST, REG_TCYCLE, 0x27, 0x27);
	
	/* 3.5 Set SDEVID */
	set_sdevid(rdram_devices);

	/* 3.6 Set DEVID */
	set_devid(rdram_devices);

	/* 3.7 Write PDNX, PDNXA Registers */
	/* FIXME don't hard code these timings... */
	rdram_write_reg(0, BCAST, REG_PDNXA, 0x7, 0x7);
	rdram_write_reg(0, BCAST, REG_PDNX, 0x5, 0x5);

	/* 3.8 Write NAPX */
	/* FIXME don't hard code this timing */
	rdram_write_reg(0, BCAST, REG_NAPX, 0x525, 0x525);
	
	/* 3.9 Write TPARM */
	/* FIXME figure how to compute this initial timing. */
	rdram_write_reg(0, BCAST, REG_TPARM, 0x3a, 0x3a);

	/* 3.10 Write TCDLY1 Register */
	/* FIXME figure how to compute this initial timing. */
	rdram_write_reg(0, BCAST, REG_TCDLY1, 2, 2);

	/* 3.11 Write TFRM Register */
	/* FIXME refine the computation on this register. */
#if 0
	rdram_write_reg(0, BCAST, REG_TFRM, 10, 10);
#else
	rdram_write_reg(0, BCAST, REG_TFRM, 9, 9);
#endif

	/* 3.12 SETR/CLRR */
	rdram_set_clear_reset();

	/* 3.13 Write CCA and CCB Registers */
	/* Program all Current controll registers with
	 * an initial approximation.  1/2 their maximum is recommended.
	 */
	/* FIXME figure out voltage assymetry */
	rdram_write_reg(0, BCAST, REG_CCA, 0x40, 0x40);
	rdram_write_reg(0, BCAST, REG_CCB, 0x40, 0x40);

	/* 3.14 Powerdown Exit */
	/* FIXME not all RDRAMS models need this */
	rdram_run_command(0, BCAST, CMD_RDRAM_POWERDOWN_EXIT);

	/* 3.15 SETF */
	rdram_run_command(0, BCAST, CMD_SET_FAST_CLK);
}

void mch_init(void)
{
	u8 byte;
	u16 word;
	u32 dword;
	/* FIXME unhard code these values */
	/* Program Group Attribute Registers */
	/* 1KB pages, 2x16 banks, 128/144Mbit */
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR0, 0x92); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR1, 0x92); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR2, 0x92); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR3, 0x92); 
	/* Nothing.. */
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR4, 0x80); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR5, 0x80); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR6, 0x80); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR7, 0x80); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR8, 0x80); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR9, 0x80); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR10, 0x80); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR11, 0x80); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR12, 0x80); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR13, 0x80); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR14, 0x80); 
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GAR15, 0x80); 

	/* Memory Controller Hub Configuration */
	/* 2 System Bus Stop Grant, 400Mhz, Non-ECC, MDA Not Present, Apic Enabled */
	pcibios_read_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_MCHCFG, &word); 
	word &= ~((7 << 13) | (1 << 9) | (3 << 7) | (1 << 5) | (1 << 0));
	word |= (1 << 13) | (1 << 11);
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_MCHCFG, word); 
	
	/* Fixed DRAM Hole Control */
	/* Disabled */
	pcibios_read_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_FDHC, &byte);
	byte &= ~(1 << 7);
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_FDHC, byte);

	/* Programmable Attribute Map */
	/* All Ram, Except the mandatory VGA hole */
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_PAM0, 0x30);
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_PAM1, 0x33);
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_PAM2, 0x33);
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_PAM3, 0x33);
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_PAM4, 0x33);
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_PAM5, 0x33);
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_PAM6, 0x33);

	/* RDRAM Device Group Boundary Addresses */
	/* 4 groups of 8*16 MB each */
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA0, (0<<11)|(8<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA1, (1<<11)|(16<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA2, (2<<11)|(24<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA3, (3<<11)|(32<<0));
	/* The rest of the groups are empty */
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA4, (4<<11)|(32<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA5, (5<<11)|(32<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA6, (6<<11)|(32<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA7, (7<<11)|(32<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA8, (8<<11)|(32<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA9, (9<<11)|(32<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA10, (10<<11)|(32<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA11, (11<<11)|(32<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA12, (12<<11)|(32<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA13, (13<<11)|(32<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA14, (14<<11)|(32<<0));
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_GBA15, (15<<11)|(32<<0));

	/* RDRAM Deice Pool Sizeing Register */
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_RDPS, 0x0f);

	/* RDRAM Device Timing */
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_RDT, 0x8e);

	/* Top of Memory */
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_TOM, 0x2000);
	
	/* Error Command Register */
	/* Disable reporting errors for now */
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_ERRCMD, 0);
	
	/* SMI Command Register */
	/* Disable reporting errors for now */
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_SMICMD, 0);

	/* SCI Command Register */
	/* Disable reporting errors for now */
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_SCICMD, 0);

	/* RDRAM Device Refresh Control */
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_DRAMRC, 1);
};


static void init_memory(void)
{
	int i;
	int rdram_devices = 16;
	u32 ricm;
	/* 1. Start the clocks */
	rdram_run_command(0, 0, CMD_POWERUP_ALL_SEQUENCE);

	/* 2. RAC initialization */
	rdram_run_command(0, 0, CMD_MCH_INITIALIZE_RAC);

	/* 3. Rdram initialize */
	rdram_init(rdram_devices);

	/* 4. Initialize Memory Controller */
	mch_init();

	/* 6.1 RDRAM Core Initialization */
	for(i = 0; i < 192; i++) {
		rdram_run_command(0, BCAST, CMD_RDRAM_REFRESH);
	}
	
	/* 5. Rdram Current Control */
	/* You can disable the final memory initialized bit 
	 * to see if this is working and it isn't!
	 * It is close though.
	 */
#if 1
	rdram_run_command(0, 0, CMD_MCH_RAC_CURRENT_CALIBRATION);
#endif
#if 0
	rdram_run_command(0, 0, CMD_MANUAL_CURRENT_CALIBRATION);
#endif
	for(i = 0; i < 128; i++) {
		rdram_run_command(0, BCAST, CMD_RDRAM_CURRENT_CALIBRATION);
	}
#if 0
#endif
	rdram_run_command(0, BCAST, CMD_TEMP_CALIBRATE_ENABLE);
	rdram_run_command(0, BCAST, CMD_TEMP_CALIBRATE);

	/* 6.2 RDRAM Read Domain Initialization */
#if 1
	rdram_read_domain_initialization(rdram_devices);
#endif

	/* 7.0 Remaining init bits (LSR, NSR, PSR) */
	set_init_bits(rdram_devices);

	/* Final Set the memory initialized bit */
#if 1
	pcibios_read_config_dword(I860_MCH_BUS, I860_MCH_DEVFN, MCH_RICM, &ricm);
	ricm |= RICM_DONE;
	pcibios_write_config_dword(I860_MCH_BUS, I860_MCH_DEVFN,
		MCH_RICM, ricm);
#endif
	
}

static void ram_fill(unsigned long start, unsigned long stop)
{
	unsigned long addr;
	/* 
	 * Fill.
	 */
	printk_debug("DRAM fill: %08lx-%08lx\n", start, stop);
	for(addr = start; addr < stop ; addr += 4) {
		/* Display address being filled */
		if ((addr & 0xffff) == 0)
			printk_debug("%08lx\r", addr);
		RAM(unsigned long, addr) = addr;
	};
	/* Display final address */
	printk_debug("%08lx\nDRAM filled\n", addr);

	
}

static int ram_verify(unsigned long start, unsigned long stop, int max_errors)
{
	unsigned long addr;
	int errors = 0;
	/* 
	 * Verify.
	 */
	printk_debug("DRAM verify: %08lx-%08lx\n", start, stop);
	for(addr = start; addr < stop ; addr += 4) {
		unsigned long value;
		/* Display address being tested */
		if ((addr & 0xffff) == 0)
			printk_debug("%08lx\r", addr); 
		value = RAM(unsigned long, addr);
		if (value != addr) {
			if (++errors <= max_errors) {
				/* Display address with error */
				printk_debug("%08lx:%08lx\n", addr, value);
			}
		}
	}
	/* Display final address */
	printk_debug("%08lx\nDRAM verified %d/%d errors\n", 
		addr, errors, (stop - start)/4);
	return errors;
}


static int ram_odd_verify(unsigned long start, unsigned long stop, int max_errors)
{
	unsigned long addr;
	int errors = 0;
	/* 
	 * Verify.
	 */
	printk_debug("DRAM odd verify: %08lx-%08lx\n", start, stop);
	for(addr = start; addr < stop ; addr += 4) {
		unsigned long value;
		/* Display address being tested */
		if ((addr & 0xffff) == 0)
			printk_debug("%08lx\r", addr); 
		value = RAM(unsigned long, addr);
		if (value != (addr ^ 0x20)) {
			if (++errors < max_errors) {
				/* Display address with error */
				printk_debug("%08lx:%08lx\n", addr, value);
			}
		}
	}
	/* Display final address */
	printk_debug("%08lx\nDRAM odd verified %d/%d errors\n", 
		addr, errors, (stop - start)/4);
	return 0;
}


static int ramcheck(unsigned long start, unsigned long stop, int max_errors)
{
	int result;
	/*
	 * This is much more of a "Is my DRAM properly configured?"
	 * test than a "Is my DRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 */
	printk_debug("Testing DRAM : %08lx-%08lx\n",
		start, stop);

	ram_fill(start, stop);
	result = ram_verify(start, stop, max_errors);
	printk_debug("Done.\n");
	return result;
}


static int ramcheck2(unsigned long start, unsigned long stop, int max_errors)
{
	int result;
	/*
	 * This is much more of a "Is my DRAM properly configured?"
	 * test than a "Is my DRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 */
	printk_debug("Testing DRAM : %08lx-%08lx\n",
		start, stop);

	ram_fill(start, stop);
	result = ram_odd_verify(start, stop, max_errors);

	printk_debug("Done.\n");
	return result;
}

/* setting variable mtrr, comes from linux kernel source */
static void set_var_mtrr(unsigned int reg, unsigned long base, unsigned long size, unsigned char type)
{
	unsigned int tmp;

	if (reg >= 8)
		return;

	// it is recommended that we disable and enable cache when we 
	// do this. 
	/* Disable cache */
	/* Write back the cache and flush TLB */
	asm volatile (
		"movl  %%cr0, %0\n\t"
		"orl  $0x40000000, %0\n\t"
		"movl  %0, %%cr0\n\t"
		:"=r" (tmp)
		::"memory");

	if (size == 0) {
		/* The invalid bit is kept in the mask, so we simply clear the
		   relevant mask register to disable a range. */
		wrmsr (MTRRphysMask_MSR (reg), 0, 0);
	} else {
		/* Bit 32-35 of MTRRphysMask should be set to 1 */
		wrmsr (MTRRphysBase_MSR (reg), base | type, 0);
		wrmsr (MTRRphysMask_MSR (reg), ~(size - 1) | 0x800, 0x0F);
	}

	// turn cache back on. 
	asm volatile ("movl  %%cr0, %0\n\t"
		      "andl  $0x9fffffff, %0\n\t"
		      "movl  %0, %%cr0\n\t":"=r" (tmp)::"memory");

}

void cache_ram_start(void)
{
	int i,j;
	int error;
	error = 0;
	/* displayinit MUST PRECEDE ALL PRINTK! */
	displayinit();
	printk_info("printk: Testing %d %s\n", 123, "testing!!!");
	printk_info("Finding PCI configuration type.\n");
	pci_set_method();
	printk_info("Setting up smbus controller\n");
	smbus_setup();
	printk_info("Selecting rdram i2c bus\n");
	select_rdram_i2c();	

#if 0
	for(j = SMBUS_MEM_DEVICE_0; j < SMBUS_MEM_DEVICE_0 + 8; j++) {
		int status = 0;
		if ((j == 0x1b) || 0) {
			printk_debug("skipping device: %02x\n", j);
			continue;
		}
		printk_debug("smbus_device: %02x\n", j);
		for(i = 0; (i < 256) && (status == 0); i++) {
			unsigned char byte;
			status = smbus_read_byte(j, i, &byte);
			if (status != 0) {
				printk_debug("bad device\n");
				continue;
			}
			printk_debug("0x%02x ", byte);
			if ((i &0x0f) == 0x0f) {
				printk_debug("\n");
			}
		}
	}
	printk_debug("\n");
#endif
	init_memory();
	printk_debug("set_var_mtrr\n");
#if 0
	set_var_mtrr(0, 0, 512*1024*1024, MTRR_TYPE_WRBACK);
#else
	set_var_mtrr(0, 0, 512*1024*1024, MTRR_TYPE_WRCOMB);
#endif
	printk_debug("set_var_mtrr done\n");
#if 0
	error |= ramcheck(0x000f0000, 0x000f1000, 20);
#endif
#if 1
	error |= ramcheck(   0x00000000, 0x00000400, 20);
	error |= ramcheck(   0x00000400, 0x00000800, 20);
	error |= ram_verify( 0x00000000, 0x00000400, 20);
	error |= ram_verify( 0x00000400, 0x00000800, 20);
#endif
#if 1
	{
		unsigned long addr;
		for(addr =  0; addr < 0x20000000; addr += 0x02000000) {
			ram_fill(addr, addr + 0x400);
		}
		/* Do some dummy writes to flush a write cache, in the
		 * processor.
		 */
		ram_fill(0xc0000000, 0xc0000400);
		for(addr =  0; addr < 0x20000000; addr += 0x02000000) {
			ram_verify(addr, addr + 0x400, 1);
		}
	}
#endif
#if 0
	error |= ramcheck( 0x00000000, 0x00000400, 128);
	error |= ramcheck2(0x00000000, 0x00000400, 128);
	error |= ramcheck( 0x14000000, 0x14000400, 128);
	error |= ramcheck2(0x14000000, 0x14000400, 128);
#endif
#if 0
	error |= ramcheck2(0x00100000, 0x00180000, 128);
#endif
	error |= ramcheck(0x00000000, 0x00080000, 20);
	error |= ramcheck(0x02000000, 0x02080000, 20);
	error |= ramcheck(0x04000000, 0x04080000, 20);
	error |= ramcheck(0x06000000, 0x06080000, 20);
	error |= ramcheck(0x08000000, 0x08080000, 20);
	error |= ramcheck(0x0a000000, 0x0a080000, 20);
	error |= ramcheck(0x0c000000, 0x0c080000, 20);

	error |= ramcheck(0x0e000000, 0x0e080000, 20);
	error |= ramcheck(0x10000000, 0x10080000, 20);
	error |= ramcheck(0x12000000, 0x12080000, 20);
	error |= ramcheck(0x14000000, 0x14080000, 20);
	error |= ramcheck(0x16000000, 0x16080000, 20);
	error |= ramcheck(0x18000000, 0x18080000, 20);
	error |= ramcheck(0x1a000000, 0x1a080000, 20);
	error |= ramcheck(0x1c000000, 0x1c080000, 20);
	error |= ramcheck(0x1e000000, 0x1e080000, 20);
#if 0
	error |= ramcheck(0x00000000, 0x00080000, 20);
#endif
#if 1
	for(i = 0; i < 16; i++) {
		for(j = 0; j < sizeof(rdram_regs)/sizeof(rdram_regs[0]); j++) {
			struct rdram_reg_values values;
			u16 reg = rdram_regs[j];
			rdram_read_reg(0, i, reg, &values);
			printk_debug("rdram: %2d reg: %02x %10s a: 0x%04x b: 0x%04x\n",
				i, reg, rdram_reg_names[j], values.channel_a, values.channel_b);
		}
	}
#endif
#if 1
	for(i = 0; i < 16; i++) {
		printk_debug("%02x: ", i << 4);
		for(j = 0; j < 16; j++) {
			u8 byte;
			u8 addr = (i << 4) | j;
			pcibios_read_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, addr, &byte);
			printk_debug("%02x ", byte);
		}
		printk_debug("\n");
	}
#endif
	if (error) while(1);
}
