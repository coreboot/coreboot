/* Copyright 2000  AG Electronics Ltd. */
/* Copyright 2003-2004 Linux Networx */
/* Copyright 2004 Tyan 
   By LYH change from PC87360 */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <device/chip.h>
#include <console/console.h>
#include <string.h>
#include <bitops.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include "chip.h"
#include "w83627hf.h"


void pnp_enter_ext_func_mode(device_t dev) {
        outb(0x87, dev->path.u.pnp.port);
        outb(0x87, dev->path.u.pnp.port);
}
void pnp_exit_ext_func_mode(device_t dev) {
        outb(0xaa, dev->path.u.pnp.port);
}

void pnp_write_hwm(unsigned long port_base, uint8_t reg, uint8_t value)
{
        outb(reg, port_base+5);
        outb(value, port_base+6);
}

uint8_t pnp_read_hwm(unsigned long port_base, uint8_t reg)
{
        outb(reg, port_base + 5);
        return inb(port_base + 6);
}       

static void enable_hwm_smbus(device_t dev) {
	uint8_t reg, value;
	reg = 0x2b;
	value = pnp_read_config(dev, reg);
	value &= 0x3f;
	pnp_write_config(dev, reg, value);
}

#if 0
static void dump_pnp_device(device_t dev)
{
        int i;
        print_debug("\r\n");

        for(i = 0; i <= 255; i++) {
                uint8_t reg, val;
                if ((i & 0x0f) == 0) {
                        print_debug_hex8(i);
                        print_debug_char(':');
                }
		reg = i;
		if(i!=0xaa) {
                	val = pnp_read_config(dev, reg);
		}
		else {
			val = 0xaa;
		}
                print_debug_char(' ');
                print_debug_hex8(val);
                if ((i & 0x0f) == 0x0f) {
                        print_debug("\r\n");
                }
        }
}
#endif

static void init_hwm(unsigned long base)
{
	uint8_t  reg, value;
	int i;

	unsigned  hwm_reg_values[] = {
//	       reg                mask             data
              0x40     ,       0xff    ,       0x81,  //  ; Start Hardware Monitoring for WIN627
              0x48     ,       0xaa    ,       0x2a,  //  ; Program SIO SMBus BAR to 54h>>1	
//              0x48     ,       0xc8    ,       0x48,  //  ; Program SIO SMBus BAR to 90h>>1
              0x4A     ,       0x21    ,       0x21,  //  ; Program T2 SMBus BAR to 92h>>1 &
                                                     //  ; Program T3 SMBus BAR to 94h>>1
              0x4E     ,       0x80    ,       0x00,  
              0x43     ,       0x00    ,       0xFF,
              0x44     ,       0x00    ,       0x3F,
              0x4C     ,       0xBF    ,       0x18,
              0x4D     ,       0xFF    ,       0x80   //  ; Turn Off Beep
                                                                            
	};

	for(i = 0; i<  sizeof(hwm_reg_values)/sizeof(hwm_reg_values[0]); i+=3 ) { 
		reg = hwm_reg_values[i];	
	 	value = pnp_read_hwm(base, reg);		
		value &= 0xff & hwm_reg_values[i+1];
		value |= 0xff & hwm_reg_values[i+2];
#if 0
		printk_debug("base = 0x%04x, reg = 0x%02x, value = 0x%02x\r\n", base,reg,value);
#endif
		pnp_write_hwm(base,reg, value);
	}
}


static void w83627hf_init(device_t dev)
{
	struct superio_winbond_w83627hf_config *conf;
	struct resource *res0, *res1;
	if (!dev->enabled) {
		return;
	}
	conf = dev->chip->chip_info;
	switch(dev->path.u.pnp.device) {
	case W83627HF_SP1: 
		res0 = get_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com1);
		break;
	case W83627HF_SP2:
		res0 = get_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com2);
		break;
	case W83627HF_KBC:
		res0 = get_resource(dev, PNP_IDX_IO0);
		res1 = get_resource(dev, PNP_IDX_IO1);
		init_pc_keyboard(res0->base, res1->base, &conf->keyboard);
		break;
        case W83627HF_HWM:
                res0 = get_resource(dev, PNP_IDX_IO0);
                init_hwm(res0->base);
                break;
	}
	
}

void w83627hf_pnp_set_resources(device_t dev)
{

	pnp_enter_ext_func_mode(dev);  

	pnp_set_resources(dev);

#if 0
        dump_pnp_device(dev);
#endif
                
        pnp_exit_ext_func_mode(dev);  
        
}       
        
void w83627hf_pnp_enable_resources(device_t dev)
{       
        pnp_enter_ext_func_mode(dev);  
	
	pnp_enable_resources(dev);               

        switch(dev->path.u.pnp.device) {
	case W83627HF_HWM:
		//set the pin 91,92 as I2C bus
		printk_debug("w83627hf hwm smbus enabled\r\n");
		enable_hwm_smbus(dev);
		break;
	}

#if 0  
        dump_pnp_device(dev);
#endif

        pnp_exit_ext_func_mode(dev);  

}

void w83627hf_pnp_enable(device_t dev)
{

        if (!dev->enabled) {
                pnp_enter_ext_func_mode(dev);   

                pnp_set_logical_device(dev);
                pnp_set_enable(dev, 0);

                pnp_exit_ext_func_mode(dev);  
        }
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = w83627hf_pnp_set_resources,
	.enable_resources = w83627hf_pnp_enable_resources,
	.enable           = w83627hf_pnp_enable,
	.init             = w83627hf_init,
};

static struct pnp_info pnp_dev_info[] = {
        { &ops, W83627HF_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07f8, 0}, },
        { &ops, W83627HF_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07f8, 0}, },
        { &ops, W83627HF_SP1,  PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
        { &ops, W83627HF_SP2,  PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
        // No 4 { 0,},
        { &ops, W83627HF_KBC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, { 0x7ff, 0 }, { 0x7ff, 0x4}, },
        { &ops, W83627HF_CIR, PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
        { &ops, W83627HF_GAME_MIDI_GPIO1, PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x7ff, 0 }, {0x7fe, 4} },
        { &ops, W83627HF_GPIO2,},
        { &ops, W83627HF_GPIO3,},
        { &ops, W83627HF_ACPI, PNP_IRQ0,  },
        { &ops, W83627HF_HWM,  PNP_IO0 | PNP_IRQ0, { 0xff8, 0 } },
};

static void enumerate(struct chip *chip)
{
	pnp_enumerate(chip, sizeof(pnp_dev_info)/sizeof(pnp_dev_info[0]), 
		&pnp_ops, pnp_dev_info);
}

struct chip_control superio_winbond_w83627hf_control = {
	.enumerate = enumerate,
	.name      = "Winbond w83627hf"
};

