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

//int rdram_chips=0; /* number of ram chips on the rimms */
//static u32 total_rdram;   /* Total rdram found */

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


void display_spd_dev_row_col_bank(u8 *spd_devices, u8 *spd_row_col, u8 *spd_banks)
{
	int i;

	for(i=0;i<4;i++)
 	 	printk_debug("Devices %d, Row Bits %d, Col Bits %d, Bank Bits %d\n",
		  spd_devices[i],(spd_row_col[i]>>4),(spd_row_col[i]&0x0f),spd_banks[i]);

}

void display_rdram_regs_tparm(int rdram_chips)
{
	int i,j;

	for(i = 0; i < rdram_chips; i++) {
                for(j = 12; j < 15; j+=2) {
                        struct rdram_reg_values values;
                        u16 reg = rdram_regs[j];
                        rdram_read_reg(0, i, reg, &values);
                        printk_debug("rdram: %2d reg: %02x %10s a: 0x%04x b: 0x%04x\n",
                                i, reg, rdram_reg_names[j], values.channel_a, values.channel_b);                }
        }
}

void display_rdram_regs(int rdram_chips )
{
        int i,j;

        for(i = 0; i < rdram_chips; i++) {
                for(j = 0; j < sizeof(rdram_regs)/sizeof(rdram_regs[0]); j++) {
                        struct rdram_reg_values values;
                        u16 reg = rdram_regs[j];
                        rdram_read_reg(0, i, reg, &values);
                        printk_debug("rdram: %2d reg: %02x %10s a: 0x%04x b: 0x%04x\n",
                                i, reg, rdram_reg_names[j], values.channel_a, values.channel_b);
                }
        }

}

void display_smbus_spd(void)
{
	int i,j;

	for(j = SMBUS_MEM_DEVICE_0; j < SMBUS_MEM_DEVICE_0 + 4; j++) {
		int status = 0;
		if ((j == 0x1b) || 0) {
			printk_debug("skipping device: %02x\n", j);
			continue;
		}
		printk_debug("smbus_device: %02x\n", j);
		for(i = 0; (i < 128) && (status == 0); i++) {
			unsigned char byte;
			status = smbus_read_byte(j, i, &byte);
			if (status != 0) {
				printk_debug("ram slot empty or bad device\n");
				continue;
			}
			printk_debug("0x%02x ", byte);
			if ((i &0x0f) == 0x0f) {
				printk_debug("\n");
			}
		}
	}
	printk_debug("\n");

}

void display_mch_regs(void)
{
	int i,j;

	printk_debug("MCH Register Dump\n");
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
}


