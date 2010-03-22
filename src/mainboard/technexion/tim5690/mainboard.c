/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include <../southbridge/amd/sb600/sb600.h>
#include <../superio/ite/it8712f/it8712f.h>
#include "chip.h"
#include "tn_post_code.h"
#include "vgabios.h"

#define ADT7461_ADDRESS 0x4C
#define ARA_ADDRESS     0x0C /* Alert Response Address */
#define SMBUS_IO_BASE 0x1000


/* Video BIOS Function Extensions Specification
 */
//Callback Sub-Function 00h - Get LCD Panel ID
#define LCD_PANEL_ID_NO 0x00	/* No LCD */
#define LCD_PANEL_ID_01 0x01	/* 1024x768, 24 bits, 1 channel */
#define LCD_PANEL_ID_02 0x02	/* 1280x1024, 24 bits, 2 channels */
#define LCD_PANEL_ID_03 0x03	/* 1440x900, 24 bits, 2 channels */
#define LCD_PANEL_ID_04 0x04	/* 1680x1050, 24 bits, 2 channels */
#define LCD_PANEL_ID_05 0x05	/* 1920x1200, 24 bits, 2 channels */
#define LCD_PANEL_ID_06 0x06	/* 1920x1080, 24 bits, 2 channels */
//Callback Sub-Function 05h – Select Boot-up TV Standard
#define TV_MODE_00	0x00	/* NTSC */
#define TV_MODE_01	0x01	/* PAL */
#define TV_MODE_02	0x02	/* PALM */
#define TV_MODE_03	0x03	/* PAL60 */
#define TV_MODE_04	0x04	/* NTSCJ */
#define TV_MODE_05	0x05	/* PALCN */
#define TV_MODE_06	0x06	/* PALN */
#define TV_MODE_09	0x09	/* SCART-RGB */
#define TV_MODE_NO	0xff	/* No TV Support */


/* The base address is 0x2e or 0x4e, depending on config bytes. */
#define SIO_BASE                     0x2e
#define SIO_INDEX                    SIO_BASE
#define SIO_DATA                     SIO_BASE+1

/* Global configuration registers. */
#define IT8712F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8712F_CONFIG_REG_LDN       0x07 /* Logical Device Number. */
#define IT8712F_CONFIG_REG_CONFIGSEL 0x22 /* Configuration Select. */
#define IT8712F_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define IT8712F_CONFIG_REG_SWSUSP    0x24 /* Software Suspend, Flash I/F. */
#define IT8712F_CONFIG_REG_MFC       0x2a /* Multi-function control */
#define IT8712F_CONFIG_REG_WATCHDOG  0x72 /* Watchdog control. */

#define IT8712F_CONFIGURATION_PORT   0x2e /* Write-only. */
#define IT8712F_SIMPLE_IO_BASE       0x200 /* Simple I/O base address */


extern int do_smbus_read_byte(u32 smbus_io_base, u32 device, u32 address);
extern int do_smbus_write_byte(u32 smbus_io_base, u32 device, u32 address,
			       u8 val);
extern void lb_add_memory_range(struct lb_memory *mem, uint32_t type,
				uint64_t start, uint64_t size);
#define ADT7461_read_byte(address) \
	do_smbus_read_byte(SMBUS_IO_BASE, ADT7461_ADDRESS, address)
#define ARA_read_byte(address) \
	do_smbus_read_byte(SMBUS_IO_BASE, ARA_ADDRESS, address)
#define ADT7461_write_byte(address, val) \
	do_smbus_write_byte(SMBUS_IO_BASE, ADT7461_ADDRESS, address, val)

/* previous
 */
void tim5690_enable(device_t dev);
int add_mainboard_resources(struct lb_memory *mem);


uint64_t uma_memory_base, uma_memory_size;


/* The content of IT8712F_CONFIG_REG_LDN (index 0x07) must be set to the
   LDN the register belongs to, before you can access the register. */
static void it8712f_sio_write(uint8_t ldn, uint8_t index, uint8_t value)
{
        outb(IT8712F_CONFIG_REG_LDN, SIO_BASE);
        outb(ldn, SIO_DATA);
        outb(index, SIO_BASE);
        outb(value, SIO_DATA);
}

static void it8712f_enter_conf(void)
{
        /*  Enter the configuration state (MB PnP mode). */

        /* Perform MB PnP setup to put the SIO chip at 0x2e. */
        /* Base address 0x2e: 0x87 0x01 0x55 0x55. */
        /* Base address 0x4e: 0x87 0x01 0x55 0xaa. */
        outb(0x87, IT8712F_CONFIGURATION_PORT);
        outb(0x01, IT8712F_CONFIGURATION_PORT);
        outb(0x55, IT8712F_CONFIGURATION_PORT);
        outb(0x55, IT8712F_CONFIGURATION_PORT);
}

static void it8712f_exit_conf(void)
{
        /* Exit the configuration state (MB PnP mode). */
        it8712f_sio_write(0x00, IT8712F_CONFIG_REG_CC, 0x02);
}


/* set thermal config
 */
static void set_thermal_config(void)
{
	u8 byte;
	u16 word;
	device_t sm_dev;

	/* set ADT 7461 */
	ADT7461_write_byte(0x0B, 0x50);	/* Local Temperature Hight limit */
	ADT7461_write_byte(0x0C, 0x00);	/* Local Temperature Low limit */
	ADT7461_write_byte(0x0D, 0x50);	/* External Temperature Hight limit  High Byte */
	ADT7461_write_byte(0x0E, 0x00);	/* External Temperature Low limit High Byte */

	ADT7461_write_byte(0x19, 0x55);	/* External THERM limit */
	ADT7461_write_byte(0x20, 0x55);	/* Local THERM limit */

	byte = ADT7461_read_byte(0x02);	/* read status register to clear it */
	ARA_read_byte(0x05); /* A hardware alert can only be cleared by the master sending an ARA as a read command */
	printk(BIOS_INFO, "Init adt7461 end , status 0x02 %02x\n", byte);

	/* sb600 settings for thermal config */
	/* set SB600 GPIO 64 to GPIO with pull-up */
	byte = pm2_ioread(0x42);
	byte &= 0x3f;
	pm2_iowrite(0x42, byte);

	/* set GPIO 64 to input */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	word = pci_read_config16(sm_dev, 0x56);
	word |= 1 << 7;
	pci_write_config16(sm_dev, 0x56, word);

	/* set GPIO 64 internal pull-up */
	byte = pm2_ioread(0xf0);
	byte &= 0xee;
	pm2_iowrite(0xf0, byte);

	/* set Talert to be active low */
	byte = pm_ioread(0x67);
	byte &= ~(1 << 5);
	pm_iowrite(0x67, byte);

	/* set Talert to generate ACPI event */
	byte = pm_ioread(0x3c);
	byte &= 0xf3;
	pm_iowrite(0x3c, byte);

	/* THERMTRIP pin */
	/* byte = pm_ioread(0x68);
	 * byte |= 1 << 3;
	 * pm_iowrite(0x68, byte);
	 *
	 * byte = pm_ioread(0x55);
	 * byte |= 1 << 0;
	 * pm_iowrite(0x55, byte);
	 *
	 * byte = pm_ioread(0x67);
	 * byte &= ~( 1 << 6);
	 * pm_iowrite(0x67, byte);
	 */
}

/* Mainboard specific GPIO setup. */
void mb_gpio_init(u16 *iobase)
{
        /* Init Super I/O GPIOs. */
        it8712f_enter_conf();
        outb(IT8712F_CONFIG_REG_LDN, SIO_INDEX);
        outb(IT8712F_GPIO, SIO_DATA);
        outb(0x62, SIO_INDEX); 
        outb((*iobase >> 8), SIO_DATA);
        outb(0x63, SIO_INDEX);
        outb((*iobase & 0xff), SIO_DATA);
        it8712f_exit_conf();
}

/* The LCD's panel id seletion. */
void lcd_panel_id(rs690_vbios_regs *vbios_regs, u8 num_id)
{
	switch (num_id) {
	case 0x1:
		vbios_regs->int15_regs.fun00_panel_id = LCD_PANEL_ID_01;
		break;
	case 0x2:
		vbios_regs->int15_regs.fun00_panel_id = LCD_PANEL_ID_02;
		break;
	case 0x3:
		vbios_regs->int15_regs.fun00_panel_id = LCD_PANEL_ID_03;
		break;
	case 0x4:
		vbios_regs->int15_regs.fun00_panel_id = LCD_PANEL_ID_04;
		break;
	case 0x5:
		vbios_regs->int15_regs.fun00_panel_id = LCD_PANEL_ID_05;
		break;
	case 0x6:
		vbios_regs->int15_regs.fun00_panel_id = LCD_PANEL_ID_06;
		break;
	default:
		vbios_regs->int15_regs.fun00_panel_id = LCD_PANEL_ID_NO;
		break;
	}
}

/*************************************************
* enable the dedicated function in tim5690 board.
* This function called early than rs690_enable.
*************************************************/
void tim5690_enable(device_t dev)
{
	struct mainboard_config *mainboard =
	    (struct mainboard_config *)dev->chip_info;

	rs690_vbios_regs vbios_regs;
	u16 gpio_base = IT8712F_SIMPLE_IO_BASE;
	u8 port2;

	printk(BIOS_INFO, "Mainboard tim5690 Enable. dev=0x%p\n", dev);

	mb_gpio_init(&gpio_base);

	/* The LCD's panel id seletion by switch. */
	port2 = inb(gpio_base+1);
	lcd_panel_id(&vbios_regs, ((~port2) & 0xf));
	/* No support TV */
	vbios_regs.int15_regs.fun05_tv_standard = TV_MODE_NO;
	vgabios_init(&vbios_regs);

#if (CONFIG_GFXUMA == 1)
	msr_t msr, msr2;

	/* TOP_MEM: the top of DRAM below 4G */
	msr = rdmsr(TOP_MEM);
	printk(BIOS_INFO, "%s, TOP MEM: msr.lo = 0x%08x, msr.hi = 0x%08x\n",
		    __func__, msr.lo, msr.hi);

	/* TOP_MEM2: the top of DRAM above 4G */
	msr2 = rdmsr(TOP_MEM2);
	printk(BIOS_INFO, "%s, TOP MEM2: msr2.lo = 0x%08x, msr2.hi = 0x%08x\n",
		    __func__, msr2.lo, msr2.hi);

	switch (msr.lo) {
	case 0x10000000:	/* 256M system memory */
		uma_memory_size = 0x2000000;	/* 32M recommended UMA */
		break;

	case 0x18000000:	/* 384M system memory */
		uma_memory_size = 0x4000000;	/* 64M recommended UMA */
		break;

	case 0x20000000:	/* 512M system memory */
		uma_memory_size = 0x4000000;	/* 64M recommended UMA */
		break;

	default:		/* 1GB and above system memory */
		uma_memory_size = 0x8000000;	/* 128M recommended UMA */
		break;
	}

	uma_memory_base = msr.lo - uma_memory_size;	/* TOP_MEM1 */
	printk(BIOS_INFO, "%s: uma size 0x%08llx, memory start 0x%08llx\n",
		    __func__, uma_memory_size, uma_memory_base);

	/* TODO: TOP_MEM2 */
#else
	uma_memory_size = 0x8000000;	/* 128M recommended UMA */
	uma_memory_base = 0x38000000;	/* 1GB  system memory supposed */
#endif

	set_thermal_config();
}

int add_mainboard_resources(struct lb_memory *mem)
{
	/* UMA is removed from system memory in the northbridge code, but
	 * in some circumstances we want the memory mentioned as reserved.
 	 */
#if (CONFIG_GFXUMA == 1)
	printk(BIOS_INFO, "uma_memory_base=0x%llx, uma_memory_size=0x%llx \n",
	uma_memory_base, uma_memory_size);
	lb_add_memory_range(mem, LB_MEM_RESERVED,
		uma_memory_base, uma_memory_size);
#endif
	technexion_post_code(LED_MESSAGE_FINISH);
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("TechNexion TIM-5690   Mainboard")
	.enable_dev = tim5690_enable,
};
