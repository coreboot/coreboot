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
#include <arch/cache_ram.h>
#include <smbus.h>
#include <ramtest.h>
#include <southbridge/intel/82801.h>
#include <superio/generic.h>
#include <superio/w83627hf.h>
#include <northbridge/intel/82860/rdram.h>

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

#define TSCYCLE_SLOW 1000  /* ns */
#define TSCYCLE_FAST 10    /* ns */
void ndelay(unsigned long);

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
	ndelay(1000);   /* delay after ready because the documentation says to */
}

/* rdram timing tables */
static u16 tparm[5]={0x3a,0x3a,0x3a,0x4a,0x5a};
static u16 tcdly1[5]={0,1,2,2,2};

/* serial presence detect needed variables */
/* Byte 99 number of devices for each stick */
static u8 spd_devices[4]={0,0,0,0};
/* Byte 4 # of row address bits, # of column address bits */
static u8 spd_row_col[4]={0,0,0,0};
/* Byte 5 # of bank bits lower nibble */
static u8 spd_banks[4]={0,0,0,0};
/* Byte 9 Misc. Device configuration S28, and S3 bits are used  */
static u8 spd_misc_conf[4]={0,0,0,0};
/* Byte 31 power down exit max time phase A (tPDNXA,max) */
static u8 spd_pdnxa_max[4]={0,0,0,0};
/* Byte 32 power down exit max time phase B (tPDNXB,max) */
static u8 spd_pdnxb_max[4]={0,0,0,0};
/* Byte 33 Map exit max time phase A (tNAPXA,max) */
static u8 spd_napxa_max[4]={0,0,0,0};
/* Byte 34 Nap exit max time phase B (tNAPXB,max) */
static u8 spd_napxb_max[4]={0,0,0,0};
/* Byte 12 Min ras to cas cycles */ 
static u8 spd_rcd_min[4]={0,0,0,0};
/* Byte 100 Module data width */ 
static u8 spd_data_width[4]={0,0,0,0};
/* Byte 35 bits 0-3<<8 + byte 37 give the rdram max mhz rate */
static u16 spd_mhz_max[4]={0,0,0,0};
/* Byte 35 bits 4-7<<4 + byte 36 give the rdram min mhz rate */
static u16 spd_mhz_min[4]={0,0,0,0};
/* The size of each device in mega bytes. */
static u8 spd_size[2]={0,0};

int rdram_chips=0; /* number of ram chips on the rimms */
static u32 total_rdram;   /* Total rdram found */

/* register index tables */
static u8  mch_gar[16] ={MCH_GAR0,MCH_GAR1,MCH_GAR2,MCH_GAR3,MCH_GAR4,MCH_GAR5,
			MCH_GAR6,MCH_GAR7,MCH_GAR8,MCH_GAR9,MCH_GAR10,MCH_GAR11,
			MCH_GAR12,MCH_GAR13,MCH_GAR14,MCH_GAR15};

static u8  mch_gba[16] ={MCH_GBA0,MCH_GBA1,MCH_GBA2,MCH_GBA3,MCH_GBA4,MCH_GBA5,
			MCH_GBA6,MCH_GBA7,MCH_GBA8,MCH_GBA9,MCH_GBA10,MCH_GBA11,
			MCH_GBA12,MCH_GBA13,MCH_GBA14,MCH_GBA15};

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

void rdram_read_reg(u8 channel, u16 sdevice_id, u16 reg, struct rdram_reg_values *res)
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
	u8 rdt=0x8d;
	int i,j,k;
	u32 data;
	u32 *mem;
	u8 adj_a[32],adj_b[32];
	u8 l=20;

	total_rdram=0;
	if((spd_rcd_min[0]==8)||(spd_rcd_min[1]==8))
		rdt=0x4d;
	/* Set all the rdram devices to the fastest clock cycles */
	rdram_write_reg(0, BCAST, REG_TCDLY1, 0, 0 );
	rdram_write_reg(0, BCAST, REG_TPARM, 0x3a, 0x3a);

	/* find the slowest RDT timming */
	pcibios_write_config_byte(I860_MCH_BUS,I860_MCH_DEVFN,MCH_RDT,rdt);
	mem=RAM_ADDR(0x100000);
	for(j=0;j<rdram_devices;j++) {
		for(k=0,i=4,data=0x00000001;k< ((2048/4)+4);k++,i++,data+=0x00000001) {
			if(i>7) {
				data-=00000004;
				i=0;
			}
			mem[k]=data;
		}
  		printk_debug("Device = %d, %x,  %x\n",j,mem[0],mem[4]);
		adj_a[j]=(mem[0]&0x0ff)-1;
		adj_b[j]=(mem[4]&0x0ff)-1;
		if(adj_a[j]<l) l=adj_a[j];
		if(adj_b[j]<l) l=adj_b[j];
		if(j<spd_devices[0]) {
			if(spd_size[0]==32)
				mem+=0x1000000;  /* add 64 meg */
			else
				mem+=0x0800000;  /* add 32 meg */
		}
		else {
			if(spd_size[1]==32)
				mem+=0x1000000;  /* add 64 meg */
			else
				mem+=0x0800000;  /* add 32 meg */
		}
	}
	rdt-=l;
	printk_debug("RDT = %x, Lowest Offset = %d\n",rdt,l);
	for(i=0;i<rdram_devices;i++) {
		adj_a[i]-=l;
		adj_b[i]-=l;
  		printk_debug("Device = %d, A Offset = %d, B Offset = %d\n",
				i,adj_a[i],adj_b[i]);
	}
	/* RDRAM Device Timing */
	pcibios_write_config_byte(I860_MCH_BUS,I860_MCH_DEVFN,MCH_RDT,rdt);

	for(i = 0; i < rdram_devices; i++) {
		rdram_write_reg(0, i, REG_TCDLY1, tcdly1[adj_a[i]], tcdly1[adj_b[i]]);
		rdram_write_reg(0, i, REG_TPARM, tparm[adj_a[i]], tparm[adj_b[i]]);
	}

	display_rdram_regs_tparm(rdram_chips);
#if 0
	rdram_run_command(0, 0, CMD_MCH_RAC_LOAD_RACA_CONFIG);
	rdram_run_command(0, 0, CMD_MCH_RAC_LOAD_RACB_CONFIG);
#endif
	
}

static void rdram_set_clear_reset(void)
{
	int delay=0;
	int tcycle;

	rdram_write_reg(0, BCAST, REG_TEST78, 0x04, 0x04);
	rdram_write_reg(0, BCAST, REG_TEST34, 0x40, 0x40);
	rdram_run_command(0, BCAST, CMD_RDRAM_SET_RESET);
	/* Compute max of 16 * Tscycle or 2816 * Tcycle */
	if(spd_mhz_min[0]) {
		tcycle=(999+spd_mhz_min[0])/spd_mhz_min[0];
		delay=tcycle*2816;
	}	
	if(delay>(16*TSCYCLE_SLOW))
		ndelay(delay);
	else
		ndelay(16*TSCYCLE_SLOW);
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
	u16 tcycle;
	u16 pdnxa,pdnxb,pdnx,napx,napxa,napxb;
	u16 tfrm;
	int i;

	/* 3.1/3.2 RDRAM SIO reset */
	rdram_run_command(0, 0, CMD_RDRAM_SIO_RESET);

	/* 3.3 Clear TEST77 */
	rdram_write_reg(0, BCAST, REG_TEST77, 0, 0);
	
	/* 3.4 Write Tcycle */
	/* Calculate the Tcycle */
	for(tcycle=spd_mhz_max[0],i=1;i<4;i++)
		if(spd_mhz_max[i]&&(tcycle>spd_mhz_max[i]))
			tcycle=spd_mhz_max[i];
	tcycle = 15625 / tcycle;
	rdram_write_reg(0, BCAST, REG_TCYCLE, tcycle, tcycle);
  	printk_debug("Tcycle = %x\n",tcycle);
	
	/* 3.5 Set SDEVID */
	set_sdevid(rdram_devices);

	/* 3.6 Set DEVID */
	set_devid(rdram_devices);

	/* 3.7 Write PDNX, PDNXA Registers */
	/* tscycle=10ns or <= 100MHz MCH datasheet pg 156 */
	for(pdnxa=(u16)spd_pdnxa_max[0],i=1;i<4;i++)
		if(pdnxa<spd_pdnxa_max[i])
			pdnxa=spd_pdnxa_max[i];
	for(pdnxb=(u16)spd_pdnxb_max[0],i=1;i<4;i++)
		if(pdnxb<(u16)spd_pdnxb_max[i])
			pdnxb=(u16)spd_pdnxb_max[i];
	/* spd-31*1000 + 635 as a round up factor / (64*SCK) */
	pdnx=(pdnxa*1000+635)/640;
        printk_debug("PDNXA = %d\n",pdnx);
	rdram_write_reg(0, BCAST, REG_PDNXA, pdnx, pdnx);
	/* spd-31*1000 + spd-32*64 + 2555 round up factor / (256*SCK) */
	pdnx=((pdnxa*1000)+(pdnxb*64)+2555)/2560;
	rdram_write_reg(0, BCAST, REG_PDNX, pdnx, pdnx);
  	printk_debug("PDNX = %d\n",pdnx);

	/* 3.8 Write NAPX */
	/* (1<<10 DQS)+(((spd-33+spd-34)/SCK)<<5)+(spd-33/SCK) */
	/* find the max values of the spds for napxa and napxb */
	for(napxa=(u16)spd_napxa_max[0],i=1;i<4;i++)
		if(napxa<spd_napxa_max[i])
			napxa=spd_napxa_max[i];
	for(napxb=(u16)spd_napxb_max[0],i=1;i<4;i++)
		if(napxb<spd_napxb_max[i])
			napxb=spd_napxb_max[i];
	if(spd_misc_conf[0]&1)
		napx=(1<<10);
	else
		napx=0;
	napx+=(((napxa+napxb+9)/10)<<5)+((napxa+9)/10);
	rdram_write_reg(0, BCAST, REG_NAPX, napx, napx);
  	printk_debug("NAPX = %x\n",napx);
	
	/* 3.9 Write TPARM */
	/* Use max times for initial timing. Then redo in step 6.2 */
	rdram_write_reg(0, BCAST, REG_TPARM, 0x3a, 0x3a);

	/* 3.10 Write TCDLY1 Register */
	/* Use max times for initial timing. Then redo in step 6.2 */
	rdram_write_reg(0, BCAST, REG_TCDLY1, 2, 2);

	/* 3.11 Write TFRM Register */
	/* As far as I can tell the equation for TFRM is correct, */
	/* however, the documention is not clear, and some assumptions */
	/* were made.  If ram does not work, this would be a good */
	/* CHECKME */
	if(spd_rcd_min[0]) {
		tfrm = spd_rcd_min[0] - 1;
		while(tfrm > 10)
			tfrm-=4;
	}
	else {
		tfrm = 9;
	}
	rdram_write_reg(0, BCAST, REG_TFRM, tfrm, tfrm);

	/* 3.12 SETR/CLRR */
	rdram_set_clear_reset();

	/* 3.13 Write CCA and CCB Registers */
	/* Program all Current controll registers with
	 * an initial approximation.  1/2 their maximum is recommended.
	 */
	rdram_write_reg(0, BCAST, REG_CCA, 0x40, 0x40);
	rdram_write_reg(0, BCAST, REG_CCB, 0x40, 0x40);

	/* 3.14 Powerdown Exit */
	/* test is S28IECO is set, if yes power up ram from PDN state */
	if(spd_misc_conf[0]&4)
		rdram_run_command(0, BCAST, CMD_RDRAM_POWERDOWN_EXIT);

	/* 3.15 SETF */
	rdram_run_command(0, BCAST, CMD_SET_FAST_CLK);
}

void mch_init(void)
{
	u8 byte;
	u16 word;
	u16 top;
	int bits1,bits2;
	int reg,reg_last,dev;

	/* Program Group Attribute Registers */
	/* Calculate the GAR value */
	bits1=(spd_row_col[0]>>4)+(spd_row_col[0]&0x0f)+spd_banks[0];
	byte=0x80;
	if(bits1==21) {
		byte=0x84;
		spd_size[0]=32;
	}
	else if(bits1==20) {
		byte=0x82;
		spd_size[0]=16;
	}
	if(byte!=0x80) {
		if(spd_banks[0]==5)  byte|=0x10;
		if((spd_row_col[0]&0x0f)==7) byte|=0x40;
	}
	for(reg=dev=0;dev<spd_devices[0];reg++,dev+=4) {
		pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, mch_gar[reg], byte); 
  		printk_debug("GAR reg = %x, byte = %x\n",mch_gar[reg],byte);
	}
	/* Test for 2nd set of rimms */
	bits2=(spd_row_col[1]>>4)+(spd_row_col[1]&0x0f)+spd_banks[1];
	byte=0x80;
	if(bits2==21) {
		byte=0x84;
		spd_size[1]=32;
	}
	else if(bits2==20) {
		byte=0x82;
		spd_size[1]=16;
	}
	if(byte!=0x80) {
		if(spd_banks[1]==5)  byte|=0x10;
		if((spd_row_col[1]&0x0f)==7) byte|=0x40;
	}
	for(dev=0;dev<spd_devices[1];reg++,dev+=4) {
		pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, mch_gar[reg], byte); 
  		printk_debug("GAR reg = %x, byte = %x\n",mch_gar[reg],byte);
	}

	/* The rest are not used because the board has 4 slots & no repeter hubs */
	for(;reg<16;reg++) {
		pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, mch_gar[reg], 0x80); 
  		printk_debug("GAR reg = %x, byte = 0x80\n",mch_gar[reg]);
	}

	/* Memory Controller Hub Configuration */
	/* 1 System Bus Stop Grant,300 or 400Mhz, Non-ECC, MDA Not Present, Apic Enabled */
	pcibios_read_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_MCHCFG, &word); 
	word &= ~((7 << 13) | (1 << 9) | (3 << 7) | (1 << 5) | (1 << 1));
 	if(((spd_mhz_max[0]==400)&&(spd_mhz_max[1]==400)) ||
	   ((spd_mhz_max[0]==400)&&(spd_mhz_max[1]==0)))
		word |= (1 << 11);  /* Sets ram bus speed at 400 mhz, else 300 mhz */
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
	if(bits1==21) word=16;
	else word=8;
	for(reg=dev=0,top=0;dev<spd_devices[0];dev+=4,reg++) {
		top+=word;
		pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, mch_gba[reg], (reg<<11)|top);
  		printk_debug("GBA reg = %x, word = %x\n",mch_gba[reg],(reg<<11)|top);
	}

	word=0;
	if(bits2==21) word=16;
	else if(bits2==20) word=8;
	
	for(dev=0;dev<spd_devices[1];dev+=4,reg++) {
		top+=word;
		pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, mch_gba[reg], (reg<<11)|top);
  		printk_debug("GBA reg = %x, word = %x\n",mch_gba[reg],(reg<<11)|top);
	}

	/* The rest are filled with the high address */
	for(reg_last=reg-1;reg<16;reg++) {
		pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, mch_gba[reg], (reg_last<<11)|top);
  		printk_debug("GBA reg = %x, word = %x\n",mch_gba[reg],(reg_last<<11)|top);
	}

	/* RDRAM Deice Pool Sizeing Register */
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_RDPS, 0x0f);

	/* RDRAM Device Timing */
	pcibios_write_config_byte(I860_MCH_BUS, I860_MCH_DEVFN, MCH_RDT, 0x8e);

	/* Top of Memory */
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_TOM, top<<8);
  	printk_debug("TOM = %x <<8\n",top);
	
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
	/* Hard coded the faster refrest time to make sure memory is refreshed */
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_DRAMRC, 1);
}


static void load_spd_vars(void)
{
	int j;
	int status;
	unsigned char byte;
	u16 max,min;

	for(j = SMBUS_MEM_DEVICE_0; j < SMBUS_MEM_DEVICE_0 + 4; j++) {
		status = 0;
		if ((j == 0x1b) || 0) {
			continue;
		}
		status = smbus_read_byte(j, 4, &byte);
		if (status == 0) {
			spd_row_col[j-SMBUS_MEM_DEVICE_0]=byte;
		}
		status = smbus_read_byte(j, 5, &byte);
		if (status == 0) {
			spd_banks[j-SMBUS_MEM_DEVICE_0]=byte&0xf;
		}
		status = smbus_read_byte(j, 9, &byte);
		if (status == 0) {
			spd_misc_conf[j-SMBUS_MEM_DEVICE_0]=byte;
		}
		status = smbus_read_byte(j, 12, &byte);
		if (status == 0) {
			spd_rcd_min[j-SMBUS_MEM_DEVICE_0]=byte;
		}
		status = smbus_read_byte(j, 31, &byte);
		if (status == 0) {
			spd_pdnxa_max[j-SMBUS_MEM_DEVICE_0]=byte;
		}
		status = smbus_read_byte(j, 32, &byte);
		if (status == 0) {
			spd_pdnxb_max[j-SMBUS_MEM_DEVICE_0]=byte;
		}
		status = smbus_read_byte(j, 33, &byte);
		if (status == 0) {
			spd_napxa_max[j-SMBUS_MEM_DEVICE_0]=byte;
		}
		status = smbus_read_byte(j, 34, &byte);
		if (status == 0) {
			spd_napxb_max[j-SMBUS_MEM_DEVICE_0]=byte;
		}
		status = smbus_read_byte(j, 35, &byte);
		if (status == 0) {
			min=((u16)byte<<4)&0x0f00;  /* shift the high 4 bits into the next byte */
			max=((u16)byte<<8)&0x0f00;  /* shift the low 4 bits into the next byte */
			status = smbus_read_byte(j, 36, &byte);
			if (status == 0) {
				spd_mhz_min[j-SMBUS_MEM_DEVICE_0] =  min | byte;
			}
			status = smbus_read_byte(j, 37, &byte);
			if (status == 0) {
				spd_mhz_max[j-SMBUS_MEM_DEVICE_0] = max | byte;
			}
		}
		else {
			spd_mhz_min[j-SMBUS_MEM_DEVICE_0] = 0;
			spd_mhz_max[j-SMBUS_MEM_DEVICE_0] = 0;
		}
		status = smbus_read_byte(j, 99, &byte);
		if (status == 0) {
			spd_devices[j-SMBUS_MEM_DEVICE_0]=byte;
		}
		status = smbus_read_byte(j, 100, &byte);
		if (status == 0) {
			spd_data_width[j-SMBUS_MEM_DEVICE_0]=byte;
		}
	}
}

static void ram_zap(unsigned long start, unsigned long stop)
{
	unsigned long addr;
	/* 
	 * Fill.
	 */
	printk_debug("DRAM zero: %08lx-%08lx\n", start, stop);
	for(addr = start; addr < stop ; addr += 4) {
		/* Display address being filled */
		if ((addr & 0x0fffffff) == 0)
			printk_spew("%08lx\r", addr);
		RAM(unsigned long, addr) = 0;
	};
	/* Display final address */
	printk_debug("%08lx\nDRAM filled\n", addr);

}

void init_memory(void)
{
	int i,j;
	int rdram_devices=0;
	u32 ricm;
	u16 word;

	load_spd_vars();
	display_spd_dev_row_col_bank(spd_devices, spd_row_col, spd_banks);
	for(i=0;i<2;i++) {
		if(spd_devices&&(spd_devices[i]==spd_devices[i+2])&&
			(spd_row_col[i]==spd_row_col[i+2])&&
			(spd_banks[i]==spd_banks[i+2]))
			rdram_devices+=spd_devices[i];
	}
	if(rdram_devices==0){
		printk_debug("ERROR - Memory Rimms are not matched.\n");
		for(;;) i=1;  /* Freeze the system error */
	}
	else {
		rdram_chips=rdram_devices;
	}
	printk_debug("RDRAM Chips = %d\n",rdram_chips);

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
	 * FIXME manual current calibration.
	 */

	rdram_run_command(0, 0, CMD_MCH_RAC_CURRENT_CALIBRATION);

	/* Do not do the following command.  It will cause a lock up */
	/* rdram_run_command(0, 0, CMD_MANUAL_CURRENT_CALIBRATION);  */

	for(i = 0; i < 128; i++) {
		for(j=0;j<rdram_chips;j++) {
			rdram_run_command(0, j, CMD_RDRAM_CURRENT_CALIBRATION);
		}
	}

#if 0
	display_rdram_regs( rdram_chips );
#endif

	rdram_run_command(0, BCAST, CMD_TEMP_CALIBRATE_ENABLE);
	rdram_run_command(0, BCAST, CMD_TEMP_CALIBRATE);

	/* 6.2 RDRAM Read Domain Initialization */
	rdram_read_domain_initialization(rdram_devices);

	/* 7.0 Remaining init bits (LSR, NSR, PSR) */
	set_init_bits(rdram_devices);

	/* Final Set the memory initialized bit */
	pcibios_read_config_dword(I860_MCH_BUS, I860_MCH_DEVFN, MCH_RICM, &ricm);
	ricm |= RICM_DONE;
	pcibios_write_config_dword(I860_MCH_BUS, I860_MCH_DEVFN,
		MCH_RICM, ricm);
	
	/* Memory Controller Hub Configuration Enable ECC */
	pcibios_read_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_MCHCFG, &word); 
	word |= (1 << 8);
	pcibios_write_config_word(I860_MCH_BUS, I860_MCH_DEVFN, MCH_MCHCFG, word); 

	/* Set write combine for the memory max of 2 gig */
	printk_debug("set_var_mtrr\n");
	set_var_mtrr(0, 0, 2*1024*1024*1024UL, MTRR_TYPE_WRCOMB);

	/* Clear all memory to 0's */
	total_rdram=(unsigned long)(((unsigned long)spd_size[0]<<21)*spd_devices[0])+
			(((unsigned long)spd_size[1]<<21)*spd_devices[1]);
	ram_zap(0,0x0a0000);
	ram_zap(0x0c0000,(unsigned long)total_rdram);
}

